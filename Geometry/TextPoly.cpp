#include "stdafx.h"
#include "Geom.h"
#include "Poly.h"
#include "Text.h"
#include "TextPoly.h"
#include "Global.h"

#include "TextPolyDlg.h"

#include "../DataView/ViewInfo.h"
#include "../Style/color.h"
#include "../Style/Type.h"
#include "../Style/TypeDlg.h"
#include "../Style/Psboard.h"

#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include "../Utility/Rect.hpp"
#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"
#include <string>
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

IMPLEMENT_SERIAL(CTextPoly, CPoly, 0)

CTextPoly::CTextPoly()
	: CPoly()
{
	m_dwStyle=ALIGN_BASELINE | ORIENT_VERTICAL | SPACE_ANCHOR;

	m_bType=10;
}

CTextPoly::CTextPoly(const CRect& rect)
	: CPoly(rect, nullptr, nullptr)
{
	m_dwStyle=ALIGN_BASELINE | ORIENT_VERTICAL | SPACE_ANCHOR;

	m_bType=10;
}
void CTextPoly::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CPoly::Sha1(sha1);

	sha1.process_bytes(&m_dwStyle, sizeof(DWORD));
}
void CTextPoly::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CPoly::Sha1(sha1, offset);

	sha1.process_bytes(&m_dwStyle, sizeof(DWORD));
}
void CTextPoly::Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const
{
	if(m_pType!=nullptr)
	{
		m_pType->Sha1(sha1);
	}
	else if(pType!=nullptr)
	{
		pType->Sha1(sha1);
	}
}
BOOL CTextPoly::operator==(const CGeom& geom) const
{
	if(CPoly::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CTextPoly)) == FALSE)
		return FALSE;
	else if(m_dwStyle != ((CTextPoly&)geom).m_dwStyle)
		return FALSE;
	else
		return TRUE;
}

void CTextPoly::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPoly::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar << m_dwStyle;
	}
	else
	{
		ar >> m_dwStyle;
	}
}

void CTextPoly::ReleaseCE(CArchive& ar) const
{
	CPoly::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar << m_dwStyle;
	}
}

void CTextPoly::ReleaseDCOM(CArchive& ar)
{
	CPoly::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_dwStyle;
	}
	else
	{
		ar >> m_dwStyle;
	}
}
CGeom* CTextPoly::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(CGeom::Clip(bound, clips, in, tolerance, bStroke) == nullptr)
		return nullptr;

	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords = labelarray.GetCount();

	CTextPoly* textpoly = new CTextPoly();
	textpoly->m_pPoints = new CPoint[nWords];
	textpoly->m_nAllocs = nWords;
	textpoly->m_nAnchors = 0;
	const unsigned int nCount = min(nWords, m_nAnchors);

	for(unsigned int nAnchor = 0; nAnchor < nCount; nAnchor++)
	{
		CPoint point = m_pPoints[nAnchor];
		auto result = Clipper2Lib::PointInPolygon<int64_t>(Clipper2Lib::Point<int64_t>(point.x, point.y), clips.front());
		if((in && result == PointInPolygonResult::IsInside)||(!in && result == PointInPolygonResult::IsOutside))
		{
			textpoly->m_pPoints[textpoly->m_nAnchors] = point;
			const CText::CWord* lable = (CText::CWord*)labelarray.GetAt(nAnchor);
			textpoly->m_strName += lable->str;
			textpoly->m_nAnchors++;
		}
	}
	for(int index = 0; index < labelarray.GetCount(); index++)
	{
		const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
		delete label;
	}
	if(textpoly->m_nAnchors == 0)
	{
		delete textpoly;
		return nullptr;
	}
	else
	{
		if(this->m_pType != nullptr)
		{
			textpoly->m_pType = this->m_pType->Clone();
		}
		textpoly->RecalRect();
		return textpoly;
	}
}

bool CTextPoly::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CPoly::PickOn(point, gap) == true)
		return true;

	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords=labelarray.GetCount();
	const unsigned int nCount=min(nWords, m_nAnchors);
	for(unsigned int nAnchor=0; nAnchor < nCount; nAnchor++)
	{
		CRect rect(m_pPoints[nAnchor], m_pPoints[nAnchor]);
		rect.InflateRect(gap, gap);
		if(rect.PtInRect(point) == TRUE)
			return true;
	}

	return false;
}

void CTextPoly::CopyTo(CGeom* pGeom, bool ignore) const
{
	CPoly::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == TRUE)
	{
		CTextPoly* pTextPoly=(CTextPoly*)pGeom;
		pTextPoly->m_dwStyle=m_dwStyle;
	}
}

void CTextPoly::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == TRUE)
	{
		CTextPoly* pTextPoly=(CTextPoly*)pGeom;
		Swapdata(pTextPoly->m_dwStyle, m_dwStyle);
	}

	CPoly::Swap(pGeom);
}

void CTextPoly::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(m_strName.IsEmpty() == TRUE)
		return;

	CType* pType=m_pType != nullptr ? m_pType : aiKey.pubType;
	if(pType == nullptr)
		return;

	aiKey.ExportType(file, pType);
	const long nFontsize=pType->m_fSize*10;
	Gdiplus::PointF origin(0, 0);
	switch(m_dwStyle & 0X000FL)
	{
		case ALIGN_BASELINE:
			break;
		case ALIGN_CENTER:
			origin.X-=nFontsize/2;
			origin.Y-=nFontsize/2;
			break;
	}

	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords=labelarray.GetCount();

	switch(m_dwStyle & 0X0F00L)
	{
		case SPACE_ANCHOR:
		{
			const unsigned int nCount=min(nWords, m_nAnchors);
			for(unsigned int index=0; index < nCount; index++)
			{
				Gdiplus::PointF local=viewinfo.DocToClient <Gdiplus::PointF>(m_pPoints[index]);
				local.X+=origin.X;
				local.Y+=origin.Y;

				const CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const CString str=ConvertToOctalString(lable->str);

				_ftprintf(file, _T("0 To\n"));
				_ftprintf(file, _T("1 0 0 1 %g %g 0 Tp\n"), local.X, local.Y);
				_ftprintf(file, _T("0 Tv\n"));
				_ftprintf(file, _T("TP\n"));
				_ftprintf(file, _T("1 0 0 1 %g %g Tm\n"), local.X, local.Y);

				_ftprintf(file, _T("0 Tr\n"));
				long nFontsize=pType == nullptr ? 120 : pType->m_fSize*10;
				CColor* color=pType == nullptr ? nullptr : pType->m_pColor;
				if(color != nullptr)
				{
					color->ExportAIBrush(file, aiKey);
				}
				else
				{
					_ftprintf(file, _T("0 0 0 1 k\n"));
					aiKey.k=100;
				}

				_ftprintf(file, _T("(%s) Tx 1 0 Tk\n"), str);
				_ftprintf(file, _T("(\\r) TX\n"));
				_ftprintf(file, _T("TO\n"));
			}
		}
		break;
		case SPACE_AVERAGE:
		{
			const unsigned long length=this->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);

			long segment;
			if(nWords <= 1)
				segment=0;
			else
				segment=length/(nWords - 1);

			unsigned int sa=1;
			double st=0;
			unsigned int H=1;
			double T=0;

			for(unsigned int index=0; index < nWords; index++)
			{
				if(index != 0)
				{
					GetAandT(sa, st, segment, H, T);
					sa=H;
					st=T;
				}

				Gdiplus::PointF local=viewinfo.DocToClient <Gdiplus::PointF>(this->GetPoint(H, T));
				local.X+=origin.X;
				local.Y+=origin.Y;

				const CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const CString str=ConvertToOctalString(lable->str);

				_ftprintf(file, _T("0 To\n"));
				_ftprintf(file, _T("1 0 0 1 %g %g 0 Tp\n"), local.X, local.Y);
				_ftprintf(file, _T("0 Tv\n"));
				_ftprintf(file, _T("TP\n"));
				_ftprintf(file, _T("1 0 0 1 %g %g Tm\n"), local.X, local.Y);

				_ftprintf(file, _T("0 Tr\n"));
				long nFontsize=pType == nullptr ? 120 : pType->m_fSize*10;
				CColor* color=pType == nullptr ? nullptr : pType->m_pColor;
				if(color != nullptr)
				{
					color->ExportAIBrush(file, aiKey);
				}
				else
				{
					_ftprintf(file, _T("0 0 0 1 k\n"));
					aiKey.k=100;
				}

				_ftprintf(file, _T("(%s) Tx 1 0 Tk\n"), str);
				_ftprintf(file, _T("(\\r) TX\n"));
				_ftprintf(file, _T("TO\n"));
			}
		}
		break;
	}
	for(int index=0; index < labelarray.GetCount(); index++)
	{
		const CText::CWord* label=(CText::CWord*)labelarray.GetAt(index);
		delete label;
	}
}

void CTextPoly::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(m_strName.IsEmpty() == TRUE)
		return;

	CType* pType=m_pType != nullptr ? m_pType : context.pType;
	if(pType == nullptr)
		return;

	pType->ExportPdf(pdf, page);
	const long nFontsize=pType->m_fSize;
	Gdiplus::PointF origin(0, 0);
	switch(m_dwStyle & 0X000FL)
	{
		case ALIGN_BASELINE:
			break;
		case ALIGN_CENTER:
			origin.X-=nFontsize/2;
			origin.Y-=nFontsize/2;
			break;
	}

	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords=labelarray.GetCount();

	switch(m_dwStyle & 0X0F00L)
	{
		case SPACE_ANCHOR:
		{
			const unsigned int nCount=min(nWords, m_nAnchors);
			for(unsigned int index=0; index < nCount; index++)
			{
				Gdiplus::PointF local=viewinfo.DocToClient <Gdiplus::PointF>(m_pPoints[index]);
				local.X+=origin.X;
				local.Y+=origin.Y;

				CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const char* pString=UnicodeToMultiByte(lable->str.GetBuffer());

				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, local.X, local.Y, pString);
				HPDF_Page_EndText(page);

				lable->str.ReleaseBuffer();
				delete pString;
				pString=nullptr;
			}
		}
		break;
		case SPACE_AVERAGE:
		{
			const unsigned long length=this->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);

			long segment;
			if(nWords <= 1)
				segment=0;
			else
				segment=length/(nWords - 1);

			unsigned int sa=1;
			double st=0;
			unsigned int H=1;
			double T=0;

			for(unsigned int index=0; index < nWords; index++)
			{
				if(index != 0)
				{
					GetAandT(sa, st, segment, H, T);
					sa=H;
					st=T;
				}

				Gdiplus::PointF local=viewinfo.DocToClient <Gdiplus::PointF>(this->GetPoint(H, T));
				local.X+=origin.X;
				local.Y+=origin.Y;

				CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const char* pString=UnicodeToMultiByte(lable->str.GetBuffer());

				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, local.X, local.Y, pString);
				HPDF_Page_EndText(page);

				lable->str.ReleaseBuffer();
				delete pString;
				pString=nullptr;
			}
		}
		break;
	}
	for(int index=0; index < labelarray.GetCount(); index++)
	{
		const CText::CWord* label=(CText::CWord*)labelarray.GetAt(index);
		delete label;
	}
}

inline float CTextPoly::GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const
{
	const float fPts=m_pType != nullptr ? m_pType->m_fSize/2 : (pType != nullptr ? pType->m_fSize/2 : 0.f);
	return fPts*ratioType;
}

void CTextPoly::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const
{
	if(pWnd == nullptr)
		return;

	if(m_strName.IsEmpty() == TRUE)
		return;

	//CType* pType = m_pType;
	//if(pType == nullptr)
	//{
	//	pType = (CType*)pWnd->SendMessage(WM_GETGEOMTYPE,(UINT)this,3);
	//}

	//CDC* pDC = pWnd->GetDC();
	//Gdiplus::Font* font = nullptr;
	//if(pType != nullptr)
	//{		
	//	font = pType->GetFont(1.0f,72);
	//}
	//
	//UINT oldTextAlign = pDC->SetTextAlign(TA_LEFT | TA_BASELINE);	

	//TEXTMETRIC tm;
	//pDC->GetTextMetrics(&tm);

	//int Descent = tm.tmDescent;
	//int Height  = tm.tmHeight;
	//int Width   = tm.tmMaxCharWidth;

	//pDC->SetTextAlign(oldTextAlign);
	//if(font != nullptr)
	//{
	//	::delete font;	
	//}

	//pWnd->ReleaseDC(pDC);

	/*CSize ?(0,0);
	switch(m_dwStyle & 0X000FL)
	{
	case ALIGN_BASELINE:
		?.cy = -Descent;
		break;
	case ALIGN_CENTER:
		?.cx = -Height/2;
		?.cy = -Height/2;
		break;
	}*/
	const unsigned short nChars=m_strName.GetLength();
	switch(m_dwStyle & 0X0F00L)
	{
		case SPACE_ANCHOR:
		{
			const unsigned int nCount=min(nChars*sizeof(TCHAR)/2, m_nAnchors);
			for(unsigned int i=0; i < nCount; i++)
			{
				Gdiplus::Point cliPoint=viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[i]);
				Gdiplus::Rect cliRect(cliPoint, Gdiplus::Size(0, 0));
				cliRect.Inflate(inflate, inflate);

				pWnd->InvalidateRect(CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom()), FALSE);
			}
		}
		break;
		case SPACE_AVERAGE:
		{
			const unsigned long length=this->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc*2);
			long segment;
			if(nChars*sizeof(TCHAR)/2 <= 1)
				segment=0;
			else
				segment=length/(nChars*sizeof(TCHAR)/2 - 1);

			unsigned int sa=1;
			double st=0;
			unsigned int H=1;
			double T=0;

			for(unsigned int i=0; i < nChars*sizeof(TCHAR)/2; i++)
			{
				if(i != 0)
				{
					GetAandT(sa, st, segment, H, T);
					sa=H;
					st=T;
				}

				CPoint docPoint=GetPoint(H, T);
				Gdiplus::Point cliPoint=viewinfo.DocToClient<Gdiplus::Point>(docPoint);
				Gdiplus::Rect cliRect(cliPoint, Gdiplus::Size(0, 0));
				cliRect.Inflate(inflate, inflate);

				pWnd->InvalidateRect(CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom()), FALSE);
			}
		}
		break;
	}
}

void CTextPoly::InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const
{
	if(pWnd == nullptr)
		return;

	if(nAnchor < 1 || nAnchor > m_nAnchors)
		return;

	if(m_strName.IsEmpty() == TRUE)
		return;

	CString string=m_strName;
	const unsigned short nChars=string.GetLength();
	LPTSTR lpAnno=string.GetBuffer();

	/*
		CDC* pDC = pWnd->GetDC();

		CType* pType = m_pType;
		if(pType == nullptr)
		{
			pType = (CType*)pWnd->SendMessage(WM_GETGEOMTYPE,(UINT)this,3);
		}

		Gdiplus::Font* font = nullptr;
		if(pType != nullptr)
		{
			font = pType->GetFont(1.0f,72);
		}
		UINT oldTextAlign = pDC->SetTextAlign(TA_LEFT | TA_BASELINE);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);

		int Ascent  = tm.tmAscent;
		int Descent = tm.tmDescent;
		int Height  = tm.tmHeight;
		int Width   = tm.tmMaxCharWidth;

		pDC->SetTextAlign(oldTextAlign);
		if(font != nullptr)
		{
			::delete font;
			font = nullptr;
		}

		pWnd->ReleaseDC(pDC);

		CSize ?(0,0);
		switch(m_dwStyle & 0X000FL)
		{
		case ALIGN_BASELINE:
			?.cy = -Descent;
			break;
		case ALIGN_CENTER:
			?.cx = -Height/2;
			?.cy = -Height/2;
			break;
		}
	*/
	switch(m_dwStyle & 0X0F00L)
	{
		case SPACE_ANCHOR:
		{
			const unsigned int nCount=min(nChars*sizeof(TCHAR)/2, m_nAnchors);
			if(nAnchor <= nCount)
			{
				const Gdiplus::Point cliPoint=viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor - 1]);
				Gdiplus::Rect cliRect(cliPoint, Gdiplus::Size(0, 0));
				cliRect.Inflate(inflate, inflate);

				pWnd->InvalidateRect(CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom()), FALSE);
			}
		}
		break;
		case SPACE_AVERAGE:
		{
			Invalidate(pWnd, viewinfo, inflate);
		}
		break;
	}
	string.ReleaseBuffer();
}

void CTextPoly::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	if(m_strName.IsEmpty() == TRUE)
		return;
	if(m_pType == nullptr)
		return;

	Gdiplus::Font* font=m_pType->GetFont(ratio, viewinfo.m_sizeDPI);
	const Gdiplus::Color color=m_pType->GetColor();
	Gdiplus::Brush* brush=::new Gdiplus::SolidBrush(color);
	if(font == nullptr || brush == nullptr)
		return;

	Gdiplus::StringFormat stringFormat;
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetTrimming(Gdiplus::StringTrimmingNone);

	//	UINT oldTextAlign = pDC->SetTextAlign(TA_LEFT | TA_BASELINE);

	/*	switch(m_dwStyle & 0X000FL)
		{
		case ALIGN_BASELINE:
			break;
		case ALIGN_CENTER:
			{
				TEXTMETRIC tm;
				pDC->GetTextMetrics(&tm);

				int Descent = tm.tmDescent;
				int Height  = tm.tmHeight;
				Origin.Offset(-Height/2,-(Height/2-Descent));
			}
			break;
		}
	*/
	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords=labelarray.GetCount();

	Gdiplus::Point* points=viewinfo.DocToClient<Gdiplus::Point>(m_pPoints, m_nAnchors);
	switch(m_dwStyle & 0X0F00L)
	{
		case SPACE_ANCHOR:
		{
			const unsigned int nCount=min(nWords, m_nAnchors);
			for(unsigned int index=0; index < nCount; index++)
			{
				const Gdiplus::Point local=points[index];

				const CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const Gdiplus::GraphicsState state=g.Save();
				g.TranslateTransform(local.X, local.Y);

				_bstr_t bstr(lable->str);
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, 0), &stringFormat, brush);

				g.TranslateTransform(-local.X, -local.Y);
				g.Restore(state);
			}
		}
		break;
		case SPACE_AVERAGE:
		{
			const long length=CPoly::Length(points, m_nAnchors);
			long segment;
			if(labelarray.GetSize() <= 1)
				segment=0;
			else
				segment=length/(nWords - 1);

			unsigned int sa=1;
			double st=0;
			unsigned int H=1;
			double T=0;

			for(unsigned int index=0; index < nWords; index++)
			{
				if(index != 0)
				{
					CPoly::GetAandT(points, m_nAnchors, sa, st, segment, H, T);
					sa=H;
					st=T;
				}
				const Gdiplus::Point local=CPoly::GetPoint(points, m_nAnchors, H, T);
				const CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const Gdiplus::GraphicsState state=g.Save();
				g.TranslateTransform(local.X, local.Y);

				_bstr_t bstr(lable->str);
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, 0), &stringFormat, brush);

				g.TranslateTransform(-local.X, -local.Y);
				g.Restore(state);
			}
		}
		break;
	}
	for(int index=0; index < labelarray.GetCount(); index++)
	{
		const CText::CWord* label=(CText::CWord*)labelarray.GetAt(index);
		delete label;
	}
	::delete[] points;
	points=nullptr;
	::delete font;
	font=nullptr;
	::delete brush;
	brush=nullptr;
}

void CTextPoly::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(m_strName.IsEmpty() == TRUE)
		return;

	Gdiplus::Font* font=context.font;
	Gdiplus::Brush* brush=context.textbrush;

	Gdiplus::Font* font1=nullptr;
	Gdiplus::Brush* brush1=nullptr;
	if(m_pType != nullptr)
	{
		font1=m_pType->GetFont(1.0f, viewinfo.m_sizeDPI);
		font=font1;
		const Gdiplus::Color color=m_pType->GetColor();;
		brush1=::new Gdiplus::SolidBrush(color);
		brush=brush1;
	}

	if(font == nullptr || brush == nullptr)
		return;

	Gdiplus::StringFormat stringFormat;
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetTrimming(Gdiplus::StringTrimmingNone);

	//	UINT oldTextAlign = pDC->SetTextAlign(TA_LEFT | TA_BASELINE);

	/*	switch(m_dwStyle & 0X000FL)
		{
		case ALIGN_BASELINE:
			break;
		case ALIGN_CENTER:
			{
				TEXTMETRIC tm;
				pDC->GetTextMetrics(&tm);

				int Descent = tm.tmDescent;
				int Height  = tm.tmHeight;
				Origin.Offset(-Height/2,-(Height/2-Descent));
			}
			break;
		}
	*/
	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords=labelarray.GetCount();

	Gdiplus::Point* points=viewinfo.DocToClient<Gdiplus::Point>(m_pPoints, m_nAnchors);
	switch(m_dwStyle & 0X0F00L)
	{
		case SPACE_ANCHOR:
		{
			const unsigned int nCount=min(nWords, m_nAnchors);
			for(unsigned int index=0; index < nCount; index++)
			{
				const Gdiplus::Point local=points[index];

				const CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const Gdiplus::GraphicsState state=g.Save();
				g.TranslateTransform(local.X, local.Y);

				_bstr_t bstr(lable->str);
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, 0), &stringFormat, brush);

				g.TranslateTransform(-local.X, -local.Y);
				g.Restore(state);
			}
		}
		break;
		case SPACE_AVERAGE:
		{
			const long length=CPoly::Length(points, m_nAnchors);
			long segment;
			if(labelarray.GetSize() <= 1)
				segment=0;
			else
				segment=length/(nWords - 1);

			unsigned int sa=1;
			double st=0;
			unsigned int H=1;
			double T=0;

			for(unsigned int index=0; index < nWords; index++)
			{
				if(index != 0)
				{
					CPoly::GetAandT(points, m_nAnchors, sa, st, segment, H, T);
					sa=H;
					st=T;
				}
				const Gdiplus::Point local=CPoly::GetPoint(points, m_nAnchors, H, T);
				const CText::CWord* lable=(CText::CWord*)labelarray.GetAt(index);
				const Gdiplus::GraphicsState state=g.Save();
				g.TranslateTransform(local.X, local.Y);

				_bstr_t bstr(lable->str);
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, 0), &stringFormat, brush);

				g.TranslateTransform(-local.X, -local.Y);
				g.Restore(state);
			}
		}
		break;
	}
	for(int index=0; index < labelarray.GetCount(); index++)
	{
		const CText::CWord* label=(CText::CWord*)labelarray.GetAt(index);
		delete label;
	}
	::delete[] points;
	points=nullptr;
	::delete font1;
	font1=nullptr;
	::delete brush1;
	brush1=nullptr;
}
void CTextPoly::SetType(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HINSTANCE hInst=::LoadLibrary(_T("Style.dll"));
	const HINSTANCE hInstOld=AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CType* pNew=m_pType == nullptr ? new CType() : m_pType->Clone();
	CTypeDlg dlg(nullptr, *pNew);
	if(dlg.DoModal() == IDOK)
	{
		Invalidate(pWnd, viewinfo, 3);

		delete m_pType;
		m_pType=pNew;

		Invalidate(pWnd, viewinfo, 3);
	}
	else
	{
		delete pNew;
		pNew=nullptr;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CTextPoly::Attribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	CTextPolyDlg dlg(pWnd, m_dwStyle);

	if(dlg.DoModal() == IDOK)
	{
		Invalidate(pWnd, viewinfo, 0);
		m_dwStyle=dlg.m_dwStyle;
		Invalidate(pWnd, viewinfo, 0);
	}
}

void CTextPoly::GatherFonts(std::list<CStringA>& fontlist) const
{
	if(m_pType != nullptr)
	{
		const CStringA strFont=m_pType->m_fontdesc.GetRealName();
		if(strFont.IsEmpty() == FALSE && std::find(fontlist.begin(), fontlist.end(), strFont) == fontlist.end())
		{
			fontlist.push_back(strFont);
		}
	}
}

void CTextPoly::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
}

void CTextPoly::ExportMid(FILE* fileMid) const
{
}

DWORD CTextPoly::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size=CPoly::PackPC(pFile, bytes);

	if(pFile != nullptr)
	{
		pFile->Write(&m_dwStyle, sizeof(DWORD));
		size+=sizeof(DWORD);

		return size;
	}
	else
	{
		m_dwStyle=*(DWORD*)bytes;
		bytes+=sizeof(DWORD);
		return 0;
	}
}

DWORD CTextPoly::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size=CPoly::ReleaseCE(file, type);

	file.Write(&m_dwStyle, sizeof(DWORD));
	size+=sizeof(DWORD);

	return size;
}

void CTextPoly::ReleaseWeb(CFile& file, CSize offset) const
{
	CPoly::ReleaseWeb(file, offset);

	const DWORD dwStyle=m_dwStyle;
	file.Write(&dwStyle, sizeof(DWORD));

	const BYTE cSwitch=m_pType == nullptr ? 0 : 1;
	file.Write(&cSwitch, sizeof(BYTE));
	if(m_pType != nullptr)
	{
		m_pType->ReleaseWeb(file);
	}
}

void CTextPoly::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CPoly::ReleaseWeb(stream, offset);

	stream << m_dwStyle;
	const BYTE cSwitch=m_pType == nullptr ? 0 : 1;
	stream << cSwitch;
	if(m_pType != nullptr)
	{
		m_pType->ReleaseWeb(stream);
	}
}
void CTextPoly::ReleaseWeb(boost::json::object& json) const
{
	CPoly::ReleaseWeb(json);

	json["Style"]=m_dwStyle;

	if(m_pType != nullptr)
	{
		boost::json::object child;
		m_pType->ReleaseWeb(child);
		json["Font"]=child;
	}
}

void CTextPoly::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPoly::ReleaseWeb(writer, offset);

	writer.add_variant_uint32(m_dwStyle);

	if(m_pType != nullptr)
	{
		writer.add_bool(true);		
		m_pType->ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);
}
