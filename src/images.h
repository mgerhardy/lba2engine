/*
Little Big Adventure 2: image display
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

#ifndef IMAGES_H
#define IMAGES_H

unsigned char palette[256 * 3];
unsigned char palette2[256 * 3];
unsigned char paletteRGBA[256 * 4];

short int palReseted;

void convert_pal_2_RGBA(unsigned char * palSource, unsigned char * palDest);
void AdelineLogo(void);
void copy_screen(unsigned char * source, unsigned char * destination);
void fade_in(unsigned char * palette);
void adjust_palette(unsigned char R, unsigned char G, unsigned char B, unsigned char * palette, int intensity);
int regle_trois_32(int modifier, int color, int param, int intensity);
void load_image(int imageNumber);
void load_image_cross_fade(int imageNumber);
void load_image_cross_fade_delay(int imageNumber, int time);
void fade_2_black(char *palette);
void fade_2_pal(char *palette);
void black_2_white(void);
void set_back_pal(void);
void clear_screen(void);

#endif
