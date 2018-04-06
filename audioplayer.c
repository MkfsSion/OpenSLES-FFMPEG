#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
#include <audiodecoder.h>
#include <playerstate.h>
#include <audioplayer.h>
#include <musiclyrics.h>
#include <musicinfo.h>
#define printerr(x) \
    printf(x);
#define EXITFUN                                                                \
  if (result != SL_RESULT_SUCCESS) {                                           \
    ReleasePlayer();                                                           \
    return -1;                                                                 \
  }
#define PLAYEXIT                                                               \
  if (playerstate != OPENSLES_PLAYERSTATE_PREPARED) {                          \
    return;                                                                    \
  }

static SLObjectItf engobj = NULL;
static SLObjectItf outputmix = NULL;
static SLObjectItf playerobj = NULL;
static SLEngineItf engine = NULL;
static SLEnvironmentalReverbItf envrev = NULL;
static SLPlayItf player = NULL;
static SLAndroidSimpleBufferQueueItf androidbufferque = NULL;
static SLVolumeItf volume = NULL;
static SLEffectSendItf effectsend = NULL;
static SLuint32 playerstate = OPENSLES_PLAYERSTATE_UNINITED;
static SLmillisecond duration = 0;
const static SLEnvironmentalReverbSettings envsettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONEROOM;
static void ReleasePlayer(void);
void ReleaseAll();
void BufferqueCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
  void *buffer = NULL;
  size_t buffersize = 0;
  int back=getAudioSource(&buffer, &buffersize);
  //printf("Position:%d\n", getPlayPosition() / 1000);
  if (buffer != NULL && buffersize != 0 && back == 0) {
    (*bq)->Enqueue(bq, buffer, buffersize);
  }
  else
  {
      StopPlay();
  }
}
void PlayerCallBack(SLPlayItf player,void *context,SLuint32 event)
{
  if (event & SL_PLAYEVENT_HEADATEND) {
    StopPlay();
  }
}
static int CreateAudioPlayer(AudioInfo *audioinfo) {
  SLresult result;
  result = slCreateEngine(&engobj, 0, NULL, 0, NULL, NULL);
  EXITFUN;
  result = (*engobj)->Realize(engobj, SL_BOOLEAN_FALSE);
  EXITFUN;
  result = (*engobj)->GetInterface(engobj, SL_IID_ENGINE, &engine);
  EXITFUN;
  result = (*engine)->CreateOutputMix(engine, &outputmix, 0, 0, 0);
  EXITFUN;
  result = (*outputmix)->Realize(outputmix, SL_BOOLEAN_FALSE);
  EXITFUN;
  result = (*outputmix)
               ->GetInterface(outputmix, SL_IID_ENVIRONMENTALREVERB, &envrev);
  if (result == SL_RESULT_SUCCESS) {
    (*envrev)->SetEnvironmentalReverbProperties(envrev, &envsettings);
  }
  SLDataLocator_AndroidSimpleBufferQueue dataerbufferque = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,1};
  SLAndroidDataFormat_PCM_EX format_android;
  format_android.formatType = SL_DATAFORMAT_PCM;
  format_android.numChannels = audioinfo->channels;
  format_android.sampleRate = audioinfo->samplerate * 1000;
  format_android.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
  format_android.containerSize = 16;
  if (audioinfo->channels == 2) {
    format_android.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
  } else {
    format_android.channelMask = SL_SPEAKER_FRONT_CENTER;
  }
  format_android.endianness = SL_BYTEORDER_LITTLEENDIAN;
  format_android.representation = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
  duration = audioinfo->duration;
  //printf("Duration:%d\n", duration / 1000);
  SLDataSource audiosource = {&dataerbufferque, &format_android};
  SLDataLocator_OutputMix locater_outputmix = {SL_DATALOCATOR_OUTPUTMIX,outputmix};
  SLDataSink datasink = {&locater_outputmix, NULL};
  const SLInterfaceID ids[3] = {SL_IID_EFFECTSEND, SL_IID_VOLUME,
                                SL_IID_ANDROIDSIMPLEBUFFERQUEUE,};
  const SLboolean reqs[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE,
                             SL_BOOLEAN_TRUE};
  result = (*engine)->CreateAudioPlayer(engine, &playerobj, &audiosource,
                                        &datasink, 3, ids, reqs);
  EXITFUN;
  result = (*playerobj)->Realize(playerobj, SL_BOOLEAN_FALSE);
  EXITFUN;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_PLAY, &player);
  EXITFUN;
  result = (*player)->RegisterCallback(player, PlayerCallBack, NULL);
  EXITFUN;
  result = (*playerobj)
               ->GetInterface(playerobj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                              &androidbufferque);
  EXITFUN;
  result = (*androidbufferque)
               ->RegisterCallback(androidbufferque, BufferqueCallback, NULL);
  EXITFUN;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_VOLUME, &volume);
  EXITFUN;
  result =
      (*playerobj)->GetInterface(playerobj, SL_IID_EFFECTSEND, &effectsend);
  EXITFUN;
  return 0;
}
int CreatePlayerInstance(MusicInfo *minfo)
{
  if (!minfo)
      return -1;
  const char *filepath = minfo->uri;
  AudioInfo *infos = malloc(sizeof(AudioInfo));
  int result = CreateDecoder(filepath, infos);
  if (result != 0) {
    printerr("Error:Can not create audio decoder.\n")
    return -1;
  }
  if (infos == NULL) {
    printerr("Error:Can not alloca memory for info.\n")
    return -1;
  }
  result = CreateAudioPlayer(infos);
  if (result != 0) {
    free(infos);
    printerr("Failed to creste instance player.\n")
    return -1;
  }
  free(infos);
  playerstate = OPENSLES_PLAYERSTATE_PREPARED;
  TimerParameters *params = (TimerParameters *) malloc(sizeof(TimerParameters));
  params->duration=duration;
  params->getPlayState=getPlayState;
  params->getPlayPosition=getPlayPosition;
  CreateTimer(params);
  struct LyricsOptions *lrc_options = minfo->lrc_options;
  if (lrc_options->has_lyrics == -1) {
      InitLyricsReader(filepath,NULL);
  } else {
      InitLyricsReaderWithOptions(lrc_options);
  }
  destroyLyricsOptions(minfo->lrc_options);
  free(minfo);
  return 0;
}
void StartPlay(void) {
  PLAYEXIT;
  if (getPlayState() == SL_PLAYSTATE_PLAYING)
      return;
  (*player)->SetPlayState(player,SL_PLAYSTATE_PLAYING);
  BufferqueCallback(androidbufferque, NULL);
  StartTimer();
}
void StopPlay(void) {
  PLAYEXIT;
  (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
  ReleaseAll();
}
void PausePlay(void) {
  PLAYEXIT;
  (*player)->SetPlayState(player, SL_PLAYSTATE_PAUSED);
}
void ResumePlay(void) {
  PLAYEXIT;
  (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
}
void ReleaseAll(void) {
    PLAYEXIT;
    ReleaseResources();
    ReleasePlayer();
  }
static void ReleasePlayer(void)
{
  if (playerobj != NULL) {
    (*playerobj)->Destroy(playerobj);
    playerobj = NULL;
    player = NULL;
    volume = NULL;
    androidbufferque = NULL;
    effectsend = NULL;
  }
  if (outputmix != NULL) {
    (*outputmix)->Destroy(outputmix);
    outputmix = NULL;
    envrev = NULL;
  }
  if (engobj != NULL) {
    (*engobj)->Destroy(engobj);
    engobj = NULL;
    engine = NULL;
  }
  playerstate = OPENSLES_PLAYERSTATE_UNINITED;
  duration = 0;
}
SLmillisecond getPlayPosition(void) {
  SLmillisecond time = 0;
  (*player)->GetPosition(player, &time);
  return time;
}
SLuint32 getPlayState(void) {
  SLuint32 playstate = 0;
  (*player)->GetPlayState(player, &playstate);
  return playstate;
}
