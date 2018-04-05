#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <audiotimer.h>
#include <musiclyrics.h>

static void *AudioTimer(void*);
static TimerParameters *tptr;
int CreateTimer(TimerParameters *params) {
  if (params == NULL)
    return -1;
  if (params->getPlayState == NULL)
    return -1;
  if (params->getPlayPosition == NULL)
    return -1;
  if (params->duration <= 0)
    return -1; // Necessary check for TimerParameters;
  tptr = params;
  return 0;
}
int StartTimer(void) {
    if (!tptr)
        return -1;
  pthread_t timerid;
  pthread_create(&timerid,NULL,AudioTimer,tptr);
  pthread_join(timerid,NULL);
  return 0;
}

static void* AudioTimer(void *params)
{
  TimerParameters *tparams = (TimerParameters *)params;
  int enable=isLyricsReaderEnable();
  while (tparams->getPlayState() == SL_PLAYSTATE_PLAYING) {
      sleep(1);
    if (tparams->getPlayState() != SL_PLAYSTATE_PLAYING) {
      break;
}
//SLmillisecond nowpos=tparams->getPlayPosition();
if (enable)
{
    printf("位置/时长:%d/%d    %s\n", tparams->getPlayPosition() / 1000,
           tparams->duration / 1000,getLyricsStr(tparams->getPlayPosition()));

}
else
{
    printf("位置/时长:%d/%d\n", tparams->getPlayPosition() / 1000, tparams->duration / 1000);
}
}
  free(params);
  if (enable)
{
  ReleaseLyricsReader();
}
  pthread_exit(NULL);
  return NULL;
}
