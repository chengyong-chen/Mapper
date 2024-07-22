#include "stdafx.h"

#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal)
{
	LPBITMAPINFOHEADER lpbi;
	DWORD dwLen;
	HANDLE hDIB;
	HANDLE handle;
	HDC hDC;

	ASSERT(bitmap.GetSafeHandle());

	// The function has no arg for bitfields
	if(dwCompression==BI_BITFIELDS)
		return nullptr;

	// If a palette has not been supplied use defaul palette
	HPALETTE hPal = (HPALETTE)pPal->GetSafeHandle();
	if(hPal==nullptr)
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	BITMAP bm;
	bitmap.GetObject(sizeof(bm), (LPSTR)&bm);

	// Initialize the bitmapinfoheader
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = bm.bmPlanes*(bm.bmBitsPixel>24 ? 24 : bm.bmBitsPixel); //bm.bmPlanes*bm.bmBitsPixel;
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1<<bi.biBitCount);
	if(bi.biBitCount>8)
		nColors = 0;
	dwLen = bi.biSize+nColors*sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(nullptr);
	hPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if(!hDIB)
	{
		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(nullptr, hDC);
		return nullptr;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a nullptr lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight, (LPBYTE)nullptr, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if(bi.biSizeImage==0)
	{
		bi.biSizeImage = ((((bi.biWidth*bi.biBitCount)+31)&~31)/8)*bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if(dwCompression!=BI_RGB)
			bi.biSizeImage = (bi.biSizeImage*3)/2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if(handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else
	{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(nullptr, hDC);
		return nullptr;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	const BOOL bGotBits = GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L, // Start scan line
		(DWORD)bi.biHeight, // # of scan lines
		(LPBYTE)lpbi // address for bitmap bits
		+(bi.biSize+nColors*sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi, // address of bitmapinfo
		(DWORD)DIB_RGB_COLORS); // Use RGB for color table

	if(!bGotBits)
	{
		GlobalFree(hDIB);

		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(nullptr, hDC);
		return nullptr;
	}

	SelectPalette(hDC, hPal, FALSE);
	::ReleaseDC(nullptr, hDC);
	return hDIB;
}

BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB)
{
	if(hDIB==nullptr)
		return FALSE;

	CFile file;
	if(file.Open(szFile, CFile::modeWrite|CFile::modeCreate)==FALSE)
		return FALSE;
	const LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDIB;
	int nColors = 1<<lpbi->biBitCount;
	if(lpbi->biBitCount>8)
		nColors = 0;

	BITMAPFILEHEADER hdr;
	hdr.bfType = ((WORD)('M'<<8)|'B'); // is always "BM"
	hdr.bfSize = GlobalSize(hDIB)+sizeof(hdr);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = (DWORD)(sizeof(hdr)+lpbi->biSize+nColors*sizeof(RGBQUAD));

	// Write the file header 
	file.Write(&hdr, sizeof(hdr));

	// Write the DIB header and the bits 
	file.Write(lpbi, GlobalSize(hDIB));

	return TRUE;
}

BOOL WriteBme(CString szFile, HANDLE hDIB)
{
	if(hDIB==nullptr)
		return FALSE;

	CFile file;
	if(file.Open(szFile, CFile::modeWrite|CFile::modeCreate)==FALSE)
		return FALSE;
	const LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDIB;
	int nColors = 1<<lpbi->biBitCount;
	if(lpbi->biBitCount>8)
		nColors = 0;

	BITMAPFILEHEADER hdr;
	hdr.bfType = ((WORD)('E'<<8)|'B'); // is always "BM"
	hdr.bfSize = GlobalSize(hDIB)+sizeof(hdr);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = (DWORD)(sizeof(hdr)+lpbi->biSize+nColors*sizeof(RGBQUAD));

	hdr.bfSize = (hdr.bfSize^0X4352);

	// Write the file header 
	file.Write(&hdr, sizeof(hdr));

	BYTE* pByte = (BYTE*)::GlobalLock(hDIB);
	pByte += lpbi->biSize+nColors*sizeof(RGBQUAD);
	BYTE prev = 0X82;
	const long size = GlobalSize(hDIB)-lpbi->biSize-nColors*sizeof(RGBQUAD);
	for(int index = 0; index<size; index++)
	{
		*pByte = (*pByte)^prev;
		prev = *pByte;
		pByte++;
	}
	::GlobalUnlock(hDIB);

	// Write the DIB header and the bits 
	file.Write(lpbi, GlobalSize(hDIB));

	return TRUE;
}
