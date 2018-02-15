#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "playerstate.h"
#include "uriplayer.h"

#define EXITFUN                                                                \
  if (result != SL_RESULT_SUCCESS) {                                           \
    return -1;                                                                 \
  }
#define PLAYEXIT                                                               \
  if (playerstate == OPENSLES_PLAYERSTATE_UNINITED) {                          \
    return -1;                                                                 \
  }
static SLObjectItf engobj = NULL;
static SLObjectItf outputmixobj = NULL;
static SLObjectItf playerobj = NULL;
static SLEngineItf engine = NULL;
static SLEnvironmentalReverbItf envrev = NULL;
static SLPlayItf player = NULL;
static SLVolumeItf volume = NULL;
static SLEffectSendItf effectsend = NULL;
static const SLEnvironmentalReverbSettings envrevsetting =
    SL_I3DL2_ENVIRONMENT_PRESET_FOREST;
static SLuint32 playerstate = OPENSLES_PLAYERSTATE_UNINITED;

void PlayerPlayCallBack(SLPlayItf player,void* context,SLuint32 event)
{
  if (SL_PLAYEVENT_HEADATEND & event) {
    StopUriPlay();
  }
}

int CreateUriPlayer(const char *uri)
{
  FILE *audiofile = NULL;
  audiofile = fopen(uri,"rb");
  if (audiofile == NULL)
     return -1;
  fclose(audiofile);
  SLresult result;
  result = slCreateEngine(&engobj, 0, NULL, 0, NULL, NULL);
  EXITFUN;
  result = (*engobj)->Realize(engobj,SL_BOOLEAN_FALSE);
  EXITFUN;
  result = (*engobj)->GetInterface(engobj, SL_IID_ENGINE, &engine);
  EXITFUN;
  const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
  const SLboolean reqs[1] = {SL_BOOLEAN_FALSE};
  result = (*engine)->CreateOutputMix(engine, &outputmixobj, 1, ids, reqs);
  EXITFUN;
  result = (*outputmixobj)->Realize(outputmixobj, SL_BOOLEAN_FALSE);
  EXITFUN;
  result =
      (*outputmixobj)
          ->GetInterface(outputmixobj, SL_IID_ENVIRONMENTALREVERB, &envrev);
  if (result == SL_RESULT_SUCCESS) {
    (*envrev)->SetEnvironmentalReverbProperties(envrev, &envrevsetting);
  }
  SLDataLocator_URI locator_uri = {SL_DATALOCATOR_URI, (SLchar*)uri};
  SLDataFormat_MIME format_mine = {SL_DATAFORMAT_MIME, NULL,
                                   SL_CONTAINERTYPE_UNSPECIFIED};
  SLDataSource audiosource = {&locator_uri, &format_mine};
  SLDataLocator_OutputMix locator_outputmix = {SL_DATALOCATOR_OUTPUTMIX,
                                               outputmixobj};
  SLDataSink datasink = {&locator_outputmix, NULL};
  const SLInterfaceID fids[2]={SL_IID_VOLUME, SL_IID_EFFECTSEND};
  const SLboolean freqs[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
  result = (*engine)->CreateAudioPlayer(engine, &playerobj, &audiosource,
                                        &datasink, 2, fids, freqs);
  EXITFUN;
  result = (*playerobj)->Realize(playerobj, SL_BOOLEAN_FALSE);
  EXITFUN;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_PLAY, &player);
  EXITFUN;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_VOLUME, &volume);
  EXITFUN;
  result =
      (*playerobj)->GetInterface(playerobj, SL_IID_EFFECTSEND, &effectsend);
  EXITFUN;
  result = (*player)->RegisterCallback(player, PlayerPlayCallBack, NULL);
  EXITFUN;
  result = (*player)->SetCallbackEventsMask(player, SL_PLAYEVENT_HEADATEND);
  EXITFUN;
  playerstate = OPENSLES_PLAYERSTATE_PREPARED;
  return 0;
}

void ReleaseUriPlayer(void)
{
  if (playerobj != NULL) {
    (*playerobj)->Destroy(playerobj);
    playerobj = NULL;
    player = NULL;
    effectsend = NULL;
    volume = NULL;
  }
  if (outputmixobj != NULL) {
    (*outputmixobj)->Destroy(outputmixobj);
    outputmixobj = NULL;
    envrev = NULL;
  }
  if (engobj != NULL) {
    (*engobj)->Destroy(engobj);
    engobj = NULL;
    engine = NULL;
  }
  playerstate = OPENSLES_PLAYERSTATE_UNINITED;
}
int StartUriPlay(void) {
  PLAYEXIT;
  if (getUriPlayState() != SL_PLAYSTATE_PLAYING) {
    (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
    return 0;
  }
  return -1;
}
int StopUriPlay(void) {
  PLAYEXIT;
  if (getUriPlayState() != SL_PLAYSTATE_STOPPED) {
    (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
    ReleaseUriPlayer();
    return 0;
  }
  return -1;
}
int PauseUriPlay(void) {
  PLAYEXIT;
  if (getUriPlayState() != SL_PLAYSTATE_PAUSED) {
    (*player)->SetPlayState(player, SL_PLAYSTATE_PAUSED);
    return 0;
  }
  return -1;
}
SLuint32 getUriPlayState(void) {
  PLAYEXIT;
  SLuint32 state;
  (*player)->GetPlayState(player, &state);
  return state;
}
SLmillisecond getUriPlayPosition(void) {
  PLAYEXIT;
  SLmillisecond milltime;
  (*player)->GetPosition(player, &milltime);
  return milltime;
}
