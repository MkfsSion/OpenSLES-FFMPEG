#include <stdio.h>
#include <musiclyrics.h>
#include <stdlib.h>

static int lrcindex=-1;
static ArrayList *reslist=NULL;

static char *getLyricsSuffix(const char *filename);
static inline void ResourceListDestroy(ArrayList *list);

int InitLyricsReader(const char *filename)
{
    FILE *pfile=fopen(filename,"rb");
    if (pfile==NULL)
        return -1;
    fclose(pfile);
    char *lrcpath=getLyricsSuffix(filename);
    if (lrcpath==NULL)
        return -1;
    FILE *plrcfile=fopen(lrcpath,"rb");
    free(lrcpath);
    if (plrcfile==NULL)
        return -1;
    reslist=getResolvedLyrics(plrcfile);
    if (reslist==NULL)
        return -1;
    return 0;
}

static char *getLyricsSuffix(const char *filename)
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
    uint32_t len=reslist->length(reslist);
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
    for (int i=0;i<len;i++)
    {
        list->get(list,i,&lrcinfo);
        free(lrcinfo);
    }
    list->destroy(list);
}