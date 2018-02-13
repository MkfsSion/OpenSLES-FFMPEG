#ifndef AUDIOTIMER_H_
#define AUDIOTIMER_H_
#include <SLES/OpenSLES.h>
struct TimerParameters {
  SLmillisecond duration;
  SLuint32 (*getPlayState)(void);
  SLmillisecond (*getPlayPosition)(void);
};
typedef struct TimerParameters TimerParameters;
int CreateTimer(TimerParameters *params);
int StartTimer(void);
#endif /* AUDIOTIMER_H_ */