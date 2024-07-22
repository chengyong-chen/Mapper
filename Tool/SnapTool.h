#pragma once

class CTool;

class __declspec(dllexport) CSnapTool : public CTool
{
public:
	virtual void OnLButtonUp(CWnd* pWnd,CViewinfo& viewinfo,UINT nFlags, const CPoint& docPoint) override;
};
