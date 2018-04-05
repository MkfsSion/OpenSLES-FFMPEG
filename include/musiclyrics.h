#ifndef MUSICLYRICS_H_
#define MUSICLYRICS_H_
#include <stdio.h>
#include "libarray/array.h"
#include "musicinfo.h"
typedef struct LyricsInfo
{
    uint32_t timeline;
    char *lyrics;
} LyricsInfo;
ArrayList *getResolvedLyrics(FILE *fptr);
int InitLyricsReaderWithOptions(struct LyricsOptions *lrc_options);
int InitLyricsReader(const char *rfilename,const char *lrcfilename);
void ReleaseLyricsReader(void);
char *getLyricsStr(uint32_t timeline);
int isLyricsReaderEnable(void);
#endif /* MUSICLYRICS_H_ */
