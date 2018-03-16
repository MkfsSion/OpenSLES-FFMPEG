#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <audiotimer.h>
#include <musiclyrics.h>

static void *AudioTimer(void*);
static void ReleaseTimer(void);
static pthread_t *timerid = NULL;
int CreateTimer(TimerParameters *params) {
  if (params == NULL)
    return -1;
  if (params->getPlayState == NULL)
    return -1;
  if (params->getPlayPosition == NULL)
    return -1;
  if (params->duration <= 0)
    return -1; // Necessary check for TimerParameters;
  pthread_t *timer = malloc(sizeof(pthread_t));
  pthread_create(timer, NULL, AudioTimer, params);
  timerid = timer;
  return 0;
}
int StartTimer(void) {
  if (timerid == NULL)
    return -1;
  pthread_join(*timerid, NULL);
  ReleaseTimer();
  return 0;
}
static void ReleaseTimer(void) {
  if (timerid == NULL)
    return;
  free(timerid);
}
static void* AudioTimer(void *params)
{
  TimerParameters *tparams = (TimerParameters *)params;
  while (tparams->getPlayState() == SL_PLAYSTATE_PLAYING) {
      sleep(1);
    if (tparams->getPlayState() != SL_PLAYSTATE_PLAYING) {
      break;
}
//SLmillisecond nowpos=tparams->getPlayPosition();
    printf("位置/时长:%d/%d    %s\n", tparams->getPlayPosition() / 1000,
           tparams->duration / 1000,getLyricsStr(tparams->getPlayPosition()));
  }
  free(params);
  ReleaseLyricsReader();
  pthread_exit(NULL);
  return NULL;
}
