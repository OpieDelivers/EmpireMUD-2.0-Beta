/* ************************************************************************
*   File: statistics.c                                    EmpireMUD 2.0b3 *
*  Usage: code related game stats                                         *
*                                                                         *
*  EmpireMUD code base by Paul Clarke, (C) 2000-2015                      *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  EmpireMUD based upon CircleMUD 3.0, bpl 17, by Jeremy Elson.           *
*  CircleMUD (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "handler.h"

/**
* Contents:
*   Stats Displays
*   Stats Getters
*   Player Stats
*   World Stats
*/

// config
int rescan_world_after = 5 * SECS_PER_REAL_MIN;


// stats data
struct stats_data_struct {
	any_vnum vnum;
	int count;
	
	UT_hash_handle hh;	// hashable
};

// stats globals
struct stats_data_struct *global_sector_count = NULL;	// hash table of sector counts
struct stats_data_struct *global_crop_count = NULL;	// hash table count of crops
struct stats_data_struct *global_building_count = NULL;	// hash table of building counts

time_t last_world_count = 0;	// timestamp of last sector/crop/building tally
time_t last_account_count = 0;	// timestamp of last time accounts were read

int total_accounts = 0;	// including inactive
int active_accounts = 0;	// not timed out (at least 1 char)
int active_accounts_week = 0;	// just this week (at least 1 char)
int max_players_today = 0;	// stats on max players seen
int max_players_this_uptime = 0;


// external consts


// external funcs


// locals
void update_world_count();


 //////////////////////////////////////////////////////////////////////////////
//// STATS DISPLAYS //////////////////////////////////////////////////////////

/**
* Displays various game stats. This is shown on login/logout.
*
* @param char_data *ch The person to show stats to.
*/
void display_statistics_to_char(char_data *ch) {
	extern int top_of_p_table;
	extern time_t boot_time;

	char populous_str[MAX_STRING_LENGTH], wealthiest_str[MAX_STRING_LENGTH], famous_str[MAX_STRING_LENGTH], greatest_str[MAX_STRING_LENGTH];
	int populous_empire = NOTHING, wealthiest_empire = NOTHING, famous_empire = NOTHING, greatest_empire = NOTHING;
	char_data *vict;
	obj_data *obj;
	empire_data *emp, *next_emp;
	int citizens, territory, members, military, count;

	char *tmstr;
	time_t mytime;
	int d, h, m;

	if (!ch || !ch->desc)
		return;

	msg_to_char(ch, "\r\nEmpireMUD Statistics:\r\n");

	mytime = boot_time;

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	mytime = time(0) - boot_time;
	d = mytime / 86400;
	h = (mytime / 3600) % 24;
	m = (mytime / 60) % 60;

	msg_to_char(ch, "Current uptime: Up since %s: %d day%s, %d:%02d\r\n", tmstr, d, ((d == 1) ? "" : "s"), h, m);

	/* Find best scores.. */
	HASH_ITER(hh, empire_table, emp, next_emp) {
		if (EMPIRE_IMM_ONLY(emp) || EMPIRE_IS_TIMED_OUT(emp)) {
			continue;
		}
		
		if (EMPIRE_MEMBERS(emp) > populous_empire) {
			populous_empire = EMPIRE_MEMBERS(emp);
		}
		if (GET_TOTAL_WEALTH(emp) > wealthiest_empire) {
			wealthiest_empire = GET_TOTAL_WEALTH(emp);
		}
		if (EMPIRE_FAME(emp) > famous_empire) {
			famous_empire = EMPIRE_FAME(emp);
		}
		if (EMPIRE_GREATNESS(emp) > greatest_empire) {
			greatest_empire = EMPIRE_GREATNESS(emp);
		}
	}
	
	// init strings
	*populous_str = '\0';
	*wealthiest_str = '\0';
	*famous_str = '\0';
	*greatest_str = '\0';
	
	// build strings
	HASH_ITER(hh, empire_table, emp, next_emp) {
		if (EMPIRE_IMM_ONLY(emp) || EMPIRE_IS_TIMED_OUT(emp)) {
			continue;
		}
		
		if (populous_empire != NOTHING && EMPIRE_MEMBERS(emp) >= populous_empire) {
			snprintf(populous_str + strlen(populous_str), sizeof(populous_str) - strlen(populous_str), "%s%s%s&0", (*populous_str ? ", " : ""), EMPIRE_BANNER(emp), EMPIRE_NAME(emp));
		}
		if (wealthiest_empire != NOTHING && GET_TOTAL_WEALTH(emp) >= wealthiest_empire) {
			snprintf(wealthiest_str + strlen(wealthiest_str), sizeof(wealthiest_str) - strlen(wealthiest_str), "%s%s%s&0", (*wealthiest_str ? ", " : ""), EMPIRE_BANNER(emp), EMPIRE_NAME(emp));
		}
		if (famous_empire != NOTHING && EMPIRE_FAME(emp) >= famous_empire) {
			snprintf(famous_str + strlen(famous_str), sizeof(famous_str) - strlen(famous_str), "%s%s%s&0", (*famous_str ? ", " : ""), EMPIRE_BANNER(emp), EMPIRE_NAME(emp));
		}
		if (greatest_empire != NOTHING && EMPIRE_GREATNESS(emp) >= greatest_empire) {
			snprintf(greatest_str + strlen(greatest_str), sizeof(greatest_str) - strlen(greatest_str), "%s%s%s&0", (*greatest_str ? ", " : ""), EMPIRE_BANNER(emp), EMPIRE_NAME(emp));
		}
	}
	
	if (*greatest_str) {
		msg_to_char(ch, "Greatest empire: %s\r\n", greatest_str);
	}
	if (*populous_str) {
		msg_to_char(ch, "Most populous empire: %s\r\n", populous_str);
	}
	if (*wealthiest_str) {
		msg_to_char(ch, "Most wealthy empire: %s\r\n", wealthiest_str);
	}
	if (*famous_str) {
		msg_to_char(ch, "Most famous empire: %s\r\n", famous_str);
	}

	// empire stats
	territory = members = citizens = military = 0;
	HASH_ITER(hh, empire_table, emp, next_emp) {
		territory += EMPIRE_CITY_TERRITORY(emp) + EMPIRE_OUTSIDE_TERRITORY(emp);
		members += EMPIRE_MEMBERS(emp);
		citizens += EMPIRE_POPULATION(emp);
		military += EMPIRE_MILITARY(emp);
	}

	msg_to_char(ch, "Total Empires:	    %3d     Claimed Area:       %d\r\n", HASH_COUNT(empire_table), territory);
	msg_to_char(ch, "Total Players:    %5d     Players in Empires: %d\r\n", top_of_p_table + 1, members);
	msg_to_char(ch, "Total Citizens:   %5d     Total Military:     %d\r\n", citizens, military);

	// creatures
	count = 0;
	for (vict = character_list; vict; vict = vict->next) {
		if (IS_NPC(vict)) {
			++count;
		}
	}
	msg_to_char(ch, "Unique Creatures:   %3d     Total Mobs:         %d\r\n", HASH_COUNT(mobile_table), count);

	// objs
	count = 0;
	for (obj = object_list; obj; obj = obj->next) {
		++count;
	}
	msg_to_char(ch, "Unique Objects:     %3d     Total Objects:      %d\r\n", HASH_COUNT(object_table), count);
}


/**
* do_mudstats: empire score averages
*
* @param char_data *ch Person to show it to.
* @param char *argument In case some stats have sub-categories.
*/
void mudstats_empires(char_data *ch, char *argument) {
	extern double empire_score_average[NUM_SCORES];
	extern const char *score_type[];
	
	int iter;
	
	msg_to_char(ch, "Empire score averages:\r\n");
	
	for (iter = 0; iter < NUM_SCORES; ++iter) {
		msg_to_char(ch, " %s: %.3f\r\n", score_type[iter], empire_score_average[iter]);
	}
}


 //////////////////////////////////////////////////////////////////////////////
//// STATS GETTERS ///////////////////////////////////////////////////////////

/**
* @param bld_data *bdg What building type.
* @return int The number of instances of that building in the world.
*/
int stats_get_building_count(bld_data *bdg) {
	struct stats_data_struct *data;
	any_vnum vnum;

	// sanity
	if (!bdg) {
		return 0;
	}
	
	if (HASH_COUNT(global_building_count) != HASH_COUNT(building_table) || last_world_count + rescan_world_after < time(0)) {
		update_world_count();
	}
	
	vnum = GET_BLD_VNUM(bdg);
	HASH_FIND_INT(global_building_count, &vnum, data);
	return data ? data->count : 0;
}


/**
* @param crop_data *cp What crop type.
* @return int The number of instances of that crop in the world.
*/
int stats_get_crop_count(crop_data *cp) {
	struct stats_data_struct *data;
	any_vnum vnum;

	if (!cp) {
		return 0;
	}
	
	if (HASH_COUNT(global_crop_count) != HASH_COUNT(crop_table) || last_world_count + rescan_world_after < time(0)) {
		update_world_count();
	}
	
	vnum = GET_CROP_VNUM(cp);
	HASH_FIND_INT(global_crop_count, &vnum, data);
	
	return data ? data->count : 0;
}


/**
* @param sector_data *sect What sector type.
* @return int The number of instances of that sector in the world.
*/
int stats_get_sector_count(sector_data *sect) {
	struct stats_data_struct *data;
	any_vnum vnum;
	
	if (!sect) {
		return 0;
	}
	
	if (HASH_COUNT(global_sector_count) != HASH_COUNT(sector_table) || last_world_count + rescan_world_after < time(0)) {
		update_world_count();
	}
	
	vnum = GET_SECT_VNUM(sect);
	HASH_FIND_INT(global_sector_count, &vnum, data);
	
	return data ? data->count : 0;
}


 //////////////////////////////////////////////////////////////////////////////
//// PLAYER STATS ////////////////////////////////////////////////////////////

/**
* This function updates the following global stats:
*  total_accounts (all)
*  active_accounts (at least one character not timed out)
*  active_accounts_week (at least one character this week)
*/
void update_account_stats(void) {
	extern bool member_is_timed_out_cfu(struct char_file_u *chdata);

	// helper type
	struct uniq_acct_t {
		int id;	// either account id (positive) or player idnum (negative)
		bool active_week;
		bool active_timeout;
		UT_hash_handle hh;
	};

	struct uniq_acct_t *acct, *next_acct, *acct_list = NULL;
	struct char_file_u chdata;
	int pos, id;
	
	// rate-limit this, as it scans the whole playerfile
	if (last_account_count + rescan_world_after > time(0)) {
		return;
	}
	
	// build list
	for (pos = 0; pos <= top_of_p_table; ++pos) {
		// need chdata either way; check deleted here
		if (load_char(player_table[pos].name, &chdata) <= NOBODY || IS_SET(chdata.char_specials_saved.act, PLR_DELETED)) {
			continue;
		}
		
		// see if we have data
		id = chdata.player_specials_saved.account_id > 0 ? chdata.player_specials_saved.account_id : (-1 * chdata.char_specials_saved.idnum);
		HASH_FIND_INT(acct_list, &id, acct);
		if (!acct) {
			CREATE(acct, struct uniq_acct_t, 1);
			acct->id = id;
			acct->active_week = acct->active_timeout = FALSE;
			HASH_ADD_INT(acct_list, id, acct);
		}
		
		// update
		acct->active_week |= ((time(0) - chdata.last_logon) < SECS_PER_REAL_WEEK);
		if (!acct->active_timeout) {
			acct->active_timeout = !member_is_timed_out_cfu(&chdata);
		}
	}

	// compute stats (and free temp data)
	total_accounts = 0;
	active_accounts = 0;
	active_accounts_week = 0;
	
	HASH_ITER(hh, acct_list, acct, next_acct) {
		++total_accounts;
		if (acct->active_week) {
			++active_accounts_week;
		}
		if (acct->active_timeout) {
			++active_accounts;
		}
		
		// and free
		HASH_DEL(acct_list, acct);
		free(acct);
	}
	
	// update timestamp
	last_account_count = time(0);
}


/**
* Updates the count of players online.
*/
void update_players_online_stats(void) {
	descriptor_data *d;
	int count;
	
	// determine current count
	count = 0;
	for (d = descriptor_list; d; d = d->next) {
		if (STATE(d) != CON_PLAYING || !d->character) {
			continue;
		}
		
		// morts only
		if (IS_IMMORTAL(d->character) || GET_INVIS_LEV(d->character) > LVL_APPROVED) {
			continue;
		}
		
		++count;
	}
	
	max_players_today = MAX(max_players_today, count);
	max_players_this_uptime = MAX(max_players_this_uptime, count);
}


 //////////////////////////////////////////////////////////////////////////////
//// WORLD STATS /////////////////////////////////////////////////////////////

/**
* Updates global_sector_count, global_crop_count, global_building_count.
*/
void update_world_count(void) {
	struct stats_data_struct *sect_inf = NULL, *crop_inf = NULL, *bld_inf = NULL, *data, *next_data;
	room_data *iter, *next_iter;
	any_vnum vnum, last_bld_vnum = NOTHING, last_crop_vnum = NOTHING, last_sect_vnum = NOTHING;
	
	// free and recreate counts
	HASH_ITER(hh, global_sector_count, data, next_data) {
		HASH_DEL(global_sector_count, data);
		free(data);
	}
	HASH_ITER(hh, global_crop_count, data, next_data) {
		HASH_DEL(global_crop_count, data);
		free(data);
	}
	HASH_ITER(hh, global_building_count, data, next_data) {
		HASH_DEL(global_building_count, data);
		free(data);
	}
	
	// scan world
	HASH_ITER(world_hh, world_table, iter, next_iter) {
		// sector
		vnum = GET_SECT_VNUM(SECT(iter));
		if (vnum != last_sect_vnum) {
			HASH_FIND_INT(global_sector_count, &vnum, sect_inf);
			if (!sect_inf) {
				CREATE(sect_inf, struct stats_data_struct, 1);
				sect_inf->vnum = vnum;
				HASH_ADD_INT(global_sector_count, vnum, sect_inf);
			}
			last_sect_vnum = vnum;
		}
		++sect_inf->count;
		
		// crop?
		vnum = ROOM_CROP_TYPE(iter);
		if (vnum != NOTHING) {
			if (vnum != last_crop_vnum) {
				HASH_FIND_INT(global_crop_count, &vnum, crop_inf);
				if (!crop_inf) {
					CREATE(crop_inf, struct stats_data_struct, 1);
					crop_inf->vnum = vnum;
					HASH_ADD_INT(global_crop_count, vnum, crop_inf);
				}
				last_crop_vnum = vnum;
			}
			
			++crop_inf->count;
		}
		
		// building?
		vnum = BUILDING_VNUM(iter);
		if (vnum != NOTHING) {
			if (vnum != last_bld_vnum) {
				HASH_FIND_INT(global_building_count, &vnum, bld_inf);
				if (!bld_inf) {
					CREATE(bld_inf, struct stats_data_struct, 1);
					bld_inf->vnum = vnum;
					HASH_ADD_INT(global_building_count, vnum, bld_inf);
				}
				last_bld_vnum = vnum;
			}
			
			++bld_inf->count;
		}
	}
	
	last_world_count = time(0);
}
