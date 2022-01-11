#include "enemies.h"

int SPACE_BETWEEN_X = 16;
int SPACE_BETWEEN_Y = 8;

/* ------------------------------------------------------------ */
/* PROTOTIPI                                                    */
/* ------------------------------------------------------------ */

/* Funzione di utilità per calcolare il numero di nemici che ci */
/* stanno nello schermo                                         */
coordinate_base calculateNumEnemies(borders border, coordinate_base startingPoint);

/* Funzione per generare una direzione random                   */
vettore generateRandomDirection();



/* ------------------------------------------------------------ */
/* FUNZIONE PRINCIPALE                                          */
/* ------------------------------------------------------------ */
void *enemies(void *args){
  enemiesArguments enArgs = *((enemiesArguments*)args);
  int max_enemies = enArgs.max_enemies;
  borders border = enArgs.border;
  coordinate_base startingPoint = enArgs.startingPoint;
  int i;
  int pipeToClose;

  enemyThread* enemiesThreads = (enemyThread*)malloc(sizeof(enemyThread)*max_enemies);
  coordinate_base numEnemies = calculateNumEnemies(border, startingPoint);
  enemyArguments* enemyArgs = (enemyArguments*)malloc(sizeof(enemyArguments)*max_enemies);
  int* enemyIndexes = (int*)malloc(sizeof(int)*max_enemies);
  sem_t* semaphores = (sem_t*)malloc(sizeof(sem_t)*max_enemies);
  sem_t* semaphoresFull = (sem_t*)malloc(sizeof(sem_t)*max_enemies);
  pthread_mutex_t* mutexes = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*max_enemies);


  vettore randDirection = generateRandomDirection();

  hitUpdate update;

  /* ---------- Inizializzo i semafori/mutex  ---------- */
  for(i=0; i<max_enemies; i++){
    enemiesThreads[i].mutex = &mutexes[i];
    enemiesThreads[i].semaphore = &semaphores[i];
    enemiesThreads[i].semaphoreFull = &semaphoresFull[i];
    pthread_mutex_init((enemiesThreads[i].mutex), NULL);
    sem_init((enemiesThreads[i].semaphore), 0, 0);
    sem_init((enemiesThreads[i].semaphoreFull), 0, ENEMY_BUFFER_SIZE);
    enemyIndexes[i] = 0;
    enemiesThreads[i].index = &(enemyIndexes[i]);
  }

  /* ----------------- Spawno i nemici ----------------- */
  int enemyCount = 0;
  coordinate_base offset_spawn = {0,0};
  coordinate_base startingEnemyPoint;
  while(enemyCount < max_enemies){
    enemiesThreads[enemyCount].enemyNumber = enemyCount;
    startingEnemyPoint.x = startingPoint.x + offset_spawn.x;
    startingEnemyPoint.y = startingPoint.y + offset_spawn.y;
    enemyArgs[enemyCount].border = border;
    enemyArgs[enemyCount].startingPoint = startingEnemyPoint;
    enemyArgs[enemyCount].direzione = randDirection;
    enemyArgs[enemyCount].self = enemiesThreads[enemyCount];

    if(pthread_create(&(enemiesThreads[enemyCount].threadID_Child), NULL, &enemy, (void*)&(enemyArgs[enemyCount]))){
      /* Errore nella creazione del thread Nemico */
      return NULL;
    }else{
      /* sposto il punto di spawn della prossima navicella nemica */
      enemyCount++;
      if(enemyCount%numEnemies.y == 0){
        offset_spawn.x += (ENEMY_SPRITE_1_WIDTH+SPACE_BETWEEN_X);
        offset_spawn.y = 0;
      }else{
        offset_spawn.y += (ENEMY_SPRITE_1_HEIGHT+SPACE_BETWEEN_Y);
      }
    }
  }

  /* ---------------- Comunico i messaggi tra il main e i singoli nemici ---------------- */
  while(enemyCount > 0){
    update = getHit();
    if(update.hitting.x == -1 && update.hitting.emitter == SPACECRAFT){
      /* Chiudo tutti i processi figli */
      enemyCount=0;
      for(i=0; i<max_enemies; i++){
        if(enemiesThreads[i].threadID_Child != -1) addEnemyUpdate(update, enemiesThreads[i]);
      }
    }else{
      /* Controllo se è un update di eliminazione nave di primo livello */
      if(update.beingHit.x == -1 && update.beingHit.emitter == ENEMY){
        enemyCount+=3; /* -1 nave di primo livello + 4 navi di secondo livello */
      }else{
        if(update.beingHit.x == -1 && update.beingHit.emitter == ENEMY_LV2){
          enemyCount--;
        }else{
          /* Cerco il figlio colpito e gli invio l'aggiornamento */
          for(i=0; i<max_enemies; i++){
            if(update.beingHit.threadID == enemiesThreads[i].threadID_Child){
              addEnemyUpdate(update, enemiesThreads[i]);
              break;
            }
          }
        }
      }
    }
  }

   /* ------------- Chiusura del thread  ------------- */

  free(enemiesThreads);
  free(enemyArgs);
}



/* ------------------------------------------------------------ */
/* FUNZIONI DI UTILITÀ                                          */
/* ------------------------------------------------------------ */

coordinate_base calculateNumEnemies(borders border, coordinate_base startingPoint){
  coordinate_base numEnemies;
  border.maxx = border.maxx - startingPoint.x;
  border.maxy = border.maxy - startingPoint.y;
  numEnemies.x = (border.maxx) / (ENEMY_SPRITE_1_WIDTH + SPACE_BETWEEN_X);
  numEnemies.y = (border.maxy) / (ENEMY_SPRITE_1_HEIGHT + SPACE_BETWEEN_Y);
  return numEnemies;
}

vettore generateRandomDirection(){
  vettore direzione;
  direzione.x = -1;
  direzione.y = (rand() % 100) > 50 ? 1 : -1;
  direzione.speed = 1;
  return direzione;
}