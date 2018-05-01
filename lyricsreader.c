#include <stdio.h>
#include <stdlib.h>
#include <libarray/array.h>
#include <musiclyrics.h>

static ArrayList *getLyricsList(FILE *fptr);
static ArrayList *ResolveInfo(ArrayList *infolist);
static uint32_t ResolveTimeline(char *timechar);
#ifdef DEBUG
static inline void printf_table(ArrayList *rlist);
#endif
static void DataDestroy(ArrayList *list);
static int isLargeSize(FILE *fptr);
static void LyricsListQuickSort(ArrayList *rlist,int32_t left,int32_t right);
static void LyricsInfoSwap(ArrayList *rlist,uint32_t indexa,uint32_t indexb);
static uint32_t getLyricsListTimeline(ArrayList *rlist,uint32_t index);

static uint32_t getLyricsListTimeline(ArrayList *rlist,uint32_t index)
{
    if (rlist==NULL)
        return 0;
    LyricsInfo *element;
    rlist->get(rlist,index,&element);
    return element->timeline;
}

static void LyricsInfoSwap(ArrayList *rlist,uint32_t indexa,uint32_t indexb)
{
    if (rlist==NULL||indexa==indexb)
        return;
    LyricsInfo *lyricsa;
    LyricsInfo *lyricsb;
    rlist->get(rlist,indexa,&lyricsa);
    rlist->get(rlist,indexb,&lyricsb);
    rlist->set(rlist,indexa,lyricsb);
    rlist->set(rlist,indexb,lyricsa);
}

static void LyricsListQuickSort(ArrayList *rlist,int32_t left,int32_t right)
{
    if (rlist==NULL||left>=right)
        return;
    int32_t i=left;
    int32_t j=right;
    uint32_t key=getLyricsListTimeline(rlist,left);
    while (i<j)
    {
        while (i<j&&getLyricsListTimeline(rlist,j)>=key)
        {
            j--;
        }
        while (i<j&&getLyricsListTimeline(rlist,i)<=key)
        {
            i++;
        }
        if (i<j)
        {
            LyricsInfoSwap(rlist,i,j);
        }
    }
    LyricsInfoSwap(rlist,left,i);
    LyricsListQuickSort(rlist,left,i-1);
    LyricsListQuickSort(rlist,i+1,right);
}

static int isLargeSize(FILE *fptr)
{
    if (fptr==NULL)
        return -1;
    fseek(fptr,0L,SEEK_END);
    uint32_t size=ftell(fptr);
    fseek(fptr,0L,SEEK_SET);
    if (size>100*1024)
    {
#ifdef DEBUG
	printf("Error:File to large,file size is %u\n",size);
#endif
        return -1;
    }
#ifdef DBUG
    printf("Info:Lyrics File size detected %u\n",size);
#endif
    return 0;
}

ArrayList *getResolvedLyrics(FILE *fptr)
{
    if (fptr==NULL)
        return NULL;
    if (isLargeSize(fptr)!=0)
    {
        return NULL;
    }
    ArrayList *list=getLyricsList(fptr);
    ArrayList *rlist=ResolveInfo(list);
    LyricsListQuickSort(rlist,0,rlist->length(rlist));
#ifdef DEBUG
    printf_table(rlist);
#endif
    DataDestroy(list);
    fclose(fptr);
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
        for (uint32_t i=0;i<len;i++)
        {
            list->get(list,i,&cptr);
            free(cptr);
        }
        list->destroy(list);
    }
}

#ifdef DEBUG
static void printf_table(ArrayList *rlist)
{
    LyricsInfo *lrcinfo;
    for (uint32_t i=0;i<rlist->length(rlist);i++)
    {
        rlist->get(rlist,i,&lrcinfo);
        printf("Time:%dms",lrcinfo->timeline);
        printf("    ");
        printf("%s\n",lrcinfo->lyrics);
    }
}
#endif

static ArrayList *getLyricsList(FILE *fptr)
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

static ArrayList *ResolveInfo(ArrayList *infolist)
{
    if (infolist==NULL)
        return NULL;
    ArrayList *lyricslist=CreateArrayList(ARRAY_TYPE_POINTER);
    uint32_t lrclen=infolist->length(infolist);
    if (lrclen==0)
        return NULL; // Can resolve empty list;
    char *buffer;
    for (uint32_t i=0;i<lrclen;i++)
    {
        infolist->get(infolist,i,&buffer);
        int size=strlen(buffer);
        int lastchar=0;
        int charindex=0;
        char *lyricschar=malloc(sizeof(char)*1024);
        memset(lyricschar,0,1024*sizeof(char));
        for (int j=0;j<size;j++)
        {
            if (buffer[j]==']')
            {
                lastchar=j;
            }
        }
        if (lastchar==0||lastchar>11)
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
                char *timechar=malloc(sizeof(char)*10);
                memset(timechar,0,10);
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
                        timechar[charindex]='\0';
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
    char *min=malloc(sizeof(char)*3);
    char *sec=malloc(sizeof(char)*3);
    char *ms=malloc(sizeof(char)*4);
    memset(min,0,3*sizeof(char));
    memset(sec,0,3*sizeof(char));
    memset(ms,0,4*sizeof(char));
    int charindex=0;
    char **operatechar=&min;
    int nowitem=0;
    for (int i=0;i<len;i++)
    {
        if (timechar[i]==':'&&nowitem==1)
        {

            operatechar=&sec;
            charindex=0;
            continue;
        }
        if (timechar[i]=='.'&&nowitem==2)
        {
            operatechar=&ms;
            charindex=0;
            continue;
        }
        (*operatechar)[charindex]=timechar[i];
        charindex++;
        if ((nowitem==0||nowitem==1)&&charindex>1)
        {
            nowitem++;
        }
        if (nowitem==2&&charindex>2)
        {
            break;
        }
    }
    min[2]='\0';
    sec[2]='\0';
    ms[3]='\0';
    uint32_t timeline=(atoi(min)*60*1000)+(atoi(sec)*1000)+atoi(ms);
    free(min);
    free(sec);
    free(ms);
    operatechar=NULL;
    return timeline;
}
