/*
Little Big Adventure 2: HQR management
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

#ifndef HQRLIB_H
#define HQRLIB_H

#include "screen.h"

typedef struct hqr_entry
{
  char fileName[12];
  char preloadedResource;

  // non preloaded data
  int size1;
  int remainingSize;
  short int numEntriesMax;
  short int numCurrentlyUsedEntries;
  unsigned char *ptr;

  // preloaded data
  unsigned char** ptrArray;
  int* sizeArray;
}hqr_entry;

typedef struct subHqr
{
  short int index;
  unsigned int offFromPtr;
  unsigned int size;
  int lastAccessedTime;
}subHqr;

short int hqrFlag;

extern int lba_time;

int hqr_check_if_file_exist(char *fileName);
FILE* hqr_open(char *fileName);
int hqr_read(FILE * resourceFile, char *ptr, int length);
void hqr_file_read(char* resourceName, long int size, unsigned char* buffer);
void hqr_close(FILE * resourceFile);
void hqr_free_ressource(hqr_entry* resource);
hqr_entry* hqr_init_ressource(char *fileName, int sizeOfBuffer, int numOfEntriesInBuffer);
long int hqr_get_size(char* resourceName);
void hqr_decompress(int decompressedSize, unsigned char* destination, unsigned char* source, int mode);
hqr_entry* hqr_preload_ressource(char *resourceName);
int hqr_load(char *resourceName, unsigned char* ptr, int entryNumber);
int hqr_get_num_entries(char* fileName);
//unsigned char *LoadMalloc_HQR(char *fileName, short int imageNumber)
subHqr *find_sub_hqr(int arg_0, int arg_4, subHqr * arg_8);
unsigned char *hqr_get(hqr_entry * hqrPtr, short int arg_4);
unsigned char *hqr_get_copy(hqr_entry * hqrPtr, short int arg_4);
void hqr_reset_ressource(hqr_entry * ptr);
int hqr_remove_entry(hqr_entry * hqrPtr, int var);
//int HQRM_Load(char *fileName, short int arg_4, unsigned char ** ptr);
int hqr_size(char *fileName, int index);

#endif
