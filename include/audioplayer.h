#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_
#include <SLES/OpenSLES.h>
#include <audiotimer.h>
#include <musicinfo.h>

void StartPlay(void);
void StopPlay(void);
void PausePlay(void);
void ResumePlay(void);
void ReleaseAll(void);
int CreatePlayerInstance(MusicInfo *minfo);
SLmillisecond getPlayPosition(void);
SLuint32 getPlayState(void);
#endif /* AUDIOPLAYER_H_ */
