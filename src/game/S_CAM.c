#include "local.h"

void ChasecamTrack (edict_t *ent);

void ChasecamStart (edict_t *ent){

    edict_t *chasecam;

    ent->client->chaseToggle = 1;

    ent->client->ps.gunindex = 0;

    chasecam = G_spawn();
    chasecam->owner = ent;
    chasecam->solid = SOLID_NOT;
    chasecam->movetype = MOVETYPE_FLYMISSILE;

    VectorCopy(ent->s.angles, chasecam->s.angles);

    VectorClear(chasecam->mins);
    VectorClear(chasecam->maxs);

    VectorCopy(ent->s.origin, chasecam->s.origin);

    chasecam->classname = "chasecam";
    chasecam->nextthink = level.time + 0.100;
    chasecam->think = ChasecamTrack;

    ent->client->chaseCam = chasecam;
    ent->client->oldPlayer = G_Spawn();
}

void ChasecamRestart (edict_t *ent){
    ent->nextthink = level.time + 0.100;

    if(ent->owner->health <= 0)
    {
        G_FreeEdict (ent);
        return;
    }

    if(ent->owner->waterlevel)
        return;

    ChasecamStart(ent->owner);
    G_FreeEdict(ent);
}

void ChasecamRemove (edict_t *ent, char *opt)
{

    VectorClear (ent->client->chaseCam->velocity);

    ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);

    ent->s.modelindex = ent->client->oldPlayer->s.modelindex;

    if(!strcmp(opt, "background"))
    {
        ent->client->chaseToggle = 3;
        ent->client->chaseCam->nextthink = level.time + 0.100;
        ent->client->chaseCam->think = ChasecamRestart;
    }else if (!strcmp(opt, "off"))
    {
        ent->client->chaseToggle = 0;
        G_FreeEdict (ent->client->oldPlayer);
        G_FreeEdict (ent->client->chaseCam);
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
    }else if (ent->owner->client->v_angle[0] > 0.000)
    {
        VectorMA(spot2, (ent->owner->client->v_angle[0] * 0.6), up, spot2);
    }
    
    tr = gi.trace (ent->owner->s.origin, NULL, NULL, spot2, ent->owner, false);

    VectorSubtract(tr.endpos, ent->owner->s.origin, spot1);

    ent->chasedist1 = VectorLength(spot1);

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
            //TODO Finish this function
        }
    }




}

//Code from here https://web.archive.org/web/20051227030117/http://www.planetquake.com/qdevels/quake2/9_3_98.html