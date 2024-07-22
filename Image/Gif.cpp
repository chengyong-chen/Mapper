#include "stdafx.h"
#include "malloc.h"
#include <limits.h>

#include "Dib.h"
#include "Decode.h"
#include "Gif.h"
#include "datasrc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OUT_OF_MEMORY -10
#define BAD_CODE_SIZE -20
#define READ_ERROR -1
#define WRITE_ERROR -2
#define OPEN_ERROR -3
#define CREATE_ERROR -4
#define MAX_CODES   4095

CGif::CGif()
	: CDecode()
{
}

void CGif::DoDecode(RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc)
{
	LPGIFHEADER gifHeader;
	LPGIFSCRDESC gifScrDesc;
	LPGIFIMAGE gifImage;
	LPGIFCONTROL gifControl;
	LPGIFPLAINTEXT gifPlanText;
	LPGIFAPPLICATION gifApplication;

	WORD wBits;
	BYTE byLabel, byBlockSize, byTemp;

	PALETTEENTRY bySysPal256[256];
	LPBYTE byGIF_Pal;
	const BYTE bySysPal16[48] = {0, 0, 0, 0, 0, 128,
		0, 128, 0, 0, 128, 128,
		128, 0, 0, 128, 0, 128,
		128, 128, 0, 128, 128, 128,
		192, 192, 192, 0, 0, 255,
		0, 255, 0, 0, 255, 255,
		255, 0, 0, 255, 0, 255,
		255, 255, 0, 255, 255, 255
	};

	// Check the file is a valid Giffile or not
	//ccy	Trace (2, "Decoding GIF.\n");

	gifHeader = (LPGIFHEADER)pDataSrc->ReadNBytes(sizeof(tagGIFHEADER));

	// Is a GifFormat?
	if(memicmp((LPSTR)gifHeader->bySignature, "GIF", 3)!=0)
	{
		//ccy		raiseError (ERR_WRONG_SIGNATURE, "Error decoding gif: Not a Giffile.");
	}

	if((memicmp((LPSTR)gifHeader->byVersion, "87a", 3)!=0)&&(memicmp((LPSTR)gifHeader->byVersion, "89a", 3)!=0))
	{
		//ccy		raiseError (ERR_WRONG_SIGNATURE, "Error decoding gif: Not a Giffile.");
	}

	// GifLogical Screen Descriptor
	gifScrDesc = (LPGIFSCRDESC)pDataSrc->ReadNBytes(sizeof(tagGIFSCRDESC));

	wBits = (WORD)gifScrDesc->GlobalFlag.PalBits+1;
	if(gifScrDesc->GlobalFlag.GlobalPal)
	{
		byGIF_Pal = nullptr;

		// Read Global Color Table
		byGIF_Pal = (LPBYTE)pDataSrc->ReadNBytes(3*(1<<wBits));
	}

	byTemp = ReadByte(pDataSrc);
	while(TRUE)
	{
		if(byTemp==0x2C)
		{
			// GifImage Descriptor
			break;
		}
		if(byTemp==0x21)
		{
			// GifExtension
			byLabel = ReadByte(pDataSrc);
			switch(byLabel)
			{
			case 0xF9:
			{
				// Graphic Control Extension
				gifControl = (LPGIFCONTROL)pDataSrc->ReadNBytes(sizeof(tagGIFCONTROL));
				break;
			}
			case 0x01:
			{
				// Plain Text Extension
				gifPlanText = (LPGIFPLAINTEXT)pDataSrc->ReadNBytes(sizeof(tagGIFPLAINTEXT));
				byBlockSize = ReadByte(pDataSrc);

				// Skip All Block
				while(byBlockSize)
				{
					LPBYTE lpTemp = (LPBYTE)pDataSrc->ReadNBytes(byBlockSize);
					byBlockSize = ReadByte(pDataSrc);
				}
				break;
			}
			case 0xFE:
			{
				// Skip Comment Extension
				byBlockSize = ReadByte(pDataSrc);
				while(byBlockSize)
				{
					LPBYTE lpTemp = (LPBYTE)pDataSrc->ReadNBytes(byBlockSize);
					byBlockSize = ReadByte(pDataSrc);
				}
				break;
			}
			case 0xFF:
			{
				// Application Extension
				gifApplication = (LPGIFAPPLICATION)pDataSrc->ReadNBytes(sizeof(tagGIFAPPLICATION));

				// Skip All Block
				byBlockSize = ReadByte(pDataSrc);
				while(byBlockSize)
				{
					LPBYTE lpTemp = (LPBYTE)pDataSrc->ReadNBytes(byBlockSize);
					byBlockSize = ReadByte(pDataSrc);
				}
				break;
			}
			default:
			{
				// Skip Unknown Extension
				byBlockSize = ReadByte(pDataSrc);
				while(byBlockSize)
				{
					LPBYTE lpTemp = (LPBYTE)pDataSrc->ReadNBytes(byBlockSize);
					byBlockSize = ReadByte(pDataSrc);
				}
				break;
			}
			}
		}

		byTemp = ReadByte(pDataSrc);
	}

	// GifImage Descriptor
	int byte = sizeof(tagGIFIMAGE);
	gifImage = (LPGIFIMAGE)pDataSrc->ReadNBytes(sizeof(tagGIFIMAGE));

	if(gifImage->LocalFlag.LocalPal)
	{
		byGIF_Pal = nullptr;

		// Read Local Color Table
		wBits = (WORD)gifImage->LocalFlag.PalBits+1;
		byGIF_Pal = (LPBYTE)pDataSrc->ReadNBytes(3*(1<<wBits));
	}
	wBits = ((wBits==1) ? 1 : (wBits<=4) ? 4 : 8);
	const WORD wColors = 1<<wBits;

	// Create Bmp
	CalcDestBPP(wBits, pDestBPP);
	this->Create(gifImage->wWidth, gifImage->wDepth, *pDestBPP, FALSE);

	BITMAPINFOHEADER* DibInfo = (BITMAPINFOHEADER*)::GlobalLock((HGLOBAL)m_hInfo);
	RGBQUAD* pClrTab = (RGBQUAD*)(((BITMAPINFO*)(DibInfo))->bmiColors);

	// Correct the BYTE ordering & copy the data.
	if(gifScrDesc->GlobalFlag.GlobalPal||gifImage->LocalFlag.LocalPal)
	{
		WORD wTemp = 0;
		for(int i = 0; i<wColors; i++)
		{
			pClrTab[i].rgbRed = byGIF_Pal[wTemp++];
			pClrTab[i].rgbGreen = byGIF_Pal[wTemp++];
			pClrTab[i].rgbBlue = byGIF_Pal[wTemp++];
			pClrTab[i].rgbReserved = 0XFF;
		}
	}
	else
	{
		switch(wColors)
		{
		case 2:
		{
			pClrTab[0].rgbRed = 0x00;
			pClrTab[0].rgbGreen = 0x00;
			pClrTab[0].rgbBlue = 0x00;
			pClrTab[0].rgbReserved = 0XFF;

			pClrTab[1].rgbRed = 0XFF;
			pClrTab[1].rgbGreen = 0XFF;
			pClrTab[1].rgbBlue = 0XFF;
			pClrTab[1].rgbReserved = 0XFF;
			break;
		}
		case 16:
		{
			WORD wTemp = 0;
			for(int i = 0; i<wColors; i++)
			{
				pClrTab[i].rgbRed = bySysPal16[wTemp++];
				pClrTab[i].rgbGreen = bySysPal16[wTemp++];
				pClrTab[i].rgbBlue = bySysPal16[wTemp++];
				pClrTab[i].rgbReserved = 0XFF;
			}
			break;
		}
		case 256:
		{
			const HDC hDC = GetDC(nullptr);
			GetSystemPaletteEntries(hDC, 0, 256, (LPPALETTEENTRY)bySysPal256);
			for(int i = 0; i<wColors; i++)
			{
				pClrTab[i].rgbRed = bySysPal256[i].peRed;
				pClrTab[i].rgbGreen = bySysPal256[i].peGreen;
				pClrTab[i].rgbBlue = bySysPal256[i].peBlue;
				pClrTab[i].rgbReserved = 0x00;
			}

			ReleaseDC(nullptr, hDC);
			break;
		}
		}
	}
	::GlobalUnlock(m_hInfo);

	rownum = 0;
	bypass = 0x00;
	m_bInterlaced = gifImage->LocalFlag.Interlace ? TRUE : FALSE;
	// Decode LZW Data
	decode(pDataSrc);
}

static WORD wIncTable[5] = {8,8,4,2,0}; /* Interlace Pass Increments */
static WORD wBgnTable[5] = {0,4,2,1,0}; /* Interlace Pass Beginnings */

int CGif::out_line(LPBYTE pixels, int linelen)
{
	memcpy(m_pLineArray[rownum], pixels, linelen);

	if(m_bInterlaced)
	{
		rownum += wIncTable[bypass];
		if(rownum>=m_Size.cy)
		{
			bypass++;
			rownum = wBgnTable[bypass];
		}
	}
	else
	{
		rownum++;
		if(rownum>=m_Size.cy)
			return -1;
	}

	return 0;
}

static LONG code_mask[13] =
{
	0,
	0x0001, 0x0003,
	0x0007, 0x000F,
	0x001F, 0x003F,
	0x007F, 0x00FF,
	0x01FF, 0x03FF,
	0x07FF, 0x0FFF
};

/* This function initializes the decoder for reading a new image.
*/
SHORT CGif::init_exp(SHORT size)
{
	curr_size = size+1;
	top_slot = 1<<curr_size;
	clear = 1<<size;
	ending = clear+1;
	slot = newcodes = ending+1;
	navail_bytes = 0;
	nbits_left = 0;

	return (0);
}

/* get_next_code()
*- gets the next code from the Giffile.  Returns the code, or else
*a negative number in case of file errors...
 */
SHORT CGif::get_next_code(CDataSource* pDataSrc)
{
	SHORT i, x;
	ULONG ret;

	if(nbits_left==0)
	{
		if(navail_bytes<=0)
		{
			/* Out of bytes in current block, so read next block
			*/
			pbytes = byte_buff;
			if((navail_bytes = ReadByte(pDataSrc))<0)
				return (navail_bytes);
			else if(navail_bytes)
			{
				for(i = 0; i<navail_bytes; ++i)
				{
					if((x = ReadByte(pDataSrc))<0)
						return (x);
					byte_buff[i] = (BYTE)x;
				}
			}
		}
		b1 = *pbytes++;
		nbits_left = 8;
		--navail_bytes;
	}

	ret = b1>>(8-nbits_left);
	while(curr_size>nbits_left)
	{
		if(navail_bytes<=0)
		{
			/* Out of bytes in current block, so read next block
			*/
			pbytes = byte_buff;
			if((navail_bytes = ReadByte(pDataSrc))<0)
				return (navail_bytes);
			else if(navail_bytes)
			{
				for(i = 0; i<navail_bytes; ++i)
				{
					if((x = ReadByte(pDataSrc))<0)
						return (x);
					byte_buff[i] = (BYTE)x;
				}
			}
		}
		b1 = *pbytes++;
		ret |= b1<<nbits_left;
		nbits_left += 8;
		--navail_bytes;
	}
	nbits_left -= curr_size;
	ret &= code_mask[curr_size];
	return ((SHORT)(ret));
}

static BYTE stack[MAX_CODES+1]; /* Stack for storing pixels */
static BYTE suffix[MAX_CODES+1]; /* Suffix table */
static WORD prefix[MAX_CODES+1]; /* Prefix linked list */

short CGif::decode(CDataSource* pDataSrc)
{
	register BYTE* sp,*bufptr;
	register SHORT code, fc, oc, bufcnt;
	LPBYTE buf;
	SHORT c, size, ret;
	const int linewidth = m_Size.cx;
	INT bad_code_count;

	/* Initialize for decoding a new image...
	 */
	bad_code_count = 0;
	if((size = ReadByte(pDataSrc))<0)
		return (size);
	//		return 0;
	if(size<2||9<size)
		return (BAD_CODE_SIZE);
	/* out_line = outline;
	 */
	init_exp(size);

	/* Initialize in case they forgot to put in a clear code.
	 *(This shouldn't happen, but we'll try and decode it anyway...)
	 */
	oc = fc = 0;

	/* Allocate space for the decode buffer
	 */
	if((buf = (BYTE*)malloc(linewidth+1))==nullptr)
		return (OUT_OF_MEMORY);

	/* Set up the stack pointer and decode buffer pointer
	 */
	sp = stack;
	bufptr = buf;
	bufcnt = linewidth;

	/* This is the main loop.  For each code we get we pass through the
	*linked list of prefix codes, pushing the corresponding "character" for
	*each code onto the stack.  When the list reaches a single "character"
	*we push that on the stack too, and then start unstacking each
	*character for output in the correct order.  Special handling is
	*included for the clear code, and the whole thing ends when we get
	*an ending code.
	*/
	while((c = get_next_code(pDataSrc))!=ending)
	{
		/* ifwe had a file error, return without completing the decode
		 */
		if(c<0)
		{
			free(buf);
			return (0);
		}

		/* ifthe code is a clear code, reinitialize all necessary items.
		 */
		if(c==clear)
		{
			curr_size = size+1;
			slot = newcodes;
			top_slot = 1<<curr_size;

			/* Continue reading codes until we get a non - clear code
			 *(Another unlikely, but possible case...)
			 */
			while((c = get_next_code(pDataSrc))==clear);
			/* ifwe get an ending code immediately after a clear code
			 *(Yet another unlikely case), then break out of the loop.
			 */
			if(c==ending)
				break;

			/* Finally, ifthe code is beyond the range of already set codes,
			 *(This one had better NOT happen...  I have no idea what will
			*result from this, but I doubt it will look good...) then set it
			*to color zero.
			 */
			if(c>=slot)
				c = 0;

			oc = fc = c;

			/* And let us not forget to put the char into the buffer... And
			*if, on the off chance, we were exactly one pixel from the end
			*of the line, we have to send the buffer to the out_line()
			*routine...
			 */
			*bufptr++ = c;
			if(--bufcnt==0)
			{
				if((ret = out_line(buf, linewidth))<0)
				{
					free(buf);
					return (ret);
				}
				bufptr = buf;
				bufcnt = linewidth;
			}
		}
		else
		{
			/* In this case, it's not a clear code or an ending code, so
			* it must be a code code...  So we can now decode the code into
			* a stack of character codes.(Clear as mud, right?)
			*/
			code = c;

			/* Here we go again with one of those off chances...  If, on the
			* off chance, the code we got is beyond the range of those already
			* set up(Another thing which had better NOT happen...) we trick
			* the decoder into thinking it actually got the last code read.
			*(Hmmn... I'm not sure why this works...  But it does...)
			*/
			if(code>=slot)
			{
				if(code>slot)
					++bad_code_count;
				code = oc;
				*sp++ = fc;
			}

			/* Here we scan back along the linked list of prefixes, pushing
			* helpless characters(ie. suffixes) onto the stack as we do so.
			*/
			while(code>=newcodes)
			{
				*sp++ = suffix[code];
				code = prefix[code];
			}

			/* Push the last character on the stack, and set up the new
			* prefix and suffix, and ifthe required slot number is greater
			* than that allowed by the current bit size, increase the bit
			* size.(NOTE - ifwe are all full, we *don't* save the new
			* suffix and prefix...  I'm not certain ifthis is correct...
			* it might be more proper to overwrite the last code...
			*/
			*sp++ = code;
			if(slot<top_slot)
			{
				fc = code;
				suffix[slot] = (BYTE)fc;
				prefix[slot++] = oc;
				oc = c;
			}
			if(slot>=top_slot)
			{
				if(curr_size<12)
				{
					top_slot <<= 1;
					++curr_size;
				}
			}

			/* Now that we've pushed the decoded string(in reverse order)
			* onto the stack, lets pop it off and put it into our decode
			* buffer...  And when the decode buffer is full, write another
			* line...
			*/
			while(sp>stack)
			{
				*bufptr++ = *(--sp);
				if(--bufcnt==0)
				{
					if((ret = out_line(buf, linewidth))<0)
					{
						free(buf);
						return (ret);
					}
					bufptr = buf;
					bufcnt = linewidth;
				}
			}
		}
	}
	ret = 0;
	if(bufcnt!=linewidth)
		ret = out_line(buf, (linewidth-bufcnt));
	free(buf);
	return (ret);
}

CDib* CGif::Clone() const
{
	CGif* pGif = new CGif;

	Copy(pGif);

	return pGif;
}

void CGif::Copy(CGif* denGif) const
{
	CDecode::Copy(denGif);
}
