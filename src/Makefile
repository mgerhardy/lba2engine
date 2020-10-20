all: lba2engine

CC=gcc
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`
CFLAGS=-ggdb -DUNIX -Wall $(SDL_CFLAGS)
OBJS=screen.o hqrLib.o images.o  music.o menu.o main.o

LIBS=$(SDL_LIBS) -lSDLmain -lSDL_mixer

lba2engine: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJS)