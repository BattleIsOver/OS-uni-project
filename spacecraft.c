#include "spacecraft.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE COSTANTI                                         */
const int SPACECRAFT_SPRITE_WIDTH = 6;
const int SPACECRAFT_SPRITE_HEIGHT = 3;
char* spriteSpacecraft[] = {" /-   ","<[][]>"," \\-   "};
float fireRate = 0.15;
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/* FUNZIONE PRINCIPALE                                          */
/* ------------------------------------------------------------ */
void *spacecraft(void *args){
  borders border = *((borders*) args);
  bool quit = false;
  timeout(0);
  float elapsedTime = 0;
  bulletArguments bulletArgs;
  bulletArguments bulletArgs2;
  pthread_t bombThreadID;

  /* ------------ Inizializzo le coordinate ------------ */
  coordinate coords;
  coords.emitter=SPACECRAFT;
  coords.x = 1;
  coords.y = border.maxy/2;
  coords.threadID = pthread_self();
  coords.prev_coordinate.x = coords.x;
  coords.prev_coordinate.y = coords.y;

  fflush(stdin);
  /* ---------------- Ciclo principale  ---------------- */
  while(!quit){
    int c = getch();
    switch(c){
      case KEY_UP:
        if(coords.y>2){       /* Se non sono già in cima */
          coords.y-=2;
        }
        break;
      case KEY_DOWN:
        if(coords.y<border.maxy-SPACECRAFT_SPRITE_HEIGHT-2){  /* Se non sono già in fondo */
          coords.y+=2;
        }
        break;
      case 32: /* Spazio */
        if(elapsedTime>=DELAY_MS/fireRate){ /* sparo solo se è passato il tempo di attesa tra un colpo e l'altro */
          elapsedTime = 0;
          bulletArgs.startingPoint = coords;
          bulletArgs.startingPoint.x = bulletArgs.startingPoint.x + SPACECRAFT_SPRITE_WIDTH;
          bulletArgs.startingPoint.y += SPACECRAFT_SPRITE_HEIGHT/2; /* Faccio partire il colpo dalla punta della navicella */
          bulletArgs.startingPoint.prev_coordinate.x = bulletArgs.startingPoint.x;
          bulletArgs.startingPoint.emitter = BULLET;
          bulletArgs.border = border;
          bulletArgs.direzione = RIGHT_UP;
          if(pthread_create(&bombThreadID, NULL, &bullet, (void*)&bulletArgs)){
            return NULL;
          }
          
          bulletArgs2 = bulletArgs;
          bulletArgs2.direzione = RIGHT_DOWN;
          if(pthread_create(&bombThreadID, NULL, &bullet, (void*)&bulletArgs2)){
            return NULL;
          }
            
        }
        break;
      case 113: /* q */
        coords.x = -1;
        quit = true;
        break;
    }

    if(elapsedTime<=DELAY_MS/fireRate){ /* aggiorno il tempo solo se necessario (per evitare overflow) */
      elapsedTime += DELAY_MS;
    }

    addUpdate(coords);
    coords.prev_coordinate.y = coords.y;
    napms(DELAY_MS);
  }
}