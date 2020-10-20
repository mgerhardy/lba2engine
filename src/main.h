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

#ifndef MAIN_H
#define MAIN_H

typedef struct configFile
{
	char Language[10];
	char LanguageCD[10];
	char FlagKeepVoice[3];
	char FlagDisplayText[3];
	int WaveVolume;
	int VoiceVolume;
	int MusicVolume;
	int CDVolume;
	int MasterVolume;
	int CompressSave;
	int WinMode;
	char LastSave[256];
	int Shadow;
	int AllCameras;
	int ReverseStereo;
	int DetailLevel;
	int Version;
	int FullScreen;
	int UseCD;
}configFile;

extern configFile cfgfile;
extern char * directory;

void allocate_vars_memory();
int get_config_type_index(char* lineBuffer);
void get_file_configurations(unsigned char *filename);
void check_cdrom();
void init_engine(void);
int main(int argc, char *argv[]);

#endif
