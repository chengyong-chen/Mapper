#pragma once
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CColor;
class CViewinfo;
class CLibrary;
struct Context;

class AFX_EXT_CLASS CGraticule
{
public:
	CGraticule();
	~CGraticule();

public:	
	unsigned short m_lngInterval;
	unsigned short m_latInterval;

public:
	CColor* m_pColor;
	float m_fWidth;

public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context);
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion);
	void ReleaseWeb(boost::json::object& json) const;
	void ReleaseWeb(pbf::writer& writer) const;
};