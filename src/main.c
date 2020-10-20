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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "screen.h"
#include "images.h"
#include "music.h"
#include "menu.h"

configFile cfgfile;
char * directory;
unsigned char version;

char CFGList[][18]=
{
	"Language:",
	"LanguageCD:",
	"FlagKeepVoice:",
	"FlagDisplayText:",
	"WaveVolume:",
	"VoiceVolume:",
	"MusicVolume:",
	"CDVolume:",
	"MasterVolume:",
	"CompressSave:",
	"WinMode:",
	"LastSave:",
	"Shadow:",
	"AllCameras:",
	"ReverseStereo:",
	"DetailLevel:",
	"Version:",
	"FullScreen:"
};

void allocate_vars_memory()
{
	workVideoBuffer = (unsigned char *) malloc(307700 * sizeof(unsigned char));
	frontVideoBuffer = frontVideoBufferbis = (unsigned char *) malloc(sizeof(unsigned char) * 307200);
	init_screen_buffer((char *) frontVideoBuffer, 640, 480);
}

int get_config_type_index(char* lineBuffer)
{
    int i;
    char buffer[256];
    char* ptr;

    strcpy(buffer,lineBuffer);

	ptr = strchr(buffer,' ');

    if(ptr)
    {
        *ptr = 0;
    }

    for(i=0;i<=(sizeof(CFGList)/18);i++)
    {
        if(strlen(CFGList[i]))
        {
            if(!strcmp(CFGList[i],buffer)){
                return i;
            }
        }
    }

    return -1;	
}

void get_file_configurations(unsigned char *filename)
{
	FILE *fd;
	char buffer[256];
	int cfgtype=-1;
	int i;

	fd = fopen(filename, "rb");
	if(!fd)
		printf("Error: Can't find config file %s\n",filename);

	for(i=0;i<131;i++)
	//while(!feof(fd))
	{
		fgets(buffer,256,fd);
		*strchr(buffer,0x0D0A) = 0;
		cfgtype = get_config_type_index(buffer);
		if(cfgtype!=-1)
		{
			//printf("%d-%s %s\n",cfgtype,CFGList[cfgtype],buffer);
			switch(cfgtype)
			{
				case 0: sscanf(buffer,"Language: %s",cfgfile.Language); break;
				case 1: sscanf(buffer,"LanguageCD: %s",cfgfile.LanguageCD); break;
				case 2: sscanf(buffer,"FlagKeepVoice: %s",cfgfile.FlagKeepVoice); break;
				case 3: sscanf(buffer,"FlagDisplayText: %s",cfgfile.FlagDisplayText); break;
				case 4: sscanf(buffer,"WaveVolume: %d",&cfgfile.WaveVolume); break;
				case 5: sscanf(buffer,"VoiceVolume: %d",&cfgfile.VoiceVolume); break;
				case 6: sscanf(buffer,"MusicVolume: %d",&cfgfile.MusicVolume); break;
				case 7: sscanf(buffer,"CDVolume: %d",&cfgfile.CDVolume); break;
				case 8: sscanf(buffer,"MasterVolume: %d",&cfgfile.MasterVolume); break;
				case 9: sscanf(buffer,"CompressSave: %d",&cfgfile.CompressSave); break;
				case 10: sscanf(buffer,"WinMode: %d",&cfgfile.WinMode); break;
				case 11: sscanf(buffer,"LastSave: %s",cfgfile.LastSave); break;
				case 12: sscanf(buffer,"Shadow: %d",&cfgfile.Shadow); break;
				case 13: sscanf(buffer,"AllCameras: %d",&cfgfile.AllCameras); break;
				case 14: sscanf(buffer,"ReverseStereo: %d",&cfgfile.ReverseStereo); break;
				case 15: sscanf(buffer,"DetailLevel: %d",&cfgfile.DetailLevel); break;
				case 16: sscanf(buffer,"Version: %d",&cfgfile.Version); break;
				case 17: sscanf(buffer,"FullScreen: %d",&cfgfile.FullScreen); break;
			}
		}
	}
	fclose(fd);
	fullscreen = cfgfile.FullScreen;
}

void check_cdrom()
{
	load_image(60); // check cdrom image
	init_cdrom();   // cdrom initialize
}

void init_engine()
{
	// original engine texts
	printf("Identifying Operating System. Please wait...\n");
	printf("Identifying CPU. Please wait...\n");
  
	// SDL initialization
	sdl_initialize();
	printf("Success!\n");
	
	// init smacker device
	printf("Initialising Smacker. Please wait...\n");

	// Getting configuration file 
	get_file_configurations("lba2.cfg");
	
	// allocate memory and init files
	allocate_vars_memory();

	toggle_fullscreen();

	// check cd
	check_cdrom();

	// publisher image
	if(cfgfile.Version==1 || cfgfile.Version==2) // Twinsen's Odyssey (U.S., New Zealand, Brazil), Activision
		load_image_cross_fade_delay(72,2);
	else if(cfgfile.Version==3) // LBA 2 (Europe), Electronic Arts
		load_image_cross_fade_delay(74,2);
	else if(cfgfile.Version==4)
		load_image_cross_fade_delay(76,2); // Twinsen's Odyssey (Asia), Virgin

	// Company logo
	AdelineLogo();

	// start main menu
	main_menu();
}

int main(int argc, char *argv[])
{
	init_engine();	
	sdl_close();
	return 0;
}
