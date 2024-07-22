#pragma once

#include "Graduated.h"

using namespace std;
class CViewinfo;

class CCylinder : public CGraduated
{
public:
	CCylinder();

	~CCylinder() override;

public:
	int m_diameter;

public:
	void Draw(Gdiplus::Graphics& g, double fRatio, Gdiplus::Point point, double& value) override;
public:
	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 7; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
