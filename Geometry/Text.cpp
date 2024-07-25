#include "stdafx.h"
#include <stdio.h>

#include "Text.h"
#include "Geom.h"
#include "Global.h"
#include "Poly.h"
#include "PRect.h"

#include "TextEditDlg.h"

#include "../Dataview/viewinfo.h"
#include "../Public/Enumeration.h"
#include "../Public/BinaryStream.h"
#include "../Style/color.h"
#include "../Style/Library.h"
#include "../Style/Type.h"
#include "../Style/TypeDlg.h"
#include "../Style/Psboard.h"

#include "../Public/Function.h"

#include "../Rectify/Tin.h"
#include "../Utility/Rect.hpp"

#include <boost/locale.hpp>
#include <codecvt>
#include <cassert>

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include "../../Thirdparty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"
#include <boost/json.hpp>
using namespace boost::locale;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CText, CPRect, 0)

CText::CText()
	: CPRect()
{
	m_bGroup = false;
	m_pType = nullptr;
	m_bType = 9;
	m_Align = TEXT_ALIGN::TAL_START;
	m_Anchor = TEXT_ANCHOR::TAC_START;
}

CText::CText(CPoint origin)
	: CPRect(CRect(origin, CSize(1, 1)), nullptr, nullptr)
{
	m_bGroup = false;
	m_Origin.x = origin.x;
	m_Origin.y = origin.y;
	m_Align = TEXT_ALIGN::TAL_START;
	m_Anchor = TEXT_ANCHOR::TAC_START;
	m_bType = 9;
}
void CText::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CPRect::Sha1(sha1);

	sha1.process_bytes(&m_Origin.x, sizeof(int));
	sha1.process_bytes(&m_Origin.y, sizeof(int));
	sha1.process_byte(m_Align);
	sha1.process_byte(m_Anchor);
}
void CText::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CPRect::Sha1(sha1, offset);

	LONG  x = m_Origin.x - offset.cx;
	LONG  y = m_Origin.y - offset.cy;
	sha1.process_bytes(&x, sizeof(int));
	sha1.process_bytes(&y, sizeof(int));
	sha1.process_byte(m_Align);
	sha1.process_byte(m_Anchor);
}
void CText::Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const
{
	if(m_pType != nullptr)
	{
		m_pType->Sha1(sha1);
	}
	else if(pType != nullptr)
	{
		pType->Sha1(sha1);
	}
}
BOOL CText::operator==(const CGeom& geom) const
{
	if(CPRect::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CText)) == FALSE)
		return FALSE;
	else if(m_Origin != ((CText&)geom).m_Origin)
		return FALSE;
	else if(m_Align != ((CText&)geom).m_Align)
		return FALSE;
	else if(m_Anchor != ((CText&)geom).m_Anchor)
		return FALSE;
	else
		return TRUE;
}

bool CText::IsValid() const
{
	if(m_strName.IsEmpty() == true)
		return false;
	else
		return true;
}
bool CText::IsOrphan() const
{
	CString str = m_strName;
	str.TrimLeft();
	return str.IsEmpty();
}
void CText::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPRect::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar << m_Origin.x;
		ar << m_Origin.y;
		ar << m_Align;
		ar << m_Anchor;
	}
	else
	{
		ar >> m_Origin.x;
		ar >> m_Origin.y;
		ar >> m_Align;
		ar >> m_Anchor;
	}
}

void CText::ReleaseCE(CArchive& ar) const
{
	CPRect::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		CPoint origin = m_Origin;
		origin.x /= 10000;
		origin.y /= 10000;
		ar << origin.x;
		ar << origin.y;
	}
}

void CText::Attribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	CTextEditDlg dlg(pWnd);

	if(dlg.DoModal() == IDOK)
	{
		Invalidate(pWnd, viewinfo, 0);
		m_strName = dlg.m_String;
		CText::CalCorner(m_pType, viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
		Invalidate(pWnd, viewinfo, 0);
	}
}

void CText::ReleaseDCOM(CArchive& ar)
{
	CPRect::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_Origin.x;
		ar << m_Origin.y;
	}
	else
	{
		ar >> m_Origin.x;
		ar >> m_Origin.y;
	}
}

bool CText::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	return false;
}

bool CText::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	return false;
}

void CText::CopyTo(CGeom* pGeom, bool ignore) const
{
	CPRect::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE)
	{
		CText* pText = (CText*)pGeom;
		pText->m_Origin = m_Origin;
	}
}

void CText::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE)
	{
		CText* pText = (CText*)pGeom;
		Swapclass<CPoint>(pText->m_Origin, m_Origin);
	}

	CPRect::Swap(pGeom);
}
void CText::Move(const int& dx, const int& dy)
{
	m_Origin.x += dx;
	m_Origin.y += dy;

	CPRect::Move(dx, dy);
}
void CText::Move(const CSize& Δ)
{
	m_Origin.x += Δ.cx;
	m_Origin.y += Δ.cy;

	CPRect::Move(Δ);
}

Gdiplus::RectF CText::GetVewBoundary(const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	const Gdiplus::PointF origin = viewinfo.DocToClient <Gdiplus::PointF>(m_Origin);
	CType* pType = m_pType != nullptr ? m_pType : (CType*)context.pType;
	if(pType == nullptr)
		return Gdiplus::RectF(origin.X, origin.Y, 0, 0);
	const CString strFontFamily = pType->m_fontdesc.GetFamilyName();
	float fontSize = pType->m_fSize * context.ratioType * viewinfo.m_sizeDPI.cx / 72.f;
	const _bstr_t btrFontFamily(strFontFamily);
	const Gdiplus::FontFamily fontFamily(btrFontFamily);
	::SysFreeString(btrFontFamily);
	Gdiplus::RectF rect(0, 0, 0, 0);
	if(fontFamily.IsAvailable() == true)
	{
		const Gdiplus::Font font(&fontFamily, fontSize, pType->m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
		const UINT16 emLineSpacing = fontFamily.GetLineSpacing(pType->m_fontdesc.GetStyle());
		const UINT16 emHeight = fontFamily.GetEmHeight(pType->m_fontdesc.GetStyle());
		const UINT16 emDescent = fontFamily.GetCellDescent(pType->m_fontdesc.GetStyle());
		fontSize = font.GetSize();
		Gdiplus::Bitmap bitmap(16, 16);
		const Gdiplus::Graphics g(&bitmap);
		const Gdiplus::StringFormat stringFormat(Gdiplus::StringFormat::GenericTypographic());
		const _bstr_t bstr(m_strName);

		g.MeasureString(bstr, -1, &font, Gdiplus::PointF(0, 0), &stringFormat, &rect);
		rect.Offset(0, -fontSize * (emHeight - emDescent) / emHeight);
		rect.Height -= fontSize * (emLineSpacing - emHeight) / emHeight;

		switch(m_Anchor)
		{
			case TEXT_ANCHOR::TAC_START:
				break;
			case TEXT_ANCHOR::TAC_MIDDLE:
				rect.Offset(-rect.Width / 2, 0);
				break;
			case TEXT_ANCHOR::TAC_END:
				rect.Offset(-rect.Width, 0);
				break;

			default:
				break;
		}
		if(m_pPoints != nullptr)
		{
			const double fTg = this->GetTg();
			const double fSin = this->GetSin();
			const double fCos = this->GetCos();
			const Gdiplus::Matrix matrix(fCos, -fSin, fSin - fTg * fCos, fTg * fSin + fCos, 0, 0);

			Gdiplus::PointF point0 = Gdiplus::PointF(rect.GetLeft(), rect.GetTop());
			Gdiplus::PointF point1 = Gdiplus::PointF(rect.GetLeft(), rect.GetBottom());
			Gdiplus::PointF point2 = Gdiplus::PointF(rect.GetRight(), rect.GetBottom());
			Gdiplus::PointF point3 = Gdiplus::PointF(rect.GetRight(), rect.GetTop());
			matrix.TransformPoints(&point0);
			matrix.TransformPoints(&point1);
			matrix.TransformPoints(&point2);
			matrix.TransformPoints(&point3);

			rect.X = min(min(point0.X, point1.X), min(point2.X, point3.X));
			rect.Y = min(min(point0.Y, point1.Y), min(point2.Y, point3.Y));
			rect.Width = max(max(point0.X, point1.X), max(point2.X, point3.X)) - rect.X;
			rect.Height = max(max(point0.Y, point1.Y), max(point2.Y, point3.Y)) - rect.Y;
		}
	}

	rect.Offset(origin.X, origin.Y);
	return rect;
}

void CText::CalCorner(CType* pType, float fPointToDoc, const float& ratio)
{
	pType = m_pType != nullptr ? m_pType : pType;
	if(pType == nullptr)
		return;

	Gdiplus::Font* pFont = pType->GetFont(1, 72);
	if(pFont != nullptr && pFont->GetLastStatus() == Gdiplus::Status::Ok)
	{
		Gdiplus::FontFamily fontFamily;
		pFont->GetFamily(&fontFamily);
		const UINT16 emLineSpacing = fontFamily.GetLineSpacing(pType->m_fontdesc.GetStyle());
		const UINT16 emHeight = fontFamily.GetEmHeight(pType->m_fontdesc.GetStyle());
		const UINT16 emAscent = fontFamily.GetCellAscent(pType->m_fontdesc.GetStyle());
		const UINT16 emDescent = fontFamily.GetCellDescent(pType->m_fontdesc.GetStyle());
		const float fontSize = pFont->GetSize();
		Gdiplus::Bitmap bitmap(16, 16);
		const Gdiplus::Graphics g(&bitmap);
		const _bstr_t bstr(m_strName);

		Gdiplus::RectF rect;
		g.MeasureString(bstr, -1, pFont, Gdiplus::PointF(0, 0), Gdiplus::StringFormat::GenericTypographic(), &rect);
		rect.Offset(0, -fontSize * emAscent / emHeight);

		::delete pFont;
		switch(m_Anchor)
		{
			case TEXT_ANCHOR::TAC_START:
				break;
			case TEXT_ANCHOR::TAC_MIDDLE:
				rect.Offset(-rect.Width / 2, 0);
				break;
			case TEXT_ANCHOR::TAC_END:
				rect.Offset(-rect.Width, 0);
				break;
		}
		m_Rect.left = rect.GetLeft() * fPointToDoc / ratio;
		m_Rect.right = rect.GetRight() * fPointToDoc / ratio;
		m_Rect.top = -rect.GetBottom() * fPointToDoc / ratio;
		m_Rect.bottom = -rect.GetTop() * fPointToDoc / ratio;

		if(m_pPoints != nullptr)
		{
			const double fTg = this->GetTg();
			const double fSin = this->GetSin();
			const double fCos = this->GetCos();
			delete[] m_pPoints;
			m_pPoints = nullptr;
			m_bRegular = true;

			CPRect::Transform(fCos, fTg * fCos - fSin, 0, fSin, fTg * fSin + fCos, 0);
		}

		CPRect::Move(CSize(m_Origin.x, m_Origin.y));
	}
}

void CText::GatherFonts(std::list<CStringA>& fontlist) const
{
	if(m_pType != nullptr)
	{
		const CStringA strFont = m_pType->m_fontdesc.GetRealName();
		if(strFont.IsEmpty()==FALSE && std::find(fontlist.begin(), fontlist.end(), strFont) == fontlist.end())
		{
			fontlist.push_back(strFont);
		}
	}
}

void CText::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(this->IsValid() == false)
		return;

	if(m_strName.IsEmpty() == TRUE)
		return;

	CType* pType = m_pType != nullptr ? m_pType : aiKey.pubType;
	if(pType == nullptr)
		return;

	aiKey.ExportType(file, pType);

	_ftprintf(file, _T("u\n"));

	Gdiplus::PointF origin = viewinfo.DocToClient <Gdiplus::PointF>(m_Origin);
	const float lineHeight = pType->GetlineSpacing(1, 72);
	CString strName = m_strName;
	strName.Replace(_T("\r\n"), _T("\r"));
	while(strName.IsEmpty() == FALSE)
	{
		CString strLine;
		const int t = strName.Find(_T('\r'));
		if(t == -1)
		{
			strLine = strName;
			strName.Empty();
		}
		else
		{
			strLine = strName.Left(t);
			strName = strName.Mid(t + 1);
		}

		_ftprintf(file, _T("0 To\n"));
		if(m_bRegular == FALSE && m_pPoints != nullptr)
		{
			const double fTg = this->GetTg();
			const double fSin = this->GetSin();
			const double fCos = this->GetCos();
			_ftprintf(file, _T("%g %g %g %g %g %g 0 Tp\n"), fCos, fSin - fTg * fCos, -fSin, fTg * fSin + fCos, origin.X, origin.Y);
			_ftprintf(file, _T("0 Tv\n"));
			_ftprintf(file, _T("TP\n"));
			_ftprintf(file, _T("%g %g %g %g %g %g Tm\n"), fCos, fSin - fTg * fCos, -fSin, fTg * fSin + fCos, origin.X, origin.Y);

			origin.X += lineHeight * (fSin - fTg * fCos);
			origin.Y -= lineHeight * (fTg * fSin + fCos);
		}
		else
		{
			_ftprintf(file, _T("1 0 0 1 %g %g 0 Tp\n"), origin.X, origin.Y);
			_ftprintf(file, _T("0 Tv\n"));
			_ftprintf(file, _T("TP\n"));
			_ftprintf(file, _T("1 0 0 1 %g %g Tm\n"), origin.X, origin.Y);

			origin.Y -= lineHeight;
		}

		_ftprintf(file, _T("0 Tr\n"));
		CColor* color = pType == nullptr ? nullptr : pType->m_pColor;
		if(color != nullptr)
		{
			color->ExportAIBrush(file, aiKey);
		}
		else if(aiKey.K != 20)
		{
			_ftprintf(file, _T("0 0 0 0.2 k\n"));
			aiKey.k = 20;
		}

		strLine = ConvertToOctalString(strLine);
		_ftprintf(file, _T("(%s) Tx 1 0 Tk\n"), strLine);
		_ftprintf(file, _T("(\\r) TX\n"));
		_ftprintf(file, _T("TO\n"));
	}

	_ftprintf(file, _T("U\n"));
}

void CText::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(this->IsValid() == false)
		return;

	if(m_strName.IsEmpty() == TRUE)
		return;

	CType* pType = m_pType != nullptr ? m_pType : context.pType;
	if(pType == nullptr)
		return;

	pType->ExportPdf(pdf, page);
	Gdiplus::PointF origin = viewinfo.DocToClient <Gdiplus::PointF>(m_Origin);
	CString strName = m_strName;
	strName.Replace(_T("\r\n"), _T("\r"));
	const float lineHeight = pType->GetlineSpacing(1, 72);
	while(strName.IsEmpty() == FALSE)
	{
		CString strLine;
		const int t = strName.Find(_T('\r'));
		if(t == -1)
		{
			strLine = strName;
			strName.Empty();
		}
		else
		{
			strLine = strName.Left(t);
			strName = strName.Mid(t + 1);
		}

		char* pString = UnicodeToMultiByte(strLine.GetBuffer());
		if(m_bRegular == false && m_pPoints != nullptr)
		{
			const double fTg = this->GetTg();
			const double fSin = this->GetSin();
			const double fCos = this->GetCos();
			const double m11 = fCos;
			const double m12 = fSin - fTg * fCos;
			const double m21 = -fSin;
			const double m22 = fTg * fSin + fCos;
			HPDF_Page_BeginText(page);
			HPDF_Page_SetTextMatrix(page, m11, m12, m21, m22, origin.X, origin.Y);
			HPDF_Page_ShowText(page, pString);

			HPDF_Page_EndText(page);

			origin.X += lineHeight * (fSin - fTg * fCos);
			origin.Y -= lineHeight * (fTg * fSin + fCos);
		}
		else
		{
			HPDF_Page_BeginText(page);
			HPDF_Page_TextOut(page, origin.X, origin.Y, pString);
			HPDF_Page_EndText(page);
			origin.Y -= lineHeight;
		}
		strLine.ReleaseBuffer();
		delete pString;
		pString = nullptr;
	}
}

void CText::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
}

void CText::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	if(m_pType == nullptr)
		return;

	Gdiplus::Font* font = m_pType->GetFont(ratio, viewinfo.m_sizeDPI);
	const Gdiplus::Color color = m_pType->GetColor();
	Gdiplus::Brush* brush = ::new Gdiplus::SolidBrush(color);
	if(font == nullptr || brush == nullptr)
		return;

	Gdiplus::FontFamily fontFamily;
	font->GetFamily(&fontFamily);
	const Gdiplus::REAL sizeFont = font->GetSize();
	const UINT16 emLineSpacing = fontFamily.GetLineSpacing(m_pType->m_fontdesc.GetStyle());
	const UINT16 emHeight = fontFamily.GetEmHeight(m_pType->m_fontdesc.GetStyle());
	UINT16 emAscent = fontFamily.GetCellAscent(m_pType->m_fontdesc.GetStyle());
	const UINT16 emDescent = fontFamily.GetCellDescent(m_pType->m_fontdesc.GetStyle());

	const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();
	const Gdiplus::PointF origin = viewinfo.DocToClient<Gdiplus::PointF>(m_Origin);
	g.TranslateTransform(origin.X, origin.Y);
	const double fSin = this->GetSin();
	const double fCos = this->GetCos();
	const double fTg = this->GetTg();
	const Gdiplus::GraphicsState state = g.Save();
	Gdiplus::Matrix matrix;
	matrix.SetElements(fCos, -fSin, -fTg * fCos + fSin, fTg * fSin + fCos, 0, 0);
	g.MultiplyTransform(&matrix);

	CString strName = m_strName;
	const float LineSpacing = sizeFont * emLineSpacing / emHeight;
	float Y = -sizeFont * (emLineSpacing - emDescent) / emHeight;
	do
	{
		const int t = strName.Find(0X000A000D);
		if(t == -1)
		{
			_bstr_t bstr(strName);
			g.DrawString(bstr, -1, font, Gdiplus::PointF(0, Y), pStringFormat, brush);
			strName.Empty();
		}
		else
		{
			CString sub = strName.Left(t);
			if(sub.IsEmpty() == FALSE)
			{
				_bstr_t bstr(sub);
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, Y), pStringFormat, brush);
			}

			strName = strName.Mid(t + 2);
		}

		Y += LineSpacing;
	} while(strName.IsEmpty() == FALSE);

	g.Restore(state);
	g.TranslateTransform(-origin.X, -origin.Y);

	::delete font;
	font = nullptr;
	::delete brush;
	brush = nullptr;
}

void CText::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	Gdiplus::Font* font = context.font;
	Gdiplus::Brush* brush = context.textbrush;

	Gdiplus::Font* font1 = nullptr;
	Gdiplus::Brush* brush1 = nullptr;
	if(m_pType != nullptr)
	{
		font1 = m_pType->GetFont(context.ratioType, viewinfo.m_sizeDPI);
		font = font1;
		const Gdiplus::Color color = m_pType->GetColor();
		brush1 = ::new Gdiplus::SolidBrush(color);
		brush = brush1;
	}
	else if(font == nullptr || brush == nullptr)
		return;

	Gdiplus::FontFamily fontFamily;
	font->GetFamily(&fontFamily);
	const Gdiplus::FontStyle style = (Gdiplus::FontStyle)font->GetStyle();
	const Gdiplus::REAL sizeFont = font->GetSize();
	const UINT16 emLineSpacing = fontFamily.GetLineSpacing(style);
	const UINT16 emHeight = fontFamily.GetEmHeight(style);//heighr is nor a=d
	const UINT16 emAscent = fontFamily.GetCellAscent(style);
	const UINT16 emDescent = fontFamily.GetCellDescent(style);

	const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();
	const Gdiplus::PointF origin = viewinfo.DocToClient<Gdiplus::PointF>(m_Origin);

	g.TranslateTransform(origin.X, origin.Y);
	const double fSin = this->GetSin();
	const double fCos = this->GetCos();
	const double fTg = this->GetTg();
	const Gdiplus::GraphicsState state = g.Save();
	Gdiplus::Matrix matrix;
	matrix.SetElements(fCos, -fSin, -fTg * fCos + fSin, fTg * fSin + fCos, 0, 0);
	g.MultiplyTransform(&matrix);

	CString strName = m_strName;
	const float LineSpacing = sizeFont * emLineSpacing / emHeight;
	float Y = -sizeFont * emAscent / emHeight;
	do
	{
		const int t = strName.Find(0X000A000D);
		if(t == -1)
		{
			_bstr_t bstr(strName);
			g.DrawString(bstr, -1, font, Gdiplus::PointF(0, Y), pStringFormat, brush);
			strName.Empty();
		}
		else
		{
			CString sub = strName.Left(t);
			if(sub.IsEmpty() == FALSE)
			{
				_bstr_t bstr(sub);
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, Y), pStringFormat, brush);
			}

			strName = strName.Mid(t + 2);
		}

		Y += LineSpacing;
	} while(strName.IsEmpty() == FALSE);

	g.Restore(state);
	g.TranslateTransform(-origin.X, -origin.Y);

	::delete font1;
	font1 = nullptr;
	::delete brush1;
	brush1 = nullptr;
}
void CText::DrawAnchors(CDC* pDC, const CViewinfo& viewinfo) const
{
	CGeom::DrawAnchors(pDC, viewinfo);
	const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_Origin);
	CRect cliRect = CRect(CPoint(point.X, point.Y), CSize(0, 0));
	cliRect.InflateRect(1, 1);

	pDC->InvertRect(cliRect);
}

void CText::SetType(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CType* pNew = m_pType == nullptr ? new CType() : m_pType->Clone();
	CTypeDlg dlg(nullptr, *pNew);
	if(dlg.DoModal() == IDOK)
	{
		Invalidate(pWnd, viewinfo, 3);

		delete m_pType;
		m_pType = pNew;

		CalCorner(m_pType, viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);

		Invalidate(pWnd, viewinfo, 3);
	}
	else
	{
		delete pNew;
		pNew = nullptr;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CText::CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* pSpot, const CType* pType) const
{
	if(m_pType != nullptr)
		types.CountIn(m_pType);
	else
		types.CountIn(pType);
}

bool CText::HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const
{
	pType2 = m_pType != nullptr ? m_pType : pType2;

	if(activeall == ACTIVEALL::Type)
	{
		if(pType1 == nullptr)
			return false;
		else if(pType2 == nullptr)
			return false;
		else if(pType1 == pType2)
			return true;
		else
			return (*pType1 == *pType2);
	}

	return false;
}

void CText::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	CPRect::Transform(m11, m21, m31, m12, m22, m32);
	const int x = round(m_Origin.x * m11 + m_Origin.y * m21 + m31);
	const int y = round(m_Origin.x * m12 + m_Origin.y * m22 + m32);
	m_Origin.x = x;
	m_Origin.y = y;
}

void CText::Transform(const CViewinfo& viewinfo)
{
	const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_Origin);
	m_Origin.x = point.X;
	m_Origin.y = point.Y;

	CPRect::Transform(viewinfo);
}
void CText::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	const CPoint origin = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, m_Origin);
	const CSize Δ = CSize(origin.x - m_Origin.x, origin.y - m_Origin.y);

	m_Origin.x = origin.x;
	m_Origin.y = origin.y;

	CPRect::Move(Δ);
}

CGeom* CText::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	const CPoint origin = CDatainfo::Project(fMapinfo, tMapinfo, m_Origin);
	const CSize Δ = CSize(origin.x - m_Origin.x, origin.y - m_Origin.y);

	m_Origin.x = origin.x;
	m_Origin.y = origin.y;

	CPRect::Move(Δ);
	return this;
}

void CText::Rectify(CTin& tin)
{
	const CPoint origin = tin.Rectify(m_Origin);
	const CSize Δ = CSize(origin.x - m_Origin.x, origin.y - m_Origin.y);

	m_Origin.x = origin.x;
	m_Origin.y = origin.y;

	CPRect::Move(Δ);
}

CGeom* CText::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(CGeom::Clip(bound, clips, in, tolerance, bStroke) == nullptr)
		return nullptr;

	if(in == true && Clipper2Lib::PointInPolygon<int64_t>(Clipper2Lib::Point64(m_Origin.x, m_Origin.y), clips.front()) == Clipper2Lib::PointInPolygonResult::IsInside)
		return this;
	else if(in == false && Clipper2Lib::PointInPolygon<int64_t>(Clipper2Lib::Point64(m_Origin.x, m_Origin.y), clips.front()) == Clipper2Lib::PointInPolygonResult::IsOutside)
		return this;
	else
		return nullptr;
}

void CText::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
	CString strName = m_strName;
	strName.Replace(_T("\r\n"), _T(""));

	_ftprintf(fileMif, _T("Text\n"));
	_ftprintf(fileMif, _T("\"%s\"\n"), strName);

	if(bTransGeo == true)
	{
		const CPoint docPoint1(m_Rect.left, m_Rect.top);
		const CPointF geoPoint1 = datainfo.DocToGeo(docPoint1);
		_ftprintf(fileMif, _T("%g %g "), geoPoint1.x, geoPoint1.y);
		const CPoint docPoint2 = CPoint(m_Rect.right, m_Rect.bottom);
		const CPointF geoPoint2 = datainfo.DocToGeo(docPoint2);
		_ftprintf(fileMif, _T("%g %g\n"), geoPoint2.x, geoPoint2.y);
	}
	else
	{
		_ftprintf(fileMif, _T("%d %d %d %d\n"), m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom);
	}

	_ftprintf(fileMif, _T("Font (\"System\",0,0,0)"));
}

DWORD CText::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CGeom::PackPC(pFile, bytes);

	if(pFile != nullptr)
	{
		pFile->Write(&m_Origin, sizeof(CPoint));
		size += sizeof(CPoint);

		return size;
	}
	else
	{
		m_Origin = *(CPoint*)bytes;
		bytes += sizeof(CPoint);
		return 0;
	}
}

DWORD CText::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file, type);

	CPoint point = m_Origin;
	point.x /= 10000;
	point.y /= 10000;
	file.Write(&point, sizeof(CPoint));
	size += sizeof(CPoint);

	return size;
}

void CText::ReleaseWeb(CFile& file, CSize offset) const
{
	CPRect::ReleaseWeb(file, offset);;

	short x = (short)(m_Origin.x - offset.cx);
	short y = (short)(m_Origin.y - offset.cy);
	ReverseOrder(x);
	file.Write(&x, sizeof(short));
	ReverseOrder(y);
	file.Write(&y, sizeof(short));

	const BYTE cSwitch = m_pType == nullptr ? 0 : 1;
	file.Write(&cSwitch, sizeof(BYTE));
	if(m_pType != nullptr)
	{
		m_pType->ReleaseWeb(file);
	}
}

void CText::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CPRect::ReleaseWeb(stream, offset);;
	const short x = (short)(m_Origin.x - offset.cx);
	const short y = (short)(m_Origin.y - offset.cy);
	stream << x;
	stream << y;
	const BYTE cSwitch = m_pType == nullptr ? 0 : 1;
	stream << cSwitch;
	if(m_pType != nullptr)
	{
		m_pType->ReleaseWeb(stream);
	}
}
void CText::ReleaseWeb(boost::json::object& json) const
{
	CPRect::ReleaseWeb(json);;
	const double fTg = this->GetTg();
	const double fSin = this->GetSin();
	const double fCos = this->GetCos();
	const Gdiplus::Matrix matrix(fCos, -fSin, fSin - fTg * fCos, fTg * fSin + fCos, 0, 0);
	if(std::abs(fSin) > std::numeric_limits<double>::epsilon() || std::abs(fTg) > std::numeric_limits<double>::epsilon())
	{
		std::vector<float> child;
		child.push_back(fCos);
		child.push_back(-fSin);
		child.push_back(fSin - fTg * fCos);
		child.push_back(fTg * fSin + fCos);
		json["Matrix"] = boost::json::value_from(child);
	}
	{
		std::vector<int> child;
		child.push_back(m_Origin.x);
		child.push_back(m_Origin.y);
		json["Origin"] = boost::json::value_from(child);
	}
	if(m_pType != nullptr)
	{
		boost::json::object child;
		m_pType->ReleaseWeb(child);
		json["Font"] = child;
	}
}
void CText::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPRect::ReleaseWeb(writer, offset);

	writer.add_variant_sint32(m_Origin.x - offset.cx);
	writer.add_variant_sint32(m_Origin.y - offset.cy);

	const double fTg = this->GetTg();
	const double fSin = this->GetSin();
	const double fCos = this->GetCos();
	const Gdiplus::Matrix matrix(fCos, -fSin, fSin - fTg * fCos, fTg * fSin + fCos, 0, 0);
	if(std::abs(fSin) > std::numeric_limits<double>::epsilon() || std::abs(fTg) > std::numeric_limits<double>::epsilon())
	{
		writer.add_bool(true);

		writer.add_float(fCos);
		writer.add_float(-fSin);
		writer.add_float(fSin - fTg * fCos);
		writer.add_float(fTg * fSin + fCos);
	}
	else
		writer.add_bool(false);

	if(m_pType != nullptr)
	{
		writer.add_bool(true);
		m_pType->ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);
}
void CText::AnalyzeString(const CString& string, CObArray& labelarray)
{
	//const boost::locale::generator gen;
	//std::wstring wstr(string);
	//	boundary::wssegment_index map(boundary::word, wstr.begin(), wstr.end(), gen("zh_CN.UTF-8"));
	//	map.rule(boundary::word_any);
	//	for(auto it = map.begin(), e = map.end(); it!=e; ++it){
	//		CText::CWord* label = new CText::CWord();
	//		//	label->str = CA2W(it->str()); fix this
	//		labelarray.Add(label);
	//	}
}

