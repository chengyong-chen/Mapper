#pragma once

#include "Tool.h"

class __declspec(dllexport) CInfoTool : public CTool
{
public:
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
