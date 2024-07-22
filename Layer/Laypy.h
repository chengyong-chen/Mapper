#pragma once

#include "Laydb.h"

class AFX_EXT_CLASS CLaypy : public CLaydb
{
public:
	CLaypy(CTree<CLayer>& tree);
	CLaypy(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);

	~CLaypy() override;

public:
	BYTE Gettype() const override { return 7; };

public:
	void Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect) override;
};
