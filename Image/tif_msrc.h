#pragma once

extern struct tiff;
typedef struct tiff TIFF;

typedef struct
{
	unsigned char* pData;
	int CurPos;
	int MaxFileSize;
	// used when open for reading
	int rFileSize;
	// not nullptr when open for writing
	int* pFileSize;
	char mode[2];
} MemSrcTIFFInfo;
extern TIFF* TIFFOpenMem(unsigned char* pData, int FileSize, int* pCurPos);
