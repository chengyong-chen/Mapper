#pragma once

class CTool;
class CGPSRouteDlg;
class CGPSRoute;

class __declspec(dllexport) CRouteTool : public CTool
{
public:
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
