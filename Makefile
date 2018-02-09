PROGRAMNAME=openslplay
SRCS+=main.c
SRCS+=audioplayer.c
SRCS+=audiodecoder.c
SRCS+=uriplayer.c
SRCS+=urimain.c

OBJS=$(SRCS:.c=.o)
CFLAGS+=-I. -std=c99
LDFLAGS+= -L/system/lib64 -lOpenSLES -lavformat -lavcodec -lswresample -lavutil -llog -lm -lz -Wl,-rpath=/system/lib64

all: player
player: $(OBJS)
	$(CC) -o $(PROGRAMNAME) $(OBJS) $(LDFLAGS)
clean:
	@rm -rf $(PROGRAMNAME) $(OBJS)
