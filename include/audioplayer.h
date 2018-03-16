#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_
#include <SLES/OpenSLES.h>
#include "audiotimer.h"

void StartPlay(void);
void StopPlay(void);
void PausePlay(void);
void ResumePlay(void);
void ReleaseAll(void);
int CreatePlayerInstance(const char *filepath,TimerParameters *params);
SLmillisecond getPlayPosition(void);
SLuint32 getPlayState(void);
#endif /* AUDIOPLAYER_H_ */
