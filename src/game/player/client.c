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
 * Interface between client <-> game and client calculations.
 *
 * =======================================================================
 */

#include "../header/local.h"
#include "../monster/misc/player.h"

void ClientUserinfoChanged(edict_t *ent, char *userinfo);
void SP_misc_teleporter_dest(edict_t *ent);
void Touch_Item(edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);

/*
 * The ugly as hell coop spawnpoint fixup function.
 * While coop was planed by id, it wasn't part of
 * the initial release and added later with patch
 * to version 2.00. The spawnpoints in some maps
 * were SNAFU, some have wrong targets and some
 * no name at all. Fix this by matching the coop
 * spawnpoint target names to the nearest named
 * single player spot.
 */
void
SP_FixCoopSpots(edict_t *self)
{
	edict_t *spot;
	vec3_t d;

	if (!self)
	{
		return;
	}

	spot = NULL;

	while (1)
	{
		spot = G_Find(spot, FOFS(classname), "info_player_start");

		if (!spot)
		{
			return;
		}

		if (!spot->targetname)
		{
			continue;
		}

		VectorSubtract(self->s.origin, spot->s.origin, d);

		if (VectorLength(d) < 550)
		{
			if ((!self->targetname) || (Q_stricmp(self->targetname, spot->targetname) != 0))
			{
				self->targetname = spot->targetname;
			}

			return;
		}
	}
}

/*
 * Some maps have no coop spawnpoints at
 * all. Add these by injecting entities
 * into the map where they should have
 * been
 */
void
SP_CreateCoopSpots(edict_t *self)
{
	edict_t *spot;

	if (!self)
	{
		return;
	}

	if (Q_stricmp(level.mapname, "security") == 0)
	{
		spot = G_Spawn();
		spot->classname = "info_player_coop";
		spot->s.origin[0] = 188 - 64;
		spot->s.origin[1] = -164;
		spot->s.origin[2] = 80;
		spot->targetname = "jail3";
		spot->s.angles[1] = 90;

		spot = G_Spawn();
		spot->classname = "info_player_coop";
		spot->s.origin[0] = 188 + 64;
		spot->s.origin[1] = -164;
		spot->s.origin[2] = 80;
		spot->targetname = "jail3";
		spot->s.angles[1] = 90;

		spot = G_Spawn();
		spot->classname = "info_player_coop";
		spot->s.origin[0] = 188 + 128;
		spot->s.origin[1] = -164;
		spot->s.origin[2] = 80;
		spot->targetname = "jail3";
		spot->s.angles[1] = 90;

		return;
	}
}

/*
 * Some maps have no unnamed (e.g. generic)
 * info_player_start. This is no problem in
 * normal gameplay, but if the map is loaded
 * via console there is a huge chance that
 * the player will spawn in the wrong point.
 * Therefore create an unnamed info_player_start
 * at the correct point.
 */
void
SP_CreateUnnamedSpawn(edict_t *self)
{
	edict_t *spot = G_Spawn();

	if (!self)
	{
		return;
	}

	/* mine1 */
    if (Q_stricmp(level.mapname, "mine1") == 0)
	{
		if (Q_stricmp(self->targetname, "mintro") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

	/* mine2 */
    if (Q_stricmp(level.mapname, "mine2") == 0)
	{
		if (Q_stricmp(self->targetname, "mine1") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

	/* mine3 */
    if (Q_stricmp(level.mapname, "mine3") == 0)
	{
		if (Q_stricmp(self->targetname, "mine2a") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

	/* mine4 */
    if (Q_stricmp(level.mapname, "mine4") == 0)
	{
		if (Q_stricmp(self->targetname, "mine3") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

 	/* power2 */
    if (Q_stricmp(level.mapname, "power2") == 0)
	{
		if (Q_stricmp(self->targetname, "power1") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

	/* waste1 */
    if (Q_stricmp(level.mapname, "waste1") == 0)
	{
		if (Q_stricmp(self->targetname, "power2") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

	/* waste2 */
    if (Q_stricmp(level.mapname, "waste2") == 0)
	{
		if (Q_stricmp(self->targetname, "waste1") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}

	/* city3 */
    if (Q_stricmp(level.mapname, "city2") == 0)
	{
		if (Q_stricmp(self->targetname, "city2NL") == 0)
		{
			spot->classname = self->classname;
			spot->s.origin[0] = self->s.origin[0];
			spot->s.origin[1] = self->s.origin[1];
			spot->s.origin[2] = self->s.origin[2];
			spot->s.angles[1] = self->s.angles[1];
			spot->targetname = NULL;

			return;
		}
	}
}

/*
 * QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
 * The normal starting point for a level.
 */
void
SP_info_player_start(edict_t *self)
{
	if (!self)
	{
		return;
	}

    /* Call function to hack unnamed spawn points */
	self->think = SP_CreateUnnamedSpawn;
	self->nextthink = level.time + FRAMETIME;

	if (!coop->value)
	{
		return;
	}

	if (Q_stricmp(level.mapname, "security") == 0)
	{
		/* invoke one of our gross, ugly, disgusting hacks */
		self->think = SP_CreateCoopSpots;
		self->nextthink = level.time + FRAMETIME;
	}
}

/*
 * QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32)
 * potential spawning position for deathmatch games
 */
void
SP_info_player_deathmatch(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (!deathmatch->value)
	{
		G_FreeEdict(self);
		return;
	}

	SP_misc_teleporter_dest(self);
}

/*
 * QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 32)
 * potential spawning position for coop games
 */
void
SP_info_player_coop(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (!coop->value)
	{
		G_FreeEdict(self);
		return;
	}

	if ((Q_stricmp(level.mapname, "jail2") == 0) ||
		(Q_stricmp(level.mapname, "jail4") == 0) ||
		(Q_stricmp(level.mapname, "mintro") == 0) ||
		(Q_stricmp(level.mapname, "mine1") == 0) ||
		(Q_stricmp(level.mapname, "mine2") == 0) ||
		(Q_stricmp(level.mapname, "mine3") == 0) ||
		(Q_stricmp(level.mapname, "mine4") == 0) ||
		(Q_stricmp(level.mapname, "lab") == 0) ||
		(Q_stricmp(level.mapname, "boss1") == 0) ||
		(Q_stricmp(level.mapname, "fact1") == 0) ||
		(Q_stricmp(level.mapname, "fact3") == 0) ||
		(Q_stricmp(level.mapname, "waste1") == 0) || /* really? */
		(Q_stricmp(level.mapname, "biggun") == 0) ||
		(Q_stricmp(level.mapname, "space") == 0) ||
		(Q_stricmp(level.mapname, "command") == 0) ||
		(Q_stricmp(level.mapname, "power2") == 0) ||
		(Q_stricmp(level.mapname, "strike") == 0) ||
		(Q_stricmp(level.mapname, "city2") == 0))
	{
		/* invoke one of our gross, ugly, disgusting hacks */
		self->think = SP_FixCoopSpots;
		self->nextthink = level.time + FRAMETIME;
	}
}

/*
 * QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
 * The deathmatch intermission point will be at one of these
 * Use 'angles' instead of 'angle', so you can set pitch or
 * roll as well as yaw.  'pitch yaw roll'
 */
void
SP_info_player_intermission(edict_t *self)
{
	/* Thus function cannot be removed
	 * since the info_player_intermission
	 * needs a callback function. Like
	 * every entity. */
}

/* ======================================================================= */

void
player_pain(edict_t *self /* unused */, edict_t *other /* unused */,
	   	float kick /* unused */, int damage /* unused */)
{
	/* Player pain is handled at the end
	 * of the frame in P_DamageFeedback.
	 * This function is still here since
	 * the player is an entity and needs
	 * a pain callback */
}

qboolean
IsFemale(edict_t *ent)
{
	char *info;

	if (!ent)
	{
		return false;
	}

	if (!ent->client)
	{
		return false;
	}

	info = Info_ValueForKey(ent->client->pers.userinfo, "gender");

	if (strstr(info, "crakhor"))
	{
		return true;
	}

	if ((info[0] == 'f') || (info[0] == 'F'))
	{
		return true;
	}

	return false;
}

qboolean
IsNeutral(edict_t *ent)
{
	char *info;

	if (!ent)
	{
		return false;
	}

	if (!ent->client)
	{
		return false;
	}

	info = Info_ValueForKey(ent->client->pers.userinfo, "gender");

	if (strstr(info, "crakhor"))
	{
		return false;
	}

	if ((info[0] != 'f') && (info[0] != 'F') && (info[0] != 'm') &&
		(info[0] != 'M'))
	{
		return true;
	}

	return false;
}

void
ClientObituary(edict_t *self, edict_t *inflictor /* unused */,
	   	edict_t *attacker)
{
	int mod;
	char *message;
	char *message2;
	qboolean ff;

	if (!self || !inflictor)
	{
		return;
	}

	if (coop->value && attacker && attacker->client)
	{
		meansOfDeath |= MOD_FRIENDLY_FIRE;
	}

	if (deathmatch->value || coop->value)
	{
		ff = meansOfDeath & MOD_FRIENDLY_FIRE;
		mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
		message = NULL;
		message2 = "";

		switch (mod)
		{
			case MOD_SUICIDE:
				message = "suicides";
				break;
			case MOD_FALLING:
				message = "cratered";
				break;
			case MOD_CRUSH:
				message = "was squished";
				break;
			case MOD_WATER:
				message = "sank like a rock";
				break;
			case MOD_SLIME:
				message = "melted";
				break;
			case MOD_LAVA:
				message = "does a back flip into the lava";
				break;
			case MOD_EXPLOSIVE:
			case MOD_BARREL:
				message = "blew up";
				break;
			case MOD_EXIT:
				message = "found a way out";
				break;
			case MOD_TARGET_LASER:
				message = "saw the light";
				break;
			case MOD_TARGET_BLASTER:
				message = "got blasted";
				break;
			case MOD_BOMB:
			case MOD_SPLASH:
			case MOD_TRIGGER_HURT:
				message = "was in the wrong place";
				break;
		}

		if (attacker == self)
		{
			switch (mod)
			{
				case MOD_HELD_GRENADE:
					message = "tried to put the pin back in";
					break;
				case MOD_HG_SPLASH:
				case MOD_G_SPLASH:

					if (IsNeutral(self))
					{
						message = "tripped on its own grenade";
					}
					else if (IsFemale(self))
					{
						message = "tripped on her own grenade";
					}
					else
					{
						message = "tripped on his own grenade";
					}

					break;
				case MOD_R_SPLASH:

					if (IsNeutral(self))
					{
						message = "blew itself up";
					}
					else if (IsFemale(self))
					{
						message = "blew herself up";
					}
					else
					{
						message = "blew himself up";
					}

					break;
				case MOD_BFG_BLAST:
					message = "should have used a smaller gun";
					break;
				default:

					if (IsNeutral(self))
					{
						message = "killed itself";
					}
					else if (IsFemale(self))
					{
						message = "killed herself";
					}
					else
					{
						message = "killed himself";
					}

					break;
			}
		}

		if (message)
		{
			gi.bprintf(PRINT_MEDIUM, "%s %s.\n",
					self->client->pers.netname,
					message);

			if (deathmatch->value)
			{
				self->client->resp.score--;
			}

			self->enemy = NULL;
			return;
		}

		self->enemy = attacker;

		if (attacker && attacker->client)
		{
			switch (mod)
			{
				case MOD_BLASTER:
					message = "was blasted by";
					break;
				case MOD_SHOTGUN:
					message = "was gunned down by";
					break;
				case MOD_SSHOTGUN:
					message = "was blown away by";
					message2 = "'s super shotgun";
					break;
				case MOD_MACHINEGUN:
					message = "was machinegunned by";
					break;
				case MOD_CHAINGUN:
					message = "was cut in half by";
					message2 = "'s chaingun";
					break;
				case MOD_GRENADE:
					message = "was popped by";
					message2 = "'s grenade";
					break;
				case MOD_G_SPLASH:
					message = "was shredded by";
					message2 = "'s shrapnel";
					break;
				case MOD_ROCKET:
					message = "ate";
					message2 = "'s rocket";
					break;
				case MOD_R_SPLASH:
					message = "almost dodged";
					message2 = "'s rocket";
					break;
				case MOD_HYPERBLASTER:
					message = "was melted by";
					message2 = "'s hyperblaster";
					break;
				case MOD_RAILGUN:
					message = "was railed by";
					break;
				case MOD_BFG_LASER:
					message = "saw the pretty lights from";
					message2 = "'s BFG";
					break;
				case MOD_BFG_BLAST:
					message = "was disintegrated by";
					message2 = "'s BFG blast";
					break;
				case MOD_BFG_EFFECT:
					message = "couldn't hide from";
					message2 = "'s BFG";
					break;
				case MOD_HANDGRENADE:
					message = "caught";
					message2 = "'s handgrenade";
					break;
				case MOD_HG_SPLASH:
					message = "didn't see";
					message2 = "'s handgrenade";
					break;
				case MOD_HELD_GRENADE:
					message = "feels";
					message2 = "'s pain";
					break;
				case MOD_TELEFRAG:
					message = "tried to invade";
					message2 = "'s personal space";
					break;
			}

			if (message)
			{
				gi.bprintf(PRINT_MEDIUM, "%s %s %s%s\n",
						self->client->pers.netname,
						message, attacker->client->pers.netname,
						message2);

				if (deathmatch->value)
				{
					if (ff)
					{
						attacker->client->resp.score--;
					}
					else
					{
						attacker->client->resp.score++;
					}
				}

				return;
			}
		}
	}

	gi.bprintf(PRINT_MEDIUM, "%s died.\n", self->client->pers.netname);

	if (deathmatch->value)
	{
		self->client->resp.score--;
	}
}

void
TossClientWeapon(edict_t *self)
{
	gitem_t *item;
	edict_t *drop;
	qboolean quad;
	float spread;

	if (!self)
	{
		return;
	}

	if (!deathmatch->value)
	{
		return;
	}

	item = self->client->pers.weapon;

	if (!self->client->pers.inventory[self->client->ammo_index])
	{
		item = NULL;
	}

	if (item && (strcmp(item->pickup_name, "Blaster") == 0))
	{
		item = NULL;
	}

	if (!((int)(dmflags->value) & DF_QUAD_DROP))
	{
		quad = false;
	}
	else
	{
		quad = (self->client->quad_framenum > (level.framenum + 10));
	}

	if (item && quad)
	{
		spread = 22.5;
	}
	else
	{
		spread = 0.0;
	}

	if (item)
	{
		self->client->v_angle[YAW] -= spread;
		drop = Drop_Item(self, item);
		self->client->v_angle[YAW] += spread;
		drop->spawnflags = DROPPED_PLAYER_ITEM;
	}

	if (quad)
	{
		self->client->v_angle[YAW] += spread;
		drop = Drop_Item(self, FindItemByClassname("item_quad"));
		self->client->v_angle[YAW] -= spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;

		drop->touch = Touch_Item;
		drop->nextthink = level.time +
						  (self->client->quad_framenum -
						   level.framenum) * FRAMETIME;
		drop->think = G_FreeEdict;
	}
}

void
LookAtKiller(edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	vec3_t dir;

	if (!self)
	{
		return;
	}

	if (attacker && (attacker != world) && (attacker != self))
	{
		VectorSubtract(attacker->s.origin, self->s.origin, dir);
	}
	else if (inflictor && (inflictor != world) && (inflictor != self))
	{
		VectorSubtract(inflictor->s.origin, self->s.origin, dir);
	}
	else
	{
		self->client->killer_yaw = self->s.angles[YAW];
		return;
	}

	if (dir[0])
	{
		self->client->killer_yaw = 180 / M_PI *atan2(dir[1], dir[0]);
	}
	else
	{
		self->client->killer_yaw = 0;

		if (dir[1] > 0)
		{
			self->client->killer_yaw = 90;
		}
		else if (dir[1] < 0)
		{
			self->client->killer_yaw = -90;
		}
	}

	if (self->client->killer_yaw < 0)
	{
		self->client->killer_yaw += 360;
	}
}

void
player_die(edict_t *self, edict_t *inflictor, edict_t *attacker,
		int damage, vec3_t point /* unused */)
{
	int n;

	if (!self || !inflictor || !attacker)
	{
		return;
	}

	VectorClear(self->avelocity);

	self->takedamage = DAMAGE_YES;
	self->movetype = MOVETYPE_TOSS;

	self->s.modelindex2 = 0; /* remove linked weapon model */

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;

	self->s.sound = 0;
	self->client->weapon_sound = 0;

	self->maxs[2] = -8;

	self->svflags |= SVF_DEADMONSTER;

	if (!self->deadflag)
	{
		self->client->respawn_time = level.time + 1.0;
		LookAtKiller(self, inflictor, attacker);
		self->client->ps.pmove.pm_type = PM_DEAD;
		ClientObituary(self, inflictor, attacker);
		TossClientWeapon(self);

		if (deathmatch->value)
		{
			Cmd_Help_f(self); /* show scores */
		}

		/* clear inventory: this is kind of ugly, but
		   it's how we want to handle keys in coop */
		for (n = 0; n < game.num_items; n++)
		{
			if (coop->value && itemlist[n].flags & IT_KEY)
			{
				self->client->resp.coop_respawn.inventory[n] =
					self->client->pers.inventory[n];
			}

			self->client->pers.inventory[n] = 0;
		}
	}

	/* remove powerups */
	self->client->quad_framenum = 0;
	self->client->invincible_framenum = 0;
	self->client->breather_framenum = 0;
	self->client->enviro_framenum = 0;
	self->client->speed_framenum = 0;
	self->flags &= ~FL_POWER_ARMOR;

	if (self->health < -40)
	{
		/* gib (sound is played at end of server frame) */
		self->sounds = gi.soundindex("misc/udeath.wav");

		for (n = 0; n < 4; n++)
		{
			ThrowGib(self, "models/objects/gibs/sm_meat/tris.md2",
					damage, GIB_ORGANIC);
		}

		ThrowClientHead(self, damage);

		self->takedamage = DAMAGE_NO;
	}
	else
	{
		/* normal death */
		if (!self->deadflag)
		{
			static int i;

			i = (i + 1) % 3;

			/* start a death animation */
			self->client->anim_priority = ANIM_DEATH;

			if (self->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				self->s.frame = FRAME_crdeath1 - 1;
				self->client->anim_end = FRAME_crdeath5;
			}
			else
			{
				switch (i)
				{
					case 0:
						self->s.frame = FRAME_death101 - 1;
						self->client->anim_end = FRAME_death106;
						break;
					case 1:
						self->s.frame = FRAME_death201 - 1;
						self->client->anim_end = FRAME_death206;
						break;
					case 2:
						self->s.frame = FRAME_death301 - 1;
						self->client->anim_end = FRAME_death308;
						break;
				}
			}

			/* sound is played at end of server frame */
			if (!self->sounds)
			{
				self->sounds = gi.soundindex(va("*death%i.wav",
								(randk() % 4) + 1));
			}
		}
	}

	self->deadflag = DEAD_DEAD;

	gi.linkentity(self);
}

/* ======================================================================= */

/*
 * This is only called when the game first
 * initializes in single player, but is called
 * after each death and level change in deathmatch
 */
void
InitClientPersistant(gclient_t *client)
{
	gitem_t *item;

	if (!client)
	{
		return;
	}

	memset(&client->pers, 0, sizeof(client->pers));

	item = FindItem("Blaster");
	client->pers.selected_item = ITEM_INDEX(item);
	client->pers.inventory[client->pers.selected_item] = 1;

	client->pers.weapon = item;

	client->pers.health = 100;
	client->pers.max_health = 100;

	client->pers.max_bullets = 200;
	client->pers.max_shells = 100;
	client->pers.max_rockets = 50;
	client->pers.max_grenades = 50;
	client->pers.max_cells = 200;
	client->pers.max_slugs = 50;

	client->pers.connected = true;
}

void
InitClientResp(gclient_t *client)
{
	if (!client)
	{
		return;
	}

	memset(&client->resp, 0, sizeof(client->resp));
	client->resp.enterframe = level.framenum;
	client->resp.coop_respawn = client->pers;
}

/*
 * Some information that should be persistant, like health,
 * is still stored in the edict structure, so it needs to
 * be mirrored out to the client structure before all the
 * edicts are wiped.
 */
void
SaveClientData(void)
{
	int i;
	edict_t *ent;

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];

		if (!ent->inuse)
		{
			continue;
		}

		game.clients[i].pers.health = ent->health;
		game.clients[i].pers.max_health = ent->max_health;
		game.clients[i].pers.savedFlags =
			(ent->flags & (FL_GODMODE | FL_NOTARGET | FL_POWER_ARMOR));

		if (coop->value)
		{
			game.clients[i].pers.score = ent->client->resp.score;
		}
	}
}

void
FetchClientEntData(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	ent->health = ent->client->pers.health;
	ent->max_health = ent->client->pers.max_health;
	ent->flags |= ent->client->pers.savedFlags;

	if (coop->value)
	{
		ent->client->resp.score = ent->client->pers.score;
	}
}

/* ======================================================================= */

/*
 * Returns the distance to the
 * nearest player from the given spot
 */
float
PlayersRangeFromSpot(edict_t *spot)
{
	edict_t *player;
	float bestplayerdistance;
	vec3_t v;
	int n;
	float playerdistance;

	if (!spot)
	{
		return 0;
	}

	bestplayerdistance = 9999999;

	for (n = 1; n <= maxclients->value; n++)
	{
		player = &g_edicts[n];

		if (!player->inuse)
		{
			continue;
		}

		if (player->health <= 0)
		{
			continue;
		}

		VectorSubtract(spot->s.origin, player->s.origin, v);
		playerdistance = VectorLength(v);

		if (playerdistance < bestplayerdistance)
		{
			bestplayerdistance = playerdistance;
		}
	}

	return bestplayerdistance;
}

/*
 * go to a random point, but NOT the two
 * points closest to other players
 */
edict_t *
SelectRandomDeathmatchSpawnPoint(void)
{
	edict_t *spot, *spot1, *spot2;
	int count = 0;
	int selection;
	float range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find(spot, FOFS(classname),
					"info_player_deathmatch")) != NULL)
	{
		count++;
		range = PlayersRangeFromSpot(spot);

		if (range < range1)
		{
			range1 = range;
			spot1 = spot;
		}
		else if (range < range2)
		{
			range2 = range;
			spot2 = spot;
		}
	}

	if (!count)
	{
		return NULL;
	}

	if (count <= 2)
	{
		spot1 = spot2 = NULL;
	}
	else
	{
		if (spot1)
		{
			count--;
		}

		if (spot2)
		{
			count--;
		}
	}

	selection = randk() % count;

	spot = NULL;

	do
	{
		spot = G_Find(spot, FOFS(classname), "info_player_deathmatch");

		if ((spot == spot1) || (spot == spot2))
		{
			selection++;
		}
	}
	while (selection--);

	return spot;
}

edict_t *
SelectFarthestDeathmatchSpawnPoint(void)
{
	edict_t *bestspot;
	float bestdistance, bestplayerdistance;
	edict_t *spot;

	spot = NULL;
	bestspot = NULL;
	bestdistance = 0;

	while ((spot = G_Find(spot, FOFS(classname),
					"info_player_deathmatch")) != NULL)
	{
		bestplayerdistance = PlayersRangeFromSpot(spot);

		if (bestplayerdistance > bestdistance)
		{
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if (bestspot)
	{
		return bestspot;
	}

	/* if there is a player just spawned on each and every start spot/
	   we have no choice to turn one into a telefrag meltdown */
	spot = G_Find(NULL, FOFS(classname), "info_player_deathmatch");

	return spot;
}

edict_t *
SelectDeathmatchSpawnPoint(void)
{
	if ((int)(dmflags->value) & DF_SPAWN_FARTHEST)
	{
		return SelectFarthestDeathmatchSpawnPoint();
	}
	else
	{
		return SelectRandomDeathmatchSpawnPoint();
	}
}

edict_t *
SelectCoopSpawnPoint(edict_t *ent)
{
	int index;
	edict_t *spot = NULL;
	char *target;

	if (!ent)
	{
		return NULL;
	}

	index = ent->client - game.clients;

	/* player 0 starts in normal player spawn point */
	if (!index)
	{
		return NULL;
	}

	spot = NULL;

	/* assume there are four coop spots at each spawnpoint */
	while (1)
	{
		spot = G_Find(spot, FOFS(classname), "info_player_coop");

		if (!spot)
		{
			return NULL; /* we didn't have enough... */
		}

		target = spot->targetname;

		if (!target)
		{
			target = "";
		}

		if (Q_stricmp(game.spawnpoint, target) == 0)
		{
			/* this is a coop spawn point
			   for one of the clients here */
			index--;

			if (!index)
			{
				return spot; /* this is it */
			}
		}
	}

	return spot;
}

/*
 * Chooses a player start, deathmatch start, coop start, etc
 */
void
SelectSpawnPoint(edict_t *ent, vec3_t origin, vec3_t angles)
{
	edict_t *spot = NULL;
	edict_t *coopspot = NULL;
	int index;
	int counter = 0;
	vec3_t d;

	if (!ent)
	{
		return;
	}

	if (deathmatch->value)
	{
		spot = SelectDeathmatchSpawnPoint();
	}
	else if (coop->value)
	{
		spot = SelectCoopSpawnPoint(ent);
	}

	/* find a single player start spot */
	if (!spot)
	{
		while ((spot = G_Find(spot, FOFS(classname), "info_player_start")) != NULL)
		{
			if (!game.spawnpoint[0] && !spot->targetname)
			{
				break;
			}

			if (!game.spawnpoint[0] || !spot->targetname)
			{
				continue;
			}

			if (Q_stricmp(game.spawnpoint, spot->targetname) == 0)
			{
				break;
			}
		}

		if (!spot)
		{
			if (!game.spawnpoint[0])
			{
				/* there wasn't a spawnpoint without a target, so use any */
				spot = G_Find(spot, FOFS(classname), "info_player_start");
			}

			if (!spot)
			{
				gi.error("Couldn't find spawn point %s\n", game.spawnpoint);
			}
		}
	}

	/* If we are in coop and we didn't find a coop
	   spawnpoint due to map bugs (not correctly
	   connected or the map was loaded via console
	   and thus no previously map is known to the
	   client) use one in 550 units radius. */
	if (coop->value)
	{
		index = ent->client - game.clients;

		if (Q_stricmp(spot->classname, "info_player_start") == 0 && index != 0)
		{
			while(counter < 3)
			{
				coopspot = G_Find(coopspot, FOFS(classname), "info_player_coop");

				if (!coopspot)
				{
					break;
				}

				VectorSubtract(coopspot->s.origin, spot->s.origin, d);

				if ((VectorLength(d) < 550))
				{
					if (index == counter)
					{
						spot = coopspot;
						break;
					}
					else
					{
						counter++;
					}
				}
			}
		}
	}

	VectorCopy(spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy(spot->s.angles, angles);
}

/* ====================================================================== */

void
InitBodyQue(void)
{
	if (deathmatch->value || coop->value)
	{
		int i;
		edict_t *ent;

		level.body_que = 0;

		for (i = 0; i < BODY_QUEUE_SIZE; i++)
		{
			ent = G_Spawn();
			ent->classname = "bodyque";
		}
	}
}

void
body_die(edict_t *self, edict_t *inflictor /* unused */,
	   	edict_t *attacker /* unused */, int damage,
		vec3_t point /* unused */)
{
	int n;

	if (!self)
	{
		return;
	}

	if (self->health < -40)
	{
		gi.sound(self, CHAN_BODY, gi.soundindex(
						"misc/udeath.wav"), 1, ATTN_NORM, 0);

		for (n = 0; n < 4; n++)
		{
			ThrowGib(self, "models/objects/gibs/sm_meat/tris.md2",
					damage, GIB_ORGANIC);
		}

		self->s.origin[2] -= 48;
		ThrowClientHead(self, damage);
		self->takedamage = DAMAGE_NO;
	}
}

void
CopyToBodyQue(edict_t *ent)
{
	edict_t *body;

	if (!ent)
	{
		return;
	}

	/* grab a body que and cycle to the next one */
	body = &g_edicts[(int)maxclients->value + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	gi.unlinkentity(ent);
	gi.unlinkentity(body);
	body->s = ent->s;
	body->s.number = body - g_edicts;

	body->svflags = ent->svflags;
	VectorCopy(ent->mins, body->mins);
	VectorCopy(ent->maxs, body->maxs);
	VectorCopy(ent->absmin, body->absmin);
	VectorCopy(ent->absmax, body->absmax);
	VectorCopy(ent->size, body->size);
	body->solid = ent->solid;
	body->clipmask = ent->clipmask;
	body->owner = ent->owner;
	body->movetype = ent->movetype;

	body->die = body_die;
	body->takedamage = DAMAGE_YES;

	gi.linkentity(body);
}

void
respawn(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (deathmatch->value || coop->value)
	{
		/* spectator's don't leave bodies */
		if (self->movetype != MOVETYPE_NOCLIP)
		{
			CopyToBodyQue(self);
		}

		self->svflags &= ~SVF_NOCLIENT;
		PutClientInServer(self);

		/* add a teleportation effect */
		self->s.event = EV_PLAYER_TELEPORT;

		/* hold in place briefly */
		self->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
		self->client->ps.pmove.pm_time = 14;

		self->client->respawn_time = level.time;

		return;
	}

	/* restart the entire server */
	gi.AddCommandString("menu_loadgame\n");
}

/*
 * only called when pers.spectator changes
 * note that resp.spectator should be the
 * opposite of pers.spectator here
 */
void
spectator_respawn(edict_t *ent)
{
	int i, numspec;

	if (!ent)
	{
		return;
	}

	/* if the user wants to become a spectator,
	   make sure he doesn't exceed max_spectators */
	if (ent->client->pers.spectator)
	{
		char *value = Info_ValueForKey(ent->client->pers.userinfo, "spectator");

		if (*spectator_password->string &&
			strcmp(spectator_password->string, "none") &&
			strcmp(spectator_password->string, value))
		{
			gi.cprintf(ent, PRINT_HIGH, "Spectator password incorrect.\n");
			ent->client->pers.spectator = false;
			gi.WriteByte(svc_stufftext);
			gi.WriteString("spectator 0\n");
			gi.unicast(ent, true);
			return;
		}

		/* count spectators */
		for (i = 1, numspec = 0; i <= maxclients->value; i++)
		{
			if (g_edicts[i].inuse && g_edicts[i].client->pers.spectator)
			{
				numspec++;
			}
		}

		if (numspec >= maxspectators->value)
		{
			gi.cprintf(ent, PRINT_HIGH, "Server spectator limit is full.");
			ent->client->pers.spectator = false;

			/* reset his spectator var */
			gi.WriteByte(svc_stufftext);
			gi.WriteString("spectator 0\n");
			gi.unicast(ent, true);
			return;
		}
	}
	else
	{
		/* he was a spectator and wants to join the
		   game he must have the right password */
		char *value = Info_ValueForKey(ent->client->pers.userinfo, "password");

		if (*password->string && strcmp(password->string, "none") &&
			strcmp(password->string, value))
		{
			gi.cprintf(ent, PRINT_HIGH, "Password incorrect.\n");
			ent->client->pers.spectator = true;
			gi.WriteByte(svc_stufftext);
			gi.WriteString("spectator 1\n");
			gi.unicast(ent, true);
			return;
		}
	}

	/* clear client on respawn */
	ent->client->resp.score = ent->client->pers.score = 0;

	ent->svflags &= ~SVF_NOCLIENT;
	PutClientInServer(ent);

	/* add a teleportation effect */
	if (!ent->client->pers.spectator)
	{
		/* send effect */
		gi.WriteByte(svc_muzzleflash);
		gi.WriteShort(ent - g_edicts);
		gi.WriteByte(MZ_LOGIN);
		gi.multicast(ent->s.origin, MULTICAST_PVS);

		/* hold in place briefly */
		ent->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
		ent->client->ps.pmove.pm_time = 14;
	}

	ent->client->respawn_time = level.time;

	if (ent->client->pers.spectator)
	{
		gi.bprintf(PRINT_HIGH, "%s has moved to the sidelines\n",
				ent->client->pers.netname);
	}
	else
	{
		gi.bprintf(PRINT_HIGH, "%s joined the game\n",
				ent->client->pers.netname);
	}
}

/* ============================================================== */

/*
 * Called when a player connects to
 * a server or respawns in a deathmatch.
 */
void
PutClientInServer(edict_t *ent)
{
	char userinfo[MAX_INFO_STRING];

	if (!ent)
	{
		return;
	}

	vec3_t mins = {-16, -16, -24};
	vec3_t maxs = {16, 16, 32};
	int index;
	vec3_t spawn_origin, spawn_angles;
	gclient_t *client;
	int i;
	client_persistant_t saved;
	client_respawn_t resp;

	/* find a spawn point do it before setting
	   health back up, so farthest ranging
	   doesn't count this client */
	SelectSpawnPoint(ent, spawn_origin, spawn_angles);

	index = ent - g_edicts - 1;
	client = ent->client;

	/* deathmatch wipes most client data every spawn */
	if (deathmatch->value)
	{
		resp = client->resp;
		memcpy(userinfo, client->pers.userinfo, sizeof(userinfo));
		InitClientPersistant(client);
		ClientUserinfoChanged(ent, userinfo);
	}
	else if (coop->value)
	{
		resp = client->resp;
		memcpy(userinfo, client->pers.userinfo, sizeof(userinfo));
		resp.coop_respawn.game_helpchanged = client->pers.game_helpchanged;
		resp.coop_respawn.helpchanged = client->pers.helpchanged;
		client->pers = resp.coop_respawn;
		ClientUserinfoChanged(ent, userinfo);

		if (resp.score > client->pers.score)
		{
			client->pers.score = resp.score;
		}
	}
	else
	{
		memset(&resp, 0, sizeof(resp));
	}

	memcpy(userinfo, client->pers.userinfo, sizeof(userinfo));
	ClientUserinfoChanged(ent, userinfo);

	/* clear everything but the persistant data */
	saved = client->pers;
	memset(client, 0, sizeof(*client));
	client->pers = saved;

	if (client->pers.health <= 0)
	{
		InitClientPersistant(client);
	}

	client->resp = resp;

	/* copy some data from the client to the entity */
	FetchClientEntData(ent);

	/* clear entity values */
	ent->groundentity = NULL;
	ent->client = &game.clients[index];
	ent->takedamage = DAMAGE_AIM;
	ent->movetype = MOVETYPE_WALK;
	ent->viewheight = 22;
	ent->inuse = true;
	ent->classname = "player";
	ent->mass = 200;
	ent->solid = SOLID_BBOX;
	ent->deadflag = DEAD_NO;
	ent->air_finished = level.time + 12;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->model = "players/male/tris.md2";
	ent->pain = player_pain;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags &= ~FL_NO_KNOCKBACK;
	ent->svflags = 0;

	VectorCopy(mins, ent->mins);
	VectorCopy(maxs, ent->maxs);
	VectorClear(ent->velocity);

	/* clear playerstate values */
	memset(&ent->client->ps, 0, sizeof(client->ps));

	client->ps.pmove.origin[0] = spawn_origin[0] * 8;
	client->ps.pmove.origin[1] = spawn_origin[1] * 8;
	client->ps.pmove.origin[2] = spawn_origin[2] * 8;

	if (deathmatch->value && ((int)dmflags->value & DF_FIXED_FOV))
	{
		client->ps.fov = 90;
	}
	else
	{
		client->ps.fov = (int)strtol(Info_ValueForKey(client->pers.userinfo, "fov"), (char **)NULL, 10);

		if (client->ps.fov < 1)
		{
			client->ps.fov = 90;
		}
		else if (client->ps.fov > 160)
		{
			client->ps.fov = 160;
		}
	}

	client->ps.gunindex = gi.modelindex(client->pers.weapon->view_model);

	/* clear entity state values */
	ent->s.effects = 0;
	ent->s.modelindex = 255; /* will use the skin specified model */
	ent->s.modelindex2 = 255; /* custom gun model */

	/* sknum is player num and weapon number
	   weapon number will be added in changeweapon */
	ent->s.skinnum = ent - g_edicts - 1;

	ent->s.frame = 0;
	VectorCopy(spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;  /* make sure off ground */
	VectorCopy(ent->s.origin, ent->s.old_origin);

	/* set the delta angle */
	for (i = 0; i < 3; i++)
	{
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(
				spawn_angles[i] - client->resp.cmd_angles[i]);
	}

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy(ent->s.angles, client->ps.viewangles);
	VectorCopy(ent->s.angles, client->v_angle);

	/* spawn a spectator */
	if (client->pers.spectator)
	{
		client->chase_target = NULL;

		client->resp.spectator = true;

		ent->movetype = MOVETYPE_NOCLIP;
		ent->solid = SOLID_NOT;
		ent->svflags |= SVF_NOCLIENT;
		ent->client->ps.gunindex = 0;
		gi.linkentity(ent);
		return;
	}
	else
	{
		client->resp.spectator = false;
	}

	if (!KillBox(ent))
	{
		/* could't spawn in? */
	}

	gi.linkentity(ent);

	/* force the current weapon up */
	client->newweapon = client->pers.weapon;
	ChangeWeapon(ent);
}

/*
 * A client has just connected to the server in
 * deathmatch mode, so clear everything out before
 * starting them.
 */
void
ClientBeginDeathmatch(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	G_InitEdict(ent);

	InitClientResp(ent->client);

	/* locate ent at a spawn point */
	PutClientInServer(ent);

	if (level.intermissiontime)
	{
		MoveClientToIntermission(ent);
	}
	else
	{
		/* send effect */
		gi.WriteByte(svc_muzzleflash);
		gi.WriteShort(ent - g_edicts);
		gi.WriteByte(MZ_LOGIN);
		gi.multicast(ent->s.origin, MULTICAST_PVS);
	}

	gi.bprintf(PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);

	/* make sure all view stuff is valid */
	ClientEndServerFrame(ent);
}

/*
 * called when a client has finished connecting, and is ready
 * to be placed into the game.  This will happen every level load.
 */
void
ClientBegin(edict_t *ent)
{
	int i;

	if (!ent)
	{
		return;
	}

	ent->client = game.clients + (ent - g_edicts - 1);

	if (deathmatch->value)
	{
		ClientBeginDeathmatch(ent);
		return;
	}

	/* if there is already a body waiting for us (a loadgame),
	   just take it, otherwise spawn one from scratch */
	if (ent->inuse == true)
	{
		/* the client has cleared the client side viewangles upon
		   connecting to the server, which is different than the
		   state when the game is saved, so we need to compensate
		   with deltaangles */
		for (i = 0; i < 3; i++)
		{
			ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(
					ent->client->ps.viewangles[i]);
		}
	}
	else
	{
		/* a spawn point will completely reinitialize the entity
		   except for the persistant data that was initialized at
		   ClientConnect() time */
		G_InitEdict(ent);
		ent->classname = "player";
		InitClientResp(ent->client);
		PutClientInServer(ent);
	}

	if (level.intermissiontime)
	{
		MoveClientToIntermission(ent);
	}
	else
	{
		/* send effect if in a multiplayer game */
		if (game.maxclients > 1)
		{
			gi.WriteByte(svc_muzzleflash);
			gi.WriteShort(ent - g_edicts);
			gi.WriteByte(MZ_LOGIN);
			gi.multicast(ent->s.origin, MULTICAST_PVS);

			gi.bprintf(PRINT_HIGH, "%s entered the game\n",
					ent->client->pers.netname);
		}
	}

	/* make sure all view stuff is valid */
	ClientEndServerFrame(ent);
}

/*
 * Called whenever the player updates a userinfo variable.
 * The game can override any of the settings in place
 * (forcing skins or names, etc) before copying it off.
 */
void
ClientUserinfoChanged(edict_t *ent, char *userinfo)
{
	char *s;
	int playernum;

	if (!ent || !userinfo)
	{
		return;
	}

	/* check for malformed or illegal info strings */
	if (!Info_Validate(userinfo))
	{
		strcpy(userinfo, "\\name\\badinfo\\skin\\male/grunt");
	}

	/* set name */
	s = Info_ValueForKey(userinfo, "name");
	Q_strlcpy(ent->client->pers.netname, s, sizeof(ent->client->pers.netname));

	/* set spectator */
	s = Info_ValueForKey(userinfo, "spectator");

	/* spectators are only supported in deathmatch */
	if (deathmatch->value && *s && strcmp(s, "0"))
	{
		ent->client->pers.spectator = true;
	}
	else
	{
		ent->client->pers.spectator = false;
	}

	/* set skin */
	s = Info_ValueForKey(userinfo, "skin");

	playernum = ent - g_edicts - 1;

	/* combine name and skin into a configstring */
	gi.configstring(CS_PLAYERSKINS + playernum,
			va("%s\\%s", ent->client->pers.netname, s));

	/* fov */
	if (deathmatch->value && ((int)dmflags->value & DF_FIXED_FOV))
	{
		ent->client->ps.fov = 90;
	}
	else
	{
		ent->client->ps.fov = (int)strtol(Info_ValueForKey(userinfo, "fov"), (char **)NULL, 10);

		if (ent->client->ps.fov < 1)
		{
			ent->client->ps.fov = 90;
		}
		else if (ent->client->ps.fov > 160)
		{
			ent->client->ps.fov = 160;
		}
	}

	/* handedness */
	s = Info_ValueForKey(userinfo, "hand");

	if (strlen(s))
	{
		ent->client->pers.hand = (int)strtol(s, (char **)NULL, 10);
	}

	/* save off the userinfo in case we want to check something later */
	Q_strlcpy(ent->client->pers.userinfo, userinfo, sizeof(ent->client->pers.userinfo));
}

/*
 * Called when a player begins connecting to the server.
 * The game can refuse entrance to a client by returning false.
 * If the client is allowed, the connection process will continue
 * and eventually get to ClientBegin(). Changing levels will NOT
 * cause this to be called again, but loadgames will.
 */
qboolean
ClientConnect(edict_t *ent, char *userinfo)
{
	char *value;

	if (!ent || !userinfo)
	{
		return false;
	}

	/* check to see if they are on the banned IP list */
	value = Info_ValueForKey(userinfo, "ip");

	if (SV_FilterPacket(value))
	{
		Info_SetValueForKey(userinfo, "rejmsg", "Banned.");
		return false;
	}

	/* check for a spectator */
	value = Info_ValueForKey(userinfo, "spectator");

	if (deathmatch->value && *value && strcmp(value, "0"))
	{
		int i, numspec;

		if (*spectator_password->string &&
			strcmp(spectator_password->string, "none") &&
			strcmp(spectator_password->string, value))
		{
			Info_SetValueForKey(userinfo, "rejmsg",
					"Spectator password required or incorrect.");
			return false;
		}

		/* count spectators */
		for (i = numspec = 0; i < maxclients->value; i++)
		{
			if (g_edicts[i + 1].inuse && g_edicts[i + 1].client->pers.spectator)
			{
				numspec++;
			}
		}

		if (numspec >= maxspectators->value)
		{
			Info_SetValueForKey(userinfo, "rejmsg",
					"Server spectator limit is full.");
			return false;
		}
	}
	else
	{
		/* check for a password */
		value = Info_ValueForKey(userinfo, "password");

		if (*password->string && strcmp(password->string, "none") &&
			strcmp(password->string, value))
		{
			Info_SetValueForKey(userinfo, "rejmsg",
					"Password required or incorrect.");
			return false;
		}
	}

	/* they can connect */
	ent->client = game.clients + (ent - g_edicts - 1);

	/* if there is already a body waiting for us (a loadgame),
	   just take it, otherwise spawn one from scratch */
	if (ent->inuse == false)
	{
		/* clear the respawning variables */
		InitClientResp(ent->client);

		if (!game.autosaved || !ent->client->pers.weapon)
		{
			InitClientPersistant(ent->client);
		}
	}

	ClientUserinfoChanged(ent, userinfo);

	if (game.maxclients > 1)
	{
		gi.dprintf("%s connected\n", ent->client->pers.netname);
	}

	ent->svflags = 0; /* make sure we start with known default */
	ent->client->pers.connected = true;
	return true;
}

/*
 * Called when a player drops from the server.
 * Will not be called between levels.
 */
void
ClientDisconnect(edict_t *ent)
{
	int playernum;

	if (!ent)
	{
		return;
	}

	if (!ent->client)
	{
		return;
	}

	gi.bprintf(PRINT_HIGH, "%s disconnected\n", ent->client->pers.netname);

	/* send effect */
	gi.WriteByte(svc_muzzleflash);
	gi.WriteShort(ent - g_edicts);
	gi.WriteByte(MZ_LOGOUT);
	gi.multicast(ent->s.origin, MULTICAST_PVS);

	gi.unlinkentity(ent);
	ent->s.modelindex = 0;
	ent->solid = SOLID_NOT;
	ent->inuse = false;
	ent->classname = "disconnected";
	ent->client->pers.connected = false;

	playernum = ent - g_edicts - 1;
	gi.configstring(CS_PLAYERSKINS + playernum, "");
}

/* ============================================================== */

static edict_t *pm_passent;

/*
 * pmove doesn't need to know
 * about passent and contentmask
 */
trace_t
PM_trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
	if (pm_passent->health > 0)
	{
		return gi.trace(start, mins, maxs, end, pm_passent, MASK_PLAYERSOLID);
	}
	else
	{
		return gi.trace(start, mins, maxs, end, pm_passent, MASK_DEADSOLID);
	}
}

unsigned
CheckBlock(void *b, int c)
{
	int v, i;

	if (!b)
	{
		return 0;
	}

	v = 0;

	for (i = 0; i < c; i++)
	{
		v += ((byte *)b)[i];
	}

	return v;
}

void
PrintPmove(pmove_t *pm)
{
	unsigned c1, c2;

	if (!pm)
	{
		return;
	}

	c1 = CheckBlock(&pm->s, sizeof(pm->s));
	c2 = CheckBlock(&pm->cmd, sizeof(pm->cmd));
	gi.dprintf("sv %3i:%i %i\n", pm->cmd.impulse, c1, c2);
}

/*
 * This will be called once for each client frame, which will
 * usually be a couple times for each server frame.
 */
void
ClientThink(edict_t *ent, usercmd_t *ucmd)
{
	gclient_t *client;
	edict_t *other;
	int i, j;
	pmove_t pm;

	if (!ent || !ucmd)
	{
		return;
	}

	level.current_entity = ent;
	client = ent->client;

	if (level.intermissiontime)
	{
		client->ps.pmove.pm_type = PM_FREEZE;

		/* can exit intermission after five seconds */
		if ((level.time > level.intermissiontime + 5.0) &&
			(ucmd->buttons & BUTTON_ANY))
		{
			level.exitintermission = true;
		}

		return;
	}

	pm_passent = ent;

	if (ent->client->chase_target)
	{
		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);
	}
	else
	{
		/* set up for pmove */
		memset(&pm, 0, sizeof(pm));

		if (ent->movetype == MOVETYPE_NOCLIP)
		{
			client->ps.pmove.pm_type = PM_SPECTATOR;
		}
		else if (ent->s.modelindex != 255)
		{
			client->ps.pmove.pm_type = PM_GIB;
		}
		else if (ent->deadflag)
		{
			client->ps.pmove.pm_type = PM_DEAD;
		}
		else
		{
			client->ps.pmove.pm_type = PM_NORMAL;
		}

		client->ps.pmove.gravity = sv_gravity->value;
		pm.s = client->ps.pmove;

		for (i = 0; i < 3; i++)
		{
			pm.s.origin[i] = ent->s.origin[i] * 8;
			/* save to an int first, in case the short overflows
			 * so we get defined behavior (at least with -fwrapv) */
			int tmpVel = ent->velocity[i] * 8;
			pm.s.velocity[i] = tmpVel;
		}

		if (memcmp(&client->old_pmove, &pm.s, sizeof(pm.s)))
		{
			pm.snapinitial = true;
		}

		pm.cmd = *ucmd;

		pm.trace = PM_trace; /* adds default parms */
		pm.pointcontents = gi.pointcontents;

		/* perform a pmove */
		gi.Pmove(&pm);

		/* save results of pmove */
		client->ps.pmove = pm.s;
		client->old_pmove = pm.s;

		for (i = 0; i < 3; i++)
		{
			ent->s.origin[i] = pm.s.origin[i] * 0.125;
			ent->velocity[i] = pm.s.velocity[i] * 0.125;
		}

		VectorCopy(pm.mins, ent->mins);
		VectorCopy(pm.maxs, ent->maxs);

		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

		if (ent->groundentity && !pm.groundentity && (pm.cmd.upmove >= 10) &&
			(pm.waterlevel == 0))
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex(
							"*jump1.wav"), 1, ATTN_NORM, 0);
			PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
		}

		ent->viewheight = pm.viewheight;
		ent->waterlevel = pm.waterlevel;
		ent->watertype = pm.watertype;
		ent->groundentity = pm.groundentity;

		if (pm.groundentity)
		{
			ent->groundentity_linkcount = pm.groundentity->linkcount;
		}

		if (ent->deadflag)
		{
			client->ps.viewangles[ROLL] = 40;
			client->ps.viewangles[PITCH] = -15;
			client->ps.viewangles[YAW] = client->killer_yaw;
		}
		else
		{
			VectorCopy(pm.viewangles, client->v_angle);
			VectorCopy(pm.viewangles, client->ps.viewangles);
		}

		gi.linkentity(ent);

		if (ent->movetype != MOVETYPE_NOCLIP)
		{
			G_TouchTriggers(ent);
		}

		/* touch other objects */
		for (i = 0; i < pm.numtouch; i++)
		{
			other = pm.touchents[i];

			for (j = 0; j < i; j++)
			{
				if (pm.touchents[j] == other)
				{
					break;
				}
			}

			if (j != i)
			{
				continue; /* duplicated */
			}

			if (!other->touch)
			{
				continue;
			}

			other->touch(other, ent, NULL, NULL);
		}
	}

	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~client->oldbuttons;

	/* save light level the player is standing
	   on for monster sighting AI */
	ent->light_level = ucmd->lightlevel;

	/* fire weapon from final position if needed */
	if (client->latched_buttons & BUTTON_ATTACK)
	{
		if (client->resp.spectator)
		{
			client->latched_buttons = 0;

			if (client->chase_target)
			{
				client->chase_target = NULL;
				client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			}
			else
			{
				GetChaseTarget(ent);
			}
		}
		else if (!client->weapon_thunk)
		{
			client->weapon_thunk = true;
			Think_Weapon(ent);
		}
	}

	if (client->resp.spectator)
	{
		if (ucmd->upmove >= 10)
		{
			if (!(client->ps.pmove.pm_flags & PMF_JUMP_HELD))
			{
				client->ps.pmove.pm_flags |= PMF_JUMP_HELD;

				if (client->chase_target)
				{
					ChaseNext(ent);
				}
				else
				{
					GetChaseTarget(ent);
				}
			}
		}
		else
		{
			client->ps.pmove.pm_flags &= ~PMF_JUMP_HELD;
		}
	}

	/* update chase cam if being followed */
	for (i = 1; i <= maxclients->value; i++)
	{
		other = g_edicts + i;

		if (other->inuse && (other->client->chase_target == ent))
		{
			UpdateChaseCam(other);
		}
	}
}

/*
 * This will be called once for each server
 * frame, before running any other entities
 * in the world.
 */
void
ClientBeginServerFrame(edict_t *ent)
{
	gclient_t *client;
	int buttonMask;

	if (!ent)
	{
		return;
	}

	if (level.intermissiontime)
	{
		return;
	}

	client = ent->client;

	if (deathmatch->value &&
		(client->pers.spectator != client->resp.spectator) &&
		((level.time - client->respawn_time) >= 5))
	{
		spectator_respawn(ent);
		return;
	}

	/* run weapon animations if it hasn't been done by a ucmd_t */
	if (!client->weapon_thunk && !client->resp.spectator)
	{
		Think_Weapon(ent);
	}
	else
	{
		client->weapon_thunk = false;
	}

	if (ent->deadflag)
	{
		/* wait for any button just going down */
		if (level.time > client->respawn_time)
		{
			/* in deathmatch, only wait for attack button */
			if (deathmatch->value)
			{
				buttonMask = BUTTON_ATTACK;
			}
			else
			{
				buttonMask = -1;
			}

			if ((client->latched_buttons & buttonMask) ||
				(deathmatch->value && ((int)dmflags->value & DF_FORCE_RESPAWN)))
			{
				respawn(ent);
				client->latched_buttons = 0;
			}
		}

		return;
	}

	/* add player trail so monsters can follow */
	if (!deathmatch->value)
	{
		if (!visible(ent, PlayerTrail_LastSpot()))
		{
			PlayerTrail_Add(ent->s.old_origin);
		}
	}

	client->latched_buttons = 0;
}

void ChasecamTrack (edict_t *ent);

void ChasecamStart (edict_t *ent){

    edict_t *chasecam;

    ent->client->chaseToggle = 1;

    ent->client->ps.gunindex = 0;

    chasecam = G_Spawn();
    chasecam->owner = ent;
    chasecam->solid = SOLID_NOT;
    chasecam->movetype = MOVETYPE_FLYMISSILE;
    ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
    ent->svflags |= SVF_NOCLIENT;

    VectorCopy(ent->s.angles, chasecam->s.angles);

    VectorClear(chasecam->mins);
    VectorClear(chasecam->maxs);

    VectorCopy(ent->s.origin, chasecam->s.origin);

    chasecam->classname = "chasecam";
    chasecam->prethink = ChasecamTrack;

    ent->client->chaseCam = chasecam;
    ent->client->oldPlayer = G_Spawn();
}

void ChasecamRestart (edict_t *ent){

    if(ent->owner->health <= 0)
    {
        G_FreeEdict (ent);
        return;
    }

    if(ent->owner->waterlevel)
	{
		return;
	}
    ChasecamStart(ent->owner);
    G_FreeEdict(ent);
}

void ChasecamRemove (edict_t *ent, char *opt)
{

    VectorClear (ent->client->chaseCam->velocity);

    ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);

    ent->s.modelindex = ent->client->oldPlayer->s.modelindex;

    ent->svflags &= ~SVF_NOCLIENT;

    if(!strcmp(opt, "background"))
    {
        ent->client->chaseToggle = 0;
        G_FreeEdict(ent->client->chaseCam);
        G_FreeEdict(ent->client->oldPlayer);
        ent->client->chaseCam = G_Spawn();
        ent->client->chaseCam->owner = ent;
        ent->client->chaseCam->solid = SOLID_NOT;
        ent->client->chaseCam->movetype = MOVETYPE_FLYMISSILE;
        VectorClear(ent->client->chaseCam->mins);
        VectorClear(ent->client->chaseCam->maxs);
        ent->client->chaseCam->classname = "chasecam";
        ent->client->chaseCam->prethink = ChasecamRestart;
        
    }
	else if (!strcmp(opt, "off"))
    {
        if(ent->client->chaseToggle)
        {
            free(ent->client->oldPlayer->client);
            G_FreeEdict(ent->client->oldPlayer);
        }
        ent->client->chaseToggle = 0;
        G_FreeEdict(ent->client->chaseCam);
    }
}

void ChasecamTrack (edict_t *ent)
{
    trace_t tr;
    vec3_t spot1, spot2, dir;
    vec3_t forward, right, up;
    int distance;
    int tot;

    ent->nextthink = level.time + 100;
    if(ent->owner->waterlevel)
    {
        ChasecamRemove (ent, "background");
        return;
    }

    AngleVectors(ent->owner->client->v_angle, forward, right, up);

    VectorMA(ent->owner->s.origin, ent->chasedist1, forward, spot2);

    spot2[2] = (spot2[2] + 40.000);

    if(ent->owner->client->v_angle[0] < 0.000)
    {
        VectorMA(spot2, -(ent->owner->client->v_angle[0] * 0.6), up, spot2);
    }
    else if (ent->owner->client->v_angle[0] > 0.000)
    {
        VectorMA(spot2, (ent->owner->client->v_angle[0] * 0.6), up, spot2);
    }
    
    tr = gi.trace (ent->owner->s.origin, NULL, NULL, spot2, ent->owner, false);

    VectorSubtract(tr.endpos, ent->owner->s.origin, spot1);

    ent->chasedist1 = VectorLength(spot1);

    VectorMA(tr.endpos, 2, forward, spot2);

    VectorCopy(spot2, spot1);
    spot1[1] += 32;

    tr = gi.trace (spot2, NULL, NULL, spot1, ent->owner, false);

    if(tr.fraction < 1.000)
    {
        VectorCopy(tr.endpos, spot2);
        spot2[2] -= 32;
    }

    VectorSubtract(spot2, ent->s.origin, dir);
    VectorNormalize(dir);

    VectorSubtract(spot2, ent->s.origin, spot1);
    distance = VectorLength(spot1);

    tr = gi.trace(ent->s.origin, NULL, NULL, spot2, ent->owner, false);

    if(tr.fraction == 1.000)
    {
        VectorSubtract(ent->s.origin, ent->owner->s.origin, spot1);
        VectorNormalize(spot1);
        VectorCopy(spot1, ent->s.angles);

        tot = (distance * 0.400);

        if (tot > 5.200)
        {
            ent->velocity[0] = ((dir[0] * distance) * 5.2);
            ent->velocity[1] = ((dir[1] * distance) * 5.2);
            ent->velocity[2] = ((dir[2] * distance) * 5.2);
        }
        else
        {
            if(tot > 1.000)
            {
                ent->velocity[0] = ((dir[0] * distance) * tot);
                ent->velocity[1] = ((dir[1] * distance) * 5.2);
                ent->velocity[2] = ((dir[2] * distance) * 5.2);
            }
            else
            {
                ent->velocity[0] = ((dir[0] * distance) * distance);
                ent->velocity[1] = ((dir[1] * distance) * distance);
                ent->velocity[2] = ((dir[2] * distance) * distance);
            }
        }

        VectorSubtract(ent->owner->s.origin, ent->s.origin, spot1);

        if(VectorLength(spot1)<20)
        {
            ent->velocity[0] *= 2;
            ent->velocity[1] *= 2;
            ent->velocity[2] *= 2;
        }
    }
    else
    {
        VectorCopy(spot2, ent->s.origin);
    }

    ent->chasedist1 += 2;

    if(ent->chasedist1 > 60.000)
    {
        ent->chasedist1 = 60.000;
    }
    
    if(ent->movedir == ent->s.origin)
    {
        if(distance>20)
        {
            ent->chasedist2++;
        }
    }
        
    if(ent->chasedist2 > 3)
    {
        ChasecamStart(ent->owner);
        G_FreeEdict(ent);
        return;
    }
    VectorCopy(ent->s.origin, ent->movedir);
}


void Cmd_Chasecam_Toggle (edict_t *ent)
{
    if(!ent->waterlevel && !ent->deadflag)
    {
        if(ent->client->chaseToggle)
            ChasecamRemove(ent, "off");
        else
            ChasecamStart(ent);
    }
    else if (ent->waterlevel && ent->deadflag)
        gi.cprintf(ent, PRINT_HIGH, "Camera cannot be modified while in water\n");
}


void CheckChasecam_Viewent(edict_t *ent)
{
   gclient_t   *cl;
   if(!ent->client->oldPlayer->client)
   {
    cl = (gclient_t*)malloc(sizeof(gclient_t));
    ent->client->oldPlayer->client = cl;
   }
   if((ent->client->chaseToggle == 1) && (ent->client->oldPlayer))
   {
    ent->client->oldPlayer->s.frame = ent->s.frame;
    VectorCopy(ent->s.origin, ent->client->oldPlayer->s.origin);
    VectorCopy(ent->velocity, ent->client->oldPlayer->velocity);
    VectorCopy(ent->s.angles, ent->client->oldPlayer->s.angles);
    ent->client->oldPlayer->s = ent->s;
    gi.linkentity(ent->client->oldPlayer);
   }
}

//All chasecam code from: https://web.archive.org/web/20051227030921/http://www.planetquake.com/qdevels/quake2/29_6_98.html
