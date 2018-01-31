#include "decoder.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static SLObjectItf engineobj;
static SLEngineItf engine;
static SLObjectItf outputmixobj;
static SLVolumeItf volume;
static SLEnvironmentalReverbItf envrev;
static SLObjectItf playerobj;
static SLPlayItf player;
static SLAndroidSimpleBufferQueueItf simplebuffer;
static SLMuteSoloItf mutesolo;
static SLEffectSendItf effectsend;
static SLSeekItf seeker;
static const SLEnvironmentalReverbSettings envrevsetting =
    SL_I3DL2_ENVIRONMENT_PRESET_CAVE;
static void *buffer;
static size_t buffersize;
static int duration;

void PlayCallBackAndroid(SLAndroidSimpleBufferQueueItf bq, void *context) {
  if (bq != simplebuffer) {
    return;
  }
  SLresult result;
  buffersize = 0;
  getSource(&buffer, &buffersize);
  if (buffersize != 0 && buffer != NULL) {
    result = (*bq)->Enqueue(bq, buffer, buffersize);
    if (result == SL_RESULT_SUCCESS) {
    }
  }
}
void* playmusic(void* arg) {
  PlayCallBackAndroid(simplebuffer, NULL);
  SLuint32 state = SL_PLAYSTATE_PLAYING;
  SLmillisecond times = 0;
  while (state == SL_PLAYSTATE_PLAYING) {
    (*player)->GetPlayState(player, &state);
    sleep(1);
    (*player)->GetPosition(player, &times);
    printf("\rnow/rest: %ds/%ds", times / 1000, duration - (times / 1000));
    fflush(stdout);
  }
  return 0;
}
int createEngine(void) {
  SLresult result;
  result = slCreateEngine(&engineobj, 0, NULL, 0, NULL, NULL);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*engineobj)->Realize(engineobj, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*engineobj)->GetInterface(engineobj, SL_IID_ENGINE, &engine);

  if (result != SL_RESULT_SUCCESS)
    return -1;
  const SLInterfaceID outputmixid = SL_IID_ENVIRONMENTALREVERB;
  const SLboolean requires = SL_BOOLEAN_FALSE;
  result = (*engine)->CreateOutputMix(engine, &outputmixobj, 1,&outputmixid,
                                      &requires);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*outputmixobj)->Realize(outputmixobj, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*outputmixobj)->GetInterface(outputmixobj, outputmixid, &envrev);
  if (result == SL_RESULT_SUCCESS) {
    (*envrev)->SetEnvironmentalReverbProperties(envrev, &envrevsetting);
  }
  return 0;
}
int CreatePlayer(int channel, int samplerate) {
  SLresult result;
  SLDataLocator_AndroidSimpleBufferQueue locater_a = {
      SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1};
      SLAndroidDataFormat_PCM_EX format_android;
  format_android.formatType = SL_DATAFORMAT_PCM;
  format_android.numChannels = channel;
  format_android.sampleRate = samplerate * 1000;
  format_android.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
  format_android.containerSize = 16;
  if (channel == 2) {
    format_android.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
  } else {
    format_android.channelMask = SL_SPEAKER_FRONT_CENTER;
  }
  format_android.endianness = SL_BYTEORDER_LITTLEENDIAN;
  format_android.representation = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
  SLDataSource audiosource = {&locater_a, &format_android};
  SLDataLocator_OutputMix locater_outputmix = {SL_DATALOCATOR_OUTPUTMIX,
                                               outputmixobj};
  SLDataSink datasink = {&locater_outputmix, NULL};
  const SLInterfaceID ids[5] = {SL_IID_EFFECTSEND, SL_IID_VOLUME,
                                SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                SL_IID_MUTESOLO,SL_IID_SEEK};
  const SLboolean reqs[5] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE, SL_BOOLEAN_TRUE,
                             SL_BOOLEAN_TRUE,SL_BOOLEAN_FALSE};
  result = (*engine)->CreateAudioPlayer(engine, &playerobj, &audiosource,
                                        &datasink, 5, ids, reqs);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*playerobj)->Realize(playerobj, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_PLAY, &player);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*playerobj)
               ->GetInterface(playerobj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                              &simplebuffer);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*simplebuffer)
               ->RegisterCallback(simplebuffer, PlayCallBackAndroid, NULL);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_MUTESOLO, &mutesolo);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result = (*playerobj)->GetInterface(playerobj, SL_IID_VOLUME, &volume);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  result =
      (*playerobj)->GetInterface(playerobj, SL_IID_EFFECTSEND, &effectsend);
  if (result != SL_RESULT_SUCCESS)
    return -1;
 result = (*playerobj)->GetInterface(playerobj, SL_IID_SEEK, &seeker);
  if (result!=SL_RESULT_SUCCESS)
     return -1;
  result = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
  if (result != SL_RESULT_SUCCESS)
    return -1;
  return 0;
}
void play(void) {
  int channel;
  int samplerare;
  FFMPEGInit(&channel, &samplerare,&duration);
  createEngine();
  CreatePlayer(channel, samplerare);
  PlayCallBackAndroid(simplebuffer, NULL);
  pthread_t play;
  pthread_create(&play, NULL, playmusic, NULL);
  pthread_join(play, NULL);
}

void release(void)
{
  if (playerobj != NULL) {
    (*playerobj)->Destroy(playerobj);
    playerobj = NULL;
    player = NULL;
    simplebuffer = NULL;
    effectsend = NULL;
    mutesolo = NULL;
    volume = NULL;
    seeker = NULL;
  }
  if (outputmixobj != NULL) {
    (*outputmixobj)->Destroy(outputmixobj);
    outputmixobj = NULL;
    envrev = NULL;
  }
  if (engineobj != NULL) {
    (*engineobj)->Destroy(engineobj);
    engineobj = NULL;
    engine = NULL;
  }
  ReleaseFFMPEGResource();
}