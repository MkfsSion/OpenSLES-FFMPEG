#include <stdio.h>
#include "uriplayer.h"
#include <unistd.h>
#include <pthread.h>
void *timerfun(void *context);

int rain(void)
{
  const char *uri = "/sdcard/netease/cloudmusic/Music/水城新人 - 花葬.mp3";
  int r = CreateUriPlayer(uri);
  if (r < 0) {
    printf("Failed to create uri player.\n");
    return -1;
  }
  StartUriPlay();
  pthread_t timer;
  pthread_create(&timer, NULL, timerfun, NULL);
  pthread_join(timer,NULL);
  return 0;
}

void *timerfun(void *context)
{
  while (getUriPlayState() == SL_PLAYSTATE_PLAYING) {
    sleep(1);
    printf("Position:%d\n", getUriPlayPosition());
  }
  return NULL;
}
