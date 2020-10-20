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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hqrLib.h"

int hqr_check_if_file_exist(char *fileName)
{
  FILE *fd; // file descriptor
  fd = fopen(fileName, "rb");

  if(fd)
  {
    fclose(fd);
    return(1);
  }

  return (0);
}

FILE* hqr_open(char *fileName)
{
  FILE *fd;

  if (!fileName)
    return (NULL);

  fd = fopen(fileName, "rb");

  if (!fd)
  {
    printf("%s can't be found !\n", fileName);
  }

  return (fd);
}

int hqr_read(FILE * resourceFile, char *ptr, int length)
{
  if (!resourceFile)
    return (1);

  fread((char *) ptr, length, 1, resourceFile);

  return (0);
}

void hqr_file_read(char* resourceName, long int size, unsigned char* buffer)
{
  FILE* fd;
  fd = fopen(resourceName, "rb");
  fread(buffer,size,1,fd);
  fclose(fd);
}

void hqr_close(FILE * resourceFile)
{
  fclose(resourceFile);
}

void hqr_free_ressource(hqr_entry* resource)
{
  free(resource->ptr);
  free(resource);
}

hqr_entry* hqr_init_ressource(char *fileName, int sizeOfBuffer, int numOfEntriesInBuffer)
{
  hqr_entry *hqr_ptr;
  unsigned char *dataPtr;

  if( !hqr_check_if_file_exist( fileName ) )
    return (NULL);

  hqr_ptr = (hqr_entry *) malloc(numOfEntriesInBuffer * sizeof(subHqr) + sizeof(hqr_entry));

  if (!hqr_ptr)
    return (NULL);

  dataPtr = (unsigned char *) malloc(sizeOfBuffer + 500);

  if (!dataPtr)
    return (NULL);

  strcpy(hqr_ptr->fileName, fileName);
  hqr_ptr->preloadedResource = 0;//false;

  hqr_ptr->size1 = sizeOfBuffer;
  hqr_ptr->remainingSize = sizeOfBuffer;
  hqr_ptr->numEntriesMax = numOfEntriesInBuffer;
  hqr_ptr->numCurrentlyUsedEntries = 0;
  hqr_ptr->ptr = dataPtr;

  return (hqr_ptr);
}

long int hqr_get_size(char* resourceName)
{
  FILE* fd;
  int size;

  fd = fopen(resourceName, "rb");
  fseek(fd,0L,SEEK_END);
  size = ftell(fd);
  fclose(fd);
  return size;
}

void hqr_decompress(int decompressedSize, unsigned char* destination, unsigned char* source, int mode)
{
	unsigned char loop;
	unsigned char indic;
	unsigned char *jumpBack;
	int size;
	unsigned short int temp;
	int i;

	do{
		loop=8;
		indic=*(source++);
		do{
			if(!(indic&1))
			{
				temp=*(unsigned short int*)(source);
				source+=2;
				size=temp&0x0F;
				size+=mode+1;
				decompressedSize-=size;
				jumpBack=destination-(temp>>4)-1;
				for(i=0;i<size;i++)
				{
					*(destination++)=*(jumpBack++);
				}
				if(decompressedSize<=0)
					return;
				loop--;
			}
			else
			{
				*(destination++)=*(source++);
				loop--;
				decompressedSize--;
				if(decompressedSize<=0)
					return;
			}
			indic>>=1;
		}while(loop);
	}while(decompressedSize);
	return;
}

hqr_entry* hqr_preload_ressource(char *resourceName)
{
  hqr_entry *hqr_ptr;
  unsigned char **ptrTable;
  int fileSize;
  unsigned char* buffer;
  int numEntries;
  int i;

  if( !hqr_check_if_file_exist( resourceName ) )
    return (NULL);

  hqr_ptr = (hqr_entry *) malloc(sizeof(hqr_entry));

  if (!hqr_ptr)
    return (NULL);

  fileSize = hqr_get_size( resourceName );
  buffer = (unsigned char*)malloc(fileSize);
  hqr_file_read(resourceName,fileSize,buffer);

  numEntries = (*(unsigned int*)buffer)/4; // always alligned

  ptrTable = (unsigned char**)malloc(numEntries*sizeof(unsigned char*));
  hqr_ptr->sizeArray = (int*)malloc(numEntries*sizeof(int));

  for(i=0;i<numEntries-1;i++)
  {
    int offsetToData;
    int dataSize;
    int compressedSize;
    short int mode;

    offsetToData = *(unsigned int*)(buffer+4*i); // always alligned

    dataSize = *(unsigned int*)(buffer+offsetToData); //READ_LE_U32(buffer+offsetToData);
    compressedSize = *(unsigned int*)(buffer+offsetToData+4); //READ_LE_U32(buffer+offsetToData+4);
    mode = *(unsigned short*)(buffer+offsetToData+8);//READ_LE_U16(buffer+offsetToData+8);

    ptrTable[i] = (unsigned char*) malloc(dataSize);
    hqr_ptr->sizeArray[i] = dataSize;

    if(mode == 0) // uncompressed data
    {     
      memcpy(ptrTable[i], buffer+offsetToData+10,dataSize);
    }
    else // compressed data
    {
      hqr_decompress(dataSize, ptrTable[i], buffer+offsetToData+10, mode);
    }
  }

  free(buffer);

  strcpy(hqr_ptr->fileName, resourceName);
  hqr_ptr->preloadedResource = 1; //true;

  hqr_ptr->ptrArray=ptrTable;

  return (hqr_ptr);
}

int hqr_load(char *resourceName, unsigned char* ptr, int entryNumber)
{
	FILE *fd;
	unsigned int headerSize;
	unsigned int offToImage;
	unsigned int dataSize;
	unsigned int compressedSize;
	unsigned short int mode;

	if(!(fd=hqr_open(resourceName)))
		return 0;

	hqr_read(fd,(char *)&headerSize,4);
 
	if((unsigned int)entryNumber >= headerSize /4)
	{
		hqr_close(fd);
		return 0;
	}
 
	fseek(fd,entryNumber*4,SEEK_SET);
	hqr_read(fd,(char *)&offToImage,4);
 
	fseek(fd,offToImage,SEEK_SET);
	hqr_read(fd,(char *)&dataSize,4);
	hqr_read(fd,(char *)&compressedSize,4);
	hqr_read(fd,(char *)&mode,2);

/*  ptr=(unsigned char*)malloc(dataSize);

	if(!(ptr))
		return (-1);
*/
	if(mode==0) // uncompressed
	{
		hqr_read(fd,(char*)ptr,dataSize);
	}
	else // compressed: both modes (1 & 2)
	{
		unsigned char* compressedDataPtr;
		compressedDataPtr=(unsigned char*)malloc(compressedSize);
   		hqr_read(fd,(char*)compressedDataPtr,compressedSize);
		hqr_decompress(dataSize,(char*)ptr,compressedDataPtr,mode);
		free(compressedDataPtr);
	}

	hqr_close(fd);

	return (dataSize);
}

int hqr_get_num_entries(char* fileName)
{
	FILE *fd = NULL;
	int headerSize;

	if (!fd)
		return (0);

	hqr_read(fd, (char *)&headerSize, 4);

	return(headerSize/4);
}
/*
unsigned char *LoadMalloc_HQR(char *fileName, short int imageNumber)
{
  unsigned char* ptr;
  HQRM_Load(fileName,imageNumber, &ptr);

  return (ptr);
}*/

subHqr *find_sub_hqr(int arg_0, int arg_4, subHqr * arg_8)
{
	subHqr *temp;
	int i;

	if (arg_4 == 0)
		return (0);

	temp = arg_8;

	for (i = 0; i < arg_4; i++)
	{
		if (temp->index == arg_0)
			return (temp);

		temp++;
	}

	return (0);
}

unsigned char *hqr_get(hqr_entry * hqrPtr, short int arg_4)
{
	FILE *fd;
	unsigned int headerSize;
	unsigned int offToData;
	unsigned int dataSize;
	unsigned int compressedSize;
	short int mode;

	short int var_4;
	int ltime;
	int temp2;
	unsigned char *ptr;

	int i;

	int dataSize2;
	subHqr *hqrdata;
	subHqr *hqrdataPtr;

	if (arg_4 < 0)
		return (0);

	if(hqrPtr->preloadedResource)
	{
		return hqrPtr->ptrArray[arg_4];
	}

	hqrdata = (subHqr *) ((unsigned char *) hqrPtr + sizeof(hqr_entry));

	hqrdataPtr = find_sub_hqr(arg_4, hqrPtr->numCurrentlyUsedEntries, hqrdata);

	if (hqrdataPtr)
	{
		hqrdataPtr->lastAccessedTime = lba_time;
		hqrFlag = 0;
		return (hqrdataPtr->offFromPtr + hqrPtr->ptr);
	}
  
	printf("Need to load: %s-%d\n", hqrPtr->fileName, arg_4);

	hqr_size(hqrPtr->fileName, arg_4);

	if(!(fd=hqr_open(hqrPtr->fileName)))
		return 0;
  
	hqr_read(fd,(char *)&headerSize, 4);

	if( (unsigned short int) arg_4 >= headerSize / 4 )
	{
		hqr_close(fd);
		return 0;
	}
	 
	fseek(fd,arg_4*4,SEEK_SET);
	hqr_read(fd,(char *)&offToData,4);
 
	fseek(fd,offToData,SEEK_SET);
	hqr_read(fd,(char *)&dataSize,4);
	hqr_read(fd,(char *)&compressedSize,4);
	hqr_read(fd,(char *)&mode,2);

	dataSize2 = dataSize;

	ltime = lba_time;

	while (dataSize2 >= hqrPtr->remainingSize || hqrPtr->numCurrentlyUsedEntries >= hqrPtr->numEntriesMax)
	{
		var_4 = 0;
		temp2 = 0;
		for (i = 0; i < hqrPtr->numCurrentlyUsedEntries; i++)
		{
			if (temp2 <= ltime - hqrdata[i].lastAccessedTime)
			{
				temp2 = ltime - hqrdata[var_4].lastAccessedTime;
				var_4 = i;
			}
		}
		hqr_remove_entry(hqrPtr, var_4);
	}

	ptr = hqrPtr->ptr + hqrPtr->size1 - hqrPtr->remainingSize;

	if (mode <= 0)    // uncompressed
	{
		hqr_read(fd, (char *)ptr, dataSize);
	}
	else // compressed
	{
		hqr_read(fd, (char*)(ptr+dataSize-compressedSize+500), compressedSize);
		hqr_decompress(dataSize, (char *)ptr, (ptr + dataSize - compressedSize + 500),mode);
	}

	hqr_close(fd);

	hqrdataPtr = &hqrdata[hqrPtr->numCurrentlyUsedEntries];

	hqrdataPtr->index = arg_4;
	hqrFlag = 1;
	hqrdataPtr->lastAccessedTime = lba_time;
	hqrdataPtr->offFromPtr = hqrPtr->size1 - hqrPtr->remainingSize;
	hqrdataPtr->size = dataSize2;

	hqrPtr->numCurrentlyUsedEntries++;
	hqrPtr->remainingSize -= dataSize2;

	return (ptr);
}

unsigned char *hqr_get_copy(hqr_entry * hqrPtr, short int arg_4)
{
	unsigned char* newPtr;

	if(hqrPtr->preloadedResource)
	{
		newPtr = (unsigned char*)malloc(hqrPtr->sizeArray[arg_4]);
		memcpy(newPtr, hqrPtr->ptrArray[arg_4],hqrPtr->sizeArray[arg_4]);
		return newPtr;
	}

	return NULL;
}

void hqr_reset_ressource(hqr_entry * ptr)
{
	ptr->remainingSize = ptr->size1;
	ptr->numCurrentlyUsedEntries = 0;
}

// TODO: to implement
int hqr_remove_entry(hqr_entry * hqrPtr, int var)
{
  /*subHqr *subPtr;

  int lvar;

  unsigned char *source;
  unsigned char *dest;
  int size;
  int retVal;*/

  hqr_reset_ressource(hqrPtr);
  return(var);
/*
  lvar = var;

  subPtr = (subHqr *) (hqrPtr + sizeof(hqr_entry) + var * sizeof(subHqr));

  retVal = subPtr->size;

  if (hqrPtr->numCurrentlyUsedEntries - 1 > lvar)
  {
    source = hqrPtr->ptr + subPtr->offFromPtr;

    dest = source + retVal;
    size = hqrPtr->ptr + hqrPtr->size1 - dest;

    memmove(source, dest, size);

    printf("HQR_RemoveEntryFromHQR\n");
    exit(1);
   // implementer la suite
  }

  hqrPtr->numCurrentlyUsedEntries--;

  hqrPtr->remainingSize = retVal;

  return (retVal); */
}
/*
int HQRM_Load(char *fileName, short int arg_4, unsigned char ** ptr)  // recheck
{
  unsigned int headerSize;
  unsigned int offToData;
  unsigned int dataSize;
  unsigned int compressedSize;
  unsigned short int mode;
  unsigned char *temp;

  if(!streamReader_open( &fileReader, (int8*)fileName, 1 ))
    return(-1);

  streamReader_get( &fileReader, &headerSize, 4);
  headerSize = convertDWFromLE(headerSize);
  
  if(arg_4 >= headerSize /4)
  {
    streamReader_close( &fileReader );
    return (-1);
  }

  streamReader_seek( &fileReader, arg_4 * 4 );
  streamReader_get( &fileReader, &offToData, 4 );
  offToData = convertDWFromLE(offToData);

  streamReader_seek( &fileReader, offToData );
  streamReader_get( &fileReader, &dataSize, 4 );
  dataSize = convertDWFromLE(dataSize);
  streamReader_get( &fileReader, &compressedSize, 4 );
  compressedSize = convertDWFromLE(compressedSize);
  streamReader_get( &fileReader, &mode , 2 );
  mode = convertWFromLE(mode);

  (*ptr) = (unsigned char*)Malloc(dataSize);

  if(!(*ptr))
  {
    streamReader_close(&fileReader);
    return(-1);
  }

  if(mode == 0)
  {
    streamReader_get( &fileReader, (*ptr), dataSize );
  }
  else
  if(mode == 1)
  {
    temp = (unsigned char *) Malloc(compressedSize);

    streamReader_get( &fileReader, temp, compressedSize );
    HQR_Expand( dataSize, *ptr, temp );
    MemFree(temp);
  }

  streamReader_close( &fileReader );

  return (dataSize);
}*/

int hqr_size(char *fileName, int index)
{
	FILE *fd;
	unsigned int headerSize;
	unsigned int dataSize;
	unsigned int offToData;

	if(!(fd=hqr_open(fileName)))
		return 0;

	hqr_read(fd,(char *)&headerSize,4);
 
	if((unsigned int)index >= headerSize /4)
	{
		hqr_close(fd);
		return 0;
	}
 
	fseek(fd,index*4,SEEK_SET);
	hqr_read(fd,(char *)&offToData,4);
 
	fseek(fd,offToData,SEEK_SET);
	hqr_read(fd,(char *)&dataSize,4);

	return (dataSize);
}
