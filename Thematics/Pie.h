#pragma once

#include "Chart.h"

using namespace std;
#include <map>

class CViewinfo;

class CPie : public CChart
{
public:
	CPie();
	~CPie() override;

public:
	int m_angleStarting;

public:
	void Draw(Gdiplus::Graphics& g, double fRatio, Gdiplus::Point point, std::map<CString, double>& elements) override;

private:
	void DrawTop(Gdiplus::Graphics& g, Gdiplus::Rect& rect, double& total, std::map<CString, double>& values);

	static void Draw3DArc(Gdiplus::Graphics& g, Gdiplus::Brush& brush, Gdiplus::Rect& rect1, Gdiplus::Rect& rect2, double start, double angle);

public:
	void RecalculateLimit(CDatabase& database, CATTDatasource& datasource) override;

public:
	Gdiplus::ARGB DefaultColor(std::map<CString, Gdiplus::ARGB>& exsiting) override;

	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 5; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
