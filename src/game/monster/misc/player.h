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
 * Player (the arm and the weapons) animation.
 *
 * =======================================================================
 */


#define FRAME_stand01 0
#define FRAME_stand02 1
#define FRAME_stand03 2
#define FRAME_stand04 3
#define FRAME_stand05 4
#define FRAME_stand06 5
#define FRAME_stand07 6
#define FRAME_stand08 7
#define FRAME_stand09 8
#define FRAME_stand10 9
#define FRAME_stand11 10
#define FRAME_stand12 11
#define FRAME_stand13 12
#define FRAME_stand14 13
#define FRAME_stand15 14
#define FRAME_stand16 15
#define FRAME_stand17 16
#define FRAME_stand18 17
#define FRAME_stand19 18
#define FRAME_stand20 19
#define FRAME_stand21 20
#define FRAME_stand22 21
#define FRAME_stand23 22
#define FRAME_stand24 23
#define FRAME_stand25 24
#define FRAME_stand26 25
#define FRAME_stand27 26
#define FRAME_stand28 27
#define FRAME_stand29 28
#define FRAME_stand30 29
#define FRAME_stand31 30
#define FRAME_stand32 31
#define FRAME_stand33 32
#define FRAME_stand34 33
#define FRAME_stand35 34
#define FRAME_stand36 35
#define FRAME_stand37 36
#define FRAME_stand38 37
#define FRAME_stand39 38
#define FRAME_stand40 39
#define FRAME_run1 40
#define FRAME_run2 41
#define FRAME_run3 42
#define FRAME_run4 43
#define FRAME_run5 44
#define FRAME_run6 45
#define FRAME_attack1 46
#define FRAME_attack2 47
#define FRAME_attack3 48
#define FRAME_attack4 49
#define FRAME_attack5 50
#define FRAME_attack6 51
#define FRAME_attack7 52
#define FRAME_attack8 53
#define FRAME_pain101 54
#define FRAME_pain102 55
#define FRAME_pain103 56
#define FRAME_pain104 57
#define FRAME_pain201 58
#define FRAME_pain202 59
#define FRAME_pain203 60
#define FRAME_pain204 61
#define FRAME_pain301 62
#define FRAME_pain302 63
#define FRAME_pain303 64
#define FRAME_pain304 65
#define FRAME_jump1 66
#define FRAME_jump2 67
#define FRAME_jump3 68
#define FRAME_jump4 69
#define FRAME_jump5 70
#define FRAME_jump6 71
#define FRAME_flip01 72
#define FRAME_flip02 73
#define FRAME_flip03 74
#define FRAME_flip04 75
#define FRAME_flip05 76
#define FRAME_flip06 77
#define FRAME_flip07 78
#define FRAME_flip08 79
#define FRAME_flip09 80
#define FRAME_flip10 81
#define FRAME_flip11 82
#define FRAME_flip12 83
#define FRAME_salute01 84
#define FRAME_salute02 85
#define FRAME_salute03 86
#define FRAME_salute04 87
#define FRAME_salute05 88
#define FRAME_salute06 89
#define FRAME_salute07 90
#define FRAME_salute08 91
#define FRAME_salute09 92
#define FRAME_salute10 93
#define FRAME_salute11 94
#define FRAME_taunt01 95
#define FRAME_taunt02 96
#define FRAME_taunt03 97
#define FRAME_taunt04 98
#define FRAME_taunt05 99
#define FRAME_taunt06 100
#define FRAME_taunt07 101
#define FRAME_taunt08 102
#define FRAME_taunt09 103
#define FRAME_taunt10 104
#define FRAME_taunt11 105
#define FRAME_taunt12 106
#define FRAME_taunt13 107
#define FRAME_taunt14 108
#define FRAME_taunt15 109
#define FRAME_taunt16 110
#define FRAME_taunt17 111
#define FRAME_wave01 112
#define FRAME_wave02 113
#define FRAME_wave03 114
#define FRAME_wave04 115
#define FRAME_wave05 116
#define FRAME_wave06 117
#define FRAME_wave07 118
#define FRAME_wave08 119
#define FRAME_wave09 120
#define FRAME_wave10 121
#define FRAME_wave11 122
#define FRAME_point01 123
#define FRAME_point02 124
#define FRAME_point03 125
#define FRAME_point04 126
#define FRAME_point05 127
#define FRAME_point06 128
#define FRAME_point07 129
#define FRAME_point08 130
#define FRAME_point09 131
#define FRAME_point10 132
#define FRAME_point11 133
#define FRAME_point12 134
#define FRAME_crstnd01 135
#define FRAME_crstnd02 136
#define FRAME_crstnd03 137
#define FRAME_crstnd04 138
#define FRAME_crstnd05 139
#define FRAME_crstnd06 140
#define FRAME_crstnd07 141
#define FRAME_crstnd08 142
#define FRAME_crstnd09 143
#define FRAME_crstnd10 144
#define FRAME_crstnd11 145
#define FRAME_crstnd12 146
#define FRAME_crstnd13 147
#define FRAME_crstnd14 148
#define FRAME_crstnd15 149
#define FRAME_crstnd16 150
#define FRAME_crstnd17 151
#define FRAME_crstnd18 152
#define FRAME_crstnd19 153
#define FRAME_crwalk1 154
#define FRAME_crwalk2 155
#define FRAME_crwalk3 156
#define FRAME_crwalk4 157
#define FRAME_crwalk5 158
#define FRAME_crwalk6 159
#define FRAME_crattak1 160
#define FRAME_crattak2 161
#define FRAME_crattak3 162
#define FRAME_crattak4 163
#define FRAME_crattak5 164
#define FRAME_crattak6 165
#define FRAME_crattak7 166
#define FRAME_crattak8 167
#define FRAME_crattak9 168
#define FRAME_crpain1 169
#define FRAME_crpain2 170
#define FRAME_crpain3 171
#define FRAME_crpain4 172
#define FRAME_crdeath1 173
#define FRAME_crdeath2 174
#define FRAME_crdeath3 175
#define FRAME_crdeath4 176
#define FRAME_crdeath5 177
#define FRAME_death101 178
#define FRAME_death102 179
#define FRAME_death103 180
#define FRAME_death104 181
#define FRAME_death105 182
#define FRAME_death106 183
#define FRAME_death201 184
#define FRAME_death202 185
#define FRAME_death203 186
#define FRAME_death204 187
#define FRAME_death205 188
#define FRAME_death206 189
#define FRAME_death301 190
#define FRAME_death302 191
#define FRAME_death303 192
#define FRAME_death304 193
#define FRAME_death305 194
#define FRAME_death306 195
#define FRAME_death307 196
#define FRAME_death308 197

#define MODEL_SCALE 1.000000

void ChasecamTrack (edict_t *ent);

void ChasecamStart (edict_t *ent);

void ChasecamRestart (edict_t *ent);

void ChasecamRemove (edict_t *ent, char *opt);

void ChasecamTrack (edict_t *ent);

void Cmd_Chasecam_Toggle (edict_t *ent);

void CheckChasecam_Viewent(edict_t *ent);