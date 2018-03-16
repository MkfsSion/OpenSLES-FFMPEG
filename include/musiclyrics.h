#ifndef MUSICLYRICS_H_
#define MUSICLYRICS_H_
#include <stdio.h>
#include "libarray/array.h"
typedef struct LyricsInfo
{
    uint32_t timeline;
    char *lyrics;
} LyricsInfo;
ArrayList *getResolvedLyrics(FILE *fptr);
int InitLyricsReader(const char *filename);
void ReleaseLyricsReader(void);
char *getLyricsStr(uint32_t timeline);
#endif /* MUSICLYRICS_H_ */
