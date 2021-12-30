#include "spacecraft.h"


const int SPACECRAFT_SPRITE_WIDTH = 6;
const int SPACECRAFT_SPRITE_HEIGHT = 4;
char* spriteSpacecraft[] = {" /-","<[][]>","<[][]>"," \\-"};
float fireRate = 0.3; /* 3 colpo ogni 10 frame, 30 frame al secondo = 9 colpi al secondo */

void spacecraft(int pipeIN, int pipeOUT, borders borders){
  bool quit = false;
  int pid;
  timeout(0);
  float elapsedTime = 0;

  /* Inizializzazione coordinate */
  coordinate coords;
  coords.emitter=SPACECRAFT;
  coords.x = 1;
  coords.y = borders.maxy/2;
  coords.PID = getpid();
  coords.prev_coordinate.x = coords.x;
  coords.prev_coordinate.y = coords.y;

  fflush(stdin);
  /* ciclo principale */
  while(!quit){
    int c = getch();
    switch(c){
      case KEY_UP:
        if(coords.y>1){       /* Se non sono già in cima */
          coords.y--;
        }
        break;
      case KEY_DOWN:
        if(coords.y<borders.maxy-SPACECRAFT_SPRITE_HEIGHT){  /* Se non sono già in fondo */
          coords.y++;
        }
        break;
      case 32: /* Spazio */
        if(elapsedTime>=DELAY_MS/fireRate){ /* sparo solo se è passato il tempo di attesa tra un colpo e l'altro */
          elapsedTime = 0;
          pid = fork();
          if(pid == 0){
            coords.x = coords.x + SPACECRAFT_SPRITE_WIDTH;
            coords.y += SPACECRAFT_SPRITE_HEIGHT/2; /* Faccio partire il colpo dalla punta della navicella */
            coords.prev_coordinate.x = coords.x;
            coords.emitter = BULLET;
            bullet(pipeOUT, borders, RIGHT_UP, coords);
            return;
          }else{
            pid = fork();
            if(pid == 0){
              coords.x = coords.x + SPACECRAFT_SPRITE_WIDTH;
              coords.y += SPACECRAFT_SPRITE_HEIGHT/2; /* Faccio partire il colpo dalla punta della navicella */
              coords.prev_coordinate.x = coords.x;
              coords.emitter = BULLET;
              bullet(pipeOUT, borders, RIGHT_DOWN, coords);
              return;
            }
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

    write(pipeOUT, &coords, sizeof(coordinate));
    coords.prev_coordinate.y = coords.y;
    if(c > 0) napms(DELAY_MS);
  }
}