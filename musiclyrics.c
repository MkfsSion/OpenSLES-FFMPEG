#include <stdio.h>
#include <musiclyrics.h>
#include <musicinfo.h>
#include <stdlib.h>
#include <config.h>

static int lrcindex=-1;
static ArrayList *reslist=NULL;
static int valid=0;

static char *getLyricsFilePath(const char *filename);
static inline void ResourceListDestroy(ArrayList *list);

int isLyricsReaderEnable(void)
{
    return valid;
}

int InitLyricsReaderWithOptions(struct LyricsOptions *lrc_options) {
    if (!lrc_options)
        return -1;
    if (lrc_options->has_lyrics) {
    const char *lrc_path=lrc_options->lyrics_file_path;
#ifdef DEBUG
    printf("The value of lrc_path:%s\n",lrc_path);
#endif
    int r=InitLyricsReader(NULL,lrc_path);
    destroyLyricsOptions(lrc_options);
    return r;
    } else {
#ifdef DEBUG
	printf("Disable lyrics for user setting.\n");
#endif
        valid = 0;
        return -1;
    }
}

int InitLyricsReader(const char *rfilename,const char *lrcfilename)
{
    FILE *pfile=fopen(rfilename,"rb");
    FILE *lrcfile=fopen(lrcfilename,"rb");
    if (pfile==NULL&&lrcfile==NULL) {
#ifdef DEBUG
	printf("InitLyricsReader:Invalid arguments.\n");
#endif
        return -1;
    }
    if (lrcfile==NULL)
    {
    fclose(pfile);
    char *lrcpath=getLyricsFilePath(rfilename);
    if (lrcpath==NULL) {
#ifdef DEBUG
	printf("Error:Failed to get lyrics filename by source file nsme.\n");
#endif
        return -1;
    }
    FILE *plrcfile=fopen(lrcpath,"rb");
    free(lrcpath);
    if (plrcfile==NULL) {
#ifdef DEBUG
	printf("Error:Opening file %s failed,file not exist.\n",lrcpath);
#endif
	never_allow_test("lyrics file failed to open");
        return -1;
    }
    reslist=getResolvedLyrics(plrcfile);
    }
    else
    {
        if (pfile!=NULL)
        {
            fclose(pfile);
        }
        reslist=getResolvedLyrics(lrcfile);
    }
    if (reslist==NULL)
        return -1;
    valid=1;
#ifdef DEBUG
    printf("Lyrics is eanbled now.\n");
#endif
    return 0;
}

static char *getLyricsFilePath(const char *filename)
{
    int charindex=0;
    int len=strlen(filename);
    for (int i=0;i<len;i++)
    {
        if (filename[i]=='.')
        {
            charindex=i;
        }
    }
    if (charindex==0)
        return NULL;
    char *lrcpath=malloc(sizeof(char)*(len+4));
    for (int i=0;i<=charindex;i++)
    {
        lrcpath[i]=filename[i];
    }
    lrcpath[charindex+1]='l';
    lrcpath[charindex+2]='r';
    lrcpath[charindex+3]='c';
    lrcpath[charindex+4]='\0';
    return lrcpath;
}

char *getLyricsStr(uint32_t timeline)
{
    LyricsInfo *lrcinfo;
    LyricsInfo *nextlrcinfo;
    int64_t len=(int64_t) reslist->length(reslist);
    if (lrcindex==-1)
    {
        reslist->get(reslist,0,&lrcinfo);
        if (timeline>=lrcinfo->timeline)
        {
            lrcindex++;
        }
    }
    if (lrcindex==-1)
        return "";
    reslist->get(reslist,lrcindex,&lrcinfo);
    if (lrcindex<len-1)
    {
        reslist->get(reslist,lrcindex+1,&nextlrcinfo);
    }
    else
    {
        reslist->get(reslist,lrcindex,&nextlrcinfo);
    }
    if (timeline>=lrcinfo->timeline&&timeline<nextlrcinfo->timeline)
    {
        return lrcinfo->lyrics;
    }
    else
    {
        if (lrcindex<len-1)
        {
        lrcindex++;
        }
        return nextlrcinfo->lyrics;
    }
}
void ReleaseLyricsReader(void)
{
    ResourceListDestroy(reslist);
    reslist=NULL;
    lrcindex=-1;
}

static inline void ResourceListDestroy(ArrayList *list)
{
    LyricsInfo *lrcinfo;
    uint32_t len=list->length(list);
    for (uint32_t i=0;i<len;i++)
    {
        list->get(list,i,&lrcinfo);
        free(lrcinfo);
    }
    list->destroy(list);
}
