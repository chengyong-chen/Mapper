#pragma once


#include "../Projection/Geocentric.h"
class CViewMonitor;


class __declspec(dllexport) CBackground
{
public:
	CBackground(CViewMonitor& foreground);
	virtual ~CBackground();

public:
	DWORD m_dwThreadId;
	DWORD m_dwProcessId;
	HANDLE m_hReadPipe;
	HANDLE m_hWritePipe;
	bool m_bThreadAlive;

public:
	CStringA m_strProvider;
	CStringA m_strVariety;
	CViewMonitor& m_ViewMonitor;

public:
	CGeocentric m_geocentric;
	CPointF m_mapOrigin;
	CSizeF m_mapCanvas;

	CGeocentric* m_bufGeocentric;
	CPointF m_bufmapOrigin;
	CSizeF m_bufmapCanvas;

public:
	static HANDLE m_eventGiscakeReady;
	static HANDLE m_eventMapLoaded;
	static HANDLE m_eventVarietyChanged;
	static HANDLE m_eventMapViewChanged;

public:
	BOOL Create(void* point, void (WINAPI* callback)(void* point, CStringA strCommand, CStringA strArgument), CRect rect);
	BOOL ShowWindow(int nCmdShow) const;
	BOOL SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) const;
	BOOL SendMessageToPipe(CStringA strMessage) const;

	BOOL OpenBack(HWND hWnd, CStringA strProvider, CStringA strVariety);
	BOOL ZoomBy(const CPoint& point, float fRatio) const;
	BOOL LevelTo(float fLevel, const CPointF& mapPoint) const;
	BOOL ScaleTo(float fScale, const CPointF& geoPoint) const;

	BOOL MoveBy(int cx, int cy) const;
	BOOL JumpTo(double geoX, double geoY) const;
	void Alphait(int alpha) const;

private:
	void* m_pPoint;
	void (WINAPI* funcCallBack)(void* point, CStringA strCommand, CStringA strArgument);
	static UINT ReadFromPipe(LPVOID lpVoid);
};
