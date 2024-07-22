#include "stdafx.h"

#include "VehiclePane.h"

#include "../Coding/Coder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CVehiclePane::CVehiclePane()
{
	m_pCoder = nullptr;
	m_dwVehicle = -1;
	m_dwStatus = 0;

	m_pLED = nullptr;
	m_pTitle = nullptr;
	m_nCount = 0;
}

BEGIN_MESSAGE_MAP(CVehiclePane, CDockablePane)
	//{{AFX_MSG_MAP(CVehiclePane)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVehiclePane message handlers
void CVehiclePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd()!=nullptr)
	{
	}
}

void CVehiclePane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CVehiclePane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
}

BOOL CVehiclePane::DestroyWindow()
{
	if(m_pLED!=nullptr)
	{
		for(int index = 0; index<m_nCount; index++)
		{
			m_pLED[index].PostMessage(WM_DESTROY, 0, 0);
			m_pLED[index].DestroyWindow();
		}

		delete[] m_pLED;
		m_pLED = nullptr;
	}

	if(m_pTitle!=nullptr)
	{
		for(int index = 0; index<m_nCount; index++)
		{
			m_pTitle[index].PostMessage(WM_DESTROY, 0, 0);
			m_pTitle[index].DestroyWindow();
		}
		delete[] m_pTitle;
		m_pTitle = nullptr;
	}

	m_nCount = 0;

	return CDockablePane::DestroyWindow();
}

void CVehiclePane::SetVehicle(DWORD dwVehicle)
{
	m_dwVehicle = dwVehicle;
}

void CVehiclePane::ShowStatus(CCoder* pCoder, DWORD dwVehicle, DWORD dwStatus)
{
	if(pCoder==m_pCoder&&dwVehicle==m_dwVehicle&&dwStatus==m_dwStatus)
		return;

	if(pCoder!=m_pCoder)
	{
		ChangeIVU(pCoder);
		SetStatus(dwStatus);
	}
	else if(dwStatus!=m_dwStatus)
	{
		SetStatus(dwStatus);
	}

	m_dwVehicle = dwVehicle;
}

void CVehiclePane::ChangeIVU(CCoder* pCoder)
{
	if(pCoder==m_pCoder)
		return;

	if(m_pLED!=nullptr)
	{
		for(int index = 0; index<m_nCount; index++)
		{
			m_pLED[index].ShowWindow(SW_HIDE);
			m_pLED[index].PostMessage(WM_DESTROY, 0, 0);
			m_pLED[index].DestroyWindow();
		}
		delete[] m_pLED;
		m_pLED = nullptr;
	}

	if(m_pTitle!=nullptr)
	{
		for(int index = 0; index<m_nCount; index++)
		{
			m_pTitle[index].PostMessage(WM_DESTROY, 0, 0);
			m_pTitle[index].DestroyWindow();
		}
		delete[] m_pTitle;
		m_pTitle = nullptr;
	}

	m_nCount = 0;

	if(pCoder==nullptr)
	{
		m_pCoder = nullptr;
	}
	else if(pCoder->m_nStatus>0)
	{
		m_pLED = new CLED[pCoder->m_nStatus];
		m_pTitle = new CStatic[pCoder->m_nStatus];
		m_nCount = pCoder->m_nStatus;

		for(int index = 0; index<pCoder->m_nStatus; index++)
		{
			if(m_pTitle[index].Create(pCoder->m_pStatus[index].strMeaning, WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 54, 13), this)==TRUE)
			{
				m_pTitle[index].SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
				const int x = index%2==0 ? 4 : 89;
				const int y = 10+(index/2)*29;
				m_pTitle[index].SetWindowPos(&wndTop, x, y, 0, 0, SWP_NOSIZE);
				m_pTitle[index].ShowWindow(SW_SHOW);
			}

			if(m_pLED[index].Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP, CRect(0, 0, 15, 15), this)==TRUE)
			{
				const int x = index%2==0 ? 58 : 143;
				const int y = 9+(index/2)*29;
				m_pLED[index].SetWindowPos(&wndTop, x, y, 0, 0, SWP_NOSIZE);
				m_pLED[index].ShowWindow(SW_SHOW);
			}
		}

		m_pCoder = pCoder;
	}
}

void CVehiclePane::SetStatus(DWORD dwStatus)
{
	if(m_pCoder==nullptr)
		return;

	for(int index = 0; index<m_pCoder->m_nStatus; index++)
	{
		if(index>=m_nCount)
			break;

		if((dwStatus&m_pCoder->m_pStatus[index].dwMasker)==m_pCoder->m_pStatus[index].dwMasker)
		{
			switch(m_pCoder->m_pStatus[index].nAbnomal)
			{
			case 0:
				m_pLED[index].SetMode(0);
				break;
			case 1:
				m_pLED[index].SetMode(1);
				break;
			case 2:
				m_pLED[index].SetMode(2);
				break;
			}
		}
		else
		{
			m_pLED[index].SetMode(-1);
		}
	}

	m_dwStatus = dwStatus;
}
