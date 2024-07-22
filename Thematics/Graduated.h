#pragma once

#include "Theme.h"


using namespace std;
class CViewinfo;

class CGraduated : public CTheme
{
public:
	CGraduated();

	~CGraduated() override;

public:
	BYTE m_symbol;
	COLORREF m_colorBorder;
	float m_widthBorder;
	COLORREF m_colorFill;

public:
	void Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, std::map<DWORD, CGeom*>& list) override;
	virtual void Draw(Gdiplus::Graphics& g, double fRatio, Gdiplus::Point point, double& value);
public:
	void RecalculateLimit(CDatabase& database, CATTDatasource& datasource) override;

public:
	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 2; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;

public:
	static bool ComparePair(std::pair<Gdiplus::Point, double>& pair1, std::pair<Gdiplus::Point, double>& pair2);
};
