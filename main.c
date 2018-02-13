#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "audioplayer.h"
#include "audiotimer.h"

extern void *timer(void *);
int main(int argc,char **argv){
  if (argc!=2)
  {
    printf("Usage: openslplay <filepath>\n");
    return -1;
  }
  TimerParameters *params=malloc(sizeof(TimerParameters));
  char *uri=argv[1];
  int result = CreatePlayerInstance(uri,params);
  if (result != 0) {
    printf("Failed to create player!\n");
    return -1;
  }
  StartPlay();
  result = CreateTimer(params);
  if (result < 0) {
    return -1;
  }
  StartTimer();
  return 0;
}
