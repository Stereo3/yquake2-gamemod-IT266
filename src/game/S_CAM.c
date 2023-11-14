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

    ent->nextthink = level.time +100;
}

//Code from here https://web.archive.org/web/20051227030117/http://www.planetquake.com/qdevels/quake2/9_3_98.html