/*
Little Big Adventure 2: SDL main routines
Copyright (C) 2006 The LBA2Engine team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mixer.h>

#include "screen.h"
#include "images.h"
#include "music.h"

char *tempBuffer;
SDL_Surface *screenBuffer=NULL;
SDL_Surface *bufferStretch=NULL;
SDL_Surface *bufferRGBA=NULL;
SDL_Surface *screen=NULL;
SDL_Color screenColors[256];
SDL_Surface *surfaceTable[16];

static void atexit_callback(void)
{
	//TODO: Stop Musics
	Mix_CloseAudio();
	stop_cdrom();
	SDL_Quit();
}

void sdl_close()
{
	atexit(atexit_callback);
}

int sdl_initialize() // object used for the SDL port                      
{
  unsigned char *keyboard;
  int size;
  int i;

  Uint32 rmask, gmask, bmask, amask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif
// SDL_INIT_VIDEO|SDL_INIT_CDROM|SDL_INIT_AUDIO
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  #ifdef DEBUG
    SDL_version compile_version, *link_version;
    SDL_VERSION(&compile_version);
    printf("compiled with SDL version: %d.%d.%d\n", 
            compile_version.major,
            compile_version.minor,
            compile_version.patch);
    link_version = SDL_Linked_Version();
    printf("running with SDL version: %d.%d.%d\n", 
            link_version->major,
            link_version->minor,
            link_version->patch);
	#endif

	sdl_close();

	printf("Initialising SDL device. Please wait...\n");

	if(Mix_OpenAudio(44100, AUDIO_S16, 2, 4096) < 0)
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(1);
	}

	printf("Initialising Sample device. Please wait...\n");

	SDL_WM_SetCaption("Little Big Adventure 2 Redux", "LBA2");
	SDL_PumpEvents();

	keyboard = SDL_GetKeyState(&size);

	keyboard[SDLK_RETURN] = 0;

	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);

	if (screen == NULL)
	{
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
		exit(1);
	}

	for (i = 0; i < 16; i++)
	{
		surfaceTable[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32, rmask, gmask, bmask, 0);
	}

	return 0;
}

void sdl_main_loop()
{
	long int t_start,t_left;
	long unsigned int t_end;
	long int q=0;              /* Dummy */

  while(1) // TODO: break the loop
  {
    t_start=SDL_GetTicks();

    // main game loop

    t_end=t_start+SPEED;
    t_left=t_start-SDL_GetTicks()+SPEED;

    if(t_left>0)
    {
        if(t_left>SLEEP_MIN)
        {
            SDL_Delay(t_left-SLEEP_GRAN);
        }
        while(SDL_GetTicks()<t_end)
        {
          q++;
        };
    }
    lba_time++;
  }
}

void delay(int time)
{
	SDL_Event keyevent;
	unsigned int startTicks = SDL_GetTicks();
	unsigned int stopTicks=0;
	
	while(stopTicks < (unsigned int)time)
	{
		SDL_PollEvent(&keyevent);
		if(keyevent.type == SDL_KEYDOWN)
			if(keyevent.key.keysym.sym == SDLK_ESCAPE)
			{
				break;
			}
		stopTicks = SDL_GetTicks()-startTicks;
		SDL_PumpEvents();
	}
}

void put_pixel(int x, int y, int pixel)
{
	int bpp = screen->format->BytesPerPixel;

	Uint8 *p = (Uint8 *) screen->pixels + y * screen->pitch + x * bpp;
	*p = pixel;
}

void set_color(unsigned char i, unsigned char R, unsigned char G, unsigned char B)
{
	screenColors[i].r = R;
	screenColors[i].g = G;
	screenColors[i].b = B;

	SDL_SetColors(screenBuffer, screenColors, i, 1);
}

void set_palette(unsigned char * palette)
{
	SDL_Color *screenColorsTemp = (SDL_Color *) palette;

	SDL_SetColors(screenBuffer, screenColorsTemp, 0, 256);
	SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void fade_black_2_white()
{
	int i;

	SDL_Color colorPtr[256];

	SDL_UpdateRect(screen, 0, 0, 0, 0);

	for (i = 0; i < 256; i += 3)
	{
		memset(colorPtr, i, 1024);
		SDL_SetPalette(screen, SDL_PHYSPAL, colorPtr, 0, 256);
	}
}

void flip(unsigned char *videoBuffer)
{
	SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void copy_block_phys(unsigned char *videoBuffer, int left, int top, int right, int bottom)
{
	SDL_Rect rectangle;

	rectangle.x = left;
	rectangle.y = top;
	rectangle.w = right - left +1 ;
	rectangle.h = bottom - top +1 ;

	SDL_BlitSurface(screenBuffer, &rectangle, screen, &rectangle);
	SDL_UpdateRect(screen, left, top, right - left +1, bottom - top+1);
}

void init_screen_buffer(char *buffer, int width, int height)
{
	screenBuffer = SDL_CreateRGBSurfaceFrom(buffer, width, height, 8, 640, 0, 0, 0, 0);
}

void cross_fade(char *buffer, char *palette)
{
	int i;
	SDL_Surface *backupSurface;
	SDL_Surface *newSurface;
	SDL_Surface *tempSurface;
	Uint32 rmask, gmask, bmask, amask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
	#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
	#endif

    backupSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32, rmask, gmask, bmask, 0);
    newSurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 640, 480, 32, rmask, gmask, bmask, 0);

    tempSurface = SDL_CreateRGBSurfaceFrom(buffer, 640, 480, 8, 640, 0, 0, 0, 0);
    SDL_SetColors(tempSurface, (SDL_Color *) palette, 0, 256);

    SDL_BlitSurface(screen, NULL, backupSurface, NULL);
    SDL_BlitSurface(tempSurface, NULL, newSurface, NULL);

    for (i = 0; i < 8; i++)
    {
      SDL_BlitSurface(backupSurface, NULL, surfaceTable[i], NULL);
      SDL_SetAlpha(newSurface, SDL_SRCALPHA | SDL_RLEACCEL, i * 32);
      SDL_BlitSurface(newSurface, NULL, surfaceTable[i], NULL);
      SDL_BlitSurface(surfaceTable[i], NULL, screen, NULL);
      SDL_UpdateRect(screen, 0, 0, 0, 0);
      delay(50);
	}

    SDL_BlitSurface(newSurface, NULL, screen, NULL);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    SDL_FreeSurface(backupSurface);
    SDL_FreeSurface(newSurface);
    SDL_FreeSurface(tempSurface);
}

void toggle_fullscreen()
{
	SDL_FreeSurface(screen);
	screen = 0;
	
	fullscreen = 1 - fullscreen;
	SDL_FreeSurface(screen);

	if (fullscreen == 1)
	{
		screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
		SDL_SetColors(screen, (SDL_Color *)palette, 0, 256);
		SDL_ShowCursor(1);
	}
	else
	{
		screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE|SDL_FULLSCREEN);

		SDL_SetColors(screen, (SDL_Color *)palette, 0, 256);
		SDL_ShowCursor(0);
	}
	//requestBackgroundRedraw = 1;
}
