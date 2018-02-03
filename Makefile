PROGRAMNAME=openslplay
SRCS+=main.c
SRCS+=opensl_instance.c
SRCS+=decoder.c

OBJS=$(SRCS:.c=.o)
CFLAGS+=-I. -std=c99
LDFLAGS+= -L/system/lib64 -lOpenSLES -lavformat -lavcodec -lswresample -lavutil -llog -lm -lz

all: player
player: $(OBJS)
	gcc -o $(PROGRAMNAME) $(OBJS) $(LDFLAGS)
clean:
	rm -rf $(PROGRAMNAME) $(OBJS)
