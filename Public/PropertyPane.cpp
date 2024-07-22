#include "stdafx.h"

#include "PropertyPane.h"
#include "AttributeCtrl.h"
#include "RecordCtrl.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_FORMBAR_TAB  101

CPropertyPane::CPropertyPane()
{
}

BEGIN_MESSAGE_MAP(CPropertyPane, CDockablePane)
	//{{AFX_MSG_MAP(CPropertyPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(100000, OnComboBoxSelChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane message handlers

int CPropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	if(m_ComboBox.Create(dwViewStyle, rectDummy, this, 100000)==TRUE)
	{
		m_ComboBox.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
		m_ComboBox.EnableWindow(FALSE);
	}

	CRecordCtrl* pRecordCtrl = new CRecordCtrl();
	if(pRecordCtrl->Create(this)==TRUE)
	{
		pRecordCtrl->ModifyStyle(0, WS_TABSTOP);
		pRecordCtrl->ShowWindow(SW_HIDE);
		m_mapForm[_T("R")] = pRecordCtrl;
		const CString strName = GetTabLabel('R');
		const int index = m_ComboBox.AddString(strName);
		m_ComboBox.SetItemData(index, (DWORD)pRecordCtrl);
	}
	else
	{
		delete pRecordCtrl;
		pRecordCtrl = nullptr;
	}

	CAttributeCtrl* pAttributeCtrl = new CAttributeCtrl();
	if(pAttributeCtrl->Create(this)==TRUE)
	{
		pAttributeCtrl->ShowWindow(SW_HIDE);
		m_mapForm[_T("A")] = pAttributeCtrl;
		const CString strName = GetTabLabel('A');
		const int index = m_ComboBox.AddString(strName);
		m_ComboBox.SetItemData(index, (DWORD)pAttributeCtrl);
	}
	else
	{
		delete pAttributeCtrl;
		pAttributeCtrl = nullptr;
	}
	const HICON hPropertiesBarIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_PROPERTY), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	HICON hOld = CDockablePane::SetIcon(hPropertiesBarIcon, FALSE);
	CDockablePane::EnableToolTips(TRUE);
	CDockablePane::EnableDocking(CBRS_ALIGN_RIGHT);
	return 0;
}

void CPropertyPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd()!=nullptr)
	{
		CRect rectClient, rectCombo;
		GetClientRect(rectClient);

		if(m_ComboBox.m_hWnd!=nullptr)
		{
			m_ComboBox.GetWindowRect(&rectCombo);
			const int cyCmb = rectCombo.Size().cy;
			m_ComboBox.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE|SWP_NOZORDER);

			POSITION pos = m_mapForm.GetStartPosition();
			while(pos!=nullptr)
			{
				CString strKey;
				CWnd* pWnd = nullptr;

				m_mapForm.GetNextAssoc(pos, strKey, (void*&)pWnd);
				if(pWnd!=nullptr)
				{
					((CWnd*)pWnd)->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+1, rectClient.Width()-2, rectClient.Height()-cyCmb-2, SWP_NOACTIVATE|SWP_NOZORDER);
				}
			}
		}
	}
}

void CPropertyPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPropertyPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(m_ComboBox.m_hWnd!=nullptr)
	{
		const int index = m_ComboBox.GetCurSel();
		if(index!=-1)
		{
			CWnd* pWnd = (CWnd*)m_ComboBox.GetItemDataPtr(index);
			if(pWnd!=nullptr)
			{
				pWnd->SetFocus();
			}
		}
	}
}

void CPropertyPane::OnComboBoxSelChanged()
{
	const int indexSel = m_ComboBox.GetCurSel();
	for(int index = 0; index<m_ComboBox.GetCount(); index++)
	{
		void* pWnd = m_ComboBox.GetItemDataPtr(index);
		if(index==indexSel)
		{
			((CWnd*)pWnd)->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
		}
		else
		{
			((CWnd*)pWnd)->ShowWindow(SW_HIDE);
		}
	}
}

void CPropertyPane::ValidateTab(char cTab)
{
	CWnd* pWnd = this->GetTabWnd(cTab);
	if(pWnd==nullptr)
		return;
	const CString strName = GetTabLabel(cTab);
	const int index = m_ComboBox.FindString(-1, strName);
	if(index==-1)
	{
		m_ComboBox.EnableWindow(TRUE);
		const int index = m_ComboBox.InsertString(0, strName);
		pWnd->ShowWindow(SW_SHOW);
		m_ComboBox.SetItemData(index, (DWORD)pWnd);
	}

	if(m_ComboBox.GetCount()==1)
	{
		m_ComboBox.SetCurSel(0);
		this->OnComboBoxSelChanged();
		m_ComboBox.EnableWindow(FALSE);
	}
	else if(m_ComboBox.GetCount()>1)
	{
		m_ComboBox.EnableWindow(TRUE);
	}
}

void CPropertyPane::InvalidateTab(char cTab)
{
	if(m_ComboBox.m_hWnd == nullptr)
		return;

	const int nActive = m_ComboBox.GetCurSel();
	const CString strName = GetTabLabel(cTab);
	const int index = m_ComboBox.FindString(-1, strName);
	if(index!=-1)
	{
		CWnd* pWnd = (CWnd*)m_ComboBox.GetItemDataPtr(index);
		if(pWnd!=nullptr)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		m_ComboBox.DeleteString(index);
	}

	if(m_ComboBox.GetCount()==0)
	{
		m_ComboBox.EnableWindow(FALSE);
	}
	else if(index==nActive)
	{
		m_ComboBox.SetCurSel(0);
		this->OnComboBoxSelChanged();
	}
	if(m_ComboBox.GetCount()==1)
	{
		m_ComboBox.EnableWindow(FALSE);
	}
}

void CPropertyPane::ActivateTab(char cTab)
{
	const CString strName = GetTabLabel(cTab);
	const int index = m_ComboBox.FindString(-1, strName);
	if(index!=-1)
	{
		CWnd* pWnd = (CWnd*)m_ComboBox.GetItemDataPtr(index);
		if(pWnd!=nullptr)
		{
			m_ComboBox.SetCurSel(index);
			this->OnComboBoxSelChanged();
		}
	}
}

BOOL CPropertyPane::DestroyWindow()
{
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon!=nullptr)
	{
		DestroyIcon(hIcon);
	}
	POSITION pos = m_mapForm.GetStartPosition();
	while(pos!=nullptr)
	{
		CString strKey;
		CWnd* pWnd = nullptr;

		m_mapForm.GetNextAssoc(pos, strKey, (void*&)pWnd);
		if(pWnd!=nullptr)
		{
			pWnd->PostMessage(WM_DESTROY, 0, 0);
			pWnd->DestroyWindow();
			delete pWnd;
			pWnd = nullptr;
		}
	}
	m_mapForm.RemoveAll();

	m_ComboBox.ResetContent();

	return CDockablePane::DestroyWindow();
}

CWnd* CPropertyPane::GetTabWnd(char cTab) const
{
	CWnd* pWnd = nullptr;
	m_mapForm.Lookup(CString(cTab), (void*&)pWnd);

	return pWnd;
}

CString CPropertyPane::GetTabLabel(char cTab)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString str;
	switch(cTab)
	{
	case 'N':
		str.LoadString(IDS_FORMBAR_NODEFORM);
		break;
	case 'E':
		str.LoadString(IDS_FORMBAR_EDGEFORM);
		break;
	case 'A':
		str.LoadString(IDS_FORMBAR_ATTRIBUTE);
		break;
	case 'R':
		str.LoadString(IDS_FORMBAR_RECORD);
		break;
	default:
		break;
	}
	return str;
}
