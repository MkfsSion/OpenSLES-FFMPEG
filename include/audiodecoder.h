#ifndef AUDIODECODER_H_
#define AUDIODECODER_H_
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <SLES/OpenSLES.h>
typedef struct AudioInfo {
  uint8_t channels;
  size_t samplerate;
  SLmillisecond duration;
} AudioInfo;
int CreateDecoder(const char *filepath, AudioInfo *infos);
int getAudioSource(void **buffer,size_t *buffersize);
void ReleaseResources(void);
#endif /* AUDIODECODER_H_ */