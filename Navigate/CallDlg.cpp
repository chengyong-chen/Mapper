#include "stdafx.h"
#include "CallDlg.h"
#include  <stdlib.h>
#include <afxdb.h>

#include "Port.h"
#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCallDlg dialog

CCallDlg::CCallDlg(CWnd* pParent, CPort* pPort)
	: CDialog(CCallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCallDlg)
	//}}AFX_DATA_INIT

	m_pPort = pPort;
}

void CCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCallDlg, CDialog)
	//{{AFX_MSG_MAP(CCallDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ONCERADIO, OnClickOnceRadio)
	ON_BN_CLICKED(IDC_DURATIVERADIO, OnClickDurativeRadio)
	ON_BN_CLICKED(IDC_NOINTERVAL, OnClickNointerval)
	ON_BN_CLICKED(IDC_LIMITLESS, OnClickLimitless)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCallDlg message handlers
BOOL CCallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_SelectDlg.m_pDatabase = &(m_pPort->m_database);
	m_SelectDlg.m_dwUser = m_pPort->m_dwUser;
	if(m_SelectDlg.Create(IDD_SELECT, this)==TRUE)
	{
		m_SelectDlg.SetWindowPos(&wndTop, 3, 3, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		m_SelectDlg.OnVehicle();

		((CButton*)GetDlgItem(IDC_DURATIVERADIO))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_NOINTERVAL))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_LIMITLESS))->SetCheck(TRUE);
		OnClickOnceRadio();

		CenterWindow();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCallDlg::OnOk()
{
	UpdateData(TRUE);

	if(m_pPort==nullptr)
		return;

	/*
		if(((CButton*)GetDlgItem(IDC_DURATIVERADIO))->GetCheck() == BST_CHECKED)
		{
			command.m_wCommand = CCoder::VHC_CMD_RESPOND_CONTINUE;

			CString str;
			GetDlgItem(IDC_INTERVAL)->GetWindowText(str);
			command.dwParam[0] = _ttoi(str);
			GetDlgItem(IDC_TIMES)->GetWindowText(str);
			command.dwParam[1] = _ttoi(str);
		}
		else
		{
			command.m_wCommand = CCoder::VHC_CMD_RESPOND_ONCE;
		}
	*/
	if(((CButton*)m_SelectDlg.GetDlgItem(IDC_VEHICLE))->GetCheck()==BST_CHECKED)
	{
		POSITION pos = m_SelectDlg.m_listctrl.GetFirstSelectedItemPosition();
		while(pos!=nullptr)
		{
			const int nItem = m_SelectDlg.m_listctrl.GetNextSelectedItem(pos);
			const DWORD dwId = m_SelectDlg.m_listctrl.GetItemData(nItem);

			m_pPort->SendControl(dwId, GPSCLIENT_VEHICLE_CALL);
		}
	}
	else if(((CButton*)m_SelectDlg.GetDlgItem(IDC_GROUP))->GetCheck()==TRUE&&m_SelectDlg.m_pDatabase!=nullptr)
	{
		POSITION pos = m_SelectDlg.m_listctrl.GetFirstSelectedItemPosition();
		while(pos!=nullptr)
		{
			const int nItem = m_SelectDlg.m_listctrl.GetNextSelectedItem(pos);
			const DWORD dwId = m_SelectDlg.m_listctrl.GetItemData(nItem);

			CRecordset rs(m_SelectDlg.m_pDatabase);
			try
			{
				CString strSQL;
				strSQL.Format(_T("SELECT Vehicle FROM VehicleGroup WHERE GroupID=%d"), dwId);
				rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
				while(!rs.IsEOF())
				{
					CString strValue;
					rs.GetFieldValue(_T("Vehicle"), strValue);
					const DWORD dwId = _ttoi(strValue);

					m_pPort->SendControl(dwId, GPSCLIENT_VEHICLE_CALL);

					rs.MoveNext();
				}

				rs.Close();
			}
			catch(CDBException*ex)
			{
				ex->ReportError();
				ex->Delete();
				return;
			}
		}
	}

	CDialog::OnOK();
}

void CCallDlg::OnClickOnceRadio()
{
	if(((CButton*)GetDlgItem(IDC_DURATIVERADIO))->GetCheck()==BST_CHECKED)
	{
		GetDlgItem(IDC_INTERVAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_NOINTERVAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIMITLESS)->EnableWindow(TRUE);
		OnClickNointerval();
		OnClickLimitless();
	}
	else
	{
		GetDlgItem(IDC_INTERVAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_NOINTERVAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIMITLESS)->EnableWindow(FALSE);
	}
}

void CCallDlg::OnClickDurativeRadio()
{
	if(((CButton*)GetDlgItem(IDC_DURATIVERADIO))->GetCheck()==BST_CHECKED)
	{
		GetDlgItem(IDC_INTERVAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_NOINTERVAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIMITLESS)->EnableWindow(TRUE);
		OnClickNointerval();
		OnClickLimitless();
	}
	else
	{
		GetDlgItem(IDC_INTERVAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_NOINTERVAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIMITLESS)->EnableWindow(FALSE);
	}
}

void CCallDlg::OnClickNointerval()
{
	if(((CButton*)GetDlgItem(IDC_NOINTERVAL))->GetCheck()==BST_CHECKED)
		GetDlgItem(IDC_INTERVAL)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_INTERVAL)->EnableWindow(TRUE);
}

void CCallDlg::OnClickLimitless()
{
	if(((CButton*)GetDlgItem(IDC_LIMITLESS))->GetCheck()==BST_CHECKED)
		GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
}
