#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "audioplayer.h"
extern void *timer(void *);
int run(void){
  int result = CreatePlayerInstance("/sdcard/netease/cloudmusic/Music/水城新人 - 花葬.mp3");
  if (result != 0) {
    printf("Failed to create player!\n");
  }
  printf("Created\n");
  StartPlay();
  pthread_t timerid;
  int r=pthread_create(&timerid, NULL, timer, NULL);
  if (r < 0) {
    return -1;
  }
  pthread_join(timerid, NULL);
  result = CreatePlayerInstance("/sdcard/test.mp3");
  if (result != 0) {
    return -1;
  }
  StartPlay();
  pthread_t timer2;
  pthread_create(&timer2, NULL, timer, NULL);
  pthread_join(timer2,NULL);
  return 0;
}

void* timer(void *context){
  while (getPlayState() == SL_PLAYSTATE_PLAYING) {
    sleep(1);
   // if (getPlayState() != SL_PLAYSTATE_PLAYING) {
   //   pthread_exit(NULL);
   // }
    printf("Position:%d\n", getPlayPosition());
  }
  return NULL;
}