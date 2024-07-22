#pragma once
#include "../Public/Global.h"
#include <vector>

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace json {
		class object;
	}
};

class CColor;
class CViewinfo;
class CLibrary;
struct Context;

class AFX_EXT_CLASS CSphere
{
public:
	CSphere();
	~CSphere();
	
public:
	CColor* m_pColor1;
	CColor* m_pColor2;
	float m_fWidth;

private:
	CPointF m_keypoints[8];
	std::vector<CPointF> samples;

public:
	void Reform(const CViewinfo& viewinfo, float tolerance);
public:
	void Draw1(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CLibrary& library, Context& context);
	void Draw2(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CLibrary& library, Context& context);
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion);
	void ReleaseWeb(boost::json::object& json) const;
	void ReleaseWeb(pbf::writer& writer) const;
};

