#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "audioplayer.h"
extern void *timer(void *);
int main(int argc,char **argv){
  if (argc!=2)
  {
    printf("Usage: openslplay <filepath>\n");
    return -1;
  }
  char *uri=argv[1];
  int result = CreatePlayerInstance(uri);
  if (result != 0) {
    printf("Failed to create player!\n");
    return -1;
  }
  StartPlay();
  pthread_t timerid;
  int r=pthread_create(&timerid, NULL, timer, NULL);
  if (r < 0) {
    return -1;
  }
  pthread_join(timerid, NULL);
  return 0;
}

void* timer(void *context){
  while (getPlayState() == SL_PLAYSTATE_PLAYING) {
    sleep(1);
    if (getPlayState() != SL_PLAYSTATE_PLAYING) {
      pthread_exit(NULL);
    }
    printf("Position:%d\n", getPlayPosition()/1000);
  }
  return NULL;
}
