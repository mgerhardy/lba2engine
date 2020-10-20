/*
Little Big Adventure 2: music
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
#include <SDL/SDL_mixer.h>

#include "music.h"
#include "main.h"
#include "screen.h"

SDL_CD *cdrom;
const char *cdname;

Mix_Chunk *sound = NULL;
int channel;

int init_cdrom()
{
	int numOfCDROM;
	int cdNum;

	numOfCDROM = SDL_CDNumDrives();
	printf("Found %d CDROM devices\n", numOfCDROM);

	if (!numOfCDROM)
	{
		fprintf(stderr, "No CDROM devices available\n");
		exit(-1);
	}

	for(cdNum=0; cdNum < numOfCDROM; cdNum++)
	{
		cdname = SDL_CDName(cdNum);
		printf("Testing drive %s\n", cdname);
		cdrom = SDL_CDOpen(cdNum);
		if(!cdrom)
		{ 
			fprintf(stderr, "Couldn't open drive: %s\n", SDL_GetError()); 
		} 
		else 
		{
			SDL_CDStatus(cdrom);
			if(cdrom->numtracks==2 || cdrom->numtracks==8) // EU=2 / US=8
			{
				if(cdrom->numtracks==2){
					printf("Assuming that it is LBA2 cd... %s\n",cdname);
					directory = "LBA2";
					cfgfile.UseCD = 1;
				}
				else if(cdrom->numtracks==8){
					printf("Assuming that it is TWINSEN cd... %s\n",cdname);
					directory = "TWINSEN";
					cfgfile.UseCD = 1;
				}
				return(1);
			}
		}
		// not found the right CD
		cfgfile.UseCD = 0;
		SDL_CDClose(cdrom);
  }

  cdrom=NULL;

  printf("Can't find LBA2 CD!\n");
  return (0);
}

void stop_cdrom()
{
	if(cdrom!=NULL)
	{
		if(SDL_CDStatus(cdrom) == CD_PLAYING)
			fade_music(500)		;
	
		SDL_CDStop(cdrom);
	}
}

void play_adeline_music()
{
	char logofile[256];
	char *filename = "MUSIC\\LOGADPCM.WAV";

	if(cfgfile.UseCD)
	{
		strcpy(logofile, cdname);
		strcat(logofile, directory);
		strcat(logofile, "\\");
		strcat(logofile, filename);
	}
	else
	{
		strcpy(logofile, filename);
	}
  
	sound = Mix_LoadWAV(logofile);
	if(sound == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}
	 
	channel = Mix_PlayChannel(-1, sound, 0);
	if(channel == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
}

void play_music(int track)
{
	if(cdrom->numtracks == 8)
	{
		if(CD_INDRIVE(SDL_CDStatus(cdrom)))
			SDL_CDPlayTracks(cdrom, track, 0, 1, 0);
	}
	else if(cdrom->numtracks == 2)
	{
		if(track==1)
		{
			if(CD_INDRIVE(SDL_CDStatus(cdrom)))
				SDL_CDPlayTracks(cdrom, track, 0, 1, 0);
		}
		else
		{
			char logofile[256];
			char tracknum[2];
			strcpy(logofile, "MUSIC\\TADPCM");
			sprintf(tracknum, "%d",(track+2));
			strcat(logofile, tracknum);
			strcat(logofile, ".WAV");
	  
			sound = Mix_LoadWAV(logofile);
			if(sound == NULL) {
				fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
			}
			 
			channel = Mix_PlayChannel(-1, sound, 0);
			if(channel == -1) {
				fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
			}
		}
	}
}

void fade_music(int time)
{
	Mix_FadeOutChannel(-1, time);
	delay(time);
}

void free_adeline_music()
{
	fade_music(500);
	if(sound!=NULL)
		Mix_FreeChunk(sound);
}
