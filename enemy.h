#ifndef ENEMY_H
#define ENEMY_H

#include "utils_struct.h"
#include "bullet.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

extern int ENEMY_SPRITE_1_WIDTH;
extern int ENEMY_SPRITE_2_WIDTH;
extern int ENEMY_SPRITE_1_HEIGHT;
extern int ENEMY_SPRITE_2_HEIGHT;
extern char* sprite1Enemy[];
extern char* sprite2Enemy[];

void enemy(enemyPipes pipe, borders borders, vettore direzione, coordinate_base startingPoint);

#endif