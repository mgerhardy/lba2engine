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

#ifndef SCREEN_H
#define SCREEN_H

#define SPEED 15             /* Ticks per Frame */
#define SLEEP_MIN 2          /* Minimum time a sleep takes, usually 2*GRAN */
#define SLEEP_GRAN 1         /* Granularity of sleep */

int lba_time;
int fullscreen;

unsigned char *workVideoBuffer;
unsigned char *frontVideoBuffer;
unsigned char *frontVideoBufferbis;

static void atexit_callback(void);
void sdl_close();
int sdl_initialize();
void sdl_main_loop();
void delay(int time);
void put_pixel(int x, int y, int pixel);
void set_color(unsigned char i, unsigned char R, unsigned char G, unsigned char B);
void set_palette(unsigned char * palette);
void fade_black_2_white();
void flip(unsigned char *videoBuffer);
void copy_block_phys(unsigned char *videoBuffer, int left, int top, int right, int bottom);
void init_screen_buffer(char *buffer, int width, int height);
void cross_fade(char *buffer, char *palette);
void toggle_fullscreen();

#endif