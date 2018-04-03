#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <audioplayer.h>
#include <audiotimer.h>
#include <signal.h>

#ifdef ENABLE_CUSTOM_SIG_HANDLER
static void sig_handler(int sig)
{
    switch (sig)
    {
        case SIGKILL : //Fall through
        case SIGQUIT : //Fall through
        case SIGINT: {
                         printf("Music player is interrupted.\n");
                         exit(1);
                         break;
                     }
        default : printf("Unknown signal received:%d\n",sig);
    }
}
#endif

int main(int argc,char *argv[]){
  if (argc!=2)
  {
    printf("Usage: openslplay <filepath>\n");
    return -1;
  }
#ifdef DEBUG
  for (int i=0;i<argc;i++)
  {
      printf("argv %d:%s\n",i,argv[i]);
  }
#endif // DEBUG
#ifdef ENABLE_CUSTOM_SIG_HANDLER
signal(SIGINT,sig_handler);
signal(SIGQUIT,sig_handler);
signal(SIGKILL,sig_handler);
#endif // ENABLE_CUSTOM_SIG_HANDLER
  TimerParameters *params=malloc(sizeof(TimerParameters));
  char *uri=argv[1];
  int result = CreatePlayerInstance(uri,params);
  if (result != 0) {
    printf("Failed to create player!\n");
    return -1;
  }
  StartPlay();
  result = CreateTimer(params);
  if (result < 0) {
    return -1;
  }
  StartTimer();
  return 0;
}
