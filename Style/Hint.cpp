#include "stdafx.h"

#include "Hint.h"
#include "HintDlg.h"
#include "Color.h"
#include "Type.h"
#include "Psboard.h"

#include "../Dataview/viewinfo.h"
#include "../Geometry/Text.h"
#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHint CHint::m_default;

IMPLEMENT_SERIAL(CHint, CObject, 0)

CHint::CHint()
{
	m_fSize = 12;
	m_pColor = nullptr;
	m_filter = Filter::None;
}

CHint::~CHint()
{
	delete m_pColor;
}
void CHint::Sha1(boost::uuids::detail::sha1::digest_type& hash) const
{
	boost::uuids::detail::sha1 sha1;
	this->Sha1(sha1);
	sha1.get_digest(hash);
}
void CHint::Sha1(boost::uuids::detail::sha1& sha1) const
{
	m_fontdesc.Sha1(sha1);
	sha1.process_bytes(&m_fSize, sizeof(float));
	sha1.process_byte(m_filter);
	if(m_pColor!=nullptr)
	{
		m_pColor->Sha1(sha1);
	}
}
void CHint::Scale(float ratio)
{
	m_fSize *= ratio;
}

void CHint::operator=(const CHint& hint)
{
	hint.CopyTo(this);
}

BOOL CHint::operator==(const CHint& hint) const
{
	if(m_fontdesc != hint.m_fontdesc)
		return FALSE;
	else if(m_pColor != nullptr && hint.m_pColor != nullptr && *m_pColor != *hint.m_pColor)
		return FALSE;
	else if(m_fSize != hint.m_fSize)
		return FALSE;
	if(m_filter != hint.m_filter)
		return FALSE;
	else if(m_frame != hint.m_frame)
		return FALSE;
	else
		return TRUE;
}

BOOL CHint::operator !=(const CHint& hint) const
{
	return !(*this == hint);
}

CHint* CHint::Clone() const
{
	CHint* pClone = new CHint();
	this->CopyTo(pClone);
	return pClone;
}

void CHint::CopyTo(CHint* pHint) const
{
	pHint->m_fontdesc = m_fontdesc;
	pHint->m_fSize = m_fSize;
	pHint->m_filter = m_filter;
	pHint->m_frame = m_frame;
	pHint->m_outline = m_outline;
	delete pHint->m_pColor;
	pHint->m_pColor = nullptr;
	if(m_pColor != nullptr)
	{
		pHint->m_pColor = m_pColor->Clone();
	}
}

Gdiplus::Font* CHint::GetFont(const float& fZoom, const CSize& dpi) const
{
	const CString strFont = m_fontdesc.GetFamily();
	const _bstr_t btrFont(strFont);
	const Gdiplus::FontFamily fontFamily(btrFont);
	::SysFreeString(btrFont);
	if(fontFamily.IsAvailable())
	{
		const float pixels = roundf(m_fSize*fZoom*dpi.cx/72.f, 10);
		return ::new Gdiplus::Font(&fontFamily, pixels, m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
	}
	else if(this != &m_default)
	{
		return CHint::Default(m_fSize, m_fontdesc.GetStyle())->GetFont(fZoom, dpi);
	}
	else
	{
		const Gdiplus::FontFamily arial(L"Arial");
		const float pixels = m_fSize*fZoom*dpi.cx/72.f;
		return ::new Gdiplus::Font(&arial, pixels, m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
	}
}

Gdiplus::Color CHint::GetColor() const
{
	return m_pColor == nullptr ? Gdiplus::Color(255, 0, 0, 0) : m_pColor->GetColor();
}

float CHint::GetAscent(const float& fZoom, const CSize& dpi) const
{
	Gdiplus::Font* font = GetFont(fZoom, dpi);
	if(font != nullptr)
	{
		Gdiplus::FontFamily fontFamily;
		font->GetFamily(&fontFamily);
		const UINT ascent = fontFamily.GetCellAscent(m_fontdesc.GetStyle());
		const float fAscent = font->GetSize()*ascent/fontFamily.GetEmHeight(m_fontdesc.GetStyle());

		::delete font;
		font = nullptr;

		return fAscent;
	}
	else
		return 0.f;
}

float CHint::GetDescent(const float& fZoom, const CSize& dpi) const
{
	Gdiplus::Font* font = this->GetFont(fZoom, dpi);
	if(font != nullptr)
	{
		Gdiplus::FontFamily fontFamily;
		font->GetFamily(&fontFamily);
		const UINT descent = fontFamily.GetCellDescent(m_fontdesc.GetStyle());
		const float fDescent = font->GetSize()*descent/fontFamily.GetEmHeight(m_fontdesc.GetStyle());

		::delete font;
		font = nullptr;

		return fDescent;
	}
	else
		return 0.f;
}

float CHint::GetlineSpacing(const float& fZoom, const CSize& dpi) const
{
	Gdiplus::Font* font = this->GetFont(fZoom, dpi);
	if(font != nullptr)
	{
		Gdiplus::FontFamily fontFamily;
		font->GetFamily(&fontFamily);
		const Gdiplus::REAL sizeFont = font->GetSize();
		const UINT16 emLineSpacing = fontFamily.GetLineSpacing(m_fontdesc.GetStyle());
		const UINT16 emHeight = fontFamily.GetEmHeight(m_fontdesc.GetStyle());
		const float LineSpacing = sizeFont*emLineSpacing/emHeight;

		::delete font;
		font = nullptr;

		return LineSpacing;
	}
	else
		return 0;
}

Gdiplus::RectF CHint::DrawString(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CString& string, const Gdiplus::PointF& origin, WORD direction, HALIGN hAlign, VALIGN vAlign) const
{
	Gdiplus::SizeF textSize = CHint::MeasureText<Gdiplus::SizeF>(g, viewinfo, string, direction);
	Gdiplus::RectF textRect = ::GetTagRect(origin, textSize, hAlign, vAlign);
	switch(m_filter)
	{
		case Filter::Frame:
			{
				const Gdiplus::SizeF lacuna1 = CViewinfo::PointsToPixels(m_frame.m_widthBorder, viewinfo.m_sizeDPI);
				textRect.Inflate(round(-lacuna1.Width/2.f), round(-lacuna1.Height/2.f));//here the lacuna1 is supposed to be lacuna1/2, but if lacuna1<2, will be integed and make 

				if(m_frame.m_bRoundCorner == TRUE)
				{
					const float radius = m_fSize*viewinfo.m_sizeDPI.cx/(2*72.f);
					Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
					path.AddArc(textRect.GetRight() - radius, textRect.GetTop(), radius, radius, 270, 90);
					path.AddArc(textRect.GetRight() - radius, textRect.GetBottom() - radius, radius, radius, 0, 90);
					path.AddArc(textRect.GetLeft(), textRect.GetBottom() - radius, radius, radius, 90, 90);
					path.AddArc(textRect.GetLeft(), textRect.GetTop(), radius, radius, 180, 90);

					path.AddLine(textRect.GetLeft() + radius, textRect.GetTop(), textRect.GetRight() - radius/2, textRect.GetTop());

					path.CloseAllFigures();

					if(m_frame.m_bBackground == TRUE)
					{
						const Gdiplus::SolidBrush brush(m_frame.m_colorBackground);
						g.FillPath(&brush, &path);
					}
					if(m_frame.m_widthBorder > 0)
					{
						Gdiplus::Pen pen(m_frame.m_colorBorder, m_frame.m_widthBorder);
						pen.SetAlignment(Gdiplus::PenAlignmentCenter);
						g.DrawPath(&pen, &path);
					}
				}
				else
				{
					if(m_frame.m_bBackground == TRUE)
					{
						const Gdiplus::SolidBrush brush(m_frame.m_colorBackground);
						g.FillRectangle(&brush, textRect);
					}
					if(m_frame.m_widthBorder > 0)
					{
						Gdiplus::Pen pen(m_frame.m_colorBorder, m_frame.m_widthBorder);
						pen.SetAlignment(Gdiplus::PenAlignmentCenter);
						g.DrawRectangle(&pen, textRect);
					}
				}

				textRect.Inflate(round(lacuna1.Width/2.f), round(lacuna1.Height/2.f));
				textRect.Inflate(round(-lacuna1.Width), round(-lacuna1.Height));//for accuracy
				const Gdiplus::SizeF lacuna2 = CViewinfo::PointsToPixels(3, viewinfo.m_sizeDPI);
				textRect.Inflate(round(-lacuna2.Width), round(-lacuna2.Height));
			}
			break;
	}

	const CString strFontFamily = m_fontdesc.GetFamily();
	const _bstr_t btrFontFamily(strFontFamily);
	const Gdiplus::FontFamily fontFamily(btrFontFamily);
	::SysFreeString(btrFontFamily);
	const Gdiplus::Font font(&fontFamily, m_fSize*viewinfo.m_sizeDPI.cx/72.f, m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormat(Gdiplus::StringFormat::GenericTypographic());
	if(direction == 90)
	{
		stringFormat.SetFormatFlags(stringFormat.GetFormatFlags() | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical);
	}
	switch(m_filter)
	{
		case Filter::Outline:
			{
				Gdiplus::GraphicsPath path;
				const Gdiplus::Status status = path.AddString(string, -1, &fontFamily, font.GetStyle(), font.GetSize(), Gdiplus::PointF(textRect.GetLeft(), textRect.GetTop()), &stringFormat);
				if(status == Gdiplus::Status::Ok)
				{
					Gdiplus::Pen pen(m_outline.m_color, m_outline.m_width);
					pen.SetLineJoin(Gdiplus::LineJoinRound);
					g.DrawPath(&pen, &path);
				}
			}
			break;
		case None:
		case Frame:
			break;
		default:
			break;
	}
	//Pen pen(Color(255,128, 0, 0),1); //this 2 lines are used to check if the rect is correct
	//g.DrawRectangle(&pen,textRect);
	const int X = textRect.GetLeft();
	const int Y = textRect.GetTop();//for chinese Y is ok, but for english the Y is a little up moved.
	float offset = 0;//m_fSize*(fontFamily.GetLineSpacing(GetStyle())-fontFamily.GetCellAscent(GetStyle())-fontFamily.GetCellDescent(GetStyle()))/fontFamily.GetEmHeight(GetStyle());
	g.TranslateTransform(X, Y);
	const _bstr_t bstr(string);
	const Gdiplus::SolidBrush textBrush(m_pColor == nullptr ? 0XFF808080 : CColor::RGBtoARGB(m_pColor->GetMonitorRGB()));
	g.DrawString(bstr, -1, &font, Gdiplus::PointF(0, 0), &stringFormat, &textBrush);
	g.TranslateTransform(-X, -Y);

	return textRect;
}

void CHint::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	m_fontdesc.Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar << m_fSize;
		ar << m_filter;
		switch(m_filter)
		{
			case Filter::Frame:
				ar << m_frame;
				break;
			case Filter::Outline:
				ar << m_outline;
				break;
			default:
				break;
		}
		const BYTE colortype = m_pColor == nullptr ? 0XFF : m_pColor->Gettype();
		ar << colortype;
	}
	else
	{
		ar >> m_fSize;
		ar >> m_filter;
		switch(m_filter)
		{
			case Filter::Frame:
				ar >> m_frame;
				break;
			case Filter::Outline:
				ar >> m_outline;
				break;
			default:
				break;
		}

		char colortype;
		ar >> colortype;
		m_pColor = CColor::Apply(colortype);
	}

	if(m_pColor != nullptr)
	{
		m_pColor->Serialize(ar, dwVersion);
	}
}

void CHint::ReleaseCE(CArchive& ar) const
{
	//	ar << m_fontdesc.m_strName;	
	ar << m_fSize;
	const BYTE hint = m_pColor == nullptr ? -1 : m_pColor->Gettype();
	ar << hint;

	if(m_pColor != nullptr)
	{
		m_pColor->ReleaseCE(ar);
	}

	ar << m_filter;
	ar << m_frame;
}

void CHint::ReleaseDCOM(CArchive& ar)
{
	m_fontdesc.ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_fSize;
	}
	else
	{
		ar >> m_fSize;
	}

	if(m_pColor != nullptr)
	{
		m_pColor->ReleaseDCOM(ar);
	}
}

void CHint::ReleaseWeb(CFile& file) const
{
	m_fontdesc.ReleaseWeb(file);

	float fSize = m_fSize;
	ReverseOrder(fSize);
	file.Write(&fSize, sizeof(float));

	if(m_pColor != nullptr)
	{
		m_pColor->ReleaseWeb(file);
	}
	else
	{
		const BYTE alpha = 255;
		file.Write(&alpha, sizeof(BYTE));

		const DWORD color = 0X808080FF;
		file.Write(&color, sizeof(DWORD));
	}
	file.Write(&m_filter, sizeof(BYTE));
	switch(m_filter)
	{
		case Filter::Frame:
			m_frame.ReleaseWeb(file);
			break;
		case Filter::Outline:
			m_outline.ReleaseWeb(file);
			break;
		default:
			break;
	}
}

void CHint::ReleaseWeb(BinaryStream& stream) const
{
	m_fontdesc.ReleaseWeb(stream);

	stream << m_fSize;
	if(m_pColor != nullptr)
	{
		m_pColor->ReleaseWeb(stream);
	}
	else
	{
		const BYTE alpha = 255;
		stream << alpha;
		const DWORD color = 0X808080FF;
		stream << color;
	}
	stream << m_filter;
	switch(m_filter)
	{
		case Filter::Frame:
			m_frame.ReleaseWeb(stream);
			break;
		case Filter::Outline:
			m_outline.ReleaseWeb(stream);
			break;
		default:
			break;
	}
}
void CHint::ReleaseWeb(boost::json::object& json) const
{
	m_fontdesc.ReleaseWeb(json);

	json["Size"] = m_fSize;

	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(json);
	else
		json["Color"] = "rgba(128,128,128,1)";

	switch(m_fontdesc.GetStyle())
	{
		case Gdiplus::FontStyle::FontStyleBold:
			json["Style"] = "bold";
			break;
		case Gdiplus::FontStyle::FontStyleBoldItalic:
			json["Style"] = "bold italic";
			break;
		case Gdiplus::FontStyle::FontStyleItalic:
			json["Style"] = "italic";
			break;
			//case FontStyle::FontStyleRegular:
			//	break;
			//case FontStyle::FontStyleStrikeout:
			//	json.put("Style", "strikeout"); 
			//	break;
			//case FontStyle::FontStyleUnderline:
			//	json.put("Style", "underline"); 
			//	break;
		default:
			break;
	}
	switch(m_filter)
	{
		case Filter::Frame:
			m_frame.ReleaseWeb(json);
			break;
		case Filter::Outline:
			m_outline.ReleaseWeb(json);
			break;
	}
}
void CHint::ReleaseWeb(pbf::writer& writer) const
{
	m_fontdesc.ReleaseWeb(writer);

	writer.add_float(m_fSize);

	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(writer);
	else
		writer.add_string("rgba(128,128,128,1)");

	switch(m_fontdesc.GetStyle())
	{
		case Gdiplus::FontStyle::FontStyleBold:
			writer.add_string("bold");
			break;
		case Gdiplus::FontStyle::FontStyleBoldItalic:
			writer.add_string("bold italic");
			break;
		case Gdiplus::FontStyle::FontStyleItalic:
			writer.add_string("italic");
			break;
			//case FontStyle::FontStyleRegular:
			//	break;
			//case FontStyle::FontStyleStrikeout:
			//	json.put("Style", "strikeout"); 
			//	break;
			//case FontStyle::FontStyleUnderline:
			//	json.put("Style", "underline"); 
			//	break;
		default:
			writer.add_string("");
			break;
	}
	writer.add_byte(m_filter);
	switch(m_filter)
	{
		case Filter::Frame:
			m_frame.ReleaseWeb(writer);
			break;
		case Filter::Outline:
			m_outline.ReleaseWeb(writer);
			break;
	}
}
void CHint::ExportPs3(FILE* file, CPsboard& aiKey) const
{
	const CStringA strFont = m_fontdesc.GetReal();
	if(m_pColor != nullptr)
	{
		m_pColor->ExportAIBrush(file, aiKey);
	}
	else if(aiKey.K != 100)
	{
		_ftprintf(file, _T("0 0 0 1 k\n"));
		aiKey.k = 100;
	}

	if(aiKey.strFont != strFont || aiKey.sizeFont != m_fSize)
	{
		const float nDescent = this->GetDescent(1.0f, 72);
		const float nAscent = this->GetAscent(1.0f, 72);

		_ftprintf(file, _T("0 To\n"));
		_ftprintf(file, _T("1 0 0 1 34.4756 256.5342 0 Tp\n"));
		_ftprintf(file, _T("0 Tv\n"));
		_ftprintf(file, _T("TP\n"));
		_ftprintf(file, _T("0 Tr\n"));

		_ftprintf(file, _T("/_%s %g %g -%g Tf\n"), strFont, m_fSize, nAscent, nDescent);
		_ftprintf(file, _T("100 100 Tz\n"));
		_ftprintf(file, _T("0 Tt\n"));
		_ftprintf(file, _T("%%AI55J_GlyphSubst: GlyphSubstNone \n"));
		_ftprintf(file, _T("%%_ 0 XL\n"));
		_ftprintf(file, _T("0 0 Tl\n"));
		_ftprintf(file, _T("0 Tc\n"));
		_ftprintf(file, _T("() Tx 1 0 Tk\n"));
		_ftprintf(file, _T("TO\n"));

		aiKey.strFont = strFont;
		aiKey.sizeFont = m_fSize;
	}
}

void CHint::ExportPdf(HPDF_Doc pdf, HPDF_Page page) const
{
	const CStringA strFont = m_fontdesc.GetReal();

	HPDF_Font font = nullptr;
	if(CFontDesc::FontRealNameExists(strFont) == false)
		font = HPDF_GetFont(pdf, m_default.m_fontdesc.GetReal(), "UTF-8");
	else
	{
		const HPDF_FontDef fontdef = HPDF_GetFontDef(pdf, strFont);//there is a problem here, when the fond style is bold or other, can't find the fonddef from its name
		if(fontdef == nullptr)
		{
			HPDF_ResetError(pdf);
			font = HPDF_GetFont(pdf, m_default.m_fontdesc.GetReal(), "UTF-8");
		}
		else
		{
			CStringA strEncoding = m_fontdesc.GetEncoding();
			const char* pEncoding = strEncoding.IsEmpty() ? nullptr : strEncoding.GetBuffer();
			font = HPDF_GetFont(pdf, strFont, "UTF-8");
			strEncoding.ReleaseBuffer();
		}
	}

	if(font == nullptr)
		return;

	HPDF_Page_SetTextRenderingMode(page, HPDF_FILL);
	HPDF_Page_SetFontAndSize(page, font, m_fSize);
	if(m_pColor != nullptr)
		m_pColor->ExportPdf(page, true);
	else
		HPDF_Page_SetRGBFill(page, 0, 0, 0);
	const float linespace = GetlineSpacing(1, 72);
	HPDF_Page_SetTextLeading(page, linespace);
	//	HPDF_Page_SetCharSpace (page, 1);
	//	HPDF_Page_SetWordSpace (page, 1);
}

void CHint::ExportTag(FILE* file, const CViewinfo& viewinfo, const CPsboard& aiKey, const Gdiplus::PointF& origin, WORD direction, const HALIGN& hAlign, const VALIGN& vAlign, CString string) const
{
	const Gdiplus::SizeF textSize = CHint::MeasureText<Gdiplus::SizeF>(viewinfo, string, 0);
	const Gdiplus::RectF textRect = ::GetTagRect(origin, textSize, hAlign, vAlign);
			
	const CString strFontFamily = m_fontdesc.GetFamily();
	const _bstr_t btrFontFamily(strFontFamily);
	const Gdiplus::FontFamily fontFamily(btrFontFamily);
	::SysFreeString(btrFontFamily);
	const Gdiplus::Font font(&fontFamily, m_fSize*viewinfo.m_sizeDPI.cx/72.f, m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
	const Gdiplus::REAL fSize = font.GetSize();
	const float fAscent = fSize*fontFamily.GetCellAscent(m_fontdesc.GetStyle())/fontFamily.GetEmHeight(m_fontdesc.GetStyle());
	float fDescent = fSize*fontFamily.GetCellDescent(m_fontdesc.GetStyle())/fontFamily.GetEmHeight(m_fontdesc.GetStyle());
	float fLinespace = fSize*fontFamily.GetLineSpacing(m_fontdesc.GetStyle())/fontFamily.GetEmHeight(m_fontdesc.GetStyle());//the real size dose not match with the measured size
	fLinespace = fSize;

	short nRow = 0;
	short f = 0;
	short t = 0;
	while(t != -1 && t != string.GetLength())
	{
		t = string.Find(_T("\r\n"), f);
		if(t == -1)
			t = string.GetLength();

		f = t + 2;
		nRow++;
	}

	_ftprintf(file, _T("u\n"));
	if(direction == 90)
	{
		float originX = textRect.GetRight();
		for(int row = 0; row < nRow; row++)
		{
			const int pos = string.Find(_T("\r\n"));
			CString str;
			if(pos != -1)
			{
				str = string.Left(pos);
				string = string.Mid(pos + 2);
			}
			else
				str = string;

			originX -= fSize;
			float originY = textRect.GetBottom() - fAscent;
			CObArray labelarray;
			CText::AnalyzeString(str, labelarray);
			const int nWords = labelarray.GetCount();
			for(short index = 0; index < nWords; index++)
			{
				const CText::CWord* word = (CText::CWord*)labelarray.GetAt(index);

				_ftprintf(file, _T("0 To\n"));
				_ftprintf(file, _T("1 0 0 1 %g %g 0 Tp\n"), originX, originY);
				_ftprintf(file, _T("0 Tv\n"));
				_ftprintf(file, _T("TP\n"));
				_ftprintf(file, _T("1 0 0 1 %g %g Tm\n"), originX, originY);
				_ftprintf(file, _T("0 Tr\n"));
				const CString str = ConvertToOctalString(word->str);
				_ftprintf(file, _T("(%s) Tx 1 0 Tk\n"), str);
				_ftprintf(file, _T("TO\n"));

				originY -= fLinespace;
			}
			for(int index = 0; index < labelarray.GetCount(); index++)
			{
				const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
				delete label;
			}
			labelarray.RemoveAll();
		}
	}
	else
	{
		const float originX = textRect.GetLeft();
		float originY = textRect.GetBottom() - fAscent;
		for(int row = 0; row < nRow; row++)
		{
			const int pos = string.Find(_T("\r\n"));
			CString str;
			if(pos != -1)
			{
				str = string.Left(pos);
				string = string.Mid(pos + 2);
			}
			else
				str = string;

			_ftprintf(file, _T("0 To\n"));
			_ftprintf(file, _T("1 0 0 1 %g %g 0 Tp\n"), originX, originY);
			_ftprintf(file, _T("0 Tv\n"));
			_ftprintf(file, _T("TP\n"));
			_ftprintf(file, _T("1 0 0 1 %g %g Tm\n"), originX, originY);
			_ftprintf(file, _T("0 Tr\n"));

			str = ConvertToOctalString(str);

			_ftprintf(file, _T("(%s) Tx 1 0 Tk\n"), str);
			_ftprintf(file, _T("TO\n"));

			originY -= fLinespace;
		}
	}
	_ftprintf(file, _T("U\n"));
}

void CHint::ExportStyle(HPDF_Doc pdf, HPDF_Page page, Gdiplus::RectF& tagRect) const
{
	tagRect.Inflate(-1, -1);

	switch(m_filter)
	{
		case Filter::Frame:
			{
				const Gdiplus::SizeF lacuna1 = CViewinfo::PointsToPixels(m_frame.m_widthBorder, 72);
				tagRect.Inflate(round(-lacuna1.Width/2.f), round(-lacuna1.Height/2.f));//here the lacuna1 is supposed to be lacuna1/2, but if lacuna1<2, will be integed and make 

				if(m_frame.m_bBackground == TRUE)
				{
					const BYTE r = Gdiplus::Color(m_frame.m_colorBackground).GetR();
					const BYTE g = Gdiplus::Color(m_frame.m_colorBackground).GetG();
					const BYTE b = Gdiplus::Color(m_frame.m_colorBackground).GetB();
					HPDF_Page_SetRGBFill(page, r/255.f, g/255.f, b/255.f);
				}
				if(m_frame.m_widthBorder > 0)
				{
					const BYTE r = Gdiplus::Color(m_frame.m_colorBorder).GetR();
					const BYTE g = Gdiplus::Color(m_frame.m_colorBorder).GetG();
					const BYTE b = Gdiplus::Color(m_frame.m_colorBorder).GetB();
					HPDF_Page_SetRGBStroke(page, r/255.f, g/255.f, b/255.f);
				}

				if(m_frame.m_bRoundCorner == TRUE)
				{
					const float radius = m_fSize/2;
					HPDF_Page_MoveTo(page, tagRect.GetLeft() + radius, tagRect.GetBottom());
					HPDF_Page_LineTo(page, tagRect.GetRight() - radius, tagRect.GetBottom());
					HPDF_Page_Arc(page, tagRect.GetRight() - radius, tagRect.GetBottom() - radius, radius, 0, 90);
					HPDF_Page_LineTo(page, tagRect.GetRight(), tagRect.GetTop() + radius);
					HPDF_Page_Arc(page, tagRect.GetRight() - radius, tagRect.GetTop() + radius, radius, 90, 180);
					HPDF_Page_LineTo(page, tagRect.GetLeft() + radius, tagRect.GetTop());
					HPDF_Page_Arc(page, tagRect.GetLeft() + radius, tagRect.GetTop() + radius, radius, 180, 270);
					HPDF_Page_LineTo(page, tagRect.GetLeft(), tagRect.GetBottom() - radius);
					HPDF_Page_Arc(page, tagRect.GetLeft() + radius, tagRect.GetBottom() - radius, radius, 270, 360);
				}
				else
				{
					HPDF_Page_Rectangle(page, tagRect.X, tagRect.Y, tagRect.Width, tagRect.Height);
				}

				if(m_frame.m_bBackground == TRUE && m_frame.m_widthBorder > 0)
					HPDF_Page_FillStroke(page);
				else if(m_frame.m_bBackground == TRUE)
					HPDF_Page_Fill(page);
				else
					HPDF_Page_Stroke(page);

				tagRect.Inflate(round(lacuna1.Width/2.f), round(lacuna1.Height/2.f));
				tagRect.Inflate(round(-lacuna1.Width), round(-lacuna1.Height));//for accuracy
				const Gdiplus::SizeF lacuna2 = CViewinfo::PointsToPixels(3, CSize(72, 72));
				tagRect.Inflate(round(-lacuna2.Width), round(-lacuna2.Height));
			}
			break;
		case Filter::Outline:
			{
				Gdiplus::GraphicsPath path;
				//	Status status = path.AddString(string,-1,&fontFamily,font.GetStyle(),font.GetSize(),PointF(tagRect.GetLeft(),tagRect.GetTop()),&stringFormat);
				//	if(status==Status::Ok)
				//	{
				//		Pen pen(m_outline.m_colorBorder,m_outline.m_widthBorder);
				//		pen.SetLineJoin(LineJoinRound);
				//		g.DrawPath(&pen, &path);		
				//	}
			}
			break;
		default:
			break;
	}
}

void CHint::ExportTag(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const Gdiplus::PointF& origin, WORD direction, const HALIGN& hAlign, const VALIGN& vAlign, CString string) const
{
	Gdiplus::SizeF textSize = CHint::MeasureText<Gdiplus::SizeF>(viewinfo, string, 0);
	Gdiplus::RectF textRect = ::GetTagRect(origin, textSize, hAlign, vAlign);
	CHint::ExportStyle(pdf, page, textRect);

	CHint::ExportPdf(pdf, page);
	const CString strFontFamily = m_fontdesc.GetFamily();
	const _bstr_t btrFontFamily(strFontFamily);
	const Gdiplus::FontFamily fontFamily(btrFontFamily);
	::SysFreeString(btrFontFamily);
	const Gdiplus::Font font(&fontFamily, m_fSize*viewinfo.m_sizeDPI.cx/72.f, m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
	const Gdiplus::REAL fSize = font.GetSize();
	const float fAscent = fSize*fontFamily.GetCellAscent(m_fontdesc.GetStyle())/fontFamily.GetEmHeight(m_fontdesc.GetStyle());
	float fDescent = fSize*fontFamily.GetCellDescent(m_fontdesc.GetStyle())/fontFamily.GetEmHeight(m_fontdesc.GetStyle());
	float fLinespace = fSize*fontFamily.GetLineSpacing(m_fontdesc.GetStyle())/fontFamily.GetEmHeight(m_fontdesc.GetStyle());//the real size dose not match with the measured size
	fLinespace = fSize;

	string.Replace(_T("\r\n"), _T("\r"));

	short nRow = 0;
	short f = 0;
	short t = 0;
	while(t != -1 && t != string.GetLength())
	{
		t = string.Find(_T("\r"), f);
		if(t == -1)
			t = string.GetLength();

		f = t + 1;
		nRow++;
	}

	if(direction == 0)
	{
		float originX = textRect.GetRight();
		for(int row = 0; row < nRow; row++)
		{
			const int pos = string.Find(_T("\r"));
			CString str;
			if(pos != -1)
			{
				str = string.Left(pos);
				string = string.Mid(pos + 1);
			}
			else
				str = string;

			originX -= fSize;
			float originY = textRect.GetBottom() - fAscent;
			CObArray labelarray;
			CText::AnalyzeString(str, labelarray);
			const int nWords = labelarray.GetCount();
			for(short index = 0; index < nWords; index++)
			{
				CText::CWord* word = (CText::CWord*)labelarray.GetAt(index);

				const char* pString = UnicodeToMultiByte(word->str.GetBuffer());

				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, originX, originY, pString);
				HPDF_Page_EndText(page);

				word->str.ReleaseBuffer();
				delete pString;
				pString = nullptr;

				originY -= fLinespace;
			}
			for(int index = 0; index < labelarray.GetCount(); index++)
			{
				const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
				delete label;
			}
			labelarray.RemoveAll();
		}
	}
	else
	{
		const float originX = textRect.GetLeft();
		float originY = textRect.GetBottom() - fAscent;
		for(int row = 0; row < nRow; row++)
		{
			const int pos = string.Find(_T("\r"));
			CString str;
			if(pos != -1)
			{
				str = string.Left(pos);
				string = string.Mid(pos + 1);
			}
			else
				str = string;

			const char* pString = UnicodeToMultiByte(str.GetBuffer());

			HPDF_Page_BeginText(page);
			HPDF_Page_TextOut(page, originX, originY, pString);
			HPDF_Page_EndText(page);

			str.ReleaseBuffer();
			delete pString;
			pString = nullptr;

			originY -= fLinespace;
		}
	}
}

CHint* CHint::Default(float size, Gdiplus::FontStyle style)
{
	m_default.m_fSize = size;
	m_default.m_fontdesc.SetStyle(style);
	return &m_default;
};

void AFXAPI operator <<(CArchive& ar, CHint::Filter& filter)
{
	ar << (BYTE)filter;
};

void AFXAPI operator >>(CArchive& ar, CHint::Filter& filter)
{
	BYTE byte;
	ar >> byte;
	filter = (CHint::Filter)byte;
};

void AFXAPI operator <<(CArchive& ar, Gdiplus::FontStyle& style)
{
	ar << (BYTE)style;
};

void AFXAPI operator >>(CArchive& ar, Gdiplus::FontStyle& style)
{
	BYTE byte;
	ar >> byte;
	style = (Gdiplus::FontStyle)byte;
};
