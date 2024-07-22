#include "stdafx.h"
#include "Global.h"
#include "BackgroundBar.h"

using namespace std;
using namespace tinyxml2;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CBackgroundBar::CBackgroundBar(CWnd* pParent)
	: CDialog(CBackgroundBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackgroundBar)
	//}}AFX_DATA_INIT
}

CBackgroundBar::~CBackgroundBar()
{
}

void CBackgroundBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackgroundBar)
	DDX_Control(pDX, IDC_PROVIDER, m_ComboProvider);
	DDX_Control(pDX, IDC_VARIETY, m_ComboVariety);
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_FORETRANSPARENCY, m_ForeTransparency);
	DDX_Control(pDX, IDC_BACKTRANSPARENCY, m_BackTransparency);
}

BEGIN_MESSAGE_MAP(CBackgroundBar, CDialog)
	//{{AFX_MSG_MAP(CBackgroundBar)
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_PROVIDER, OnProviderSelChanged)
	ON_CBN_SELCHANGE(IDC_VARIETY, OnVarietySelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CBackgroundBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strName;
	strName.LoadString(IDS_NONE);
	m_ComboProvider.AddString(strName);

	char buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileNameA(nullptr, buff, sizeof(buff));
	CStringA strPath(buff);
	strPath=strPath.Left(strPath.ReverseFind('\\'));
	if(m_xml.LoadFile(strPath + "\\Assistant\\popunder.xml") == tinyxml2::XMLError::XML_SUCCESS || m_xml.LoadFile(strPath + "\\Webmap.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pElem=m_xml.FirstChildElement();
		if(pElem != nullptr)
		{
			tinyxml2::XMLElement* pProvider=pElem->FirstChildElement("background")->FirstChildElement("provider");
			while(pProvider != nullptr)
			{
				const char* pName=pProvider->Attribute("name");
				m_ComboProvider.AddString(CString(pName));
				pProvider=pProvider->NextSiblingElement();
			}
		}
		m_ForeTransparency.SetRangeMin(100);
		m_ForeTransparency.SetRangeMax(255);
		m_ForeTransparency.SetPos(255);

		m_BackTransparency.SetRangeMin(50);
		m_BackTransparency.SetRangeMax(255);
		m_BackTransparency.SetPos(255);
	}
	else
	{

	}

	return TRUE; // return TRUE unless you set the focus to a control
}

void CBackgroundBar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(m_ForeTransparency.m_hWnd != nullptr)
	{
		CRect rect;
		m_ForeTransparency.GetWindowRect(rect);
		this->ScreenToClient(rect);
		m_ForeTransparency.SetWindowPos(&wndTop, 0, 0, cx - rect.left - 5, rect.Height(), SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}
	if(m_BackTransparency.m_hWnd != nullptr)
	{
		CRect rect;
		m_BackTransparency.GetWindowRect(rect);
		this->ScreenToClient(rect);
		m_BackTransparency.SetWindowPos(&wndTop, 0, 0, cx - rect.left - 5, rect.Height(), SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}
	if(m_ComboProvider.m_hWnd != nullptr)
	{
		CRect rect;
		m_ComboProvider.GetWindowRect(rect);
		this->ScreenToClient(rect);
		m_ComboProvider.SetWindowPos(&wndTop, 0, 0, cx - rect.left - 5, rect.Height(), SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}
	if(m_ComboVariety.m_hWnd != nullptr)
	{
		CRect rect;
		m_ComboVariety.GetWindowRect(rect);
		this->ScreenToClient(rect);
		m_ComboVariety.SetWindowPos(&wndTop, 0, 0, cx - rect.left - 5, rect.Height(), SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}
}

void CBackgroundBar::OnOK()
{
}

void CBackgroundBar::OnProviderSelChanged()
{
	CString strOldVariety;
	const int nVariety=m_ComboVariety.GetCurSel();
	if(nVariety != -1)
		m_ComboVariety.GetLBText(nVariety, strOldVariety);

	m_ComboVariety.ResetContent();
	const int nProvider=m_ComboProvider.GetCurSel();
	if(nProvider == 0)
	{
		m_ComboVariety.EnableWindow(FALSE);
		ChangeBackground(-1, -1);
	}
	else
	{
		CString strProvier;
		m_ComboProvider.GetLBText(nProvider, strProvier);

		tinyxml2::XMLElement* pElem=m_xml.FirstChildElement();
		if(pElem != nullptr)
		{
			tinyxml2::XMLElement* pProvider=pElem->FirstChildElement("background")->FirstChildElement("provider");
			while(pProvider != nullptr)
			{
				if(pProvider->Attribute("name") == strProvier)
				{
					tinyxml2::XMLElement* pVariety=pProvider->FirstChildElement("varieties")->FirstChildElement("variety");
					while(pVariety != nullptr)
					{
						m_ComboVariety.AddString(CString(pVariety->Attribute("name")));
						pVariety=pVariety->NextSiblingElement();
					}
					break;
				}

				pProvider=pProvider->NextSiblingElement();
			}

			if(strOldVariety != "" && m_ComboVariety.FindString(0, strOldVariety) != -1)
				m_ComboVariety.SelectString(-1, strOldVariety);
			else
				m_ComboVariety.SetCurSel(0);
			OnVarietySelChanged();
		}

		m_ComboVariety.EnableWindow(TRUE);
	}
}

void CBackgroundBar::OnVarietySelChanged()
{
	const int index1=m_ComboProvider.GetCurSel();
	const int index2=m_ComboVariety.GetCurSel();
	ChangeBackground(index1, index2);
}

void CBackgroundBar::ChangeBackground(int nProvider, int nVariety) const
{
	CFrameWnd* pAppFrame=(CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame == nullptr)
		return;
	CMDIChildWnd* pCldFrame=(CMDIChildWnd*)pAppFrame->GetActiveFrame();
	CView* pView=(CView*)pCldFrame->GetActiveView();
	if(pView == nullptr)
		return;

	CStringW strwProvider;
	if(nProvider > 0)
		m_ComboProvider.GetLBText(nProvider, strwProvider);
	else
		strwProvider=_T("");

	CStringW strwVariety;
	if(nVariety != -1)
		m_ComboVariety.GetLBText(nVariety, strwVariety);
	else
		strwVariety=_T("");

	static CStringA straProvider;
	static CStringA straVariety;
	straProvider=CStringA(strwProvider);
	straVariety=CStringA(strwVariety);
	pView->SendMessage(WM_CHANGEBACKGROUND, (UINT)&straProvider, (UINT)&straVariety);
}

void CBackgroundBar::Disable()
{
	m_ComboVariety.ResetContent();
	m_ComboVariety.EnableWindow(FALSE);
	m_ComboProvider.SetCurSel(0);
	m_ComboProvider.EnableWindow(FALSE);
	m_ForeTransparency.EnableWindow(FALSE);
	m_BackTransparency.EnableWindow(FALSE);
}

void CBackgroundBar::Enable(CStringA strProvider, CStringA strVariety)
{
	m_ComboProvider.EnableWindow(TRUE);
	m_ComboVariety.EnableWindow(TRUE);

	CString strProvider1;
	const int nProvider=m_ComboProvider.GetCurSel();
	if(nProvider != -1)
		m_ComboProvider.GetLBText(nProvider, strProvider1);
	if(strProvider == "")
	{
		m_ComboVariety.ResetContent();
		m_ComboProvider.SetCurSel(0);
		m_ComboVariety.EnableWindow(FALSE);
	}
	else if(CStringA(strProvider1) != strProvider)
	{
		m_ComboProvider.SelectString(-1, CStringW(strProvider));

		m_ComboVariety.ResetContent();
		tinyxml2::XMLElement* pElem=m_xml.FirstChildElement();
		if(pElem != nullptr)
		{
			tinyxml2::XMLElement* pProvider=pElem->FirstChildElement("background")->FirstChildElement("provider");
			while(pProvider != nullptr)
			{
				if(pProvider->Attribute("name") == strProvider)
				{
					tinyxml2::XMLElement* pVariety=pProvider->FirstChildElement("varieties")->FirstChildElement("variety");
					while(pVariety != nullptr)
					{
						m_ComboVariety.AddString(CString(pVariety->Attribute("name")));
						pVariety=pVariety->NextSiblingElement();
					}
					break;
				}

				pProvider=pProvider->NextSiblingElement();
			}

			m_ComboVariety.SelectString(-1, CStringW(strVariety));
		}
	}
	else
	{
		m_ComboVariety.SelectString(-1, CStringW(strVariety));
	}

	m_ForeTransparency.EnableWindow(TRUE);
	m_BackTransparency.EnableWindow(TRUE);
}

CStringA CBackgroundBar::GetProvider() const
{
	if(m_ComboProvider.IsWindowEnabled() == FALSE)
		return "";
	const int nItem=m_ComboProvider.GetCurSel();
	if(nItem < 1)
		return "";
	else
	{
		static CString strProvider;
		m_ComboProvider.GetLBText(nItem, strProvider);
		return CStringA(strProvider);
	}
}

CStringA CBackgroundBar::GetVariety() const
{
	if(m_ComboVariety.IsWindowEnabled() == FALSE)
		return "";
	const int nItem=m_ComboVariety.GetCurSel();
	if(nItem < 0)
		return "";
	else
	{
		static CString strVariety;
		m_ComboVariety.GetLBText(nItem, strVariety);
		return CStringA(strVariety);
	}
}

int CBackgroundBar::GetFroegroundTransparency() const
{
	return m_ForeTransparency.GetPos() % 256;
}

int CBackgroundBar::GetBackgroundTransparency() const
{
	return m_BackTransparency.GetPos() % 256;
}

void CBackgroundBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar->m_hWnd == m_ForeTransparency.m_hWnd)
	{
		const HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();
		if(hWnd != nullptr)
		{
			const LONG nStyle=::GetWindowLong(hWnd, GWL_EXSTYLE);
			if((nStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
			{
				const int nAlpha=m_ForeTransparency.GetPos() % 256;
				::SetLayeredWindowAttributes(hWnd, RGB(255, 255, 254), nAlpha, LWA_COLORKEY | LWA_ALPHA);
			}
		}
	}
	else if(pScrollBar->m_hWnd == m_BackTransparency.m_hWnd)
	{
		CFrameWnd* pAppFrame=(CFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pCldFrame=(CMDIChildWnd*)pAppFrame->GetActiveFrame();
		CView* pView=(CView*)pCldFrame->GetActiveView();
		if(pView == nullptr)
			return;
		const int nAlpha=m_BackTransparency.GetPos() % 256;
		pView->SendMessage(WM_ALPHATRANSPARENCY, 2, nAlpha);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

tinyxml2::XMLElement* CBackgroundBar::FindNode(tinyxml2::XMLElement* xml, const char* tag)
{
	if(xml == nullptr)
		return nullptr;

	const auto trying1=xml->FirstChildElement(tag);
	if(trying1 != nullptr)
		return trying1;

	for(tinyxml2::XMLElement* child=xml->FirstChildElement(); child != 0; child=child->NextSiblingElement())
	{
		const auto trying2=CBackgroundBar::FindNode(child, tag);
		if(trying2 != nullptr)
			return trying2;
	}

	return nullptr;
}