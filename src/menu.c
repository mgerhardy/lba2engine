/*
Little Big Adventure 2: main
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

#include "menu.h"
#include "music.h"
#include "images.h"
#include "screen.h"

void main_menu()
{
	load_image_cross_fade(4);	
	// TODO: stop samples
	// TODO: get current.lba save game

	//do
	//{
		// TODO: init dial
		play_music(1); // LBA's Theme
		// TODO: stop samples
		// TODO: process menu
		delay(1000000);
		
		// TODO: give actions according the process menu
	//}while();
	free_adeline_music();
}
