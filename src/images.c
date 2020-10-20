/*
Little Big Adventure 2: image display
Copyright (C) 2006 The LBA2Engine team
Copyright (C) 2002-2006 The TwinE team

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

#include "main.h"
#include "images.h"
#include "hqrLib.h"
#include "screen.h"
#include "music.h"

extern SDL_Surface *screen;

void convert_pal_2_RGBA(unsigned char * palSource, unsigned char * palDest)
{
  int i;

  for (i = 0; i < 256; i++)
  {
    palDest[0] = palSource[0];
    palDest[1] = palSource[1];
    palDest[2] = palSource[2];
    palDest+=4;
    palSource+=3;
  }
}

void AdelineLogo(void)
{
	play_adeline_music();
	hqr_load("screen.hqr", workVideoBuffer, 0);
	copy_screen(workVideoBuffer, frontVideoBuffer);
	hqr_load("screen.hqr", palette, 1);
	convert_pal_2_RGBA(palette, paletteRGBA);
	cross_fade((char *) frontVideoBuffer, (char *) paletteRGBA);
	delay(7000);
	free_adeline_music();
}

void copy_screen(unsigned char * source, unsigned char * destination)
{
	memcpy(destination, source, 307200);
}

void fade_in(unsigned char * palette)
{
	int i = 100;

	for (i = 0; i < 100; i += 3)
	{
		adjust_palette(255, 255, 255, palette, i);
		//readKeyboard();
	}
}

void adjust_palette(unsigned char R, unsigned char G, unsigned char B, unsigned char * palette, int intensity)
{
	unsigned char localPalette[1024];
	unsigned char *newR;
	unsigned char *newG;
	unsigned char *newB;
	unsigned char *newA;

	int local;
	int counter = 0;
	int i;

	local = intensity;

	newR = &localPalette[0];
	newG = &localPalette[1];
	newB = &localPalette[2];
	newA = &localPalette[3];

	for (i = 0; i < 256; i++)
	{
		*newR = regle_trois_32(R, palette[counter], 100, local);
		*newG = regle_trois_32(G, palette[counter + 1], 100, local);
		*newB = regle_trois_32(B, palette[counter + 2], 100, local);
		*newA = 0;

		newR += 4;
		newG += 4;
		newB += 4;
		newA += 4;

		counter += 4;
	}

	set_palette(localPalette);
}

int regle_trois_32(int modifier, int color, int param, int intensity)
{
	if (!param)
		return (color);
	return (((color - modifier) * intensity) / param) + modifier;
}

void load_image(int imageNumber)
{
	hqr_load("screen.hqr", workVideoBuffer, imageNumber);
	copy_screen(workVideoBuffer, frontVideoBuffer);
	hqr_load("screen.hqr", palette, imageNumber+1);
	convert_pal_2_RGBA(palette, paletteRGBA);
	flip(workVideoBuffer);
	fade_2_pal((char*)paletteRGBA);
}

void load_image_cross_fade(int imageNumber)
{
	hqr_load("screen.hqr", workVideoBuffer, imageNumber);
	copy_screen(workVideoBuffer, frontVideoBuffer);
	hqr_load("screen.hqr", palette, imageNumber+1);
	convert_pal_2_RGBA(palette, paletteRGBA);
	cross_fade((char *) frontVideoBuffer, (char *) paletteRGBA);
}

void load_image_cross_fade_delay(int imageNumber, int time)
{
	hqr_load("screen.hqr", workVideoBuffer, imageNumber);
	copy_screen(workVideoBuffer, frontVideoBuffer);
	hqr_load("screen.hqr", palette, imageNumber+1);
	convert_pal_2_RGBA(palette, paletteRGBA);
	cross_fade((char *) frontVideoBuffer, (char *) paletteRGBA);
	delay(time*1000);
}

void fade_2_black(char *palette)
{
	int i = 0;

	if (palReseted == 0)
	{
		for (i = 100; i >= 0; i -= 3)
		{
			adjust_palette(0, 0, 0, (unsigned char *) palette, i);
			//readKeyboard();
		}
	}

	palReseted = 1;
}

void fade_2_pal(char *palette)
{
	int i = 100;

	for (i = 0; i <= 100; i += 3)
	{
		adjust_palette(0, 0, 0, (unsigned char *) palette, i);
		//readKeyboard();
	}

	set_palette( (unsigned char*)palette );

	palReseted = 0;

}

void black_2_white(void)
{
	unsigned char palette[1024];
	int i;

	i = 256;
	for (i = 0; i < 256; i += 3)
	{
		memset(palette, i, 1024);

		set_palette(palette);
		//readKeyboard();
	}
}

void set_back_pal(void)
{
	memset(palette, 0, 768);
	memset(paletteRGBA, 0, 1024);

	set_palette(paletteRGBA);

	palReseted = 1;
}

void clear_screen(void)
{
	memset(frontVideoBuffer, 0, 307200);
}
