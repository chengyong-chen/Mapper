#pragma once

#include "Theme.h"

using namespace std;
class CViewinfo;

class CChoropleth : public CTheme
{
public:
	CChoropleth();

	~CChoropleth() override;

public:
	virtual void Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, Gdiplus::Pen* pen, std::map<DWORD, CGeom*>& list);
	//virtual void Draw(Gdiplus::Graphics& g,const double fRatio,Point point,double& value) override;

public:
	void RecalculateLimit(CDatabase& database, CATTDatasource& datasource) override;
public:
	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 1; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
