/* ************************************************************************
*   File: vnums.h                                         EmpireMUD 2.0b1 *
*  Usage: stores commonly-used virtual numbers                            *
*                                                                         *
*  EmpireMUD code base by Paul Clarke, (C) 2000-2015                      *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  EmpireMUD based upon CircleMUD 3.0, bpl 17, by Jeremy Elson.           *
*  CircleMUD (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

 //////////////////////////////////////////////////////////////////////////////
//// BUILDINGS ///////////////////////////////////////////////////////////////

// NOTE: many of these are used just for spawn data and if we could do that in
// file instead of code, these would not be needed

// TODO: guard towers could use a flag plus a damage config of some kind

#define BUILDING_RUINS_OPEN  5006	// custom icons and db.world.c
#define BUILDING_RUINS_CLOSED  5007	// custom icons and db.world.c
#define BUILDING_TUNNEL  5008  // building.c
#define BUILDING_CITY_CENTER  5009

#define BUILDING_FENCE  5112
#define BUILDING_GATE  5113

#define BUILDING_FOUNDRY  5115	// actions

#define BUILDING_LUMBER_YARD  5118	// actions

#define BUILDING_TANNERY  5120	// actions -- TODO should be a bld flag

#define BUILDING_QUARRY  5122	// actions
#define BUILDING_CLAY_PIT  5123	// actions

#define BUILDING_BRIDGE  5133
#define BUILDING_CARPENTER  5134	// trade.c	-- TODO could be bld flag

#define BUILDING_TAVERN  5138  // custom interior, drinking, etc

#define BUILDING_STEPS  5140	// custom icons

#define BUILDING_HENGE  5142	// spells.c

#define BUILDING_DOCKS  5152	// tech, custom icons, ships.c	-- TODO replace with BLD_DOCKS
#define BUILDING_SHIPYARD  5153	// custom icon, ships.c	-- TODO could be a bld flag
#define BUILDING_SHIPYARD2  5154	// custom icon, ships.c

#define BUILDING_SWAMPWALK  5155
#define BUILDING_SWAMP_PLATFORM  5156	// custom completion

#define BUILDING_TRAPPERS_POST  5161	// workforce.c

#define BUILDING_SORCERER_TOWER  5164  // custom interior

#define BUILDING_GUARD_TOWER  5167	// act.empire.c
#define BUILDING_GUARD_TOWER2  5168	// act.empire.c
#define BUILDING_GUARD_TOWER3  5169	// act.empire.c

#define BUILDING_GATEHOUSE  5173	// custom icons, rituals
#define BUILDING_WALL  5174

#define BUILDING_RIVER_GATE  5176  // custom icons, rituals


// Room building vnums
#define RTYPE_B_HELM  5500	// boat
#define RTYPE_B_ONDECK  5501	// boat
#define RTYPE_B_STORAGE  5502	// boat
#define RTYPE_B_BELOWDECK  5503	// boat
#define RTYPE_STEALTH_HIDEOUT  5510
#define RTYPE_SORCERER_TOWER  5511

#define RTYPE_BEDROOM  5601
#define RTYPE_STUDY  5608
#define RTYPE_TUNNEL  5612	// for mine complex, tunnel


 //////////////////////////////////////////////////////////////////////////////
//// OBJECTS /////////////////////////////////////////////////////////////////

// these consts can be removed as soon as their hard-coded functions are gone!

/* Boards */
#define BOARD_MORT  1
#define BOARD_IMM  2

// Natural resources
#define o_ROCK  100
#define o_CLAY  101
#define o_LIGHTNING_STONE  103
#define o_BLOODSTONE  104
#define o_STONE_BLOCK  105
#define o_TREE  120
#define o_STICK  121
#define o_FLOWER  123
#define o_LOG  124
#define o_LUMBER  125
#define o_WHEAT  141
#define o_HOPS  143
#define o_COTTON  144
#define o_BARLEY  145
#define o_IRON_ORE  160
#define o_SILVER  161
#define o_GOLD  162
#define o_NOCTURNIUM_ORE  163
#define o_IMPERIUM_ORE  164
#define o_COPPER  165
#define o_SILVER_DISC  170
#define o_GOLD_DISC  171
#define o_SILVER_BAR  172
#define o_GOLD_BAR  173
#define o_GOLD_SMALL  174
#define o_IRON_INGOT  175
#define o_NOCTURNIUM_INGOT  176
#define o_IMPERIUM_INGOT  177
#define o_COPPER_INGOT  178
#define o_COPPER_BAR  179
#define o_CHIPPED  180
#define o_HANDAXE  181
#define o_FLINT_SET  183
#define o_SPEARHEAD  186

// pickpocket items
#define o_MIRROR_SILVER  806
#define o_CANDLE  851
#define o_RING_SILVER  2120
#define o_APPLE  3001
#define o_NUTS  3013
#define o_BERRIES  3015
#define o_DATES  3016
#define o_FIG  3017

// brewing items
#define o_APPLES  3002
#define o_PEACHES  3004
#define o_CORN  3005

// clay
#define o_BOWL  250
#define o_HESTIAN_TRINKET  256
#define o_BRICKS  257
#define o_TRINKET_OF_CONVEYANCE  262

// Sewn items
#define o_ROPE  2035
#define o_TENT  2036

// forge
#define o_SHOVEL  2118
#define o_PAN  2119

// Wood crafts
#define o_STAKE  915	// could be a flag
#define o_BLANK_SIGN  918

// Ships
#define os_PINNACE  952
#define os_BRIGANTINE  953
#define os_GALLEY  954
#define os_ARGOSY  955
#define os_GALLEON  956

// core objects
#define o_CORPSE  1000
#define o_BOOK  1001
#define o_HOME_CHEST  1010

// Skill tree items
#define o_PORTAL  1100  // could probably safely generate a NOTHING item
#define o_BLOODSWORD_LOW  1101
#define o_BLOODSWORD_MEDIUM  1102
#define o_BLOODSWORD_HIGH  1103
#define o_BLOODSWORD_LEGENDARY  1104
#define o_FIREBALL  1105
#define o_IMPERIUM_SPIKE  1114

// herbs
#define o_WHITEGRASS  1200
#define o_FIVELEAF  1201
#define o_REDTHORN  1202
#define o_MAGEWHISPER  1203
#define o_DAGGERBITE  1204
#define o_BILEBERRIES  1205
#define o_IRIDESCENT_IRIS  1206

// misc stuff
#define o_GLOWING_SEASHELL  1300
#define o_BARBFISH  1301
#define o_LINEFISH  1302
#define o_ARROWFISH  1303
#define o_SEA_JUNK  1304
#define o_WORN_STATUETTE  1305
#define o_NAILS  1306

// skins and cloths
#define o_SMALL_SKIN  1350
#define o_LARGE_SKIN  1351
#define o_SMALL_LEATHER  1356
#define o_LARGE_LEATHER  1357
#define o_WOOL  1358
#define o_CLOTH  1359

// newbie gear
#define o_STAFF  1107
#define o_LEATHER_HOOD  2001
#define o_CLOTH_HOOD  2002
#define o_BRIMMED_CAP  2004
#define o_COIF  2006
#define o_SHIRT  2009
#define o_ROBE  2012
#define o_WAISTCOAT  2014
#define o_LEATHER_JACKET  2016
#define o_SPOTTED_ROBE  2017
#define o_PANTS  2027
#define o_BREECHES  2028
#define o_MOCCASINS  2031
#define o_SANDALS  2032
#define o_STONE_AXE  2100
#define o_WOODSMANS_AXE  2101
#define o_SHORT_SWORD  2102
#define o_SHIV  2103
#define o_NEWBIE_TORCH  2104
#define o_GRAVE_MARKER  2117
#define o_BREAD  3313


 //////////////////////////////////////////////////////////////////////////////
//// MOBS ////////////////////////////////////////////////////////////////////

// Various mobs used by the code

#define CHICKEN  9010
#define DOG  9012
#define QUAIL  9020

#define HUMAN_MALE_1  9029
#define HUMAN_MALE_2  9030
#define HUMAN_FEMALE_1  9031
#define HUMAN_FEMALE_2  9032

// High Sorcery
#define SWIFT_DEINONYCHUS  400
#define SWIFT_LIGER  401
#define SWIFT_SERPENT  402
#define SWIFT_STAG  403
#define SWIFT_BEAR  404
#define MIRROR_IMAGE_MOB  450

// Natural Magic
#define FAMILIAR_CAT  500
#define FAMILIAR_SABERTOOTH  501
#define FAMILIAR_SPHINX  502
#define FAMILIAR_GRIFFIN  503


// empire mobs
#define CITIZEN_MALE  202
#define CITIZEN_FEMALE  203
#define GUARD  204
#define BODYGUARD  205
#define FARMER  206
#define FELLER  207
#define MINER  208
#define BUILDER  209
#define STEALTH_MASTER  221
#define DIGGER  224
#define SCRAPER  227
#define REPAIRMAN  229
#define THUG  230
#define CITY_GUARD  251
#define SAWYER  256
#define SMELTER  257
#define WEAVER  258
#define STONECUTTER  259
#define NAILMAKER  260
#define BRICKMAKER  261
#define GARDENER  262
#define FIRE_BRIGADE  265
#define TRAPPER  266
#define TANNER  267
#define SHEARER  268
#define COIN_MAKER  269
#define DOCKWORKER  270
