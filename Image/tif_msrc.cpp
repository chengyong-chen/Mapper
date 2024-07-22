#include "stdafx.h"
#include "tif_msrc.h"

extern "C"
{
	#include "../../ThirdParty/tiff/4.5.1/libtiff/tiffio.h"
}
// in write mode, we must record the highest written to offset
#define CHECKNEWSIZE \
	if(*pInfo->mode == 'w' && pInfo->CurPos > *pInfo->pFileSize) \
		*pInfo->pFileSize = pInfo->CurPos

static tsize_t _TIFFReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	MemSrcTIFFInfo* pInfo = (MemSrcTIFFInfo*)fd;

#ifdef _WINDOWS
	//_ASSERT(*(pInfo->mode) == 'r');
#endif
	// even when writing, libTIFF initially tries to read a directory....
	if(*(pInfo->mode) != 'r')
		return 0;

	/* Make sure we don't run over the end of the file */
	if(size + pInfo->CurPos > pInfo->MaxFileSize)
		size = pInfo->MaxFileSize - pInfo->CurPos;

	memcpy((void*)buf, (void*)(pInfo->pData + pInfo->CurPos), size);
	pInfo->CurPos += size;
	CHECKNEWSIZE;

	return (tsize_t)size;
}

static tsize_t _TIFFWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
#if 0
	TIFFError("TIFFOpen", "Function disabled.");
	return 0;
#else
	MemSrcTIFFInfo* pInfo = (MemSrcTIFFInfo*)fd;

	_ASSERT(*(pInfo->mode) == 'w');

	/* Make sure we don't run over the end of the file */
	if(size + pInfo->CurPos > pInfo->MaxFileSize)
		size = pInfo->MaxFileSize - pInfo->CurPos;

	memcpy((void*)(pInfo->pData + pInfo->CurPos), (void*)buf, size);
	pInfo->CurPos += size;
	CHECKNEWSIZE;

	return (tsize_t)size;
#endif
}

static toff_t _TIFFSeekProc(thandle_t fd, toff_t off, int whence)
{
	MemSrcTIFFInfo* pInfo = (MemSrcTIFFInfo*)fd;

	switch(whence)
	{
		case 1:
			pInfo->CurPos += off;
			break;
		case 2:
			pInfo->CurPos = *pInfo->pFileSize - off;
			break;
		case 0:
		default:
			pInfo->CurPos = off;
			break;
	}
	CHECKNEWSIZE;
	return pInfo->CurPos;
}

static toff_t _TIFFSizeProc(thandle_t fd)
{
	const MemSrcTIFFInfo* pInfo = (MemSrcTIFFInfo*)fd;
	return ((toff_t)(*pInfo->pFileSize));
}

TIFF* TIFFOpenMem(unsigned char* pData, int maxFileSize, int* fSize)
{
	MemSrcTIFFInfo* pInfo = (MemSrcTIFFInfo*)malloc(sizeof(MemSrcTIFFInfo));
	pInfo->pData = pData;
	strcpy(pInfo->mode, fSize ? "w" : "r");
	// register extern int
	pInfo->pFileSize = fSize ? fSize : &(pInfo->rFileSize);
	pInfo->MaxFileSize = maxFileSize;
	*pInfo->pFileSize = fSize ? 0 : maxFileSize;
	pInfo->CurPos = 0;

	return TIFFFdOpen((int)(pInfo), "MemSource", pInfo->mode);
}

TIFFErrorHandler _TIFFerrorHandler = 0;
TIFFErrorHandler _TIFFwarningHandler = 0;
