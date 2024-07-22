// DlgCmd.cpp : implementation file
//

#include "stdafx.h"
#include "CmdParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD staicID[5];
DWORD editID[5];

/////////////////////////////////////////////////////////////////////////////
// CCmdParamDlg dialog

CCmdParamDlg::CCmdParamDlg(CWnd* pParent, CCoder::SParameter* pParameter)
	: CDialog(CCmdParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCmdParamDlg)
	m_strEdit0 = _T("");
	m_strEdit1 = _T("");
	m_strEdit2 = _T("");
	m_strEdit3 = _T("");
	m_strEdit4 = _T("");
	m_strStatic0 = _T("");
	m_strStatic1 = _T("");
	m_strStatic2 = _T("");
	m_strStatic3 = _T("");
	m_strStatic4 = _T("");
	m_strDescription = _T("");
	m_dateTime1 = COleDateTime::GetCurrentTime();
	m_dateTime2 = COleDateTime::GetCurrentTime();
	m_dateTime3 = COleDateTime::GetCurrentTime();
	m_dateTime4 = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT

	m_pParameter = pParameter;
}

void CCmdParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdParamDlg)
	DDX_Text(pDX, IDC_EDIT0, m_strEdit0);
	DDX_Text(pDX, IDC_EDIT1, m_strEdit1);
	DDX_Text(pDX, IDC_EDIT2, m_strEdit2);
	DDX_Text(pDX, IDC_EDIT3, m_strEdit3);
	DDX_Text(pDX, IDC_EDIT4, m_strEdit4);
	DDX_Text(pDX, IDC_STATIC0, m_strStatic0);
	DDX_Text(pDX, IDC_STATIC1, m_strStatic1);
	DDX_Text(pDX, IDC_STATIC2, m_strStatic2);
	DDX_Text(pDX, IDC_STATIC3, m_strStatic3);
	DDX_Text(pDX, IDC_STATIC4, m_strStatic4);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_dateTime1);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_dateTime2);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER3, m_dateTime3);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER4, m_dateTime4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCmdParamDlg, CDialog)
	//{{AFX_MSG_MAP(CCmdParamDlg)
	ON_BN_CLICKED(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCmdParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowText(m_strTitle);

	if(m_pParameter==nullptr)
		return FALSE;

	GetDlgItem(IDC_STATIC_DESCRIPTION)->SetWindowText(m_strDescription);

	CenterWindow();

	staicID[0] = IDC_STATIC0;
	staicID[1] = IDC_STATIC1;
	staicID[2] = IDC_STATIC2;
	staicID[3] = IDC_STATIC3;
	staicID[4] = IDC_STATIC4;

	editID[0] = IDC_EDIT0;
	editID[1] = IDC_EDIT1;
	editID[2] = IDC_EDIT2;
	editID[3] = IDC_EDIT3;
	editID[4] = IDC_EDIT4;

	DWORD TimeCtrlID[2];
	DWORD DateCtrlID[2];

	TimeCtrlID[0] = IDC_DATETIMEPICKER1;
	TimeCtrlID[1] = IDC_DATETIMEPICKER2;
	DateCtrlID[0] = IDC_DATETIMEPICKER3;
	DateCtrlID[1] = IDC_DATETIMEPICKER4;

	int TimeNum = 0;
	int DateNum = 0;
	const int nTimeCnt = 2;
	const int nDateCnt = 2;

	for(int i = 0; i<m_saParams.GetSize(); i++)
	{
		CString strVal = m_saValues.GetAt(i);

		if(m_saTypes.GetAt(i).Find(_T("select"))!=-1)
		{
			CRect r;
			GetDlgItem(editID[i])->ShowWindow(SW_HIDE);
			GetDlgItem(editID[i])->GetWindowRect(&r);
			ScreenToClient(&r);
			GetDlgItem(IDC_COMBO_SELECT)->MoveWindow(&r);
			GetDlgItem(IDC_COMBO_SELECT)->ShowWindow(SW_SHOW);

			((CComboBox*)GetDlgItem(IDC_COMBO_SELECT))->ResetContent();

			if(!strVal.IsEmpty())
			{
				CString str = strVal;
				int idx = -1;
				while((idx = strVal.Find(_T(",")))!=-1)
				{
					str = strVal.Left(idx);
					((CComboBox*)GetDlgItem(IDC_COMBO_SELECT))->AddString(str);
					strVal = strVal.Right(strVal.GetLength()-str.GetLength()-1);
				}
				if(strVal.GetLength()>0)
					((CComboBox*)GetDlgItem(IDC_COMBO_SELECT))->AddString(strVal);
				((CComboBox*)GetDlgItem(IDC_COMBO_SELECT))->SetCurSel(0);
			}
		}
		else if(m_saTypes.GetAt(i).Find(_T("date"))!=-1)
		{
			CRect r;
			GetDlgItem(editID[i])->ShowWindow(SW_HIDE);
			GetDlgItem(editID[i])->GetWindowRect(&r);
			ScreenToClient(&r);

			if(DateNum<nDateCnt)
			{
				GetDlgItem(DateCtrlID[TimeNum])->MoveWindow(&r);
				GetDlgItem(DateCtrlID[TimeNum])->ShowWindow(SW_SHOW);
			}

			if(DateNum<nDateCnt)
				DateNum++;
		}
		else if(m_saTypes.GetAt(i).Find(_T("time"))!=-1)
		{
			CRect r;
			GetDlgItem(editID[i])->ShowWindow(SW_HIDE);
			GetDlgItem(editID[i])->GetWindowRect(&r);
			ScreenToClient(&r);

			if(TimeNum<nTimeCnt)
			{
				GetDlgItem(TimeCtrlID[TimeNum])->MoveWindow(&r);
				GetDlgItem(TimeCtrlID[TimeNum])->ShowWindow(SW_SHOW);
			}
			if(TimeNum<nTimeCnt)
				TimeNum++;
		}
		else
		{
			GetDlgItem(editID[i])->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_COMBO_SELECT)->ShowWindow(SW_HIDE);
			if(strVal.IsEmpty()==FALSE)
			{
				GetDlgItem(editID[i])->SetWindowText(strVal);
			}
			else
				GetDlgItem(editID[i])->SetWindowText(_T(""));
		}

		GetDlgItem(staicID[i])->ShowWindow(SW_SHOW);
		GetDlgItem(staicID[i])->SetWindowText(m_saParams.GetAt(i));
	}

	CRect rc1;
	GetDlgItem(editID[m_saParams.GetSize()-1])->GetWindowRect(&rc1);

	CRect rc2;
	this->GetWindowRect(&rc2);
	const int height = rc1.bottom-rc2.top+5;
	SetWindowPos(nullptr, rc2.left, rc2.top, rc2.Width(), height, SWP_SHOWWINDOW);
	return TRUE;
}

void CCmdParamDlg::OnOK()
{
	UpdateData(TRUE);

	CString strType;
	for(int i = 0; i<4; i++)
	{
		CString str;
		GetDlgItem(editID[i])->GetWindowText(str);
		m_pParameter->strParam[i] = str;
		m_pParameter->dwParam[i] = _ttol(str);
		m_pParameter->fParam[i] = _ttol(str);
	}
	m_pParameter->iSelect = ((CComboBox*)GetDlgItem(IDC_COMBO_SELECT))->GetCurSel();

	double dTime;
	SYSTEMTIME sTime;
	sTime.wYear = m_dateTime3.GetYear();
	sTime.wMonth = m_dateTime3.GetMonth();
	sTime.wDay = m_dateTime3.GetDay();
	sTime.wHour = m_dateTime1.GetHour();
	sTime.wMinute = m_dateTime1.GetMinute();
	sTime.wSecond = m_dateTime1.GetSecond();
	SystemTimeToVariantTime(&sTime, &dTime);
	m_pParameter->dDate[0] = dTime;

	sTime.wYear = m_dateTime4.GetYear();
	sTime.wMonth = m_dateTime4.GetMonth();
	sTime.wDay = m_dateTime4.GetDay();
	sTime.wHour = m_dateTime2.GetHour();
	sTime.wMinute = m_dateTime2.GetMinute();
	sTime.wSecond = m_dateTime2.GetSecond();
	SystemTimeToVariantTime(&sTime, &dTime);
	m_pParameter->dDate[1] = dTime;

	CDialog::OnOK();
}
