#pragma once

#include "Laydb.h"

class AFX_EXT_CLASS CLaypt : public CLaydb
{
public:
	CLaypt(CTree<CLayer>& tree);
	CLaypt(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);

	~CLaypt() override;

public:
	BYTE Gettype() const override { return 6; };

public:
	void Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect) override;
};
