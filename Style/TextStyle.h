#pragma once
#include <boost/compute/detail/sha1.hpp>

namespace boost {
	namespace 	json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class  BinaryStream;
struct TextFrame
{
	float m_widthBorder;
	Gdiplus::ARGB m_colorBorder;
	BOOL m_bRoundCorner;
	BOOL m_bBackground;
	Gdiplus::ARGB m_colorBackground;
	TextFrame();
	void Sha1(boost::uuids::detail::sha1& sha1) const;
	BOOL operator ==(const TextFrame& frame) const;
	BOOL operator !=(const TextFrame& frame) const;
	void ReleaseWeb(CFile& file) const;
	void ReleaseWeb(BinaryStream& stream) const;
	void ReleaseWeb(boost::json::object& json) const;
	void ReleaseWeb(pbf::writer& writer) const;
};

struct TextOutline
{
	float m_width;
	Gdiplus::ARGB m_color;
	BOOL m_bShadow;
	Gdiplus::ARGB m_argbShadow;
	TextOutline();
	void Sha1(boost::uuids::detail::sha1& sha1) const;
	BOOL operator ==(const TextOutline& outline) const;
	BOOL operator !=(const TextOutline& outline) const;
	void ReleaseWeb(CFile& file) const;
	void ReleaseWeb(BinaryStream& stream) const;
	void ReleaseWeb(boost::json::object& json) const;
	void ReleaseWeb(pbf::writer& writer) const;
};

struct TextDeform
{
	BYTE m_nLeterspace;
	BYTE m_nWordspace;
	unsigned short m_nWeight;
	BYTE m_nLeadindex;
	BYTE m_nHorzscale;
	BYTE m_nHSkewAngle;
	BYTE m_nVSkewAngle;

	TextDeform();
	void Sha1(boost::uuids::detail::sha1& sha1) const;
	BOOL operator ==(const TextDeform& type) const;
	BOOL operator !=(const TextDeform& type) const;
	void ReleaseWeb(CFile& file) const;
	void ReleaseWeb(BinaryStream& stream) const;
	void ReleaseWeb(boost::json::object& json) const;
	void ReleaseWeb(pbf::writer& writer) const;
};

void AFXAPI operator <<(CArchive& ar, const TextFrame& frame);
void AFXAPI operator >>(CArchive& ar, TextFrame& frame);

void AFXAPI operator <<(CArchive& ar, const TextOutline& outline);
void AFXAPI operator >>(CArchive& ar, TextOutline& outline);

void AFXAPI operator <<(CArchive& ar, const TextDeform& texttype);
void AFXAPI operator >>(CArchive& ar, TextDeform& texttype);
