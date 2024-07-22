#pragma once




#include "Sphere.h"
#include "Graticule.h"

class CViewinfo;
class AFX_EXT_CLASS CFrame
{
public:
	CFrame();
	~CFrame();

public:
	BOOL m_bEnabledSphere;
	BOOL m_bEnabledGraticule;

public:
	CSphere m_sphere;
	CGraticule m_graticule;

public:
	float PrevPrecision = 0;

public:
	void Draw1(Gdiplus::Graphics& g, const CViewinfo& viewinfo);
	void Draw2(Gdiplus::Graphics& g, const CViewinfo& viewinfo);
	void Serialize(CArchive& ar, const DWORD& dwVersion);
	void ReleaseWeb(pbf::writer& writer, pbf::tag tag) const;
};
