#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <audiotimer.h>
#include <musiclyrics.h>

static void *AudioTimer(void*);
static void ReleaseTimer(void);
static pthread_t timer_id = 0;
static pthread_cond_t pcondt = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;
int CreateTimer(TimerParameters *params) {
  if (params == NULL)
    return -1;
  if (params->getPlayState == NULL)
    return -1;
  if (params->getPlayPosition == NULL)
    return -1;
  if (params->duration <= 0)
    return -1; // Necessary check for TimerParameters;
  pthread_create(&timer_id,NULL,AudioTimer,params);
  return 0;
}
int StartTimer(void) {
    if (!timer_id)
        return -1;
  pthread_cond_signal(&pcondt);
  pthread_join(timer_id,NULL);
  ReleaseTimer();
  return 0;
}

static void ReleaseTimer(void) {
    pthread_mutex_destroy(&pmutex);
    pthread_cond_destroy(&pcondt);
    timer_id = 0;
}

static void* AudioTimer(void *params)
{
  pthread_mutex_lock(&pmutex);
  TimerParameters *tparams = (TimerParameters *)params;
  int enable=isLyricsReaderEnable();
  pthread_cond_wait(&pcondt,&pmutex);
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
  pthread_mutex_unlock(&pmutex);
  return NULL;
}
