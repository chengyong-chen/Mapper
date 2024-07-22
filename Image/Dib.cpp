#include "stdafx.h"
#include <Wingdi.h>
#include <comdef.h>.

#include "Dib.h"
#include "Bmp.h"
#include "Bme.h"
#include "Bmb.h"
#include "Gif.h"
#include "Gib.h"
#include "Tif.h"
#include "pngencoder.h"

#include "../Pbf/writer.hpp"
#include "../Public/Base64.h"

#include "../../Thirdparty/ZLib/1.2.11/Zlib.h"
#include <boost/json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CArchive& AFXAPI operator <<(CArchive& ar, CSizeF& size)
{
	ar.Write(&size, sizeof(CSizeF));
	return ar;
}

CArchive& AFXAPI operator >>(CArchive& ar, CSizeF& size)
{
	ar.Read(&size, sizeof(CSizeF));
	return ar;
}


CDib::CDib()
{
	m_hInfo = nullptr;
	m_hData = nullptr;

	m_nBpp = 0;
	m_DPI = CSizeF(72, 72);
	m_pLineArray = nullptr;
	m_Source = nullptr;
}

CDib::~CDib()
{
	if(m_Source != nullptr)
	{
		delete m_Source;
		m_Source = nullptr;
	}
	if(m_hData != nullptr)
	{
		::GlobalFree(m_hData);
		m_hData = nullptr;
	}
	if(m_hInfo != nullptr)
	{
		::GlobalFree(m_hInfo);
		m_hInfo = nullptr;
	}

	delete m_pLineArray;
	m_pLineArray = nullptr;
}
void CDib::Sha1(boost::uuids::detail::sha1& sha1) const
{
	if(m_hData != nullptr)
	{
		BYTE* bytes = (BYTE*)::GlobalLock(m_hData);
		const unsigned long size = GlobalSize(bytes);
		sha1.process_bytes(bytes, size);
		::GlobalUnlock(m_hData);
	}
	if(m_hInfo != nullptr)
	{
		BYTE* bytes = (BYTE*)::GlobalLock(m_hInfo);
		const unsigned long size = GlobalSize(bytes);
		sha1.process_bytes(bytes, size);
		::GlobalUnlock(m_hInfo);
	}
}
CDib* CDib::Clone() const
{
	CDib* pDib = new CDib;

	Copy(pDib);

	return pDib;
}

void CDib::Copy(CDib* denDib) const
{
	denDib->m_DPI = m_DPI;
	denDib->m_Size = m_Size;
	denDib->m_nBpp = m_nBpp;

	if(m_Source != nullptr)
	{
		denDib->m_Source = m_Source->Clone();
	}
	if(m_hInfo != nullptr)
	{
		const int bytes = ::GlobalSize(m_hInfo);
		denDib->m_hInfo = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
		const LPVOID buf1 = ::GlobalLock(m_hInfo);
		const LPVOID buf2 = ::GlobalLock(denDib->m_hInfo);

		::memcpy(buf2, buf1, bytes);

		::GlobalUnlock(denDib->m_hInfo);
		::GlobalUnlock(m_hInfo);
	}
	if(m_hData != nullptr)
	{
		denDib->m_hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, ::GlobalSize(m_hData));
		const LPVOID buf1 = ::GlobalLock(m_hData);
		const LPVOID buf2 = ::GlobalLock(denDib->m_hData);

		::memcpy(buf2, buf1, ::GlobalSize(m_hData));

		::GlobalUnlock(denDib->m_hData);
		::GlobalUnlock(m_hData);
	}
}

bool CDib::Open(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;
	BITMAPINFOHEADER infoHeader;
	DWORD FileMark;

	if(!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		CString messge;
		messge.Format(_T("Unable to open %s!"), lpszPathName);
		AfxMessageBox(messge);
		return false;
	}

	file.Read(&FileMark, 4);
	if(FileMark == 0X31424D42 || FileMark == 0X31424947)
	{
		file.Seek(8, CFile::begin);
		file.Read(&FileMark, 4);
		file.Seek(FileMark, CFile::begin);
	}
	else
	{
		file.Seek(sizeof(BITMAPFILEHEADER), CFile::begin);
	}

	file.Read(&infoHeader, sizeof(BITMAPINFOHEADER));
	m_DPI.cx = std::lround(infoHeader.biXPelsPerMeter*0.0254f);
	m_DPI.cy = std::lround(infoHeader.biYPelsPerMeter*0.0254f);
	if(m_DPI.cx == 0 || m_DPI.cy == 0)
	{
		m_DPI.cx = 72;
		m_DPI.cy = 72;
	}

	if((DWORD)infoHeader.biHeight > INT_MAX || (DWORD)infoHeader.biWidth > INT_MAX)
	{
		file.Close();
		AfxMessageBox(_T("The image's size is too big!"));
		return false;
	}

	m_Size = CSize((int)infoHeader.biWidth, (int)infoHeader.biHeight);
	m_nBpp = infoHeader.biBitCount;
	const int nColors = infoHeader.biClrUsed == 0 ? 0 : infoHeader.biClrUsed;

	m_hInfo = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(BITMAPINFOHEADER) + nColors*sizeof(RGBQUAD));
	const LPBITMAPINFO infobuf = (LPBITMAPINFO)::GlobalLock(m_hInfo);
	const int Δ = -(sizeof(BITMAPINFOHEADER) + nColors*sizeof(RGBQUAD));
	file.Seek(Δ, CFile::current);
	file.Read(infobuf, sizeof(BITMAPINFOHEADER) + nColors*sizeof(RGBQUAD));
	::GlobalUnlock(m_hInfo);

	file.Close();
	return true;
}

/*
void CDib::Draw(Gdiplus::Graphics& g,const CRect& rect,CRect drawRect,int nAlpha)
{
	if(m_image != nullptr)
	{
		float xscale = (float)m_Size.cx/rect.Width(); //Í¼ÏñºÍÎÄµµµÄ±ÈÀý
		float yscale = (float)m_Size.cy/rect.Height();

		CRect dibRect = drawRect;
		dibRect.OffsetRect(-rect.left,-rect.top); //ÖØ»æÇøÓòÏà¶ÔÍ¼Ïñ×óÏÂ½ÇµÄÎÄµµ×ø±ê

		dibRect.left   = long(dibRect.left  *xscale);
		dibRect.top    = long(dibRect.top   *yscale);
		dibRect.right  = long(dibRect.right *xscale+0.99);
		dibRect.bottom = long(dibRect.bottom*yscale+0.99);//ÖØ»æÇøÓòÏà¶ÔÍ¼Ïñ×óÏÂ½ÇµÄ Í¼Ïñ ×ø±ê

		drawRect = dibRect;
		xscale = (float)rect.Width() /m_Size.cx;//ÎÄµµºÍÍ¼ÏñµÄ±ÈÀý
		yscale = (float)rect.Height()/m_Size.cy;

		drawRect.left   = long(dibRect.left  *xscale);
		drawRect.right  = long(dibRect.right *xscale);
		drawRect.top    = long(dibRect.top   *yscale);
		drawRect.bottom = long(dibRect.bottom*yscale);//ÖØ»æÇøÓòÏà¶ÔÍ¼Ïñ×óÏÂ½ÇµÄ ÎÄµµ ×ø±ê
		drawRect.OffsetRect(rect.left,rect.top);//ÖØ»æÇøÓòµÄ×ø±ê
		RectF destination = Gdiplus::RectF(drawRect.left,drawRect.top,drawRect.Width(),drawRect.Height());

		if(nAlpha>0)
		{
			float fBlend = (100-nAlpha)/100.0;
			ColorMatrix BitmapMatrix =	{
										  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
										  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
										  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
										  0.0f, 0.0f, 0.0f, fBlend, 0.0f,
										  0.0f, 0.0f, 0.0f, 0.0f, 1.0f
										};
			ImageAttributes ImgAttr;
			ImgAttr.SetColorMatrix(&BitmapMatrix,ColorMatrixFlagsDefault,ColorAdjustTypeBitmap);

			g.DrawImage(m_image,destination,dibRect.left,m_Size.cy - dibRect.top,dibRect.Width(),-dibRect.Height(),UnitPixel,&ImgAttr);
		}
		else
		{
			Status status = g.DrawImage(m_image,destination,dibRect.left,m_Size.cy - dibRect.top,dibRect.Width(),-dibRect.Height(),UnitPixel);
			int i=0;

		}
	}
}
*/
Gdiplus::Bitmap* CDib::GetBitmap(bool asMask) const
{
	if(m_hData == nullptr && m_hInfo == nullptr)
		return nullptr;

	const BITMAPINFO* pInfo = (BITMAPINFO*)::GlobalLock((HGLOBAL)m_hInfo);
	Gdiplus::Bitmap* bitmap = ::new Gdiplus::Bitmap(pInfo->bmiHeader.biWidth, pInfo->bmiHeader.biHeight, PixelFormat32bppARGB);//Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromBITMAPINFO(pInfo, pData); this lose the alpfa channel
	if(bitmap->GetLastStatus() != Gdiplus::Ok)
	{
		::GlobalUnlock(m_hInfo);
		return nullptr;
	}

	const Gdiplus::Rect rect(0, 0, pInfo->bmiHeader.biWidth, pInfo->bmiHeader.biHeight);
	Gdiplus::BitmapData bdata;
	if(bitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bdata) != Gdiplus::Ok)
	{
		::GlobalUnlock(m_hInfo);
		return nullptr;
	}

	const BYTE* pData = (BYTE*)::GlobalLock(m_hData);
	if(pInfo->bmiHeader.biBitCount == 32)
	{
		for(int y = 0; y < pInfo->bmiHeader.biHeight; y++)
		{
			memcpy((BYTE*)bdata.Scan0 + (y*bdata.Stride), pData + ((pInfo->bmiHeader.biHeight - y - 1)*bdata.Stride), bdata.Stride);
		}
	}
	else if(pInfo->bmiHeader.biBitCount == 8 && asMask)
	{
		const int rowbytes = pInfo->bmiHeader.biWidth % 4 == 0 ? pInfo->bmiHeader.biWidth : (pInfo->bmiHeader.biWidth/4 + 1)*4;
		for(int y = 0; y < pInfo->bmiHeader.biHeight; y++)
		{
			for(int x = 0; x < pInfo->bmiHeader.biWidth; x++)
			{
				*((BYTE*)bdata.Scan0 + (y*bdata.Stride) + x*4 + 3) = *(pData + ((pInfo->bmiHeader.biHeight - y - 1)*rowbytes + x));
			}
		}
	}
	else//to do
	{
	}
	::GlobalUnlock(m_hData);
	::GlobalUnlock(m_hInfo);

	bitmap->UnlockBits(&bdata);
	return bitmap;
}
void CDib::Draw(Gdiplus::Graphics& g, const Gdiplus::Rect& fitinRect, Gdiplus::Rect drawRect, bool asMask) const
{
	if(m_hData == nullptr && m_hInfo == nullptr)
		return;

	const float xscale = (float)m_Size.cx/fitinRect.Width; //Í¼ÏñºÍÎÄµµµÄ±ÈÀý
	const float yscale = (float)m_Size.cy/fitinRect.Height;

	Gdiplus::Rect dibRect;
	dibRect.X = long(drawRect.X*xscale);
	dibRect.Width = long(drawRect.Width*xscale);
	dibRect.Y = long(drawRect.Y*yscale);//ÖØ»æÇøÓòÏà¶ÔÍ¼Ïñ×óÏÂ½ÇµÄ Í¼Ïñ ×ø±ê
	dibRect.Height = long(drawRect.Height*yscale);

	Gdiplus::Bitmap* bitmap = GetBitmap(asMask);
	g.DrawImage(bitmap, drawRect, dibRect.X, dibRect.Y, dibRect.Width, dibRect.Height, Gdiplus::UnitPixel);
	::delete bitmap;
}

void CDib::Draw(Gdiplus::Graphics& g, const CRect& fitinRect, CRect drawRect, int nAlpha)
{
	if(m_hData == nullptr && m_hInfo == nullptr)
		return;

	const float xscale = (float)m_Size.cx/fitinRect.Width(); //Í¼ÏñºÍÎÄµµµÄ±ÈÀý
	const float yscale = (float)m_Size.cy/fitinRect.Height();

	CRect rect = drawRect;
	rect.OffsetRect(-fitinRect.left, -fitinRect.top); //ÖØ»æÇøÓòÏà¶ÔÍ¼Ïñ×óÉÏ½ÇµÄÎÄµµ×ø±ê
	rect.NormalizeRect();//½«×ø±êÏµYÖá·½Ïò±äÎªÏòÏÂ

	CRect dibRect;
	dibRect.left = long(rect.left*xscale);
	dibRect.right = long(rect.right*xscale);
	dibRect.top = long(rect.top*yscale);//ÖØ»æÇøÓòÏà¶ÔÍ¼Ïñ×óÏÂ½ÇµÄ Í¼Ïñ ×ø±ê
	dibRect.bottom = long(rect.bottom*yscale);


	const BITMAPINFO* pInfo = (BITMAPINFO*)::GlobalLock((HGLOBAL)m_hInfo);
	BYTE* pData = (BYTE*)::GlobalLock(m_hData);

	// this will lose the alpfa channel
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromBITMAPINFO(pInfo, pData);
	Gdiplus::ImageAttributes ImgAttr;
	ImgAttr.SetWrapMode(Gdiplus::WrapMode::WrapModeTileFlipXY);
	if(nAlpha > 0)
	{
		const float fBlend = (100 - nAlpha)/100.0;
		const Gdiplus::ColorMatrix BitmapMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, fBlend, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		ImgAttr.SetColorMatrix(&BitmapMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
	}

	g.DrawImage(bitmap, Gdiplus::RectF(drawRect.left, drawRect.top, drawRect.Width(), drawRect.Height()), dibRect.left, dibRect.top, dibRect.Width(), dibRect.Height(), Gdiplus::UnitPixel, &ImgAttr);

	::GlobalUnlock(m_hInfo);
	::GlobalUnlock(m_hData);

	delete bitmap;
	bitmap = nullptr;
}



//Bitmap* CreateBitmapFromHBITMAP(HBITMAP hbmp)
//{	
//	int width, height;
//	byte* sourceBytes = GetBitmapSizeAndData(m_hbmp, width, height);
//
//	// Create an argb GDI+ bitmap of the same dimensions.
//	Bitmap* copy = new Bitmap(width, height, PixelFormat32bppPARGB);
//
//	// Get access to the Gdiplus::Bitmap's pixel data
//	BitmapData bmd;
//	Rect rect(0, 0, width, height);
//	copy->LockBits(&rect, ImageLockMode::ImageLockModeWrite,		PixelFormat32bppPARGB, &bmd);
//
//		int lineSize = width*4;
//	byte* destBytes = (byte*)(bmd.Scan0);
//	for(int y = 0; y<height; y++)
//	{
//		memcpy(destBytes+(y*lineSize), sourceBytes+((height-y-			1)*lineSize), lineSize);
//	}
//
//	copy->UnlockBits(&bmd);


#ifdef _DEBUG
void CDib::AssertValid() const
{
	CObject::AssertValid();
}

void CDib::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif//_DEBUG

void CDib::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_Size;
		ar << m_DPI;
		const LPSTR iBuf = (LPSTR)::GlobalLock(m_hInfo);
		const size_t iSize = GlobalSize(iBuf);
		ar << iSize;
		ar.Write(iBuf, iSize);
		::GlobalUnlock(m_hInfo);

		if(m_Source != nullptr)
		{
			ar << (unsigned int)3;
			ar << (unsigned int)m_Source->length;
			ar.Write(m_Source->bytes, m_Source->length);
			ar << (int)m_Source->format;
		}
		else if(m_hData == nullptr)
		{
			ar << (unsigned int)0;
		}
		else {
			BYTE* oriBytes = (BYTE*)::GlobalLock(m_hData);
			const unsigned long oriSize = oriBytes == nullptr ? 0 : ::GlobalSize(oriBytes);
			unsigned long zipSize = oriSize == 0 ? 0 : compressBound(oriSize);
			if(oriSize == 0)
			{
				ar << (unsigned int)0;
			}
			else if(zipSize == 0)
			{
				ar << (unsigned int)1;
				ar << oriSize;
				ar.Write(oriBytes, oriSize);
			}
			else
			{
				BYTE* zipBytes = new BYTE[zipSize];
				if(::compress(zipBytes, &zipSize, oriBytes, oriSize) == Z_OK)
				{
					ar << (unsigned int)2;
					ar << oriSize;
					ar << zipSize;
					ar.Write(zipBytes, zipSize);
				}
				else
				{
					ar << (unsigned int)1;
					ar << oriSize;
					ar.Write(oriBytes, oriSize);
				}
				delete[] zipBytes;
			}

			::GlobalUnlock(m_hData);
		}
	}
	else
	{
		ar >> m_Size;
		ar >> m_DPI;

		size_t iSize;
		ar >> iSize;
		m_hInfo = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, iSize);
		const LPSTR iBuf = (LPSTR)::GlobalLock(m_hInfo);
		ar.Read(iBuf, iSize);
		::GlobalUnlock(m_hInfo);

		unsigned int format = 1;
		ar >> format;
		switch(format)
		{
			case 0:
				break;
			case 1:
				{
					unsigned long oriSize;
					ar >> oriSize;
					m_hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, oriSize);
					const LPSTR dBuf = (LPSTR)::GlobalLock(m_hData);
					ar.Read(dBuf, oriSize);
					::GlobalUnlock(m_hData);
				}
				break;
			case 2:
				{
					unsigned long oriSize;
					ar >> oriSize;
					unsigned long zipSize;
					ar >> zipSize;

					auto hData1 = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, zipSize);
					if(hData1 == nullptr)
					{
						for(int index = 0; index < zipSize; index++)
						{
							BYTE byte;
							ar >> byte;
						}
						AfxMessageBox(_T("No enough memory to load the image"));
					}
					else
					{
						const LPSTR zipBytes = (LPSTR)::GlobalLock(hData1);
						ar.Read(zipBytes, zipSize);
						::GlobalUnlock(hData1);

						auto hData2 = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, oriSize);
						if(hData2 == nullptr)
						{
							::GlobalFree(hData1);
							AfxMessageBox(_T("No enough memory to uncomparess the image"));
						}
						else
						{
							const LPSTR oriBytes = (LPSTR)::GlobalLock(hData2);
							if(::uncompress((Bytef*)oriBytes, &oriSize, (Bytef*)zipBytes, zipSize) == Z_OK)
							{
								::GlobalUnlock(hData2);
								::GlobalFree(hData1);
								m_hData = hData2;
							}
							else
							{
								::GlobalUnlock(hData2);
								::GlobalFree(hData1);
								::GlobalFree(hData2);
								m_hData = nullptr;
							}
						}
					}
				}
				break;
			case 3:
				m_Source = new Blob();
				ar >> m_Source->length;
				m_Source->bytes = ::malloc(m_Source->length);
				ar.Read(m_Source->bytes, m_Source->length);
				ar >> (int&)m_Source->format;
				break;
			case 4:
				break;
		}
	}
}

void CDib::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_Size;
		ar << m_DPI;
		const LPSTR iBuf = (LPSTR)::GlobalLock(m_hInfo);
		const size_t iSize = GlobalSize(iBuf);
		ar << iSize;
		ar.Write(iBuf, iSize);
		::GlobalUnlock(m_hInfo);
		const LPSTR dBuf = (LPSTR)::GlobalLock(m_hData);
		const size_t dSize = GlobalSize(dBuf);
		ar << dSize;
		ar.Write(dBuf, dSize);
		::GlobalUnlock(m_hData);
	}
	else
	{
		ar >> m_Size;
		ar >> m_DPI;

		size_t iSize;
		ar >> iSize;
		m_hInfo = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, iSize);
		const LPSTR iBuf = (LPSTR)::GlobalLock(m_hInfo);
		ar.Read(iBuf, iSize);
		::GlobalUnlock(m_hInfo);

		size_t dSize;
		ar >> dSize;
		m_hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dSize);
		const LPSTR dBuf = (LPSTR)::GlobalLock(m_hData);
		ar.Read(dBuf, dSize);
		::GlobalUnlock(m_hData);
	}
}
void CDib::ReleaseWeb(boost::json::object& json, std::string key) const
{
	const LPBITMAPINFO pBitmapInfo = (LPBITMAPINFO)::GlobalLock((HGLOBAL)m_hInfo);
	const LPVOID pBitmapDara = (LPVOID)::GlobalLock(m_hData);

	VOID* ppvPNGData;
	DWORD pdwPNGSize;

	LONG lRet = WritePngFile(&ppvPNGData, &pdwPNGSize, pBitmapInfo, pBitmapDara, false, false, PngFilterTypes::WPF_FilterBest, PngCompressionLevels::WPF_BestCompression);

	char* blob = Base64_Encode((char*)ppvPNGData, pdwPNGSize);
	json[key] = std::string(blob);
	::free(blob);

	free(ppvPNGData);
	::GlobalUnlock(m_hData);
	::GlobalUnlock(m_hInfo);
}
void CDib::ReleaseWeb(pbf::writer& writer, CSize offset) const
{
	const LPBITMAPINFO pBitmapInfo = (LPBITMAPINFO)::GlobalLock((HGLOBAL)m_hInfo);
	const LPVOID pBitmapDara = (LPVOID)::GlobalLock(m_hData);

	VOID* ppvPNGData;
	DWORD pdwPNGSize;

	LONG lRet = WritePngFile(&ppvPNGData, &pdwPNGSize, pBitmapInfo, pBitmapDara, false, false, PngFilterTypes::WPF_FilterBest, PngCompressionLevels::WPF_BestCompression);
	char* blob = Base64_Encode((char*)ppvPNGData, pdwPNGSize);
	writer.add_string(blob);
	::free(blob);

	free(ppvPNGData);
	::GlobalUnlock(m_hData);
	::GlobalUnlock(m_hInfo);
}
void CDib::ExportPs3(FILE* file, double fZoom) const
{
}

void CDib::ExportPdf(HPDF_Page page) const
{
}

void CDib::Create(LONG Width, LONG Height, WORD Bpp, bool bAlphaChannel)
{
	int memInfo = sizeof(BITMAPINFOHEADER);
	if(Bpp < 16)
		memInfo += (1 << Bpp)*sizeof(RGBQUAD);

	m_hInfo = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, memInfo);
	if(m_hInfo == nullptr)
	{
		AfxThrowMemoryException();
		return;
	}

	BITMAPINFOHEADER* DibInfo = (BITMAPINFOHEADER*)::GlobalLock((HGLOBAL)m_hInfo);// Fill in the header info	
	DibInfo->biSize = sizeof(BITMAPINFOHEADER);
	DibInfo->biWidth = Width;
	DibInfo->biHeight = Height;
	DibInfo->biPlanes = 1;
	DibInfo->biBitCount = Bpp;
	DibInfo->biCompression = BI_RGB; // No compression
	DibInfo->biSizeImage = 0;
	DibInfo->biXPelsPerMeter = std::lround(m_DPI.cx/0.0254);
	DibInfo->biYPelsPerMeter = std::lround(m_DPI.cy/0.0254);
	DibInfo->biClrUsed = 0; // Always use the whole palette.
	DibInfo->biClrImportant = 0;
	if(DibInfo->biBitCount < 16)
	{
		RGBQUAD* pClrTab = (RGBQUAD*)(((BITMAPINFO*)(DibInfo))->bmiColors);
		const int NumColors = 1 << Bpp;
		const double ColFactor = 255/(NumColors - 1);
		for(int i = 0; i < NumColors; i++)
		{
			pClrTab[i].rgbRed = int(i*ColFactor);
			pClrTab[i].rgbGreen = int(i*ColFactor);
			pClrTab[i].rgbBlue = int(i*ColFactor);
			pClrTab[i].rgbReserved = 0XFF;
		}
	}

	::GlobalUnlock(m_hInfo);

	m_Size.cx = Width;
	m_Size.cy = Height;
	m_nBpp = Bpp;
	m_bAlphaChannel = bAlphaChannel;

	int nLineBytes = Width*m_nBpp/8;
	if(m_nBpp == 1 && Width % 8)
		++nLineBytes;
	const int LineLen = (nLineBytes + 3) & ~3;

	m_hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, LineLen*Height);
	if(m_hData == nullptr)
		return;

	m_pLineArray = new BYTE*[Height];
	BYTE* pBits = (BYTE*)::GlobalLock(m_hData);
	for(int y = 0; y < Height; y++)
	{
		m_pLineArray[y] = pBits + (Height - y - 1)*LineLen;
	}
	::GlobalUnlock(m_hData);

	ASSERT_VALID(this);
}

CDib* CDib::GetSubDib(const CRect& rect) const
{
	CDib* pDib = new CDib();
	pDib->Create(rect.Width(), rect.Height(), m_nBpp, m_bAlphaChannel);

	int nLineBytes1 = m_Size.cx*m_nBpp/8;
	if(m_nBpp == 1 && m_Size.cx % 8)
		++nLineBytes1;
	const int LineLen1 = (nLineBytes1 + 3) & ~3;

	int nLineBytes2 = rect.Width()*pDib->m_nBpp/8;
	if(pDib->m_nBpp == 1 && rect.Width() % 8)
		++nLineBytes2;
	const int LineLen2 = (nLineBytes2 + 3) & ~3;

	BYTE* pBits1 = (BYTE*)::GlobalLock(m_hData);
	BYTE* pBits2 = (BYTE*)::GlobalLock(pDib->m_hData);

	pBits1 += rect.top*LineLen1;
	for(int row = rect.top; row < rect.bottom; row++)
	{
		memcpy(pBits2, pBits1 + rect.left*m_nBpp/8, LineLen2);

		pBits1 += LineLen1;
		pBits2 += LineLen2;
	}

	::GlobalUnlock(m_hData);
	::GlobalUnlock(pDib->m_hData);

	return pDib;
}

void CDib::CalcDestBPP(int SrcBPP, int* pDestBPP)
{
	ASSERT(*pDestBPP == 32 || *pDestBPP == 0);
	if(*pDestBPP == 0)
	{
		if(SrcBPP <= 8)
			*pDestBPP = 8;
		else
			*pDestBPP = 32;
	}
}

void CDib::Or(CDib* pDib)
{
	const LPSTR DibDatabuf1 = (LPSTR) ::GlobalLock(m_hData);
	const LPSTR DibDatabuf2 = (LPSTR) ::GlobalLock(pDib->m_hData);
	const int cx = min(m_Size.cx, pDib->m_Size.cx);
	const int cy = min(m_Size.cy, pDib->m_Size.cy);
	for(int i = 0; i < (cx/8)*cy; i++)
	{
		DibDatabuf1[i] |= DibDatabuf2[i];
	}

	::GlobalUnlock(DibDatabuf1);
	::GlobalUnlock(DibDatabuf2);
}

void CDib::And(CDib* pDib)
{
	const LPSTR DibDatabuf1 = (LPSTR)::GlobalLock(m_hData);
	const LPSTR DibDatabuf2 = (LPSTR)::GlobalLock(pDib->m_hData);
	const int cx = min(m_Size.cx, pDib->m_Size.cx);
	const int cy = min(m_Size.cy, pDib->m_Size.cy);
	for(int i = 0; i < (cx/8)*cy; i++)
	{
		DibDatabuf1[i] &= DibDatabuf2[i];
	}

	::GlobalUnlock(DibDatabuf1);
	::GlobalUnlock(DibDatabuf2);
}

CDib* CDib::FromFile(LPCTSTR lpszPathName)
{
	CString strFile = lpszPathName;
	strFile.MakeUpper();
	if(strFile.Right(4) != _T(".GIB") && strFile.Right(4) != _T(".DIB") && strFile.Right(4) != _T(".BMB"))
	{
		_bstr_t btrPathName(lpszPathName);
		//Image* image = Image::FromFile(btrPathName);
		//if(image != nullptr)
		//{
		//	CDib* dib = new CDib();
		//	dib->m_image = image;
		//	dib->m_Size.cx = image->GetWidth();
		//	dib->m_Size.cy = image->GetHeight();
		//	dib->m_DPI.cx = image->GetHorizontalResolution();
		//	dib->m_DPI.cy = image->GetVerticalResolution();
		//	return dib;
		//}
		//else 
		{
			const CStringA filename(strFile);
			const FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename);
			if(fif != FREE_IMAGE_FORMAT::FIF_UNKNOWN)
			{
				FIBITMAP* dib = FreeImage_Load(fif, filename);
				CDib* pDib = CDib::Load(fif, dib);
				FreeImage_Unload(dib);
				return pDib;
			}
			else
				return nullptr;
		}
	}
	else
	{
		DWORD FileMark;
		CFileException fe;
		CFile file;

		if(file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe) == FALSE)
		{
			CString string;
			string.Format(_T("Unable to open %s!", lpszPathName));
			AfxMessageBox(string);
			return nullptr;
		}

		file.Read(&FileMark, 4);
		file.Close();

		CDib* dib = nullptr;

		if(FileMark == 0x31424d42)
			dib = new CBmb;
		else if(FileMark == 0X31424947)
			dib = new CGib;
		else if((FileMark & 0X0000FFFF) == 0X00004d42)
			dib = new CBmp;
		else if((FileMark & 0X0000ffff) == 0X00004947)
			dib = new CGif;
		else if((FileMark & 0X0000ffff) == 0X00004949)
			dib = new CTiff;
		else if((FileMark & 0X0000ffff) == 0x00004D4D)
			dib = new CTiff;
		else if((FileMark & 0X0000ffff) == 0x00004542)
			dib = new CBme;
		else
			return nullptr;

		if(!dib->Open(lpszPathName))
		{
			delete dib;
			dib = nullptr;
			return nullptr;
		}

		return dib;
	}
}

CDib* CDib::FromData(FREE_IMAGE_FORMAT fif, BYTE* data, int len)
{
	FIMEMORY* mem = FreeImage_OpenMemory(data, len);
	FIBITMAP* dib = FreeImage_LoadFromMemory(fif, mem, JPEG_FAST);
	FreeImage_CloseMemory(mem);

	CDib* pDib = CDib::Load(fif, dib);
	FreeImage_Unload(dib);
	return pDib;
}

CDib* CDib::Load(FREE_IMAGE_FORMAT fif, FIBITMAP* dib)
{
	if(dib == nullptr)
		return nullptr;

	const BITMAPINFOHEADER* pBITMAPINFOHEADER = FreeImage_GetInfoHeader(dib);
	CSizeF DPI;
	DPI.cx = std::lround(pBITMAPINFOHEADER->biXPelsPerMeter*0.0254f);
	DPI.cy = std::lround(pBITMAPINFOHEADER->biYPelsPerMeter*0.0254f);
	if(DPI.cx == 0)
		DPI.cx = 72;
	if(DPI.cy == 0)
		DPI.cy = 72;

	if((DWORD)pBITMAPINFOHEADER->biHeight > INT_MAX || (DWORD)pBITMAPINFOHEADER->biWidth > INT_MAX)
		return nullptr;

	CDib* pDib = new CDib();
	pDib->m_DPI = DPI;

	pDib->m_Size = CSize((int)pBITMAPINFOHEADER->biWidth, (int)pBITMAPINFOHEADER->biHeight);
	pDib->m_nBpp = pBITMAPINFOHEADER->biBitCount;
	const int nColors = FreeImage_GetColorsUsed(dib);
	const BITMAPINFO* pInfo = FreeImage_GetInfo(dib);
	if(pInfo != nullptr)
	{
		const unsigned int bytes = pInfo->bmiHeader.biSize + nColors*sizeof(RGBQUAD);
		const HGLOBAL hInfo = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
		if(hInfo != nullptr)
		{
			const LPVOID buf = ::GlobalLock(hInfo);
			::memcpy(buf, pInfo, bytes);
			::GlobalUnlock(hInfo);
			pDib->m_hInfo = hInfo;
		}
		else
		{
			delete	pDib;
			pDib = nullptr;
			return nullptr;
		}
	}

	const unsigned int pitch = FreeImage_GetPitch(dib);
	const unsigned int bytes = pitch*pBITMAPINFOHEADER->biHeight;
	const HGLOBAL hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
	if(hData == nullptr)
	{
		delete	pDib;
		pDib = nullptr;
		return nullptr;
	}

	const LPVOID buf = ::GlobalLock(hData);
	const unsigned int bpp = FreeImage_GetBPP(dib);
	if(bpp == 8)
	{
		for(unsigned int y = 0; y < pBITMAPINFOHEADER->biHeight; y++)
		{
			const BYTE* line = FreeImage_GetScanLine(dib, y);
			memcpy((BYTE*)buf + y*pitch, line, pBITMAPINFOHEADER->biWidth*bpp/8);
		}
	}
	else
	{
		const BYTE* pData = FreeImage_GetBits(dib);
		::memcpy((BYTE*)buf, pData, bytes);
	}
	pDib->m_hData = hData;
	::GlobalUnlock(hData);
	return pDib;
}

bool CDib::SaveAs(const char* pathfile) const
{
	BITMAPINFOHEADER* DibInfo = (BITMAPINFOHEADER*)::GlobalLock((HGLOBAL)m_hInfo);
	FIBITMAP* dib = FreeImage_AllocateEx(DibInfo->biWidth, DibInfo->biHeight, DibInfo->biBitCount, ((BITMAPINFO*)DibInfo)->bmiColors);
	const int nColors = FreeImage_GetColorsUsed(dib);
	FreeImage_GetInfoHeader(dib)->biClrUsed = nColors;
	FreeImage_GetInfoHeader(dib)->biClrImportant = nColors;

	const BYTE* pSource = (BYTE*)::GlobalLock(m_hData);
	BYTE* pTarget = FreeImage_GetBits(dib);
	::memcpy(pTarget, pSource, FreeImage_GetDIBSize(dib));
	::GlobalUnlock(m_hData);
	const FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(pathfile);
	if(fif != FREE_IMAGE_FORMAT::FIF_UNKNOWN)
	{
		FreeImage_Save(fif, dib, pathfile);
		FreeImage_Unload(dib);
		return true;
	}
	else
	{
		FreeImage_Unload(dib);
		return false;
	}
}
