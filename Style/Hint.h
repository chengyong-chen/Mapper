#pragma once
#include <string>
#include "FontDesc.h"
#include "TextStyle.h"
#include "../Geometry/Global.h"
#include "../DataView/viewinfo.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include <boost/compute/detail/sha1.hpp>

class CColor;
class CTag;
class CViewinfo;
class Psboard;
struct Context;
using namespace std;

class __declspec(dllexport) CHint : public CObject
{
public:
	enum Filter : BYTE
	{
		None = 0,
		Frame,
		Outline
	};

protected:
	DECLARE_SERIAL(CHint);
public:
	CHint();

	~CHint() override;

public:
	CFontDesc m_fontdesc;
	float m_fSize;
	CColor* m_pColor;

public:
	Filter m_filter;
	TextFrame m_frame;
	TextOutline m_outline;
public:
	virtual void Sha1(boost::uuids::detail::sha1::digest_type& hash) const;
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
	virtual void Scale(float ratio);
public:
	Gdiplus::RectF DrawString(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CString& string, const Gdiplus::PointF& origin, WORD direction, HALIGN hAlign, VALIGN vAlign) const;
	virtual Gdiplus::Font* GetFont(const float& fZoom, const CSize& dpi) const;

	Gdiplus::Color GetColor() const;

	float GetDescent(const float& fZoom, const CSize& dpi) const;
	float GetAscent(const float& fZoom, const CSize& dpi) const;
	float GetlineSpacing(const float& fZoom, const CSize& dpi) const;
public:
	virtual void operator =(const CHint& hint);
	virtual BOOL operator ==(const CHint& hint) const;
	virtual BOOL operator !=(const CHint& hint) const;
	virtual CHint* Clone() const;
	virtual void CopyTo(CHint* pHint) const;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;

	virtual void ExportPs3(FILE* file, CPsboard& aiKey) const;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page) const;
	virtual void ExportTag(FILE* file, const CViewinfo& viewinfo, const CPsboard& aiKey, const Gdiplus::PointF& origin, WORD direction, const HALIGN& hAlign, const VALIGN& vAlign, CString string) const;
	virtual void ExportTag(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const Gdiplus::PointF& origin, WORD direction, const HALIGN& hAlign, const VALIGN& vAlign, CString string) const;
	virtual void ExportStyle(HPDF_Doc pdf, HPDF_Page page, Gdiplus::RectF& textRect) const;

public:
	static CHint m_default;
	static CHint* Default(float size = 12, Gdiplus::FontStyle style = Gdiplus::FontStyle::FontStyleRegular);

public:
	template<typename T>
	T MeasureText(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CString& string, WORD direction) const
	{
		if(string.IsEmpty() == TRUE)
			return T(0, 0);

		float cx;
		float cy;
		{//get rect's width and height
			const CString strFontFamily = m_fontdesc.GetFamilyName();
			const _bstr_t btrFontFamily(strFontFamily);
			float fSize = m_fSize*viewinfo.m_sizeDPI.cx/72.f;			
			const Gdiplus::FontFamily fontFamily(btrFontFamily);
			::SysFreeString(btrFontFamily);
			const Gdiplus::Font font(&fontFamily, fSize, m_fontdesc.GetStyle(), Gdiplus::UnitPixel);
			fSize = font.GetSize();

			Gdiplus::StringFormat stringFormat(Gdiplus::StringFormat::GenericTypographic());
			const UINT16 emLineSpacing = fontFamily.GetLineSpacing(m_fontdesc.GetStyle());
			const UINT16 emHeight = fontFamily.GetEmHeight(m_fontdesc.GetStyle());
			const float LineSpacing = fSize*emLineSpacing/emHeight;
			const float linegap = LineSpacing - fSize;
			if(direction == 90)
			{
				stringFormat.SetFormatFlags(stringFormat.GetFormatFlags() | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical);
				const _bstr_t bstr(string);
				Gdiplus::RectF bound;
				g.MeasureString(bstr, -1, &font, Gdiplus::PointF(0, 0), &stringFormat, &bound);//there is an extra width here, havn't figured out how to get rid of it.
				cx = bound.Width;
				cy = bound.Height - linegap;
			}
			else
			{
				Gdiplus::RectF bound;
				const _bstr_t bstr(string);
				g.MeasureString(bstr, -1, &font, Gdiplus::PointF(0, 0), &stringFormat, &bound);
				cx = bound.Width;
				cy = bound.Height - linegap;
			}

			switch(m_filter)
			{
				case Filter::Frame:
					{
						const Gdiplus::SizeF lacuna1 = CViewinfo::PointsToPixels(m_frame.m_widthBorder, viewinfo.m_sizeDPI);
						cx += round(lacuna1.Width)*2;
						cy += round(lacuna1.Height)*2;
						const Gdiplus::SizeF lacuna2 = CViewinfo::PointsToPixels(3, viewinfo.m_sizeDPI);
						cx += round(lacuna2.Width)*2;
						cy += round(lacuna2.Height)*2;
					}
					break;
			}
		}
		return T(cx, cy);
	};
	template<typename T>
	T MeasureText(const CViewinfo& viewinfo, const CString& string, WORD direction) const
	{
		Gdiplus::Bitmap bitmap(1, 1);
		Gdiplus::Graphics g(&bitmap);
		return MeasureText<T>(g, viewinfo, string, direction);
	};

	template<typename T>
	T MeasureTextRect(const T& tagRect, const CSize& dpi) const
	{
		T textRect = tagRect;
		textRect.Inflate(-1, -1);

		switch(m_filter)
		{
			case Filter::Frame:
			{
				const Gdiplus::SizeF lacuna1 = CViewinfo::PointsToPixels(m_frame.m_widthBorder, dpi);
				textRect.Inflate(round(-lacuna1.Width/2.f), round(-lacuna1.Height/2.f));//here the lacuna1 is supposed to be lacuna1/2, but if lacuna1<2, will be integed and make 
				textRect.Inflate(round(lacuna1.Width/2.f), round(lacuna1.Height/2.f));
				textRect.Inflate(round(-lacuna1.Width), round(-lacuna1.Height));//for accuracy
				const Gdiplus::SizeF lacuna2 = CViewinfo::PointsToPixels(3, dpi);
				textRect.Inflate(round(-lacuna2.Width), round(-lacuna2.Height));
			}
			break;
			case None:
			case Outline:
				break;
			default:
				break;
		}

		return textRect;
	}
};

void AFXAPI operator <<(CArchive& ar, CHint::Filter& filter);
void AFXAPI operator >>(CArchive& ar, CHint::Filter& filter);

void AFXAPI operator <<(CArchive& ar, Gdiplus::FontStyle& style);
void AFXAPI operator >>(CArchive& ar, Gdiplus::FontStyle& style);
