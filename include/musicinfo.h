#ifndef MUSICINFO_H_
#define MUSICINFO_H_
#include <stdlib.h>
#include <audiotimer.h>
struct LyricsOptions {
    int has_lyrics;
    char *lyrics_file_path;
};
typedef struct MusicInfo {
    char *uri;
    struct LyricsOptions *lrc_options;
    TimerParameters *tparams;
} MusicInfo;
void destroyLyricsOptions(struct LyricsOptions *lrc_options);
#endif // MUSICINFO_H_
