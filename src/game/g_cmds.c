/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * Game command processing.
 *
 * =======================================================================
 */

#include "header/local.h"
#include "monster/misc/player.h"
static char *
ClientTeam(edict_t *ent, char* value)
{
	char *p;

	value[0] = 0;

	if (!ent)
	{
		return value;
	}

	if (!ent->client)
	{
		return value;
	}

	strcpy(value, Info_ValueForKey(ent->client->pers.userinfo, "skin"));
	p = strchr(value, '/');

	if (!p)
	{
		return value;
	}

	if ((int)(dmflags->value) & DF_MODELTEAMS)
	{
		*p = 0;
		return value;
	}

	return ++p;
}

qboolean
OnSameTeam(edict_t *ent1, edict_t *ent2)
{
	char ent1Team[512];
	char ent2Team[512];

	if (!ent1 || !ent2)
	{
		return false;
	}

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	{
		return false;
	}

	ClientTeam(ent1, ent1Team);
	ClientTeam(ent2, ent2Team);

	if (ent1Team[0] != '\0' && strcmp(ent1Team, ent2Team) == 0)
	{
		return true;
	}

	return false;
}

void
SelectNextItem(edict_t *ent, int itflags)
{
	gclient_t *cl;
	int i, index;
	gitem_t *it;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (cl->chase_target)
	{
		ChaseNext(ent);
		return;
	}

	/* scan  for the next valid one */
	for (i = 1; i <= MAX_ITEMS; i++)
	{
		index = (cl->pers.selected_item + i) % MAX_ITEMS;

		if (!cl->pers.inventory[index])
		{
			continue;
		}

		it = &itemlist[index];

		if (!it->use)
		{
			continue;
		}

		if (!(it->flags & itflags))
		{
			continue;
		}

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void
SelectPrevItem(edict_t *ent, int itflags)
{
	gclient_t *cl;
	int i, index;
	gitem_t *it;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (cl->chase_target)
	{
		ChasePrev(ent);
		return;
	}

	/* scan for the next valid one */
	for (i = 1; i <= MAX_ITEMS; i++)
	{
		index = (cl->pers.selected_item + MAX_ITEMS - i) % MAX_ITEMS;

		if (!cl->pers.inventory[index])
		{
			continue;
		}

		it = &itemlist[index];

		if (!it->use)
		{
			continue;
		}

		if (!(it->flags & itflags))
		{
			continue;
		}

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void
ValidateSelectedItem(edict_t *ent)
{
	gclient_t *cl;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (cl->pers.inventory[cl->pers.selected_item])
	{
		return; /* valid */
	}

	SelectNextItem(ent, -1);
}

/* ================================================================================= */

/*
 * Give items to a client
 */
void
Cmd_Give_f(edict_t *ent)
{
	char *name;
	gitem_t *it;
	int index;
	int i;
	qboolean give_all;
	edict_t *it_ent;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf( ent, PRINT_HIGH,
				"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	name = gi.args();

	if (Q_stricmp(name, "all") == 0)
	{
		give_all = true;
	}
	else
	{
		give_all = false;
	}

	if (give_all || (Q_stricmp(gi.argv(1), "health") == 0))
	{
		if (gi.argc() == 3)
		{
			ent->health = (int)strtol(gi.argv(2), (char **)NULL, 10);
			ent->health = ent->health < 1 ? 1 : ent->health;
		}
		else
		{
			ent->health = ent->max_health;
		}

		if (!give_all)
		{
			return;
		}
	}

	if (give_all || (Q_stricmp(name, "weapons") == 0))
	{
		for (i = 0; i < game.num_items; i++)
		{
			it = itemlist + i;

			if (!it->pickup)
			{
				continue;
			}

			if (!(it->flags & IT_WEAPON))
			{
				continue;
			}

			ent->client->pers.inventory[i] += 1;
		}

		if (!give_all)
		{
			return;
		}
	}

	if (give_all || (Q_stricmp(name, "ammo") == 0))
	{
		for (i = 0; i < game.num_items; i++)
		{
			it = itemlist + i;

			if (!it->pickup)
			{
				continue;
			}

			if (!(it->flags & IT_AMMO))
			{
				continue;
			}

			Add_Ammo(ent, it, 1000);
		}

		if (!give_all)
		{
			return;
		}
	}

	if (give_all || (Q_stricmp(name, "armor") == 0))
	{
		gitem_armor_t *info;

		it = FindItem("Jacket Armor");
		ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

		it = FindItem("Combat Armor");
		ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

		it = FindItem("Body Armor");
		info = (gitem_armor_t *)it->info;
		ent->client->pers.inventory[ITEM_INDEX(it)] = info->max_count;

		if (!give_all)
		{
			return;
		}
	}

	if (give_all || (Q_stricmp(name, "Power Shield") == 0))
	{
		it = FindItem("Power Shield");
		it_ent = G_Spawn();
		it_ent->classname = it->classname;
		SpawnItem(it_ent, it);
		Touch_Item(it_ent, ent, NULL, NULL);

		if (it_ent->inuse)
		{
			G_FreeEdict(it_ent);
		}

		if (!give_all)
		{
			return;
		}
	}

	if (give_all)
	{
		for (i = 0; i < game.num_items; i++)
		{
			it = itemlist + i;

			if (!it->pickup)
			{
				continue;
			}

			if (it->flags & (IT_ARMOR | IT_WEAPON | IT_AMMO))
			{
				continue;
			}

			ent->client->pers.inventory[i] = 1;
		}

		return;
	}

	it = FindItem(name);

	if (!it)
	{
		name = gi.argv(1);
		it = FindItem(name);

		if (!it)
		{
			gi.cprintf(ent, PRINT_HIGH, "unknown item\n");
			return;
		}
	}

	if (!it->pickup)
	{
		gi.cprintf(ent, PRINT_HIGH, "non-pickup item\n");
		return;
	}

	index = ITEM_INDEX(it);

	if (it->flags & IT_AMMO)
	{
		if (gi.argc() == 3)
		{
			ent->client->pers.inventory[index] = (int)strtol(gi.argv(2), (char **)NULL, 10);
		}
		else
		{
			ent->client->pers.inventory[index] += it->quantity;
		}
	}
	else
	{
		it_ent = G_Spawn();
		it_ent->classname = it->classname;
		SpawnItem(it_ent, it);
		Touch_Item(it_ent, ent, NULL, NULL);

		if (it_ent->inuse)
		{
			G_FreeEdict(it_ent);
		}
	}
}

/*
 * Sets client to godmode
 */
void
Cmd_God_f(edict_t *ent)
{
	char *msg;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf( ent, PRINT_HIGH,
				"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_GODMODE;

	if (!(ent->flags & FL_GODMODE))
	{
		msg = "godmode OFF\n";
	}
	else
	{
		msg = "godmode ON\n";
	}

	gi.cprintf(ent, PRINT_HIGH, msg);
}

/*
 * Sets client to notarget
 */
void
Cmd_Notarget_f(edict_t *ent)
{
	char *msg;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf( ent, PRINT_HIGH,
				"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_NOTARGET;

	if (!(ent->flags & FL_NOTARGET))
	{
		msg = "notarget OFF\n";
	}
	else
	{
		msg = "notarget ON\n";
	}

	gi.cprintf(ent, PRINT_HIGH, msg);
}

/*
 * argv(0) noclip
 */
void
Cmd_Noclip_f(edict_t *ent)
{
	char *msg;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf( ent, PRINT_HIGH,
				"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (ent->movetype == MOVETYPE_NOCLIP)
	{
		ent->movetype = MOVETYPE_WALK;
		msg = "noclip OFF\n";
	}
	else
	{
		ent->movetype = MOVETYPE_NOCLIP;
		msg = "noclip ON\n";
	}

	gi.cprintf(ent, PRINT_HIGH, msg);
}

/*
 * Use an inventory item
 */
void
Cmd_Use_f(edict_t *ent)
{
	int index;
	gitem_t *it;
	char *s;

	if (!ent)
	{
		return;
	}

	s = gi.args();
	it = FindItem(s);

	if (!it)
	{
		gi.cprintf(ent, PRINT_HIGH, "unknown item: %s\n", s);
		return;
	}

	if (!it->use)
	{
		gi.cprintf(ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}

	index = ITEM_INDEX(it);

	if (!ent->client->pers.inventory[index])
	{
		gi.cprintf(ent, PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	it->use(ent, it);
}

/*
 * Drop an inventory item
 */
void
Cmd_Drop_f(edict_t *ent)
{
	int index;
	gitem_t *it;
	char *s;

	if (!ent)
	{
		return;
	}

	s = gi.args();
	it = FindItem(s);

	if (!it)
	{
		gi.cprintf(ent, PRINT_HIGH, "unknown item: %s\n", s);
		return;
	}

	if (!it->drop)
	{
		gi.cprintf(ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}

	index = ITEM_INDEX(it);

	if (!ent->client->pers.inventory[index])
	{
		gi.cprintf(ent, PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	it->drop(ent, it);
}

void
Cmd_Score_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	ent->client->showinventory = false;
	ent->client->showhelp = false;

	if (!deathmatch->value && !coop->value)
	{
		return;
	}

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboardMessage(ent, ent->enemy);
	gi.unicast(ent, true);
}

void
Cmd_Help_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	/* this is for backwards compatibility */
	if (deathmatch->value)
	{
		Cmd_Score_f(ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if (ent->client->showhelp)
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;
	HelpComputerMessage(ent);
	gi.unicast(ent, true);
}

void
Cmd_Help2_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	/* this is for backwards compatibility */
	if (deathmatch->value)
	{
		Cmd_Score_f(ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if (ent->client->showhelp2)
	{
		ent->client->showhelp2 = false;
		return;
	}

	ent->client->showhelp2 = true;
	CustomHelpMessage(ent,"THIS IS A TEST");
	gi.unicast(ent, true);
}

void
Cmd_Inven_f(edict_t *ent)
{
	gclient_t *cl;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	cl->showscores = false;
	cl->showhelp = false;

	if (cl->showinventory)
	{
		cl->showinventory = false;
		return;
	}

	cl->showinventory = true;

	InventoryMessage(ent);
	gi.unicast(ent, true);
}

void
Cmd_InvUse_f(edict_t *ent)
{
	gitem_t *it;

	if (!ent)
	{
		return;
	}

	ValidateSelectedItem(ent);

	if (ent->client->pers.selected_item == -1)
	{
		gi.cprintf(ent, PRINT_HIGH, "No item to use.\n");
		return;
	}

	it = &itemlist[ent->client->pers.selected_item];

	if (!it->use)
	{
		gi.cprintf(ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}

	it->use(ent, it);
}

void
Cmd_WeapPrev_f(edict_t *ent)
{
	gclient_t *cl;
	int i, index;
	gitem_t *it;
	int selected_weapon;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (!cl->pers.weapon)
	{
		return;
	}

	selected_weapon = ITEM_INDEX(cl->pers.weapon);

	/* scan  for the next valid one */
	for (i = 1; i <= MAX_ITEMS; i++)
	{
		index = (selected_weapon + i) % MAX_ITEMS;

		if (!cl->pers.inventory[index])
		{
			continue;
		}

		it = &itemlist[index];

		if (!it->use)
		{
			continue;
		}

		if (!(it->flags & IT_WEAPON))
		{
			continue;
		}

		it->use(ent, it);

		if (cl->pers.weapon == it)
		{
			return; /* successful */
		}
	}
}

void
Cmd_WeapNext_f(edict_t *ent)
{
	gclient_t *cl;
	int i, index;
	gitem_t *it;
	int selected_weapon;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (!cl->pers.weapon)
	{
		return;
	}

	selected_weapon = ITEM_INDEX(cl->pers.weapon);

	/* scan  for the next valid one */
	for (i = 1; i <= MAX_ITEMS; i++)
	{
		index = (selected_weapon + MAX_ITEMS - i) % MAX_ITEMS;

		if (!cl->pers.inventory[index])
		{
			continue;
		}

		it = &itemlist[index];

		if (!it->use)
		{
			continue;
		}

		if (!(it->flags & IT_WEAPON))
		{
			continue;
		}

		it->use(ent, it);

		if (cl->pers.weapon == it)
		{
			return; /* successful */
		}
	}
}

void
Cmd_WeapLast_f(edict_t *ent)
{
	gclient_t *cl;
	int index;
	gitem_t *it;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (!cl->pers.weapon || !cl->pers.lastweapon)
	{
		return;
	}

	index = ITEM_INDEX(cl->pers.lastweapon);

	if (!cl->pers.inventory[index])
	{
		return;
	}

	it = &itemlist[index];

	if (!it->use)
	{
		return;
	}

	if (!(it->flags & IT_WEAPON))
	{
		return;
	}

	it->use(ent, it);
}

void
Cmd_InvDrop_f(edict_t *ent)
{
	gitem_t *it;

	if (!ent)
	{
		return;
	}

	ValidateSelectedItem(ent);

	if (ent->client->pers.selected_item == -1)
	{
		gi.cprintf(ent, PRINT_HIGH, "No item to drop.\n");
		return;
	}

	it = &itemlist[ent->client->pers.selected_item];

	if (!it->drop)
	{
		gi.cprintf(ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}

	it->drop(ent, it);
}

void
Cmd_Kill_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	if (((level.time - ent->client->respawn_time) < 5) ||
		(ent->client->resp.spectator))
	{
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die(ent, ent, ent, 100000, vec3_origin);
}

void
Cmd_PutAway_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showinventory = false;
}

int
PlayerSort(void const *a, void const *b)
{
	int anum, bnum;

	if (!a || !b)
	{
		return 0;
	}

	anum = *(int *)a;
	bnum = *(int *)b;

	anum = game.clients[anum].ps.stats[STAT_FRAGS];
	bnum = game.clients[bnum].ps.stats[STAT_FRAGS];

	if (anum < bnum)
	{
		return -1;
	}

	if (anum > bnum)
	{
		return 1;
	}

	return 0;
}

void
Cmd_Players_f(edict_t *ent)
{
	int i;
	int count;
	char small[64];
	char large[1280];
	int index[256];

	if (!ent)
	{
		return;
	}

	count = 0;

	for (i = 0; i < maxclients->value; i++)
	{
		if (game.clients[i].pers.connected)
		{
			index[count] = i;
			count++;
		}
	}

	/* sort by frags */
	qsort(index, count, sizeof(index[0]), PlayerSort);

	/* print information */
	large[0] = 0;

	for (i = 0; i < count; i++)
	{
		Com_sprintf(small, sizeof(small), "%3i %s\n",
				game.clients[index[i]].ps.stats[STAT_FRAGS],
				game.clients[index[i]].pers.netname);

		if (strlen(small) + strlen(large) > sizeof(large) - 100)
		{
			/* can't print all of them in one packet */
			strcat(large, "...\n");
			break;
		}

		strcat(large, small);
	}

	gi.cprintf(ent, PRINT_HIGH, "%s\n%i players\n", large, count);
}

void
Cmd_Wave_f(edict_t *ent)
{
	int i;

	if (!ent)
	{
		return;
	}

	i = (int)strtol(gi.argv(1), (char **)NULL, 10);

	/* can't wave when ducked */
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		return;
	}

	if (ent->client->anim_priority > ANIM_WAVE)
	{
		return;
	}

	ent->client->anim_priority = ANIM_WAVE;

	switch (i)
	{
		case 0:
			gi.cprintf(ent, PRINT_HIGH, "flipoff\n");
			ent->s.frame = FRAME_flip01 - 1;
			ent->client->anim_end = FRAME_flip12;
			break;
		case 1:
			gi.cprintf(ent, PRINT_HIGH, "salute\n");
			ent->s.frame = FRAME_salute01 - 1;
			ent->client->anim_end = FRAME_salute11;
			break;
		case 2:
			gi.cprintf(ent, PRINT_HIGH, "taunt\n");
			ent->s.frame = FRAME_taunt01 - 1;
			ent->client->anim_end = FRAME_taunt17;
			break;
		case 3:
			gi.cprintf(ent, PRINT_HIGH, "wave\n");
			ent->s.frame = FRAME_wave01 - 1;
			ent->client->anim_end = FRAME_wave11;
			break;
		case 4:
		default:
			gi.cprintf(ent, PRINT_HIGH, "point\n");
			ent->s.frame = FRAME_point01 - 1;
			ent->client->anim_end = FRAME_point12;
			break;
	}
}

static qboolean
flooded(edict_t *ent)
{
	gclient_t *cl;
	int i;
	int num_msgs;
	int mx;

	if (!ent)
	{
		return false;
	}

	if (!deathmatch->value && !coop->value)
	{
		return false;
	}

	num_msgs = flood_msgs->value;
	if (num_msgs <= 0)
	{
		return false;
	}

	cl = ent->client;
	mx = sizeof(cl->flood_when) / sizeof(cl->flood_when[0]);

	if (num_msgs > mx)
	{
		gi.dprintf("flood_msgs lowered to max: 10\n");

		num_msgs = mx;
		gi.cvar_forceset("flood_msgs", "10");
	}

	if (level.time < cl->flood_locktill)
	{
		gi.cprintf(ent, PRINT_HIGH, "You can't talk for %d more seconds\n",
			(int)(cl->flood_locktill - level.time));

		return true;
	}

	i = (cl->flood_whenhead - num_msgs) + 1;

	if (i < 0)
	{
		i += mx;
	}

	if (cl->flood_when[i] &&
		(level.time - cl->flood_when[i]) < flood_persecond->value)
	{
		cl->flood_locktill = level.time + flood_waitdelay->value;

		gi.cprintf(ent, PRINT_CHAT,
			"Flood protection: You can't talk for %d seconds.\n",
			(int)flood_waitdelay->value);

		return true;
	}

	cl->flood_whenhead = (cl->flood_whenhead + 1) % mx;
	cl->flood_when[cl->flood_whenhead] = level.time;

	return false;
}

void
Cmd_Say_f(edict_t *ent, qboolean team, qboolean arg0)
{
	int j;
	edict_t *other;
	char *p;
	char text[2048];

	if (!ent)
	{
		return;
	}

	if ((gi.argc() < 2) && !arg0)
	{
		return;
	}

	if (flooded(ent))
	{
		return;
	}

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	{
		team = false;
	}

	if (team)
	{
		Com_sprintf(text, sizeof(text), "(%s): ", ent->client->pers.netname);
	}
	else
	{
		Com_sprintf(text, sizeof(text), "%s: ", ent->client->pers.netname);
	}

	if (arg0)
	{
		strcat(text, gi.argv(0));
		strcat(text, " ");
		strcat(text, gi.args());
	}
	else
	{
		p = gi.args();

		if (*p == '"')
		{
			p++;
			p[strlen(p) - 1] = 0;
		}

		strcat(text, p);
	}

	/* don't let text be too long for malicious reasons */
	if (strlen(text) > 150)
	{
		text[150] = 0;
	}

	strcat(text, "\n");

	if (dedicated->value)
	{
		gi.cprintf(NULL, PRINT_CHAT, "%s", text);
	}

	for (j = 1; j <= game.maxclients; j++)
	{
		other = &g_edicts[j];

		if (!other->inuse)
		{
			continue;
		}

		if (!other->client)
		{
			continue;
		}

		if (team)
		{
			if (!OnSameTeam(ent, other))
			{
				continue;
			}
		}

		gi.cprintf(other, PRINT_CHAT, "%s", text);
	}
}

void
Cmd_PlayerList_f(edict_t *ent)
{
	int i;
	char st[80];
	char text[1400];
	edict_t *e2;

	if (!ent)
	{
		return;
	}

	/* connect time, ping, score, name */
	*text = 0;

	for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++)
	{
		if (!e2->inuse)
		{
			continue;
		}

		Com_sprintf(st, sizeof(st), "%02d:%02d %4d %3d %s%s\n",
				(level.framenum - e2->client->resp.enterframe) / 600,
				((level.framenum - e2->client->resp.enterframe) % 600) / 10,
				e2->client->ping,
				e2->client->resp.score,
				e2->client->pers.netname,
				e2->client->resp.spectator ? " (spectator)" : "");

		if (strlen(text) + strlen(st) > sizeof(text) - 50)
		{
			strcpy(text + strlen(text), "And more...\n");
			gi.cprintf(ent, PRINT_HIGH, "%s", text);
			return;
		}

		strcat(text, st);
	}

	gi.cprintf(ent, PRINT_HIGH, "%s", text);
}

static void
Cmd_Teleport_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf(ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (gi.argc() != 4)
	{
		gi.cprintf(ent, PRINT_HIGH, "Usage: teleport x y z\n");
		return;
	}

	/* Unlink it to prevent unwanted interactions with
	   other entities. This works because linkentity()
	   uses the first available slot and the player is
	   always at postion 0. */
	gi.unlinkentity(ent);

	/* Set new position */
	ent->s.origin[0] = atof(gi.argv(1));
	ent->s.origin[1] = atof(gi.argv(2));
	ent->s.origin[2] = atof(gi.argv(3)) + 10.0;

	/* Remove velocity and keep the entity briefly in place
	   to give the server and clients time to catch up. */
	VectorClear(ent->velocity);
	ent->client->ps.pmove.pm_time = 20;
	ent->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT;

	/* Remove viewangles. They'll be recalculated
	   by the client at the next frame. */
	VectorClear(ent->s.angles);
	VectorClear(ent->client->ps.viewangles);
	VectorClear(ent->client->v_angle);

	/* Telefrag everything that's in the target location. */
	KillBox(ent);

	/* And link it back in. */
	gi.linkentity(ent);
}

static void
Cmd_SpawnEntity_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf(ent, PRINT_HIGH,
			"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (gi.argc() < 5 || gi.argc() > 9)
	{
		gi.cprintf(ent, PRINT_HIGH,
			"Usage: spawnentity classname x y z <angle_x angle_y angle_z> <flags>\n");
		return;
	}

	ent = G_Spawn();

	// set position
	ent->s.origin[0] = atof(gi.argv(2));
	ent->s.origin[1] = atof(gi.argv(3));
	ent->s.origin[2] = atof(gi.argv(4));
	// angles
	if (gi.argc() >= 8)
	{
		ent->s.angles[0] = atof(gi.argv(5));
		ent->s.angles[1] = atof(gi.argv(6));
		ent->s.angles[2] = atof(gi.argv(7));
	}
	// flags
	if (gi.argc() >= 9)
	{
		ent->spawnflags = atoi(gi.argv(8));
	}

	ent->classname = G_CopyString(gi.argv(1));

	ED_CallSpawn(ent);
}

static void
Cmd_SpawnOnStartByClass(char *classname, const vec3_t origin)
{
	edict_t *opponent = G_Spawn();

	// set position
	opponent->s.origin[0] = origin[0];
	opponent->s.origin[1] = origin[1];
	opponent->s.origin[2] = origin[2];
	// and class
	opponent->classname = G_CopyString(classname);

	ED_CallSpawn(opponent);

	gi.dprintf("Spawned entity at %f %f %f\n",
		origin[0], origin[1], origin[2]);
}

static void
Cmd_SpawnOnStart_f(edict_t *ent)
{
	edict_t *cur = NULL;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf(ent, PRINT_HIGH,
			"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (gi.argc() != 2)
	{
		gi.cprintf(ent, PRINT_HIGH, "Usage: spawnonstart classname\n");
		return;
	}

	while ((cur = G_Find(cur, FOFS(classname),
		"info_player_deathmatch")) != NULL)
	{
		Cmd_SpawnOnStartByClass(gi.argv(1), cur->s.origin);
	}

	while ((cur = G_Find(cur, FOFS(classname),
		"info_player_coop")) != NULL)
	{
		Cmd_SpawnOnStartByClass(gi.argv(1), cur->s.origin);
	}

	while ((cur = G_Find(cur, FOFS(classname),
		"info_player_start")) != NULL)
	{
		Cmd_SpawnOnStartByClass(gi.argv(1), cur->s.origin);
	}
}

static void
Cmd_ListEntities_f(edict_t *ent)
{
	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf(ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (gi.argc() < 2)
	{
		gi.cprintf(ent, PRINT_HIGH, "Usage: listentities <all|ammo|items|keys|monsters|weapons>\n");
		return;
	}

	/* What to print? */
	qboolean all = false;
	qboolean ammo = false;
	qboolean items = false;
	qboolean keys = false;
	qboolean monsters = false;
	qboolean weapons = false;

	for (int i = 1; i < gi.argc(); i++)
	{
		const char *arg = gi.argv(i);

		if (Q_stricmp(arg, "all") == 0)
		{
			all = true;
		}
		else if (Q_stricmp(arg, "ammo") == 0)
		{
			ammo = true;
		}
		else if (Q_stricmp(arg, "items") == 0)
		{
			items = true;
		}
		else if (Q_stricmp(arg, "keys") == 0)
		{
			keys = true;
		}
		else if (Q_stricmp(arg, "monsters") == 0)
		{
			monsters = true;
		}
		else if (Q_stricmp(arg, "weapons") == 0)
		{
			weapons = true;
		}
		else
		{
			gi.cprintf(ent, PRINT_HIGH, "Usage: listentities <all|ammo|items|keys|monsters|weapons>\n");
		}
	}

	/* Print what's requested. */
	for (int i = 0; i < globals.num_edicts; i++)
	{
		edict_t *cur = &g_edicts[i];
		qboolean print = false;

		/* Ensure that the entity is valid. */
		if (!cur->classname)
		{
			continue;
		}

		if (all)
		{
			print = true;
		}
		else
		{
			if (ammo)
			{
				if (strncmp(cur->classname, "ammo_", 5) == 0)
				{
					print = true;
				}
			}

			if (items)
			{
				if (strncmp(cur->classname, "item_", 5) == 0)
				{
					print = true;
				}
			}

			if (keys)
			{
				if (strncmp(cur->classname, "key_", 4) == 0)
				{
					print = true;
				}
			}

			if (monsters)
			{
				if (strncmp(cur->classname, "monster_", 8) == 0)
				{
					print = true;
				}
			}

			if (weapons)
			{
				if (strncmp(cur->classname, "weapon_", 7) == 0)
				{
					print = true;
				}
			}
		}

		if (print)
		{
			/* We use dprintf() because cprintf() may flood the server... */
			gi.dprintf("%s: %f %f %f\n", cur->classname, cur->s.origin[0], cur->s.origin[1], cur->s.origin[2]);
		}
	}
}

static int
get_ammo_usage(gitem_t *weap)
{
	if (!weap)
	{
		return 0;
	}

	/* handles grenades and tesla which only use 1 ammo per shot */
	/* have to check this because they don't store their ammo usage in weap->quantity */
	if (weap->flags & IT_AMMO)
	{
		return 1;
	}

	/* weapons store their ammo usage in the quantity field */
	return weap->quantity;
}

static gitem_t *
cycle_weapon(edict_t *ent)
{
	gclient_t *cl;
	gitem_t *noammo_fallback;
	gitem_t *noweap_fallback;
	gitem_t *weap;
	gitem_t *ammo;
	int i;
	int start;
	int num_weaps;
	const char *weapname = NULL;

	if (!ent)
	{
		return NULL;
	}

	cl = ent->client;

	if (!cl)
	{
		return NULL;
	}

	num_weaps = gi.argc();

	/* find where we want to start the search for the next eligible weapon */
	if (cl->newweapon)
	{
		weapname = cl->newweapon->classname;
	}
	else if (cl->pers.weapon)
	{
		weapname = cl->pers.weapon->classname;
	}

	if (weapname)
	{
		for (i = 1; i < num_weaps; i++)
		{
			if (Q_stricmp(weapname, gi.argv(i)) == 0)
			{
				break;
			}
		}

		i++;

		if (i >= num_weaps)
		{
			i = 1;
		}
	}
	else
	{
		i = 1;
	}

	start = i;
	noammo_fallback = NULL;
	noweap_fallback = NULL;

	/* find the first eligible weapon in the list we can switch to */
	do
	{
		weap = FindItemByClassname(gi.argv(i));

		if (weap && weap != cl->pers.weapon && (weap->flags & IT_WEAPON) && weap->use)
		{
			if (cl->pers.inventory[ITEM_INDEX(weap)] > 0)
			{
				if (weap->ammo)
				{
					ammo = FindItem(weap->ammo);
					if (ammo)
					{
						if (cl->pers.inventory[ITEM_INDEX(ammo)] >= get_ammo_usage(weap))
						{
							return weap;
						}

						if (!noammo_fallback)
						{
							noammo_fallback = weap;
						}
					}
				}
				else
				{
					return weap;
				}
			}
			else if (!noweap_fallback)
			{
				noweap_fallback = weap;
			}
		}

		i++;

		if (i >= num_weaps)
		{
			i = 1;
		}
	} while (i != start);

	/* if no weapon was found, the fallbacks will be used for
	   printing the appropriate error message to the console
	*/

	if (noammo_fallback)
	{
		return noammo_fallback;
	}

	return noweap_fallback;
}

static void
Cmd_CycleWeap_f(edict_t *ent)
{
	gitem_t *weap;

	if (!ent)
	{
		return;
	}

	if (gi.argc() <= 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "Usage: cycleweap classname1 classname2 .. classnameN\n");
		return;
	}

	weap = cycle_weapon(ent);
	if (weap)
	{
		if (ent->client->pers.inventory[ITEM_INDEX(weap)] <= 0)
		{
			gi.cprintf(ent, PRINT_HIGH, "Out of item: %s\n", weap->pickup_name);
		}
		else
		{
			weap->use(ent, weap);
		}
	}
}

static gitem_t *
preferred_weapon(edict_t *ent)
{
	gclient_t *cl;
	gitem_t *noammo_fallback;
	gitem_t *noweap_fallback;
	gitem_t *weap;
	gitem_t *ammo;
	int i;
	int num_weaps;

	if (!ent)
	{
		return NULL;
	}

	cl = ent->client;

	if (!cl)
	{
		return NULL;
	}

	num_weaps = gi.argc();
	noammo_fallback = NULL;
	noweap_fallback = NULL;

	/* find the first eligible weapon in the list we can switch to */
	for (i = 1; i < num_weaps; i++)
	{
		weap = FindItemByClassname(gi.argv(i));

		if (weap && (weap->flags & IT_WEAPON) && weap->use)
		{
			if (cl->pers.inventory[ITEM_INDEX(weap)] > 0)
			{
				if (weap->ammo)
				{
					ammo = FindItem(weap->ammo);
					if (ammo)
					{
						if (cl->pers.inventory[ITEM_INDEX(ammo)] >= get_ammo_usage(weap))
						{
							return weap;
						}

						if (!noammo_fallback)
						{
							noammo_fallback = weap;
						}
					}
				}
				else
				{
					return weap;
				}
			}
			else if (!noweap_fallback)
			{
				noweap_fallback = weap;
			}
		}
	}

	/* if no weapon was found, the fallbacks will be used for
	   printing the appropriate error message to the console
	*/

	if (noammo_fallback)
	{
		return noammo_fallback;
	}

	return noweap_fallback;
}

static void
Cmd_PrefWeap_f(edict_t *ent)
{
	gitem_t *weap;

	if (!ent)
	{
		return;
	}

	if (gi.argc() <= 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "Usage: prefweap classname1 classname2 .. classnameN\n");
		return;
	}

	weap = preferred_weapon(ent);
	if (weap)
	{
		if (ent->client->pers.inventory[ITEM_INDEX(weap)] <= 0)
		{
			gi.cprintf(ent, PRINT_HIGH, "Out of item: %s\n", weap->pickup_name);
		}
		else
		{
			weap->use(ent, weap);
		}
	}
}

void
ClientCommand(edict_t *ent)
{
	char *cmd;

	if (!ent)
	{
		return;
	}

	if (!ent->client)
	{
		return; /* not fully in game yet */
	}

	cmd = gi.argv(0);

	if (Q_stricmp(cmd, "players") == 0)
	{
		Cmd_Players_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "say") == 0)
	{
		Cmd_Say_f(ent, false, false);
		return;
	}

	if (Q_stricmp(cmd, "say_team") == 0)
	{
		Cmd_Say_f(ent, true, false);
		return;
	}

	if (Q_stricmp(cmd, "score") == 0)
	{
		Cmd_Score_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "help") == 0)
	{
		Cmd_Help_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "help2") == 0)
	{
		Cmd_Help2_f(ent);
		return;
	}

	if (level.intermissiontime)
	{
		return;
	}

	if (Q_stricmp(cmd, "use") == 0)
	{
		Cmd_Use_f(ent);
	}
	else if (Q_stricmp(cmd, "drop") == 0)
	{
		Cmd_Drop_f(ent);
	}
	else if (Q_stricmp(cmd, "give") == 0)
	{
		Cmd_Give_f(ent);
	}
	else if (Q_stricmp(cmd, "god") == 0)
	{
		Cmd_God_f(ent);
	}
	else if (Q_stricmp(cmd, "notarget") == 0)
	{
		Cmd_Notarget_f(ent);
	}
	else if (Q_stricmp(cmd, "noclip") == 0)
	{
		Cmd_Noclip_f(ent);
	}
	else if (Q_stricmp(cmd, "inven") == 0)
	{
		Cmd_Inven_f(ent);
	}
	else if (Q_stricmp(cmd, "invnext") == 0)
	{
		SelectNextItem(ent, -1);
	}
	else if (Q_stricmp(cmd, "invprev") == 0)
	{
		SelectPrevItem(ent, -1);
	}
	else if (Q_stricmp(cmd, "invnextw") == 0)
	{
		SelectNextItem(ent, IT_WEAPON);
	}
	else if (Q_stricmp(cmd, "invprevw") == 0)
	{
		SelectPrevItem(ent, IT_WEAPON);
	}
	else if (Q_stricmp(cmd, "invnextp") == 0)
	{
		SelectNextItem(ent, IT_POWERUP);
	}
	else if (Q_stricmp(cmd, "invprevp") == 0)
	{
		SelectPrevItem(ent, IT_POWERUP);
	}
	else if (Q_stricmp(cmd, "invuse") == 0)
	{
		Cmd_InvUse_f(ent);
	}
	else if (Q_stricmp(cmd, "invdrop") == 0)
	{
		Cmd_InvDrop_f(ent);
	}
	else if (Q_stricmp(cmd, "weapprev") == 0)
	{
		Cmd_WeapPrev_f(ent);
	}
	else if (Q_stricmp(cmd, "weapnext") == 0)
	{
		Cmd_WeapNext_f(ent);
	}
	else if (Q_stricmp(cmd, "weaplast") == 0)
	{
		Cmd_WeapLast_f(ent);
	}
	else if (Q_stricmp(cmd, "kill") == 0)
	{
		Cmd_Kill_f(ent);
	}
	else if (Q_stricmp(cmd, "putaway") == 0)
	{
		Cmd_PutAway_f(ent);
	}
	else if (Q_stricmp(cmd, "wave") == 0)
	{
		Cmd_Wave_f(ent);
	}
	else if (Q_stricmp(cmd, "chasecam") == 0)
	{
		Cmd_Chasecam_Toggle(ent);
		ent->client->ps.gunindex = 0;
		return;
	}
	else if (Q_stricmp(cmd, "playerlist") == 0)
	{
		Cmd_PlayerList_f(ent);
	}
	else if (Q_stricmp(cmd, "teleport") == 0)
	{
		Cmd_Teleport_f(ent);
	}
	else if (Q_stricmp(cmd, "spawnentity") == 0)
	{
		Cmd_SpawnEntity_f(ent);
	}
	else if (Q_stricmp(cmd, "spawnonstart") == 0)
	{
		Cmd_SpawnOnStart_f(ent);
	}
	else if (Q_stricmp(cmd, "listentities") == 0)
	{
		Cmd_ListEntities_f(ent);
	}
	else if (Q_stricmp(cmd, "cycleweap") == 0)
	{
		Cmd_CycleWeap_f(ent);
	}
	else if (Q_stricmp(cmd, "prefweap") == 0)
	{
		Cmd_PrefWeap_f(ent);
	}
	else /* anything that doesn't match a command will be a chat */
	{
		Cmd_Say_f(ent, false, true);
	}
}