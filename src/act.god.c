/* ************************************************************************
*   File: act.god.c                                       EmpireMUD 2.0b1 *
*  Usage: Player-level God commands                                       *
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
#include "comm.h"
#include "vnums.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "skills.h"
#include "dg_scripts.h"

/**
* Contents:
*   Helpers
*   Commands
*/


 //////////////////////////////////////////////////////////////////////////////
//// HELPERS /////////////////////////////////////////////////////////////////

/**
* Actual sacrifice of an item to a god.
*
* @param char_data *ch The person sacrificing something.
* @param char_data *god The got to sacrifice to.
* @param obj_data *obj The item to sacrifice.
* @param bool message If TRUE, sends messages.
* @return int The number of items sacrificed.
*/
static int perform_sacrifice(char_data *ch, char_data *god, obj_data *obj, bool message) {
	double bonus = 1.0;
	int num = 1;

	/* Determine monument bonus */
	if (ROOM_PATRON(IN_ROOM(ch)) == GET_IDNUM(god)) {
		bonus = 1.50;
	}

	// Oops, it was created -- no bonus, or else it could be used to increase one's own resources
	if (OBJ_FLAGGED(obj, OBJ_CREATED))
		bonus = 1;

	GET_RESOURCE(god, GET_OBJ_MATERIAL(obj)) += (int)((1 + rate_item(obj)) * bonus);

	if (message) {
		*buf2 = '\0';
		if (ch == god) {
			sprintf(buf, "You sacrifice $p to yourself!");
			sprintf(buf1, "$n sacrifices $p to $mself!");
		}
		else {
			sprintf(buf, "You sacrifice $p to %s!", PERS(god, god, 1));
			sprintf(buf1, "$n sacrifices $p to %s!", PERS(god, god, 1));
			sprintf(buf2, "$n sacrifices $p to you!");
		}
		act(buf, TRUE, ch, obj, god, TO_CHAR);
		act(buf1, TRUE, ch, obj, god, TO_NOTVICT);
		if (IN_ROOM(ch) == IN_ROOM(god) && ch != god)
			act(buf2, TRUE, ch, obj, god, TO_VICT);
	}
	while (obj->contains) {
		num += perform_sacrifice(ch, god, obj->contains, FALSE);
	}
	extract_obj(obj);

	return num;
}


 //////////////////////////////////////////////////////////////////////////////
//// COMMANDS ////////////////////////////////////////////////////////////////


ACMD(do_create) {
	void scale_item_to_level(obj_data *obj, int level);
	
	char *name;
	obj_data *proto, *obj = NULL;
	obj_data *obj_iter, *next_obj;
	int cost, iter, mat, num = 1, count = 0;
	bool low_res = FALSE;

	// create [number] <name list>
	name = one_argument(argument, arg);

	if (isdigit(*arg)) {
		num = atoi(arg);
		if (num < 1) {
			msg_to_char(ch, "You need to create at LEAST one.\r\n");
			return;
		}
	}
	else {
		// no number; use full argument
		name = argument;
	}

	// process name?
	skip_spaces(&name);
	if (!*name) {
		msg_to_char(ch, "What would you like to create?\r\n");
		return;
	}

	// look it up
	proto = NULL;
	HASH_ITER(hh, object_table, obj_iter, next_obj) {
		if (OBJ_FLAGGED(obj_iter, OBJ_CREATABLE) && multi_isname(name, GET_OBJ_KEYWORDS(obj_iter))) {
			proto = obj_iter;
			break;
		}
	}

	if (!proto) {
		msg_to_char(ch, "You can't create that.\r\n");
		return;
	}
	
	// setup
	count = 0;
	cost = rate_item(proto) + 1;
	mat = GET_OBJ_MATERIAL(proto);

	// make them!
	for (iter = 0; iter < num && !low_res; ++iter) {
		// imms have no need to have the resources (just gods)
		if (GET_RESOURCE(ch, mat) >= cost || IS_IMMORTAL(ch)) {
			++count;
			GET_RESOURCE(ch, mat) = MAX(0, GET_RESOURCE(ch, mat) - cost);
			obj = read_object(GET_OBJ_VNUM(proto));
			SET_BIT(GET_OBJ_EXTRA(obj), OBJ_CREATED);
			
			if (!CAN_WEAR(obj, ITEM_WEAR_TAKE)) {
				obj_to_room(obj, IN_ROOM(ch));
			}
			else {
				obj_to_char_or_room(obj, ch);
			}
			
			if (OBJ_FLAGGED(obj, OBJ_SCALABLE)) {
				scale_item_to_level(obj, 1);	// minimum level
			}
			
			load_otrigger(obj);
		}
		else {
			low_res = TRUE;
		}
	}

	if (count == 0) {
		msg_to_char(ch, "You fail to create it.\r\n");
		if (obj) {
			extract_obj(obj);
		}
	}
	else if (count == 1) {
		act("You create $p!", FALSE, ch, obj, 0, TO_CHAR);
		act("$n creates $p from thin air!", TRUE, ch, obj, 0, TO_ROOM);
	}
	else {
		sprintf(buf, "You create $p (x%d)!", count);
		sprintf(buf1, "$n creates $p (x%d) from thin air!", count);
		act(buf, FALSE, ch, obj, 0, TO_CHAR);
		act(buf1, TRUE, ch, obj, 0, TO_ROOM);
	}

	if (low_res) {
		msg_to_char(ch, "You have insufficient resources to create more.\r\n");
	}
}


ACMD(do_sacrifice) {
	extern FILE *player_fl;

	obj_data *obj, *next_obj;
	char_data *god = NULL, *cbuf = NULL;
	descriptor_data *d;
	int amount = 0, dotmode, player_i = 0;
	bool file = FALSE;
	struct char_file_u tmp_store;

	two_arguments(argument, arg, buf);

	if (!*arg || !*buf) {
		msg_to_char(ch, "What do you wish to sacrifice, and to whom?\r\n");
		return;
	}

	/* Do this in-file */
	for (d = descriptor_list; d; d = d->next) {
		if (STATE(d) != CON_PLAYING || !d->character)
			continue;
		if (!isname(buf, GET_NAME(d->character)))
			continue;
		god = d->character;
		break;
	}
	if (!god) {
		CREATE(cbuf, char_data, 1);
		clear_char(cbuf);
		if ((player_i = load_char(buf, &tmp_store)) > NOBODY) {
			store_to_char(&tmp_store, cbuf);
			god = cbuf;
			SET_BIT(PLR_FLAGS(god), PLR_KEEP_LAST_LOGIN_INFO);
			file = TRUE;
		}
		else {
			free(cbuf);
			send_to_char("There is no such god.\r\n", ch);
			return;
		}
	}

	if (!IS_GOD(god) && !IS_IMMORTAL(god)) {
		msg_to_char(ch, "You can only sacrifice to gods.\r\n");
		if (file)
			free_char(cbuf);
		return;
	}

	dotmode = find_all_dots(arg);

	if (dotmode == FIND_ALL) {
		msg_to_char(ch, "You can't just sacrifice everything, that's how accidents happen.\r\n");
		/*
		if (!ch->carrying && (!ROOM_CONTENTS(IN_ROOM(ch)) || !can_use_room(ch, IN_ROOM(ch), GUESTS_ALLOWED)))
			msg_to_char(ch, "You don't seem to have anything to sacrifice.\r\n");
		else {
			for (obj = ch->carrying; obj; obj = next_obj) {
				next_obj = obj->next_content;
				amount += perform_sacrifice(ch, god, obj, TRUE);
			}
			for (obj = ROOM_CONTENTS(IN_ROOM(ch)); obj; obj = next_obj) {
				next_obj = obj->next_content;

				if (!can_use_room(ch, IN_ROOM(ch), GUESTS_ALLOWED))
					continue;

				if (!CAN_WEAR(obj, ITEM_WEAR_TAKE))
					continue;

				amount += perform_sacrifice(ch, god, obj, TRUE);
			}
		}
		*/
	}
	else if (dotmode == FIND_ALLDOT) {
		if (!*arg) {
			msg_to_char(ch, "What do you want to sacrifice all of?\r\n");
			return;
		}
		if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying)))
			if (!can_use_room(ch, IN_ROOM(ch), GUESTS_ALLOWED) || !(obj = get_obj_in_list_vis(ch, arg, ROOM_CONTENTS(IN_ROOM(ch)))))
				msg_to_char(ch, "You don't seem to have any %ss to sacrifice.\r\n", arg);

		while (obj) {
			next_obj = get_obj_in_list_vis(ch, arg, obj->next_content);
			if (!next_obj && can_use_room(ch, IN_ROOM(ch), GUESTS_ALLOWED) && !IN_ROOM(obj))
				next_obj = get_obj_in_list_vis(ch, arg, ROOM_CONTENTS(IN_ROOM(ch)));
			amount += perform_sacrifice(ch, god, obj, TRUE);
			obj = next_obj;
		}
	}
	else {
		if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying)) && (!can_use_room(ch, IN_ROOM(ch), GUESTS_ALLOWED) || !(obj = get_obj_in_list_vis(ch, arg, ROOM_CONTENTS(IN_ROOM(ch))))))
			msg_to_char(ch, "You don't seem to have any %ss to sacrifice.\r\n", arg);
		else
			amount += perform_sacrifice(ch, god, obj, TRUE);
	}

	if (!file)
		SAVE_CHAR(god);
	if (file) {
		char_to_store(god, &tmp_store);
		fseek(player_fl, (player_i) * sizeof(struct char_file_u), SEEK_SET);
		fwrite(&tmp_store, sizeof(struct char_file_u), 1, player_fl);
	}

	if (file)
		free_char(cbuf);
}
