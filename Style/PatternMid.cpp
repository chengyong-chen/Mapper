#include "stdafx.h"

#include  <io.h>
#include  <stdio.h>
#include "Mid.h"
#include "PatternMid.h"
#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPatternMid, CMid, 0)

CPatternMid::CPatternMid()
{
	m_nCount = 0;
	m_nHeight = 0;
	m_Bitmap = nullptr;
}

CPatternMid::CPatternMid(CString strFile)
{
	if(this->Load(strFile) == TRUE)
	{
		m_strFile = strFile;
	}
	else
	{
		m_nCount = 0;
		m_nHeight = 0;
		m_Bitmap = nullptr;
	}
	m_strFile = strFile;
}

CPatternMid::~CPatternMid()
{
	if(m_imagelist.m_hImageList != nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	delete m_Bitmap;
	m_Bitmap = nullptr;
}

void CPatternMid::CopyTo(CMid* pMid) const
{
	CMid::CopyTo(pMid);

	if(pMid->IsKindOf(RUNTIME_CLASS(CPatternMid)) == TRUE)
	{
		((CPatternMid*)pMid)->m_strFile = m_strFile;
		((CPatternMid*)pMid)->Load(m_strFile);
	}
}

Gdiplus::Bitmap* CPatternMid::GetBitmap(const int& index) const
{
	IMAGEINFO imageinfo;
	m_imagelist.GetImageInfo(index, &imageinfo);
	const CRect rect(imageinfo.rcImage);

	CDC* pDC = CDC::FromHandle(::GetDC(nullptr));
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	const HBITMAP hbitmap = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rect.Width(), rect.Height());
	const HGDIOBJ hOldBm = mdc.SelectObject(hbitmap);
	m_imagelist.DrawIndirect(&mdc, index, CPoint(0, 0), rect.Size(), CPoint(0, 0));
	mdc.SelectObject(hOldBm);
	mdc.DeleteDC();

	return  Gdiplus::Bitmap::FromHBITMAP(hbitmap, nullptr);
}

CSize CPatternMid::GetSize() const
{
	return m_nHeight == 0 ? CSize(160, 160) : CSize(m_nHeight*10, m_nHeight*10);
}

CSize CPatternMid::GetDPI()
{
	return CSize(72, 72);
}

void CPatternMid::Draw(CDC* pDC, Gdiplus::Point point, const int& index) const
{
	if(m_imagelist.m_hImageList != nullptr)
	{
		const CPoint lefttop(point.X - m_nHeight/2, point.Y - m_nHeight/2);
		m_imagelist.Draw(pDC, index, lefttop, ILD_TRANSPARENT);
	}
}

void CPatternMid::Draw(Gdiplus::Graphics& g, const int& index, float fScale) const
{
	const Gdiplus::GraphicsState state = g.Save();
	g.ScaleTransform(fScale, fScale);
	if(m_Bitmap != nullptr)
	{
		Gdiplus::ImageAttributes imaatt;
		imaatt.SetColorKey(Gdiplus::Color(192, 192, 192), Gdiplus::Color(192, 192, 192), Gdiplus::ColorAdjustTypeDefault);
		const Gdiplus::RectF destRect = Gdiplus::RectF(-m_nHeight/2, -m_nHeight/2, m_nHeight, m_nHeight);
		g.DrawImage(m_Bitmap, destRect, m_nHeight*index, 0, m_nHeight, m_nHeight, Gdiplus::UnitPixel, &imaatt);
	}
	else
	{
		const Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
		g.FillEllipse(&brush, -8, -8, 16, 16);
	}
	g.ScaleTransform(1/fScale, 1/fScale);
	g.Restore(state);
}

BOOL CPatternMid::Load(CString strPathName)
{
	if(_taccess(strPathName, 00) == -1)
		return FALSE;

	HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), strPathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(hBitmap == nullptr)
		return FALSE;
	const _bstr_t bstrPathName(strPathName);
	m_Bitmap = Gdiplus::Bitmap::FromFile(bstrPathName);

	BITMAP bm;
	if(::GetObject(hBitmap, sizeof(BITMAP), &bm) == 0)
	{
		DeleteObject(hBitmap);
		return FALSE;
	}

	if(bm.bmHeight > bm.bmWidth)
	{
		AfxMessageBox(_T("The width must be bigger than height for Patterns!"));
		DeleteObject(hBitmap);
		return FALSE;
	}

	if(m_imagelist.m_hImageList != nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	CBitmap bitmap;
	if(bitmap.Attach(hBitmap) == TRUE)
	{
		m_nHeight = bm.bmHeight;
		m_nCount = bm.bmWidth/bm.bmHeight;

		if(m_imagelist.Create(m_nHeight, m_nHeight, ILC_COLORDDB, m_nCount + 1, 0) == TRUE)
		{
			m_imagelist.Add(&bitmap, RGB(192, 192, 192));
			m_imagelist.SetBkColor(RGB(192, 192, 192));
			m_strFile = strPathName;

			bitmap.Detach();
			if(hBitmap != nullptr)
			{
				DeleteObject(hBitmap);
				hBitmap = nullptr;
			}
			return TRUE;
		}
		else
		{
			AfxMessageBox(_T("Failed to create the pattern, check the file's format please!"));
		}

		bitmap.Detach();
	}
	if(hBitmap != nullptr)
	{
		DeleteObject(hBitmap);
		hBitmap = nullptr;
	}
	return FALSE;
}

void CPatternMid::ReleaseCE(CArchive& ar) const
{
	CMid::ReleaseCE(ar);
}

void CPatternMid::ReleaseDCOM(CArchive& ar)
{
	CMid::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_nCount;
		ar << m_nHeight;

		m_imagelist.Write(&ar);
	}
	else
	{
		ar >> m_nCount;
		ar >> m_nHeight;

		if(m_imagelist.m_hImageList != nullptr)
		{
			m_imagelist.DeleteImageList();
			m_imagelist.Detach();
		}
		m_imagelist.Read(&ar);
	}
}

void CPatternMid::ReleaseWeb(CFile& file) const
{
	CMid::ReleaseWeb(file);

	unsigned short nWidth = m_Bitmap == nullptr ? 0 : m_Bitmap->GetWidth();
	ReverseOrder(nWidth);
	file.Write(&nWidth, sizeof(unsigned short));

	unsigned short nHeight = m_Bitmap == nullptr ? 0 : m_Bitmap->GetHeight();
	ReverseOrder(nHeight);
	file.Write(&nHeight, sizeof(unsigned short));

	if(m_Bitmap != nullptr)
	{
		Gdiplus::BitmapData bitmapData;
		const Gdiplus::Rect rect(0, 0, m_Bitmap->GetWidth() - 1, m_Bitmap->GetHeight() - 1);
		m_Bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
		unsigned int nBytes = bitmapData.Stride*m_Bitmap->GetHeight();
		ReverseOrder(nBytes);
		file.Write(&nBytes, sizeof(unsigned int));
		const BYTE* data = (BYTE*)bitmapData.Scan0;
		file.Write(data, bitmapData.Stride*m_Bitmap->GetHeight());
		m_Bitmap->UnlockBits(&bitmapData);
	}
}

void CPatternMid::ReleaseWeb(BinaryStream& stream) const
{
	CMid::ReleaseWeb(stream);
	const unsigned short nWidth = m_Bitmap == nullptr ? 0 : m_Bitmap->GetWidth();
	stream << nWidth;
	const unsigned short nHeight = m_Bitmap == nullptr ? 0 : m_Bitmap->GetHeight();
	stream << nHeight;
	if(m_Bitmap != nullptr)
	{
		Gdiplus::BitmapData bitmapData;
		const Gdiplus::Rect rect(0, 0, m_Bitmap->GetWidth() - 1, m_Bitmap->GetHeight() - 1);
		m_Bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
		const unsigned int nBytes = bitmapData.Stride*m_Bitmap->GetHeight();
		stream << nBytes;
		BYTE* data = (BYTE*)bitmapData.Scan0;
		stream << data;//, bitmapData.Stride*m_Bitmap->GetHeight());
		m_Bitmap->UnlockBits(&bitmapData);
	}
}
void CPatternMid::ReleaseWeb(boost::json::object& json) const
{
	CMid::ReleaseWeb(json);
	const unsigned short nWidth = m_Bitmap == nullptr ? 0 : m_Bitmap->GetWidth();
	json["Width"] = nWidth;
	const unsigned short nHeight = m_Bitmap == nullptr ? 0 : m_Bitmap->GetHeight();
	json["Height"] = nHeight;

	if(m_Bitmap != nullptr)
	{
		//BitmapData bitmapData;
		//Rect rect(0,0,m_Bitmap->GetWidth()-1,m_Bitmap->GetHeight()-1);
		//m_Bitmap->LockBits(&rect,ImageLockModeRead,PixelFormat32bppARGB,&bitmapData);
		//unsigned int nBytes = bitmapData.Stride*m_Bitmap->GetHeight();
		//ReverseOrder(nBytes);
		//file.Write(&nBytes,sizeof(unsigned int));
		//BYTE* data = (BYTE*)bitmapData.Scan0;
		//file.Write(data,bitmapData.Stride*m_Bitmap->GetHeight());
		//m_Bitmap->UnlockBits(&bitmapData);
	}
}

void CPatternMid::ReleaseWeb(pbf::writer& writer) const
{
	CMid::ReleaseWeb(writer);

	const unsigned short nWidth = m_Bitmap == nullptr ? 0 : m_Bitmap->GetWidth();
	writer.add_variant_uint16(nWidth);
	const unsigned short nHeight = m_Bitmap == nullptr ? 0 : m_Bitmap->GetHeight();
	writer.add_variant_uint16(nHeight);

	if(m_Bitmap != nullptr)
	{
		//BitmapData bitmapData;
		//Rect rect(0,0,m_Bitmap->GetWidth()-1,m_Bitmap->GetHeight()-1);
		//m_Bitmap->LockBits(&rect,ImageLockModeRead,PixelFormat32bppARGB,&bitmapData);
		//unsigned int nBytes = bitmapData.Stride*m_Bitmap->GetHeight();
		//ReverseOrder(nBytes);
		//file.Write(&nBytes,sizeof(unsigned int));
		//BYTE* data = (BYTE*)bitmapData.Scan0;
		//file.Write(data,bitmapData.Stride*m_Bitmap->GetHeight());
		//m_Bitmap->UnlockBits(&bitmapData);
	}
}