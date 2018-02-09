#ifndef URIPLAYER_H_
#define URIPLAYER_H_
#include <stdio.h>
#include <SLES/OpenSLES.h>
int CreateUriPlayer(const char *uri);
void ReleaseUriPlayer(void);
int StartUriPlay(void);
int StopUriPlay(void);
int PauseUriPlay(void);
SLuint32 getUriPlayState(void);
SLmillisecond getUriPlayPosition(void);
#endif /* URIPLAYER_H_ */