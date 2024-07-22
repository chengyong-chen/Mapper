#include "stdafx.h"

#include "TextStyle.h"

#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"

#include <format>
#include <boost/json.hpp>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetAValue(rgba)      (LOBYTE((rgba)>>24))

TextFrame::TextFrame()
{
	m_widthBorder=1;
	m_colorBorder=Gdiplus::Color::MakeARGB(255, 0, 0, 0);
	m_bRoundCorner=TRUE;
	m_bBackground=FALSE;
	m_colorBackground=Gdiplus::Color::MakeARGB(255, 255, 255, 255);
};
void TextFrame::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_bytes(&m_widthBorder, sizeof(float));
	sha1.process_bytes(&m_colorBorder, sizeof(DWORD));
	sha1.process_bytes(&m_bRoundCorner, sizeof(BYTE));
	sha1.process_bytes(&m_bBackground, sizeof(BYTE));
	sha1.process_bytes(&m_colorBackground, sizeof(DWORD));
}
BOOL TextFrame::operator ==(const TextFrame& frame) const
{
	if(m_widthBorder != frame.m_widthBorder)
		return FALSE;
	else if(m_colorBorder != frame.m_colorBorder)
		return FALSE;
	else if(m_bRoundCorner != frame.m_bRoundCorner)
		return FALSE;
	else if(m_bBackground != frame.m_bBackground)
		return FALSE;
	else if(m_colorBackground != frame.m_colorBackground)
		return FALSE;
	else
		return TRUE;
};

BOOL TextFrame::operator !=(const TextFrame& frame) const
{
	return !(*this == frame);
};

void TextFrame::ReleaseWeb(CFile& file) const
{
	float width=m_widthBorder;
	ReverseOrder(width);
	file.Write(&width, sizeof(float));
	file.Write(&m_colorBorder, sizeof(DWORD));
	file.Write(&m_bRoundCorner, sizeof(BYTE));
	file.Write(&m_bBackground, sizeof(BYTE));
	file.Write(&m_colorBackground, sizeof(DWORD));
}

void TextFrame::ReleaseWeb(BinaryStream& stream) const
{
	stream << m_widthBorder;
	stream << m_colorBorder;
	stream << m_bRoundCorner;
	stream << m_bBackground;
	stream << m_colorBackground;
}
void TextFrame::ReleaseWeb(boost::json::object& json) const
{
	boost::json::object child;
	child["BorderColor"]=m_colorBorder;
	child["BorderWidth"]=m_widthBorder;
	child["RoundCorner"]=m_bRoundCorner;
	child["Background"]=m_bBackground;
	child["BackgroundColor"]=m_colorBackground;
	json["Frame"]=child;
}
void TextFrame::ReleaseWeb(pbf::writer& writer) const
{	
	writer.add_fixed_uint32( m_colorBorder);
	writer.add_float(m_widthBorder);
	writer.add_bool(m_bRoundCorner);
	writer.add_bool(m_bBackground);
	writer.add_fixed_uint32(m_colorBackground);
}
TextOutline::TextOutline()
{
	m_width=1;
	m_color=Gdiplus::Color::MakeARGB(255, 0, 0, 0);
	m_bShadow=1;
	m_argbShadow=Gdiplus::Color::MakeARGB(255, 128, 128, 128);
}

BOOL TextOutline::operator ==(const TextOutline& outline) const
{
	if(m_width != outline.m_width)
		return FALSE;
	else if(m_color != outline.m_color)
		return FALSE;
	else if(m_bShadow != outline.m_bShadow)
		return FALSE;
	else if(m_argbShadow != outline.m_argbShadow)
		return FALSE;
	else
		return TRUE;
};
void TextOutline::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_bytes(&m_width, sizeof(float));
	sha1.process_bytes(&m_color, sizeof(DWORD));
}
BOOL TextOutline::operator !=(const TextOutline& outline) const
{
	return !(*this == outline);
}

void TextOutline::ReleaseWeb(CFile& file) const
{
	float width=m_width;
	ReverseOrder(width);
	file.Write(&width, sizeof(float));
	file.Write(&m_color, sizeof(DWORD));
}

void TextOutline::ReleaseWeb(BinaryStream& stream) const
{
	stream << m_width;
	stream << m_color;
}
void TextOutline::ReleaseWeb(boost::json::object& json) const
{
	CStringA rgba;
	rgba.Format("rgba({},{},{},{})", (m_color & 0x00FF0000) >> 16, (m_color & 0x0000FF00) >> 8, (m_color & 0x000000FF), ((m_color & 0xFF000000) >> 24)/255.f);

	boost::json::object child;
	child["Width"]=m_width;
	child["Color"]=rgba;
	json["Outline"]=child;
}
void TextOutline::ReleaseWeb(pbf::writer& writer) const
{
	std::string rgba=std::format("rgba({},{},{},{})", (m_color&0x00FF0000)>>16, (m_color & 0x0000FF00) >> 8, (m_color & 0x000000FF), ((m_color & 0xFF000000) >> 24)/255.f);
	writer.add_float(m_width);
	writer.add_string(rgba);
}
TextDeform::TextDeform()
{
	m_nLeterspace=0;
	m_nWordspace=0;
	m_nLeadindex=0;
	m_nWeight=FW_NORMAL;
	m_nHorzscale=100;
	m_nHSkewAngle=0;
	m_nVSkewAngle=0;
}

BOOL TextDeform::operator ==(const TextDeform& type) const
{
	if(m_nWordspace != type.m_nWordspace)
		return FALSE;
	else if(m_nLeadindex != type.m_nLeadindex)
		return FALSE;
	else if(m_nWeight != type.m_nWeight)
		return FALSE;
	else if(m_nHorzscale != type.m_nHorzscale)
		return FALSE;
	else if(m_nHSkewAngle != type.m_nHSkewAngle)
		return FALSE;
	else if(m_nVSkewAngle != type.m_nVSkewAngle)
		return FALSE;
	else
		return TRUE;
};
void TextDeform::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_bytes(&m_nLeterspace, sizeof(BYTE));
	sha1.process_bytes(&m_nWordspace, sizeof(BYTE));
	sha1.process_bytes(&m_nHorzscale, sizeof(BYTE));
	sha1.process_bytes(&m_nHSkewAngle, sizeof(BYTE));
	sha1.process_bytes(&m_nVSkewAngle, sizeof(BYTE));
}
BOOL TextDeform::operator !=(const TextDeform& type) const
{
	return !(*this == type);
}

void TextDeform::ReleaseWeb(CFile& file) const
{
	file.Write(&m_nLeterspace, sizeof(BYTE));
	file.Write(&m_nWordspace, sizeof(BYTE));
	file.Write(&m_nHorzscale, sizeof(BYTE));
	file.Write(&m_nHSkewAngle, sizeof(BYTE));
	file.Write(&m_nVSkewAngle, sizeof(BYTE));
}

void TextDeform::ReleaseWeb(BinaryStream& stream) const
{
	stream << m_nLeterspace;
	stream << m_nWordspace;
	stream << m_nHorzscale;
	stream << m_nHSkewAngle;
	stream << m_nVSkewAngle;
}
void TextDeform::ReleaseWeb(boost::json::object& json) const
{
	json["Leterspace"]=m_nLeterspace;
	json["Wordspace"]=m_nWordspace;
	json["Horzscale"]=m_nHorzscale;
	json["HSkewAngle"]=m_nHSkewAngle;
	json["VSkewAngle"]=m_nVSkewAngle;
}
void TextDeform::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_byte(m_nLeterspace);
	writer.add_byte(m_nWordspace);
	writer.add_byte(m_nHorzscale);
	writer.add_byte(m_nHSkewAngle);
	writer.add_byte(m_nVSkewAngle);
}
void AFXAPI operator <<(CArchive& ar, const TextFrame& frame)
{
	ar << frame.m_widthBorder;
	ar << frame.m_colorBorder;
	ar << frame.m_bRoundCorner;
	ar << frame.m_bBackground;
	ar << frame.m_colorBackground;
}

void AFXAPI operator >>(CArchive& ar, TextFrame& frame)
{
	ar >> frame.m_widthBorder;
	ar >> frame.m_colorBorder;
	ar >> frame.m_bRoundCorner;
	ar >> frame.m_bBackground;
	ar >> frame.m_colorBackground;
};

void AFXAPI operator <<(CArchive& ar, const TextOutline& outline)
{
	ar << outline.m_width;
	ar << outline.m_color;
	ar << outline.m_bShadow;
	ar << outline.m_argbShadow;
};

void AFXAPI operator >>(CArchive& ar, TextOutline& outline)
{
	ar >> outline.m_width;
	ar >> outline.m_color;
	ar >> outline.m_bShadow;
	ar >> outline.m_argbShadow;
};

void AFXAPI operator <<(CArchive& ar, const TextDeform& texttype)
{
	ar << texttype.m_nLeadindex;
	ar << texttype.m_nWeight;
	ar << texttype.m_nHSkewAngle;
	ar << texttype.m_nVSkewAngle;
	ar << texttype.m_nHorzscale;
};

void AFXAPI operator >>(CArchive& ar, TextDeform& texttype)
{
	ar >> texttype.m_nLeadindex;
	ar >> texttype.m_nWeight;
	ar >> texttype.m_nHSkewAngle;
	ar >> texttype.m_nVSkewAngle;
	ar >> texttype.m_nHorzscale;
};
