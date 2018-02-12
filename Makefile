PROGRAMNAME=openslplay
SRCS+=main.c
SRCS+=audioplayer.c
SRCS+=audiodecoder.c
SRCS+=uriplayer.c

ifeq ($(findstring arm,$(shell uname -m)), arm)
	LIBPATH:=/system/lib
else ifeq ($(findstring aarch64,$(shell uname -m)), aarch64)
	LIBPATH:=/system/lib64
endif
OBJS=$(SRCS:.c=.o)
CFLAGS+=-I. -std=c99
LDFLAGS+= -lOpenSLES -lavformat -lavcodec -lswresample -lavutil -llog -lm -lz -Wl,-rpath=$(LIBPATH)

all: player
player: $(OBJS)
	$(CC) -o $(PROGRAMNAME) $(OBJS) $(LDFLAGS)
clean:
	@rm -rf $(PROGRAMNAME) $(OBJS)
