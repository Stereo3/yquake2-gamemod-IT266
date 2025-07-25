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
 * HUD, deathmatch scoreboard, help computer and intermission stuff.
 *
 * =======================================================================
 */

#include "../header/local.h"

void
MoveClientToIntermission(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	if (deathmatch->value || coop->value)
	{
		ent->client->showscores = true;
	}

	VectorCopy(level.intermission_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0] * 8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1] * 8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2] * 8;
	VectorCopy(level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	/* clean up powerup info */
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->speed_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	gi.linkentity(ent);

	/* add the layout */
	if (deathmatch->value || coop->value)
	{
		DeathmatchScoreboardMessage(ent, NULL);
		gi.unicast(ent, true);
	}
}

void
BeginIntermission(edict_t *targ)
{
	int i;
	edict_t *ent;

	if (!targ)
	{
		return;
	}

	if (level.intermissiontime)
	{
		return; /* already activated */
	}

	game.autosaved = false;

	/* respawn any dead clients */
	for (i = 0; i < maxclients->value; i++)
	{
		edict_t *client;

		client = g_edicts + 1 + i;

		if (!client->inuse)
		{
			continue;
		}

		if (client->health <= 0)
		{
			respawn(client);
		}
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (coop->value)
		{
			for (i = 0; i < maxclients->value; i++)
			{
				int n;
				edict_t *client;

				client = g_edicts + 1 + i;

				if (!client->inuse)
				{
					continue;
				}

				/* strip players of all keys between units */
				for (n = 0; n < game.num_items; n++)
				{
					if (itemlist[n].flags & IT_KEY)
					{
						client->client->pers.inventory[n] = 0;
					}
				}

				client->client->pers.power_cubes = 0;
			}
		}
	}
	else
	{
		if (!deathmatch->value)
		{
			level.exitintermission = 1; /* go immediately to the next level */
			return;
		}
	}

	level.exitintermission = 0;

	/* find an intermission spot */
	ent = G_Find(NULL, FOFS(classname), "info_player_intermission");

	if (!ent)
	{
		/* the map creator forgot to put in an intermission point... */
		ent = G_Find(NULL, FOFS(classname), "info_player_start");

		if (!ent)
		{
			ent = G_Find(NULL, FOFS(classname), "info_player_deathmatch");
		}
	}
	else
	{
		/* chose one of four spots */
		i = randk() & 3;

		while (i--)
		{
			ent = G_Find(ent, FOFS(classname), "info_player_intermission");

			if (!ent) /* wrap around the list */
			{
				ent = G_Find(ent, FOFS(classname), "info_player_intermission");
			}
		}
	}

	VectorCopy(ent->s.origin, level.intermission_origin);
	VectorCopy(ent->s.angles, level.intermission_angle);

	/* In fact1 the intermission collides
	   with an area portal, resulting in
	   clutterings */
	if (!Q_stricmp(level.mapname, "fact1"))
	{
		level.intermission_origin[0] = 1037.0;
		level.intermission_origin[1] = 1100.0;
		level.intermission_origin[2] = 222.0;
	}

	/* move all clients to the intermission point */
	for (i = 0; i < maxclients->value; i++)
	{
		edict_t *client;

		client = g_edicts + 1 + i;

		if (!client->inuse)
		{
			continue;
		}

		MoveClientToIntermission(client);
	}
}

void
DeathmatchScoreboardMessage(edict_t *ent, edict_t *killer)
{
	char entry[1024];
	char string[1400];
	int stringlength;
	int i;
	int sorted[MAX_CLIENTS];
	int sortedscores[MAX_CLIENTS];
	int total;

	if (!ent) /* killer can be NULL */
	{
		return;
	}

	/* sort the clients by score */
	total = 0;

	for (i = 0; i < game.maxclients; i++)
	{
		int k, j, score;
		edict_t *cl_ent;

		cl_ent = g_edicts + 1 + i;

		if (!cl_ent->inuse || game.clients[i].resp.spectator)
		{
			continue;
		}

		score = game.clients[i].resp.score;

		for (j = 0; j < total; j++)
		{
			if (score > sortedscores[j])
			{
				break;
			}
		}

		for (k = total; k > j; k--)
		{
			sorted[k] = sorted[k - 1];
			sortedscores[k] = sortedscores[k - 1];
		}

		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	/* print level name and exit rules */
	string[0] = 0;

	stringlength = strlen(string);

	/* add the clients in sorted order */
	if (total > 12)
	{
		total = 12;
	}

	for (i = 0; i < total; i++)
	{
		char *tag;
		int x, y, j;
		gclient_t *cl;
		edict_t *cl_ent;

		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		x = (i >= 6) ? 160 : 0;
		y = 32 + 32 * (i % 6);

		/* add a dogtag */
		if (cl_ent == ent)
		{
			tag = "tag1";
		}
		else if (cl_ent == killer)
		{
			tag = "tag2";
		}
		else
		{
			tag = NULL;
		}

		if (tag)
		{
			Com_sprintf(entry, sizeof(entry),
					"xv %i yv %i picn %s ", x + 32, y, tag);
			j = strlen(entry);

			if (stringlength + j > 1024)
			{
				break;
			}

			strcpy(string + stringlength, entry);
			stringlength += j;
		}

		/* send the layout */
		Com_sprintf(entry, sizeof(entry),
				"client %i %i %i %i %i %i ",
				x, y, sorted[i], cl->resp.score, cl->ping,
				(level.framenum - cl->resp.enterframe) / 600);
		j = strlen(entry);

		if (stringlength + j > 1024)
		{
			break;
		}

		strcpy(string + stringlength, entry);
		stringlength += j;
	}

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
}

void
HelpComputerMessage(edict_t *ent)
{
	char string[1024];

	if (!ent)
	{
		return;
	}

	/* send the layout */
	Com_sprintf(string, sizeof(string),
			"xv 32 yv 8 picn help " /* background */
			"xv 202 yv 12 string2 \"MONKEY\" " /* skill */
			"xv 0 yv 24 cstring2 \"BANNANA LAND\" " /* level name */
			"xv 0 yv 54 cstring2 \"ROLL AROUND AND GET TO THE END\" " /* help 1 */
			"xv 0 yv 110 cstring2 \"COLLECT ALL THE BANNANAS\" " /* help 2 */
			"xv 50 yv 164 string2 \" BANNANAS    TIME    SECRETS\" "
			"xv 50 yv 172 string2 \"%3i/%3i     %i/%i       %i/%i\" ",
			level.killed_monsters, level.total_monsters,
			level.found_goals, level.total_goals,
			level.found_secrets, level.total_secrets);

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
}

void CustomHelpMessage(edict_t *ent, const char *customText)
{
    char string[1024];

    if (!ent || !customText)
    {
        return;
    }

    /* send the layout */
    Com_sprintf(string, sizeof(string),
                "xv 32 yv 8 picn help "          /* background */
                "xv 0 yv 24 cstring2 \"%s\" ",   /* custom text */
                customText);

    gi.WriteByte(svc_layout);
    gi.WriteString(string);
}




void
InventoryMessage(edict_t *ent)
{
        int i;

        if (!ent)
        {
                return;
        }

        gi.WriteByte(svc_inventory);

        for (i = 0; i < MAX_ITEMS; i++)
        {
                gi.WriteShort(ent->client->pers.inventory[i]);
        }
}


/* ======================================================================= */

void
G_SetStats(edict_t *ent)
{
	gitem_t *item;
	int index, cells = 0;
	int power_armor_type;

	if (!ent)
	{
		return;
	}

	/* health */
	ent->client->ps.stats[STAT_HEALTH_ICON] = level.pic_health;
	ent->client->ps.stats[STAT_HEALTH] = (ent->health < -99) ? -99 : ent->health;

	/* ammo */
	if (!ent->client->ammo_index)
	{
		ent->client->ps.stats[STAT_AMMO_ICON] = 0;
		ent->client->ps.stats[STAT_AMMO] = 0;
	}
	else
	{
		item = &itemlist[ent->client->ammo_index];
		ent->client->ps.stats[STAT_AMMO_ICON] = gi.imageindex(item->icon);
		ent->client->ps.stats[STAT_AMMO] =
			ent->client->pers.inventory[ent->client->ammo_index];
	}

	/* armor */
	power_armor_type = PowerArmorType(ent);

	if (power_armor_type)
	{
		cells = ent->client->pers.inventory[ITEM_INDEX(FindItem("cells"))];

		if (cells == 0)
		{
			/* ran out of cells for power armor */
			ent->flags &= ~FL_POWER_ARMOR;
			gi.sound(ent, CHAN_ITEM, gi.soundindex(
							"misc/power2.wav"), 1, ATTN_NORM, 0);
			power_armor_type = 0;
		}
	}

	index = ArmorIndex(ent);

	if (power_armor_type && (!index || (level.framenum & 8)))
	{
		/* flash between power armor and other armor icon */
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex("i_powershield");
		ent->client->ps.stats[STAT_ARMOR] = cells;
	}
	else if (index)
	{
		item = GetItemByIndex(index);
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex(item->icon);
		ent->client->ps.stats[STAT_ARMOR] = ent->client->pers.inventory[index];
	}
	else
	{
		ent->client->ps.stats[STAT_ARMOR_ICON] = 0;
		ent->client->ps.stats[STAT_ARMOR] = 0;
	}

	/* pickup message */
	if (level.time > ent->client->pickup_msg_time)
	{
		ent->client->ps.stats[STAT_PICKUP_ICON] = 0;
		ent->client->ps.stats[STAT_PICKUP_STRING] = 0;
	}

	/* timers */
	if (ent->client->quad_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex("p_quad");
		ent->client->ps.stats[STAT_TIMER] =
			(ent->client->quad_framenum - level.framenum) / 10;
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex(
				"p_invulnerability");
		ent->client->ps.stats[STAT_TIMER] =
			(ent->client->invincible_framenum - level.framenum) / 10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex("p_envirosuit");
		ent->client->ps.stats[STAT_TIMER] =
			(ent->client->enviro_framenum - level.framenum) / 10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex("p_speedboost");
		ent->client->ps.stats[STAT_TIMER] =
			(ent->client->enviro_framenum - level.framenum) / 10;
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex("p_rebreather");
		ent->client->ps.stats[STAT_TIMER] =
			(ent->client->breather_framenum - level.framenum) / 10;
	}
	else
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}

	/* selected item */
	if (ent->client->pers.selected_item == -1)
	{
		ent->client->ps.stats[STAT_SELECTED_ICON] = 0;
	}
	else
	{
		ent->client->ps.stats[STAT_SELECTED_ICON] =
			gi.imageindex(itemlist[ent->client->pers.selected_item].icon);
	}

	ent->client->ps.stats[STAT_SELECTED_ITEM] = ent->client->pers.selected_item;

	/* layouts */
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if (deathmatch->value)
	{
		if ((ent->client->pers.health <= 0) || level.intermissiontime ||
			ent->client->showscores)
		{
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		}

		if (ent->client->showinventory && (ent->client->pers.health > 0))
		{
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
		}
	}
	else
	{
		if (ent->client->showscores || ent->client->showhelp)
		{
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		}

		if (ent->client->showinventory && (ent->client->pers.health > 0))
		{
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
		}
	}

	/* frags */
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;

	/* help icon / current weapon if not shown */
	if (ent->client->pers.helpchanged && (level.framenum & 8))
	{
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex("i_help");
	}
	else if (((ent->client->pers.hand == CENTER_HANDED) ||
			  (ent->client->ps.fov > 91)) &&
			 ent->client->pers.weapon)
	{
		cvar_t *gun;
		gun = gi.cvar("cl_gun", "2", 0);

		if (gun->value != 2)
		{
			ent->client->ps.stats[STAT_HELPICON] = gi.imageindex(
					ent->client->pers.weapon->icon);
		}
		else
		{
			ent->client->ps.stats[STAT_HELPICON] = 0;
		}
	}
	else
	{
		ent->client->ps.stats[STAT_HELPICON] = 0;
	}

	ent->client->ps.stats[STAT_SPECTATOR] = 0;
}

void
G_CheckChaseStats(edict_t *ent)
{
	int i;

	if (!ent)
	{
		return;
	}

	for (i = 1; i <= maxclients->value; i++)
	{
		gclient_t *cl;

		cl = g_edicts[i].client;

		if (!g_edicts[i].inuse || (cl->chase_target != ent))
		{
			continue;
		}

		memcpy(cl->ps.stats, ent->client->ps.stats, sizeof(cl->ps.stats));
		G_SetSpectatorStats(g_edicts + i);
	}
}

void
G_SetSpectatorStats(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	gclient_t *cl = ent->client;

	if (!cl->chase_target)
	{
		G_SetStats(ent);
	}

	cl->ps.stats[STAT_SPECTATOR] = 1;

	/* layouts are independant in spectator */
	cl->ps.stats[STAT_LAYOUTS] = 0;

	if ((cl->pers.health <= 0) || level.intermissiontime || cl->showscores)
	{
		cl->ps.stats[STAT_LAYOUTS] |= 1;
	}

	if (cl->showinventory && (cl->pers.health > 0))
	{
		cl->ps.stats[STAT_LAYOUTS] |= 2;
	}

	if (cl->chase_target && cl->chase_target->inuse)
	{
		cl->ps.stats[STAT_CHASE] = CS_PLAYERSKINS +
								   (cl->chase_target - g_edicts) - 1;
	}
	else
	{
		cl->ps.stats[STAT_CHASE] = 0;
	}
}
