PROGRAMNAME=openslplay
SRCS+=main.c
SRCS+=audioplayer.c
SRCS+=audiodecoder.c
SRCS+=uriplayer.c
SRCS+=audiotimer.c
SRCS+=lyricsreader.c
SRCS+=musiclyrics.c

ifeq ($(findstring arm,$(shell uname -m)), arm)
	LIBPATH:=/system/lib
else ifeq ($(findstring aarch64,$(shell uname -m)), aarch64)
	LIBPATH:=/system/lib64
endif
OBJS=$(SRCS:.c=.o)
CFLAGS+=-I. -std=c99 -Iinclude -Wall -Wextra -DDEBUG
LDFLAGS+= -Lprebuilt -larray -lOpenSLES -lavformat -lavcodec -lswresample -lavutil -llog -lm -lz -Wl,-rpath=$(LIBPATH)


all: player

precompile:
	./precompile.sh

configure: precompile
	@echo "Configure finished."

player: $(OBJS)
	$(CC) -o $(PROGRAMNAME) $(OBJS) $(LDFLAGS)
	@termux-elf-cleaner $(PROGRAMNAME) >> /dev/null
.PHNOY: clean
clean:
	@rm -rf $(PROGRAMNAME) $(OBJS)
distclean: clean
	./removefiles.sh
