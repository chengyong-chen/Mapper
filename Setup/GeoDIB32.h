#pragma once

#pragma pack(push, before_GeoDIB, 1)

#include <afxtempl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Handle to a DIB 
#define HDIB HANDLE


// Print Area selection 
#define PW_WINDOW        1
#define PW_CLIENT        2


// Print Options selection 
#define PW_BESTFIT       1
#define PW_STRETCHTOPAGE 2
#define PW_SCALE         3

// DIB constants 
#define PALVERSION   0x300

// DIB Macros
// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth*biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed 
// to hold those bits.

#define WIDTHBYTES(bits)		(((bits) + 31)/32*4)
#define IS_WIN30_DIB(lpbi)		((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)		((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)		((lpRect)->bottom - (lpRect)->top)
#define DIB_HEADER_MARKER		((WORD) ('M' << 8) | 'B')


// Error constants 
enum {
      ERR_MIN = 0,                     // All error #s >= this value
      ERR_NOT_DIB = 0,                 // Tried to load a file, NOT a DIB!
      ERR_MEMORY,                      // Not enough memory!
      ERR_READ,                        // Error reading file!
      ERR_LOCK,                        // Error on a GlobalLock()!
      ERR_OPEN,                        // Error opening a file!
      ERR_CREATEPAL,                   // Error creating palette.
      ERR_GETDC,                       // Couldn't get a DC.
      ERR_CREATEDDB,                   // Error create a DDB.
      ERR_STRETCHBLT,                  // StretchBlt() returned failure.
      ERR_STRETCHDIBITS,               // StretchDIBits() returned failure.
      ERR_SETDIBITSTODEVICE,           // SetDIBitsToDevice() failed.
      ERR_STARTDOC,                    // Error calling StartDoc().
      ERR_NOGDIMODULE,                 // Couldn't find GDI module in memory.
      ERR_SETABORTPROC,                // Error calling SetAbortProc().
      ERR_STARTPAGE,                   // Error calling StartPage().
      ERR_NEWFRAME,                    // Error calling NEWFRAME escape.
      ERR_ENDPAGE,                     // Error calling EndPage().
      ERR_ENDDOC,                      // Error calling EndDoc().
      ERR_SETDIBITS,                   // Error calling SetDIBits().
      ERR_FILENOTFOUND,                // Error opening file in GetDib()
      ERR_INVALIDHANDLE,               // Invalid Handle
      ERR_DIBFUNCTION,                 // Error on call to DIB function
      ERR_MAX                          // All error #s < this value
     };

#define MAX_BUFF_SIZE           32768
#define MAX_STRIP_SIZE           8192
#define MAX_HASH_SIZE            5051
#define MAX_TABLE_SIZE           4096
#define MAX_SUBBLOCK_SIZE         255
#define SHADE_NUM                   2

#define BMP_PAL_VER             0x0300
#define MMB_MARK                0x4D42

#define DWORD_WBYTES(x)         ((((x) + 31UL) >> 5) << 2)
#define WORD_WBYTES(x)          ((((x) + 15 ) >> 4) << 1)
#define BYTE_WBYTES(x)          ( ((x) +  7UL) >> 3      )
#define SWAP(x,y)               ((x)^=(y)^=(x)^=(y))
#define IMAGE_COLORS(x,y)       (1 << ((x)*(y)))

// Function prototypes 
HDIB	BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
HDIB	ChangeBitmapFormat (HBITMAP	hBitmap,
                                   WORD     wBitCount,
                                   DWORD    dwCompression,
                                   HPALETTE hPal);
HDIB	ChangeDIBFormat (HDIB hDIB, WORD wBitCount,
                                DWORD dwCompression);
HBITMAP	CopyScreenToBitmap (LPRECT);
HDIB	CopyScreenToDIB (LPRECT);
HBITMAP	CopyWindowToBitmap (HWND, WORD);
HDIB	CopyWindowToDIB (HWND, WORD);
HPALETTE	CreateDIBPalette (HDIB);
HDIB	CreateDIB(DWORD, DWORD, WORD);
WORD	DestroyDIB (HDIB);
BOOL	DIBDimension(HDIB, LPSIZE);
BOOL  DIBRealSize(HDIB, LPSIZE);
RECT	DIBRect(HDIB);
void	DIBError (int ErrNo);
DWORD	DIBHeight (LPSTR lpDIB);
WORD	DIBNumColors (LPSTR lpDIB);
HBITMAP	DIBToBitmap (HDIB hDIB, HPALETTE hPal);
DWORD	DIBWidth (LPSTR lpDIB);
LPSTR	FindDIBBits (LPSTR lpDIB);
HPALETTE	GetSystemPalette (void);
HDIB	LoadDIB (LPCTSTR);
BOOL	PaintBitmap (HDC, LPRECT, HBITMAP, LPRECT, HPALETTE);
BOOL	PaintDIB (HDC, LPRECT, HDIB, LPRECT, HPALETTE);
int	PalEntriesOnDevice (HDC hDC);
WORD	PaletteSize (LPSTR lpDIB);
WORD	PrintDIB (HDIB, WORD, WORD, WORD, LPSTR);
WORD	PrintScreen (LPRECT, WORD, WORD, WORD, LPSTR);
WORD	PrintWindow (HWND, WORD, WORD, WORD, WORD, LPSTR);
WORD	SaveDIB (HDIB, LPSTR);
HANDLE	AllocRoomForDIB (BITMAPINFOHEADER bi, HBITMAP hBitmap);
HDIB	ReadDIBFile (HANDLE);
BOOL	SaveDIBFile (void);
BOOL	WriteDIB (LPSTR, HANDLE);

HDIB LoadPCX(LPCTSTR lpFileName);
BOOL SavePCX(HDIB hDIB, LPCTSTR lpFileName);
HDIB LoadGIF(LPCTSTR lpFileName);
BOOL SaveGIF(HDIB hDIB, LPCTSTR lpFileName);
HDIB LoadJPEG(LPCTSTR lpFileName);
BOOL SaveJPEG(HDIB hDIB, LPCTSTR lpFileName);
HDIB LoadTIFF(LPCTSTR lpFileName);
BOOL SaveTIFF(HDIB hDIB, LPCTSTR lpFileName);
HDIB LoadTGA(LPCTSTR lpFileName);
BOOL SaveTGA(HDIB hDIB, LPCTSTR lpFileName);
HDIB LoadWPG(LPCTSTR lpFileName);
BOOL SaveWPG(HDIB hDIB, LPCTSTR lpFileName);

struct BLOCK{
	UINT nBlockNum;		// Block Number
	HDIB hDIB;			// Block Pointer
};
typedef CArray<BLOCK, BLOCK>CBlockArray;

class CDIBBlock{
private:
	HANDLE m_hFile;

// Attributes
protected:
	BOOL m_bIsCreated;
	CBlockArray m_arrayBlocks;
	
	CSize m_block;
	BITMAPFILEHEADER    m_bmfHeader;
	BITMAPINFOHEADER	m_bi;

// Operations
protected:
	BOOL PerCreate(LPCTSTR lpDibFilePath);
	
// Attributes
public:
	UINT GetDIBBlockCount() { return m_arrayBlocks.GetSize();}
	UINT GetBlockState(UINT nBlockNum);
	CRect GetBlockRect(UINT nBlockNum);
	HDIB GetBlock(UINT nBlockNum);
	
// Operations
public:
	CDIBBlock():m_block(1024, 1024){ m_bIsCreated = FALSE;};
	CDIBBlock(LPCTSTR lpDibFilePath):m_block(1024, 1024){ m_bIsCreated = FALSE; 
																			Create(lpDibFilePath);}
	~CDIBBlock() { Release();}

	BOOL IsCreated() { return m_bIsCreated;}
	BOOL Create(LPCTSTR lpDibFilePath);
	BOOL Release();
	HDIB LoadBlock(UINT nBlockNum);
	BOOL ReleaseBlock(UINT nBlockNum);

	WORD PaletteSize(LPSTR lpDIB);
	WORD DIBNumColors(LPSTR lpDIB);
	LPSTR FindDIBBits(LPSTR lpDIB);
	DWORD DIBHeight(LPSTR lpDIB);
	DWORD DIBWidth(LPSTR lpDIB);
	CSize GetDIBSize();
	CSize GetDIBRealSize();

	BOOL PaintBlock(CDC* pDC, LPRECT lpDCRect, HDIB hDIB, LPRECT lpDIBRect, HPALETTE hPal = nullptr);
	BOOL PaintBlock(CDC* pDC, LPRECT lpDCRect, UINT nBlockNum, LPRECT lpDIBRect, HPALETTE hPal = nullptr);
	BOOL PaintDIB(CDC* pDC, LPRECT lpDCRect, LPRECT lpDIBRect, HPALETTE hPal = nullptr);
};

#pragma pack(pop, before_GeoDIB)
