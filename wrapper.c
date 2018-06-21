#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,const char **argv,const char **envp)
{
    (void)argc;
    char buf[PATH_MAX];
    char *env = getenv("LD_LIBRARY_PATH");
#if defined(ARCH_ARM)
    	strncpy(buf,"/system/lib:",sizeof("/system/lib:"));
	strncat(buf,env,strlen(env));
	setenv("LD_LIBRARY_PATH",buf,1);
#elif defined(ARCH_AARCH64)
	strncpy(buf,"/system/lib64:",sizeof("/system/lib64:"));
	strncat(buf,env,strlen(env));
	setenv("LD_LIBRARY_PATH",buf,1);
#else
#error Unsupported architecture.Only support ARM and AARCH64.
#endif
#if defined(DEBUG)
	printf("ENV: LD_LIBRARY_PATH:%s\n",buf);
#endif
	memset(buf,0x0,PATH_MAX);
	env = getenv("PWD");
	strncpy(buf,env,strlen(env));
	strncat(buf,"/binary",sizeof("/binary"));
	return execve(buf,(char *const*)argv,(char *const*)envp);
}
