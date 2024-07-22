#pragma once
#define UM_HIDEEDITOR	(WM_USER+10)

class CMsgHook
{
public:
	CMsgHook(void);
	virtual ~CMsgHook(void);

	BOOL Attach(HWND hWnd, HWND hParent, HWND hNotify = nullptr);
	BOOL Detach(void);
	BOOL IsAttached() const
	{
		return m_hWnd!=nullptr;
	}

protected:
	static LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam);

	static HHOOK m_hHook;
	static int m_nInstances;
	static CMap<HWND, HWND&, CMsgHook*, CMsgHook*&> m_mapHookedWindows; // map of windows to hooks

	HWND m_hWnd;
	HWND m_hParent;
	HWND m_hNotify;
	BOOL m_bDropDown;
};
