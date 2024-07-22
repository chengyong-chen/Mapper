#include "stdafx.h"
#include "Port.h"
#include "SelectDlg.h"
#include <afxdao.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog


CSelectDlg::CSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDlg)
		
	//}}AFX_DATA_INIT
}

CSelectDlg::CSelectDlg(CWnd* pParent,CString strTable)
	: CDialog(CSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDlg)
		
	//}}AFX_DATA_INIT

	CString strConnect;
	strConnect.Format(_T("DSN=MS Access Database;DBQ=%s;"),m_strDb);
	
	CDatabase pDB;
	BOOL bOpen = pDB.OpenEx(strConnect, CDatabase::openReadOnly | CDatabase::noOdbcDialog);
	if(bOpen == false)
	{
		AfxMessageBox(_T("The database dosn't exist!"));
		return;
	}

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM %s"), strTable);

	CRecordset rs(&pDB);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL);

		while(!rs.IsEOF()) 
		{
			Kind kind;
			CString  var;
			rs.GetFieldValue((short)0,var);
			kind.GB = _ttol(var);

			rs.GetFieldValue((short)1,var);
			kind.Code = _ttol(var);
				
			rs.GetFieldValue((short)2,var);
			kind.Name = var;

			rs.GetFieldValue((short)3,var);
			kind.inPat = false;		
			if(var.Find(_T("Pat")) == TRUE)
				kind.inPat = TRUE;		
			kind.inAat = false;	
			if(var.Find(_T("Aat")) == TRUE)
				kind.inAat = TRUE;	

			rs.GetFieldValue((short)4,var);
			if(var == _T('1'))
				kind.Check = TRUE;		
			if(var == _T('0'))
				kind.Check = false;		

			rs.MoveNext();
				
			m_Kinds.AddTail(kind);
		}		
		
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError+"\r\n");
		ex->ReportError();
		ex->Delete();
	}
	
	pDB.Close();
}

CSelectDlg::~CSelectDlg()
{
	m_Kinds.RemoveAll();
		
	delete [] m_pButtons;
}

void CSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg message handlers

BOOL CSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CSelectDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CClientDC dc(this);
	TEXTMETRIC tm;
	VERIFY(dc.GetTextMetrics(&tm));
	CSize sizeBase = CSize(tm.tmAveCharWidth, tm.tmHeight);

	long nHeight = 0;
	long nWidth  = 0;

	long i=0;
	CRect rect(CPoint(10,10), sizeBase);
	m_pButtons = new CButton[m_Kinds.GetCount()];
	POSITION pos = m_Kinds.GetHeadPosition();
	while(pos != nullptr)
	{
		Kind kind = m_Kinds.GetNext(pos);
		rect.right = rect.left + dc.GetTextExtent(kind.Name,kind.Name.GetLength()).cx + sizeBase.cx*3;
		if(!m_pButtons[i].Create(kind.Name, WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, this, i+50))
		{
			return -1;
		}
		rect.OffsetRect(0,sizeBase.cy*3/2);
		nWidth = max(nWidth,rect.right);
		m_pButtons[i].SetCheck(kind.Check);
		i++;
	}	
	if(nWidth<400) 
		nWidth = 400;
	nHeight = rect.bottom-sizeBase.cy;
	rect.SetRect(0,0,nWidth,nHeight);
	CalcWindowRect(&rect);
	CPoint Origin = CPoint((dc.GetDeviceCaps(HORZRES)-rect.Width())/2,(dc.GetDeviceCaps(VERTRES)-rect.Height())/2);
	if(Origin.x<=0)
		Origin.x=0;
	if(Origin.y<=0)
		Origin.y=0;

	SetWindowPos(nullptr, (dc.GetDeviceCaps(HORZRES)-rect.Width())/2,(dc.GetDeviceCaps(VERTRES)-rect.Height())/2, rect.Width(), rect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	return 0;
}

void CSelectDlg::OnOK() 
{	
	CDialog::OnOK();

	long i = 0;
	POSITION pos1 = m_Kinds.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		Kind kind  = m_Kinds.GetNext(pos1);
		kind.Check = m_pButtons[i++].GetCheck();
		m_Kinds.SetAt(pos2,kind);
	}
}
