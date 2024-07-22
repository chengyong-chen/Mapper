#pragma once

#include "FlashMid.h"

template<class T>
class TFlashMidDlg : public CDialog
{
public:
	TFlashMidDlg(UINT IDD, CWnd* pParent, CMidtable<CFlashMid>& midtable, T* pT)
		: CDialog(IDD, pParent), m_midtable(midtable)
	{
		//{{AFX_DATA_INIT(TFlashMidDlg)
		// NOTE: the ClassWizard will add member initialization here
		m_nScale = 100;
		//}}AFX_DATA_INIT
		if(pT!=nullptr)
		{
			m_nScale = pT->m_nScale;
		}
		m_pT = pT;
	}

	// Dialog Data
	//{{AFX_DATA(TFlashMidDlg)
	CComboBox m_comboFile;
	long m_nScale;
	//}}AFX_DATA

	CMidtable<CFlashMid>& m_midtable;
	T* m_pT;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TFlashMidDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TFlashMidDlg)
		DDX_Control(pDX, IDC_COMBOFILE, m_comboFile);
		DDX_Text(pDX, IDC_SCALE, m_nScale);
		DDV_MinMaxInt(pDX, m_nScale, 10, 300);
		//}}AFX_DATA_MAP
	}

	BOOL OnInitDialog() override
	{
		CDialog::OnInitDialog();

		for(std::map<WORD, CFlashMid*>::const_iterator it = m_midtable.m_midlist.begin(); it!=m_midtable.m_midlist.end(); ++it)
		{
			CFlashMid* pMid = it->second;
			if(m_comboFile.FindString(0, pMid->m_strFile)==-1)
			{
				m_comboFile.AddString(pMid->m_strFile);
			}
		}

		CFlashMid* pMid = m_pT==nullptr ? nullptr : m_midtable.GetMid(m_pT->m_libId);
		if(pMid!=nullptr)
		{
			const CString strFile = pMid->m_strFile;
			m_comboFile.SelectString(0, strFile);
			OnSelchangeFile();
		}
		else if(m_comboFile.GetCount()>0)
		{
			m_comboFile.SetCurSel(0);
			OnSelchangeFile();
		}

		return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void OnOK() override
	{
		CDialog::OnOK();

		const int nItem = m_comboFile.GetCurSel();
		if(nItem==-1)
		{
		}

		if(m_pT!=nullptr)
		{
			CString strFile;
			m_comboFile.GetLBText(nItem, strFile);
			CFlashMid* pMid = m_midtable.Find(strFile);
			if(pMid==nullptr)
			{
				pMid = new CFlashMid(strFile);
				pMid->m_wId = m_midtable.GetMaxId()+1;
				pMid->m_nUsers = 1;
				m_midtable.m_midlist[pMid->m_wId] = pMid;
			}
			else
			{
				pMid->m_nUsers++;
			}
			m_pT->m_libId = pMid->m_wId;
			m_pT->m_nScale = (unsigned short)m_nScale;
		}
	}

	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TFlashMidDlg)
	afx_msg void OnLoad();

	afx_msg void OnSelchangeFile()
	{
		const int nItem = m_comboFile.GetCurSel();
		if(nItem==-1)
			return;

		CString strFile;
		m_comboFile.GetLBText(nItem, strFile);
	}

	afx_msg void OnChangeScale()
	{
	}

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

template<class T>
void TFlashMidDlg<T>::OnLoad()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("FlashMid Path"), nullptr)+_T("*.swf");
	CFileDialog dlg(TRUE,
		_T("swf"),
		strPath,
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
		_T("Flash File (*.swf)|*.swf|"),
		this);
	if(IDOK!=dlg.DoModal())
	{
		SetCurrentDirectory(CurrentDir);
		return;
	}
	SetCurrentDirectory(CurrentDir);
	const CString strFile = dlg.GetPathName();
	if(m_comboFile.FindString(0, strFile)==-1)
	{
		const int nIndex = m_comboFile.AddString(strFile);
		m_comboFile.SetCurSel(nIndex);
	}
	else
	{
		m_comboFile.SelectString(0, strFile);
	}
	OnSelchangeFile();

	strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
	AfxGetApp()->WriteProfileString(_T(""), _T("FlashMid Path"), strPath);
}

BEGIN_TEMPLATE_MESSAGE_MAP(TFlashMidDlg, T, CDialog)
	//{{AFX_MSG_MAP(CSpotSymbolDlg)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
