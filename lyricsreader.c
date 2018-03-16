#include <stdio.h>
#include <stdlib.h>
#include <libarray/array.h>
#include <musiclyrics.h>

static ArrayList *getLyricsList(FILE *fptr);
static ArrayList *ResolveInfo(ArrayList *infolist);
static uint32_t ResolveTimeline(char *timechar);
static inline void printf_table(ArrayList *rlist);
static void DataDestroy(ArrayList *list);

ArrayList *getResolvedLyrics(FILE *fptr)
{
    if (fptr==NULL)
        return NULL;
    ArrayList *list=getLyricsList(fptr);
    ArrayList *rlist=ResolveInfo(list);
    DataDestroy(list);
    return rlist;
}

static void DataDestroy(ArrayList *list)
{
    if (NULL==list)
        return;
    char *cptr;
    uint32_t len=list->length(list);
    if (len==0)
    {
        list->destroy(list);
    }
    else
    {
        for (int i=0;i<len;i++)
        {
            list->get(list,i,&cptr);
            free(cptr);
        }
        list->destroy(list);
    }
}

void printf_table(ArrayList *rlist)
{
    LyricsInfo *lrcinfo;
    for (int i=0;i<rlist->length(rlist);i++)
    {
        rlist->get(rlist,i,&lrcinfo);
        printf("Time:%dms",lrcinfo->timeline);
        printf("    ");
        printf("%s\n",lrcinfo->lyrics);
    }
}

ArrayList *getLyricsList(FILE *fptr)
{
    if (NULL==fptr)
        return NULL;
    ArrayList *list=CreateArrayList(ARRAY_TYPE_POINTER);
    const int buffersize=1024;
    char *buffer=malloc(sizeof(char)*buffersize); // Set up a 1 Kbytes buffer
    while(fgets(buffer,buffersize,fptr)!=NULL&&buffer[0]!='\n')
    {
        buffer[strlen(buffer)-1]='\0';
        list->add(list,buffer);
        buffer=malloc(sizeof(char)*buffersize);
    }
    return list;
}

ArrayList *ResolveInfo(ArrayList *infolist)
{
    if (infolist==NULL)
        return NULL;
    ArrayList *lyricslist=CreateArrayList(ARRAY_TYPE_POINTER);
    uint32_t lrclen=infolist->length(infolist);
    if (lrclen==0)
        return NULL; // Can resolve empty list;
    char *buffer;
    for (int i=0;i<lrclen;i++)
    {
        infolist->get(infolist,i,&buffer);
        int size=strlen(buffer);
        int lastchar=0;
        int charindex=0;
        char *lyricschar=malloc(sizeof(char)*1024);
        memset(lyricschar,0,1024);
        for (int j=0;j<size;j++)
        {
            if (buffer[j]==']')
            {
                lastchar=j;
            }
        }
        if (lastchar==0)
        {
            printf("Find invalid lyrics!\n");
            free(lyricschar);
            continue;
        }
        for (int j=lastchar+1;j<size;j++)
        {
            lyricschar[charindex]=buffer[j];
            charindex++;
            if (j==size-1)
            {
                lyricschar[charindex+1]='\0';
            }
        }
        charindex=0;
        for (int j=0;j<=lastchar;j++)
        {
            if (buffer[j]=='[')
            {
                char *timechar=malloc(sizeof(char)*25);
                memset(timechar,0,25);
                charindex=0;
                for (int m=j+1;m<=lastchar;m++)
                {
                    if (buffer[m]==']')
                    {
                        break;
                    }
                    timechar[charindex]=buffer[m];
                    charindex++;
                    if (buffer[m+1]==']')
                    {
                        timechar[charindex+1]='\0';
                    }
                }
                uint32_t timeline=ResolveTimeline(timechar);
                free(timechar);
                LyricsInfo *lrcinfo=malloc(sizeof(LyricsInfo));
                lrcinfo->timeline=timeline;
                lrcinfo->lyrics=lyricschar;
                lyricslist->add(lyricslist,lrcinfo);
            }

        }

    }
    return lyricslist;
}

uint32_t ResolveTimeline(char *timechar)
{
    if (timechar==NULL)
        return 0;
    int len=strlen(timechar);
    if (len<=0)
        return 0;
    char *min=malloc(sizeof(char)*10);
    char *sec=malloc(sizeof(char)*10);
    char *ms=malloc(sizeof(char)*12);
    memset(min,0,10);
    memset(sec,0,10);
    memset(ms,0,12);
    int charindex=0;
    char **operatechar=&min;
    for (int i=0;i<len;i++)
    {
        if (timechar[i]==':')
        {
            operatechar=&sec;
            charindex=0;
            continue;
        }
        if (timechar[i]=='.')
        {
            operatechar=&ms;
            charindex=0;
            continue;
        }
        (*operatechar)[charindex]=timechar[i];
        charindex++;
        if (timechar[i+1]==':'||timechar[i+1]=='.')
        {
            (*operatechar)[charindex+1]='\0';
        }
    }
    uint32_t timeline=(atoi(min)*60*1000)+(atoi(sec)*1000)+atoi(ms);
    free(min);
    free(sec);
    free(ms);
    operatechar=NULL;
    return timeline;
}
