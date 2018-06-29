PROGRAMNAME=openslplay
WSRCS+=wrapper.c
SRCS+=main.c
SRCS+=audioplayer.c
SRCS+=audiodecoder.c
SRCS+=uriplayer.c
SRCS+=audiotimer.c
SRCS+=lyricsreader.c
SRCS+=musiclyrics.c

ifeq ($(findstring arm,$(shell uname -m)), arm)
	LIBPATH:=/system/lib
	ARCH:=arm
else ifeq ($(findstring aarch64,$(shell uname -m)), aarch64)
	LIBPATH:=/system/lib64
	ARCH:=aarch64
endif
WOBJS=$(WSRCS:.c=.o)
OBJS=$(SRCS:.c=.o)
CFLAGS+=-I. -std=c99 -Iinclude -Wall -Wextra
LDFLAGS+= -Lprebuilt -larray -lavformat -lavcodec -lswresample -lavutil -lOpenSLES -llog -lm -lz -Wl,-rpath,$(LIBPATH) -Wl,-rpath-link,$(LIBPATH)
-include config.mk

ifeq ($(ENABLE_DEBUG),true)
	CFLAGS += -DDEBUG
endif
ifeq ($(ENABLE_WERROR),true)
	CFLAGS += -Werror
endif
ifneq ($(C_COMPILER),)
	CC := $(C_COMPILER)
endif

ifeq ($(strip $(NEVER_ALLOW_ERROR)),true)
	CFLAGS+= -DNEVER_ALLOW_ERROR
endif

ifeq ($(ARCH),arm)
	CFLAGS+= -DARCH_ARM
else ifeq ($(ARCH),aarch64)
	CFLAGS+= -DARCH_AARCH64
endif

all: player

precompile:
	@./precompile.sh

configure: precompile
	@echo "Configure finished."


player: $(OBJS) $(WOBJS)
	$(CC) -o $(PROGRAMNAME) $(WOBJS)
	$(CC) -o binary $(OBJS) $(LDFLAGS)
	@termux-elf-cleaner $(PROGRAMNAME) >> /dev/null

.PHNOY: clean
clean:
	@rm -rf $(PROGRAMNAME) $(OBJS) $(WOBJS) binary

distclean: clean
	@./removefiles.sh
