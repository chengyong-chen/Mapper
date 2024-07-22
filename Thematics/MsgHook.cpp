#include "stdafx.h"
#include "MsgHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HHOOK CMsgHook::m_hHook = nullptr;
int CMsgHook::m_nInstances = 0;
CMap<HWND, HWND&, CMsgHook*, CMsgHook*&> CMsgHook::m_mapHookedWindows; // map of windows to hooks

CMsgHook::CMsgHook(void)
	: m_bDropDown(false)
	, m_hWnd(nullptr)
	, m_hParent(nullptr)
	, m_hNotify(nullptr)
{
	if(m_nInstances==0)
	{
		m_hHook = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, nullptr, GetCurrentThreadId());
		ASSERT(m_hHook);
	}
	m_nInstances++;
}

CMsgHook::~CMsgHook(void)
{
	m_nInstances--;
	if(m_nInstances==0)
	{
		UnhookWindowsHookEx(m_hHook);
	}
}

//LRESULT CALLBACK CMsgHook::KeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
//{
//	MSG msg;
//	PeekMessage(&msg, nullptr, 0, 0, 0);
//	if(msg.message == WM_KEYDOWN && (wParam == VK_ESCAPE || wParam == VK_RETURN))
//	{
//		CMsgHook *pHook = nullptr;
//		HWND hWnd = (HWND)msg.hwnd;
//		while(hWnd)
//		{
//			if(m_mapHookedWindows.Lookup(hWnd, pHook))
//			{
//				SendMessage(pHook->m_hNotify ? pHook->m_hNotify : pHook->m_hParent, UM_HIDEEDITOR, wParam == VK_RETURN, 0);
//				break;
//			}
//			hWnd = GetParent(hWnd);
//		}
//	}
//	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
//}
LRESULT CALLBACK CMsgHook::CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPRETSTRUCT* pMsg = (CWPRETSTRUCT*)lParam;
	if(pMsg)
	{
		CMsgHook* pHook = nullptr;

		switch(pMsg->message)
		{
		case WM_KILLFOCUS:
		{
			HWND hWnd = (HWND)pMsg->hwnd;
			while(hWnd)
			{
				if(m_mapHookedWindows.Lookup(hWnd, pHook))
				{
					if(!pHook->m_bDropDown)
					{
						hWnd = (HWND)pMsg->wParam;
						while(hWnd&&hWnd!=pHook->m_hWnd)
						{
							hWnd = GetParent(hWnd);
						}
						if(!hWnd)
						{
							SendMessage(pHook->m_hNotify ? pHook->m_hNotify : pHook->m_hParent, UM_HIDEEDITOR, TRUE, 0);
							break;
						}
					}
					break;
				}
				hWnd = GetParent(hWnd);
			}
		}
		break;
		case WM_NOTIFY:
		{
			const LPNMHDR pnmh = (LPNMHDR)pMsg->lParam;
			if(pnmh->code==DTN_DROPDOWN||pnmh->code==DTN_CLOSEUP)
			{
				HWND hWnd = (HWND)pnmh->hwndFrom;
				while(hWnd)
				{
					if(m_mapHookedWindows.Lookup(hWnd, pHook))
					{
						pHook->m_bDropDown = (DTN_DROPDOWN==pnmh->code);
						break;
					}
					hWnd = GetParent(hWnd);
				}
			}
		}
		break;
		case WM_COMMAND:
		{
			const WORD code = HIWORD(pMsg->wParam);
			if(code==CBN_DROPDOWN||code==CBN_CLOSEUP)
			{
				HWND hWnd = (HWND)pMsg->lParam;
				while(hWnd)
				{
					if(m_mapHookedWindows.Lookup(hWnd, pHook))
					{
						pHook->m_bDropDown = (CBN_DROPDOWN==code);
						break;
					}
					hWnd = GetParent(hWnd);
				}
			}
		}
		break;
		case WM_KEYDOWN:
		{
			if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN)
			{
				HWND hWnd = (HWND)pMsg->hwnd;
				while(hWnd)
				{
					if(m_mapHookedWindows.Lookup(hWnd, pHook))
					{
						SendMessage(pHook->m_hNotify ? pHook->m_hNotify : pHook->m_hParent, UM_HIDEEDITOR, pMsg->wParam==VK_RETURN, 0);
						break;
					}
					hWnd = GetParent(hWnd);
				}
			}
		}
		break;
		case WM_DESTROY:
		{
			if(m_mapHookedWindows.Lookup(pMsg->hwnd, pHook))
			{
				pHook->Detach();
			}
		}
		break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_MENUSELECT:
		case WM_MOUSEWHEEL:
		{
			HWND hWnd = (HWND)pMsg->hwnd;
			while(hWnd&&!m_mapHookedWindows.Lookup(hWnd, pHook))
			{
				hWnd = GetParent(hWnd);
			}
			if(!pHook)
			{
				POSITION pos = m_mapHookedWindows.GetStartPosition();
				HWND hWnd;
				while(pos!=nullptr)
				{
					m_mapHookedWindows.GetNextAssoc(pos, hWnd, pHook);

					if(IsWindow(hWnd)&&!pHook->m_bDropDown)
					{
						const bool dropDown = SendMessage(hWnd, CB_GETDROPPEDSTATE, 0, 0)||DateTime_GetMonthCal(hWnd);
						if(!dropDown) SendMessage(pHook->m_hNotify ? pHook->m_hNotify : pHook->m_hParent, UM_HIDEEDITOR, TRUE, 0);
					}
				}
			}
		}
		break;
		}
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

BOOL CMsgHook::Attach(HWND hWnd, HWND hParent, HWND hNotify)
{
	if(IsWindow(hWnd)&&IsWindow(hParent))
	{
		Detach();
		m_hWnd = hWnd;
		m_hParent = hParent;
		m_hNotify = hNotify;
		m_mapHookedWindows[hWnd] = this;
		m_bDropDown = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CMsgHook::Detach(void)
{
	if(m_mapHookedWindows.RemoveKey(m_hWnd))
	{
		m_hWnd = nullptr;
		m_hParent = nullptr;
		m_hNotify = nullptr;
		m_bDropDown = FALSE;
		return TRUE;
	}
	return FALSE;
}
