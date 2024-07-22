#include "stdafx.h"
#include "Tif.h"

#include "datasrc.h"
#include "except.h"
#include "tif_msrc.h"
//extern "C"
//{
	#include "../../ThirdParty/tiff/4.5.1/libtiff/tiffio.h"
//}
#define CVT(x) (((x)*255L)/((1L<<16)-1))



#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3

void SetRGBAPixel(RGBAPIXEL* pPixel, BYTE r, BYTE g, BYTE b, BYTE a)
{
	((BYTE*)pPixel)[RGBA_RED] = r;
	((BYTE*)pPixel)[RGBA_GREEN] = g;
	((BYTE*)pPixel)[RGBA_BLUE] = b;
	((BYTE*)pPixel)[RGBA_ALPHA] = a;
}

CTiff::CTiff()
	: CDecode()
{
}

CTiff::~CTiff()
{
}
//
void CTiff::DoDecode(RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc)
{
	TIFF* tif = TIFFOpenMem(pDataSrc->ReadEverything(), pDataSrc->GetFileSize(), nullptr);

	//  if(!tif)
	//	raiseError (ERR_WRONG_SIGNATURE, m_szLastErr);

	uint16 BitsPerSample;
	uint16 SamplePerPixel;
	float xDPI;
	float yDPI;

	// get tagged image parameters

	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
	TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplePerPixel);

	TIFFGetFieldDefaulted(tif, TIFFTAG_XRESOLUTION, &xDPI);
	TIFFGetFieldDefaulted(tif, TIFFTAG_YRESOLUTION, &yDPI);

	m_DPI.cx = xDPI;
	m_DPI.cy = yDPI;

	// For the time being, paintlib bitmaps only actually support 8
	// or 32bpp; so the following mapping should cover all cases ...	
	if(SamplePerPixel==1&&BitsPerSample<=8&&*pDestBPP<32&&!TIFFIsTiled(tif))
		doLoColor(tif, pDestBPP);
	else
		doHiColor(tif, pDestBPP);

	TIFFClose(tif);
}

void CTiff::doHiColor(TIFF* tif, int* pDestBPP)
{
	int ok;
	ULONG x, y;

	TIFFRGBAImage img;
	char emsg[1024];
	BYTE* pBits;

	CalcDestBPP(32, pDestBPP); // non-palette formats handled here

	ok = TIFFRGBAImageBegin(&img, tif, 0, emsg);

	if(ok == 0)
	{
		TIFFClose(tif);
		//    raiseError (ERR_WRONG_SIGNATURE, m_szLastErr);
	}

	try
	{
		Create(img.width, img.height, 32, (img.alpha != 0));
		pBits = new BYTE[img.width*img.height*4];
		//  if(pBits == nullptr)
		//      raiseError (ERR_NO_MEMORY, "Out of memory allocating TIFF buffer.");
	}
	catch(CTextException* ex)
	{
		TIFFClose(tif);
		throw;
	}

	ok = TIFFRGBAImageGet(&img, (uint32*)pBits, img.width, img.height);
	if(!ok)
	{
		TIFFRGBAImageEnd(&img);
		TIFFClose(tif);
		//    raiseError (ERR_WRONG_SIGNATURE, m_szLastErr);
	}

	// Correct the byte ordering
	for(y = 0; y < img.height; y++)
	{
		BYTE* pSrc = pBits + (img.height - y - 1)*img.width*4;
		RGBAPIXEL* pPixel = (RGBAPIXEL*)(m_pLineArray[y]);
		for(x = 0; x < img.width; x++)
		{
			SetRGBAPixel(pPixel, *pSrc, *(pSrc + 1), *(pSrc + 2), *(pSrc + 3));
			pPixel++;
			pSrc += 4;
		}
	}

	// Clean up.
	delete[]pBits;
	TIFFRGBAImageEnd(&img);
}

void CTiff::doLoColor(TIFF* tif, int* pDestBPP)
{
	uint32 imageLength;
	uint32 imageWidth;
	uint16 BitsPerSample;
	uint16 SamplePerPixel;
	int16 PhotometricInterpretation;
	uint32 row;
	BYTE* pBits;

	TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
	TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplePerPixel);
	TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &PhotometricInterpretation);

	int32 LineSize = TIFFScanlineSize(tif); //Number of bytes in one line
	const int nBPP = SamplePerPixel == 1 && BitsPerSample == 1 ? 1 : 8;
	if(*pDestBPP == 0)
		*pDestBPP = nBPP;

	try
	{
		Create(imageWidth, imageLength, nBPP, 0);
		pBits = new BYTE[LineSize];
		//		ifp(Bits == nullptr)
		//			raiseError (ERR_NO_MEMORY, "Out of memory allocating TIFF buffer.");
	}
	catch(CTextException* ex)
	{
		TIFFClose(tif);
		throw;
	}

	BITMAPINFOHEADER* DibInfo = (BITMAPINFOHEADER*)::GlobalLock((HGLOBAL)m_hInfo);
	RGBQUAD* pClrTab = (RGBQUAD*)(((BITMAPINFO*)(DibInfo))->bmiColors);

	// phase one: build color map

	if /* monochrome (=bitonal) or grayscale */
		(PhotometricInterpretation == PHOTOMETRIC_MINISWHITE || PhotometricInterpretation == PHOTOMETRIC_MINISBLACK)
	{
		const int numColors = 1 << BitsPerSample;
		const BYTE step = 255/(numColors - 1);
		BYTE* pb = (BYTE*)(pClrTab);
		int offset = sizeof(RGBAPIXEL);
		if(PhotometricInterpretation == PHOTOMETRIC_MINISWHITE)
		{
			pb += (numColors - 1)*sizeof(RGBAPIXEL);
			offset = -offset;
		}
		// warning: the following ignores possible halftone hints
		for(int i = 0; i < numColors; ++i, pb += offset)
		{
			pb[RGBA_RED] = pb[RGBA_GREEN] = pb[RGBA_BLUE] = i*step;
			pb[RGBA_ALPHA] = 255;
		}
	}

	//PhotometricInterpretation = 2 image is RGB
	//PhotometricInterpretation = 3 image has a color palette
	else if(PhotometricInterpretation == PHOTOMETRIC_PALETTE)
	{
		uint16* red;
		uint16* green;
		uint16* blue;
		int16 i, Palette16Bits;

		// we get pointers to libtiff-owned colormaps
		i = TIFFGetField(tif, TIFFTAG_COLORMAP, &red, &green, &blue);

		//Is the palette 16 or 8 bits ?
		Palette16Bits = checkcmap(1 << BitsPerSample, red, green, blue) == 16;

		//load the palette in the DIB
		for(i = 0; i < 1 << BitsPerSample; ++i)
		{
			pClrTab[i].rgbRed = (BYTE)(Palette16Bits ? CVT(red[i]) : red[i]);
			pClrTab[i].rgbGreen = (BYTE)(Palette16Bits ? CVT(green[i]) : green[i]);
			pClrTab[i].rgbBlue = (BYTE)(Palette16Bits ? CVT(blue[i]) : blue[i]);
			pClrTab[i].rgbReserved = 0XFF;
		}
	}
	//ccy	else
	//ccy		Trace(2, "unexpected PhotometricInterpretation in CTIFFDecoder::DoLoColor()");

	// phase two: read image itself
	::GlobalUnlock(m_hInfo);
	//generally, TIFF images are ordered from upper-left to bottom-right
	// we implicitly assume PLANARCONFIG_CONTIG

	if(BitsPerSample > 8)
	{
		//ccy		Trace(2, "unexpected bit-depth in CTIFFDecoder::DoLoColor()");
	}
	else
		for(row = 0; row < imageLength; ++row)
		{
			uint16 x;
			const int status = TIFFReadScanline(tif, pBits, row, 0);
			if(status == -1 && row < imageLength/3)
			{
				delete[] pBits;
				// we should maybe free the BMP memory as well...
				//ccy			raiseError (ERR_INTERNAL, m_szLastErr);
			}
			/*
			if(BitsPerSample == 1)	// go ahead, waste space ;-)
			for (x=0; x < imageWidth; ++x)
			pLineArray[row][x] = pBits[x/8] & (128 >> (x & 7)) ? 1 : 0;
			else */
			if(BitsPerSample == 4)
			{
				for(x = 0; x < imageWidth/2; ++x)
				{
					m_pLineArray[row][2*x] = (pBits[x] & 0xf0) >> 4;
					m_pLineArray[row][2*x + 1] = (pBits[x] & 0x0f);
				}
				// odd number of pixels
				if(imageWidth & 1)
					m_pLineArray[row][imageWidth - 1] = (pBits[x] & 0xf0) >> 4;
			}
			else //if(BitsPerSample == 8 || BitsPerSample == 1)
				memcpy(m_pLineArray[row], pBits, LineSize);
		}

	delete[] pBits;
}

int CTiff::checkcmap(int n, unsigned short* r, unsigned short* g, unsigned short* b)
{
	while(n-- > 0)
		if(*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
			return (16);

	return (8);
}

CDib* CTiff::Clone() const
{
	CTiff* pTiff = new CTiff;

	Copy(pTiff);

	return pTiff;
}

void CTiff::Copy(CTiff* denTiff) const
{
	CDecode::Copy(denTiff);
}
