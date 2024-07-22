#pragma once

class CTool;

class __declspec(dllexport) CSelectTool : public CTool
{
public:
	enum ActionMode
	{
		Wandering,
		NetSelect,
		MoveGeoms,
		MoveAnchor,
		ChangeCtrl
	};

public:
	CSelectTool();
	~CSelectTool();

private:
	void Dispose();

public:
	// Implementation
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint) override;

public:
	void Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC) override;
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
	void OnShiftDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nRepCnt, UINT nFlags) override;
	void OnShiftUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nRepCnt, UINT nFlags) override;

	ActionMode m_actMode;
	bool m_actReady;
	Gdiplus::Bitmap* m_captureImage;
	CPoint m_coveredOrigin;
	HBITMAP m_coveredImage;
	CPoint m_captureOrigin;

public:
	static UINT ActOKProc(LPVOID lpVoid);

public:
	struct WaitForReadyParas
	{
		CWnd* pWnd;
		CSelectTool& selecttool;
		const CViewinfo& viewinfo;
		time_t downtime;

		WaitForReadyParas(CWnd* wnd, CSelectTool& tool, const CViewinfo& info, time_t time)
			: pWnd(wnd), selecttool(tool), viewinfo(info), downtime(time)
		{
		}
	};
};
