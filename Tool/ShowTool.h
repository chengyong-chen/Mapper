#pragma once

class CTool;

class CShowTool : public CTool
{
public:
	virtual void OnLButtonDown(CWnd* pWnd,CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint) override;

public:
	virtual bool SetCursor(CWnd* pWnd,CViewinfo& viewinfo) override;
};