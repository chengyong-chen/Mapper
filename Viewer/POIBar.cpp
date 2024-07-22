#include "stdafx.h"
#include "POIBar.h"

#include "../Database/ODBCRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPOIBar, CDialog)

CPOIBar::CPOIBar(CWnd* pParent)
	: CDialog(CPOIBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPOIBar)

	//}}AFX_DATA_INIT
	m_pDatabase = nullptr;
	CLaypp* pLaypp = new CLaypp(m_layertree);
	pLaypp->m_geoDatasource.GetHeaderProfile().m_tableA = pLaypp->m_geoDatasource.GetHeaderProfile().m_tableA = _T("POI");
	pLaypp->m_geoDatasource.GetHeaderProfile().m_strAnnoField = _T("NAME");

	pLaypp->m_tableType = _T("POI_Type3");
	pLaypp->LoadIcons(_T("c:\\1.bmp"));
	m_pLaypp = pLaypp;
}

CPOIBar::~CPOIBar()
{
}

void CPOIBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPOIBar)
	DDX_Control(pDX, IDC_POITYPECOMBO1, m_POITypeCombo1);
	DDX_Control(pDX, IDC_POITYPECOMBO2, m_POITypeCombo2);
	DDX_Control(pDX, IDC_POITYPECOMBO3, m_POITypeCombo3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPOIBar, CDialog)
	//{{AFX_MSG_MAP(CPOIBar)
	//}}AFX_MSG_MAP

	ON_CBN_SELCHANGE(IDC_POITYPECOMBO1, OnTypeCom1SelChanged)
	ON_CBN_SELCHANGE(IDC_POITYPECOMBO2, OnTypeCom2SelChanged)
	ON_CBN_SELCHANGE(IDC_POITYPECOMBO3, OnTypeCom3SelChanged)
END_MESSAGE_MAP()

void CPOIBar::SetDatabase(CDatabase* pDatabase)
{
	if(pDatabase==m_pDatabase)
		return;

	m_POITypeCombo1.ResetContent();
	m_POITypeCombo2.ResetContent();
	m_POITypeCombo3.ResetContent();

	m_POITypeCombo1.EnableWindow(FALSE);
	m_POITypeCombo2.EnableWindow(FALSE);
	m_POITypeCombo3.EnableWindow(FALSE);

	if(pDatabase==nullptr)
		return;

	m_POITypeCombo1.EnableWindow(TRUE);

	m_POITypeCombo1.AddString(_T("ȫ����ʾ"));
	m_POITypeCombo1.SetItemData(0, -1);
	m_POITypeCombo1.AddString(_T("��ʾȫ��"));
	m_POITypeCombo1.SetItemData(0, -2);

	try
	{
		CODBCRecordset rs(pDatabase);
		rs.Open(_T("Select * From POI_Type1 order by Type"));
		while(!rs.IsEOF())
		{
			const CString strType = rs.Field(_T("Type")).AsString();
			CString strName = rs.Field(_T("Name")).AsString();

			const DWORD data = HextoDec(strType);
			const int index = m_POITypeCombo1.AddString(strName);
			m_POITypeCombo1.SetItemData(index, data);

			rs.MoveNext();
		}
		rs.Close();

		m_pDatabase = pDatabase;

		m_POITypeCombo1.SetCurSel(0);
		OnTypeCom1SelChanged();
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		ex->Delete();
	}
}

void CPOIBar::OnTypeCom1SelChanged()
{
	m_POITypeCombo2.ResetContent();
	m_POITypeCombo3.ResetContent();
	const int index = m_POITypeCombo1.GetCurSel();
	if(index==0||index==1)
	{
		m_POITypeCombo2.EnableWindow(FALSE);
		m_POITypeCombo3.EnableWindow(FALSE);

		InvalidateView();
	}
	else if(m_pDatabase!=nullptr)
	{
		m_POITypeCombo2.EnableWindow(TRUE);

		m_POITypeCombo2.AddString(_T("ȫ��"));
		m_POITypeCombo2.SetItemData(0, -1);

		try
		{
			const int data = m_POITypeCombo1.GetItemData(index);

			CString strSQL;
			strSQL.Format(_T("Select * From POI_Type2 Where Type Like '%X%c'  order by Type"), data, _T('%'));

			CODBCRecordset rs(m_pDatabase);
			rs.Open(LPCTSTR(strSQL));
			while(!rs.IsEOF())
			{
				const CString strType = rs.Field(_T("Type")).AsString();
				CString strName = rs.Field(_T("Name")).AsString();
				const DWORD data = HextoDec(strType);
				const int index = m_POITypeCombo2.AddString(strName);
				m_POITypeCombo2.SetItemData(index, data);

				rs.MoveNext();
			}
			rs.Close();

			m_POITypeCombo2.SetCurSel(0);
			OnTypeCom2SelChanged();
		}
		catch(CDBException*ex)
		{
			OutputDebugString(ex->m_strError);
			ex->ReportError();
			ex->Delete();
		}
	}
}

void CPOIBar::OnTypeCom2SelChanged()
{
	m_POITypeCombo3.ResetContent();
	const int index2 = m_POITypeCombo2.GetCurSel();
	if(index2==0)
	{
		m_POITypeCombo3.EnableWindow(FALSE);

		InvalidateView();
	}
	else if(m_pDatabase!=nullptr)
	{
		m_POITypeCombo3.EnableWindow(TRUE);
		m_POITypeCombo3.AddString(_T("ȫ��"));
		m_POITypeCombo3.SetItemData(0, -1);

		try
		{
			const int Type2 = m_POITypeCombo2.GetItemData(index2);

			CString strSQL;
			strSQL.Format(_T("Select * From POI_Type3 Where Type Like '%X%c' order by Type"), Type2, _T('%'));

			CODBCRecordset rs(m_pDatabase);
			rs.Open(LPCTSTR(strSQL));
			while(!rs.IsEOF())
			{
				const CString strType = rs.Field(_T("Type")).AsString();
				CString strName = rs.Field(_T("Name")).AsString();;
				const DWORD data = HextoDec(strType);
				const int index = m_POITypeCombo3.AddString(strName);
				m_POITypeCombo3.SetItemData(index, data);

				rs.MoveNext();
			}
			rs.Close();

			m_POITypeCombo3.SetCurSel(0);
			OnTypeCom3SelChanged();
		}
		catch(CDBException*ex)
		{
			OutputDebugString(ex->m_strError);
			ex->ReportError();
			ex->Delete();
		}
	}
}

void CPOIBar::OnTypeCom3SelChanged()
{
	InvalidateView();
}

void CPOIBar::InvalidateView() const
{
	CString strWhere;
	if(this->GetWhere(strWhere)==true)
	{
		m_pLaypp->m_geoDatasource.GetHeaderProfile().m_whereB = strWhere;

		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pCldFrame = (CMDIChildWnd*)pAppFrame->GetActiveFrame();
		CView* pView = (CView*)pCldFrame->GetActiveView();
		if(pView==nullptr)
			return;

		pView->Invalidate();
	}
}

bool CPOIBar::GetWhere(CString& strWhere) const
{
	if(m_POITypeCombo1.IsWindowEnabled()==FALSE)
		return false;
	const int index1 = m_POITypeCombo1.GetCurSel();
	if(index1==0)
		return false;
	else if(index1==1)
		return true;
	else
	{
		CString strType;
		const int data1 = m_POITypeCombo1.GetItemData(index1);

		CString strType1;
		strType1.Format(_T("%X"), data1);
		const int index2 = m_POITypeCombo2.GetCurSel();
		if(index2==0)
		{
			strType.Format(_T("%s%c"), strType1, '%');
		}
		else
		{
			const int data2 = m_POITypeCombo2.GetItemData(index2);

			CString strType2;
			strType2.Format(_T("%X"), data2);
			const int index3 = m_POITypeCombo3.GetCurSel();
			if(index3==0)
			{
				strType.Format(_T("%s%c"), strType2, '%');
			}
			else
			{
				const int data3 = m_POITypeCombo3.GetItemData(index3);
				strType.Format(_T("%X"), data3);
			}
		}
		if(strType.IsEmpty()==FALSE)
		{
			strWhere.Format(_T("Type Like '%s'"), strType);
		}

		return true;
	}
}

void CPOIBar::Draw(CWnd* pWnd, Gdiplus::Graphics& g, const CRectF& mapRect, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& invalidRect, const CLibrary& library) const
{
	if(m_POITypeCombo1.IsWindowEnabled()==FALSE)
	{
		return;
	}
	else
	{
		const int index1 = m_POITypeCombo1.GetCurSel();
		if(index1>0)
		{
			m_pLaypp->Reload(pWnd, datainfo, viewinfo, mapRect);
			m_pLaypp->Draw(g, viewinfo, invalidRect);
			m_pLaypp->DrawTag(g, viewinfo, invalidRect);
		}
	}
}
