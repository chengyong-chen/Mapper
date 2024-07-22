#pragma once

#include "Chart.h"

class CViewinfo;
class CPsboard;

class CBar : public CChart
{
public:
	CBar();
	~CBar() override;

public:
	float m_widthGap;
	float m_widthBar;
	int m_angleSkew;

public:
	void Draw(Gdiplus::Graphics& g, double fRatio, Gdiplus::Point point, std::map<CString, double>& elements) override;

public:
	Gdiplus::ARGB DefaultColor(std::map<CString, Gdiplus::ARGB>& exsiting) override;

	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 6; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;

public:
	void RecalculateLimit(CDatabase& database, CATTDatasource& datasource) override;
};
