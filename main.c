#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <musicinfo.h>
#include <audioplayer.h>
#include <audiotimer.h>

static void printf_usage(void);

void destroyLyricsOptions(struct LyricsOptions *lrc_options) {
    if (!lrc_options)
        return;
    if (lrc_options->lyrics_file_path)
        free(lrc_options->lyrics_file_path);
    free(lrc_options);
}

static struct LyricsOptions *initLyricsOptions(void) {
    struct LyricsOptions *lrc_options = (struct LyricsOptions *)
                                        malloc(sizeof(struct LyricsOptions));
    if (!lrc_options)
        return NULL; // Error handle
    lrc_options->lyrics_file_path = NULL;
    lrc_options->has_lyrics = -1; // Set unknown status
    return lrc_options;
}

int main(int argc,char *argv[]) {
    opterr = 0; // Disable getopt error
    int option;
    struct LyricsOptions *lrc_options = initLyricsOptions();
    if (!lrc_options)
        return -ENOMEM;
    while ((option = getopt(argc,argv,"nl:")) != -1) {
        switch (option) {
            case 'n': if (lrc_options->has_lyrics != -1) {
                          break;
                      }
                      lrc_options->has_lyrics = 0;
                      break;
            case 'l': if (lrc_options->has_lyrics != -1) {
                          break; // Ignore this parameter when has_lyrics has been set
                      }
                      lrc_options->has_lyrics = 1;
                      lrc_options->lyrics_file_path = (char *) malloc(sizeof(char)*(strlen(optarg)+1));
                      lrc_options->lyrics_file_path[strlen(optarg)] = '\0';
                      break;
            case '?' : if (optopt == 'l') {
                           fprintf(stderr,"Parameter %c is missing parameter\n",optopt); // Judge whether -l option missing parameter
                       } else {
                      fprintf(stderr,"Unknown option:%c\n",(char) optopt);
                       }
                       printf_usage();
                       destroyLyricsOptions(lrc_options);
                       return -EINVAL;
            default : break;
        }
    }
#ifdef DEBUG
    for (int i=0;i<argc;i++) {
        printf("argv[%d]:%s\n",i,argv[i]);
    }
#endif
    if (optind >= argc) {
        fprintf(stderr,"Missing file path.\n");
        printf_usage();
        return -EINVAL;
    }
    uint32_t whole_len=0;
    for (int i=optind;i<argc;i++) {
        whole_len += strlen(argv[i]);
        if (i != argc)
            whole_len +=1;
    }
    char *uri = malloc(sizeof(char)*whole_len);
    memset (uri,0,whole_len);
    char *puri = uri;
    for (int i=optind;i<argc;i++) {
        strncpy(puri,argv[i],strlen(argv[i]));
        puri += strlen(argv[i]);
        if (i != argc) {
            *puri = ' ';
            puri++;
        }
    }
    uri[whole_len-1] = '\0';
#ifdef DEBUG
    printf("Uri:%s\n",uri);
#endif
    MusicInfo *minfo = (MusicInfo *) malloc(sizeof(MusicInfo));
    if (!minfo) {
        destroyLyricsOptions(lrc_options);
        return -ENOMEM;
    }
    minfo->uri = uri;
    minfo->lrc_options = lrc_options;
    int r= CreatePlayerInstance(minfo);
    if (r < 0) {
        fprintf(stderr,"Failed to create player.Error code:%d\n",r);
        return -EINVAL;
    }
    StartPlay();
    StartTimer();
    return 0;
}

static void printf_usage(void) {
    printf("Usage:openslplay [-n] [-l lyrics_file_path] filepath\n");
    printf("-n:Disable lyrics.\n");
    printf("-l:Specific a lyrics file for this audio file.\n");
}
