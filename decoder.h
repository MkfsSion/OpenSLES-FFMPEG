#ifndef DECODER_H_
#define DECODER_H_
#include <stddef.h>
#include <stdint.h>
int FFMPEGInit(int *channel, int *samplerate,int* durartion);
int getSource(void **data, size_t *datasize);
void ReleaseFFMPEGResource(void);
#endif /* DECODER_H_ */