#include "stdafx.h"
#include "resource.h"
#include "Global.h"

#include "Tool.h"
#include "LinkTool.h"

#include "../HyperLink/Link.h"

//#include "vfw.h"
//#include "mmsystem.h"

__declspec(dllexport) CLinkTool linkTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkTool::CLinkTool()
{
	m_pLink = nullptr;
}

void CLinkTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	m_pLink = (CLink*)pWnd->SendMessage(WM_PICKLINK, (LONG)&docPoint, nFlags);
}

void CLinkTool::OnTimerMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
	{
		if(m_pLink!=nullptr)
		{
			const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_STALEMATE);
			if(hCursor!=nullptr)
			{
				::SetCursor(hCursor);
			}
		}
	}
	else
	{
		CLink* link = (CLink*)pWnd->SendMessage(WM_PICKLINK, (LONG)&cliPoint, nFlags);
		if(link!=nullptr&&m_pLink!=link)
		{
			const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_HOT);
			if(hCursor!=nullptr)
			{
				::SetCursor(hCursor);
			}

			/*
					MCI_OPEN_PARMS    mciOpenParms;
					MCI_PLAY_PARMS    mciPlayParms;
					MCI_STATUS_PARMS  mciStatusParms;

					mciSendCommand(1, MCI_CLOSE, 0, nullptr);

					mciOpenParms.lpstrDeviceType  = _T("waveaudio");
					mciOpenParms.lpstrElementName = _T("Start.wav");

					mciSendCommand(nullptr, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,(DWORD)(LPVOID) &mciOpenParms);
					DWORD wDeviceID = mciOpenParms.wDeviceID;

					mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;
					mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms);
					mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY,  (DWORD)(LPVOID) &mciPlayParms);*/
		}

		if(link==nullptr&&m_pLink!=nullptr)
		{
			HCURSOR hCursor = nullptr;
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

			if(hCursor!=nullptr)
			{
				::SetCursor(hCursor);
			}
		}

		m_pLink = link;
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, cliPoint);
}

void CLinkTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	HCURSOR hCursor = nullptr;
	hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
	}

	if(m_pLink!=nullptr)
	{
		CWnd* pParents = pWnd->GetParentFrame();
		m_pLink->Into(pParents);
		m_pLink = nullptr;
	}
}

bool CLinkTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pLink!=nullptr)
	{
		const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_HOT);
		if(hCursor!=nullptr)
		{
			::SetCursor(hCursor);
			return true;
		}
	}
	else
	{
		const HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

		if(hCursor!=nullptr)
		{
			::SetCursor(hCursor);
			return true;
		}
	}

	return false;
}
