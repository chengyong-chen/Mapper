#include "stdafx.h"
#include "Global.h"
#include "ReplaceDlg.h"

#include "GrfDoc.h"
#include "resource.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReplaceDlg dialog
CReplaceDlg::CReplaceDlg(CWnd* pParent, CDocument* pDocument, CLayerTree& layertree)
	: CDialog(CReplaceDlg::IDD, pParent), m_layertree(layertree),m_itLayertree(layertree.forwbegin()), m_pDocument(pDocument)
{
	//{{AFX_DATA_INIT(CReplaceDlg)
	m_pDocument = nullptr;
	m_pGeom = nullptr;	
	m_posGeomlist = nullptr;
	//}}AFX_DATA_INIT
}

BOOL CReplaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_REPLACEALL))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_REPLACE))->EnableWindow(FALSE);

	m_itLayertree = m_layertree.forwbegin();
	while(m_itLayertree!=m_layertree.forwend())
	{
		const CLayer* layer = *m_itLayertree;
		if(layer->m_bVisible==false)
		{
			++m_itLayertree;
		}
		else
		{
			m_posGeomlist = layer->m_geomlist.GetHeadPosition();
			break;
		}
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplaceDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReplaceDlg, CDialog)
	//{{AFX_MSG_MAP(CReplaceDlg)
	ON_BN_CLICKED(IDC_REPLACE, OnReplace)
	ON_BN_CLICKED(IDC_REPLACEALL, OnReplaceAll)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_EN_CHANGE(IDC_SOURCE, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReplaceDlg message handlers

void CReplaceDlg::OnReplace()
{
	if(m_pGeom!=nullptr)
	{
		CString str1;
		CString str2;
		((CWnd*)GetDlgItem(IDC_SOURCE))->GetWindowText(str1);
		((CWnd*)GetDlgItem(IDC_DESTINATION))->GetWindowText(str2);
		if(str1.IsEmpty()==FALSE&&m_pGeom->m_strName.Find(str1)!=-1)
		{
			m_pGeom->m_strName.Replace(str1, str2);
		}
	}

	((CButton*)GetDlgItem(IDC_REPLACE))->EnableWindow(FALSE);
}

void CReplaceDlg::OnReplaceAll()
{
	CString str1;
	CString str2;
	((CWnd*)GetDlgItem(IDC_SOURCE))->GetWindowText(str1);
	((CWnd*)GetDlgItem(IDC_DESTINATION))->GetWindowText(str2);
	if(str1.IsEmpty()==TRUE)
		return;

	if(m_pDocument==nullptr)
		return;
	const bool bEqual = ((CButton*)GetDlgItem(IDC_EQUAL))->GetCheck()==BST_CHECKED ? true : false;
	while(m_itLayertree!=m_layertree.forwend())
	{
		CLayer* layer = *m_itLayertree;
		m_itLayertree.operator++();
		if(layer->m_bVisible==false)
		{
			m_posGeomlist = nullptr;
			continue;
		}

		if(m_posGeomlist==nullptr)
			m_posGeomlist = layer->m_geomlist.GetHeadPosition();

		while(m_posGeomlist!=nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetNext(m_posGeomlist);
			if(bEqual==true ? geom->m_strName==str1 : geom->m_strName.Find(str1)!=-1)
			{
				geom->m_strName.Replace(str1, str2);
			}
		}
	}

	((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_REPLACE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_REPLACEALL))->EnableWindow(FALSE);
}

void CReplaceDlg::OnNext()
{
	CString str;
	((CWnd*)GetDlgItem(IDC_SOURCE))->GetWindowText(str);
	if(str.IsEmpty()==TRUE)
		return;

	if(m_pDocument==nullptr)
		return;
	const bool bEqual = ((CButton*)GetDlgItem(IDC_EQUAL))->GetCheck()==BST_CHECKED ? true : false;
	while(m_itLayertree!=m_layertree.forwend())
	{
		CLayer* layer = *m_itLayertree;
		m_itLayertree.operator++();
		if(layer->m_bVisible==false)
		{
			m_posGeomlist = nullptr;
			continue;
		}

		if(m_posGeomlist==nullptr)
			m_posGeomlist = layer->m_geomlist.GetHeadPosition();

		while(m_posGeomlist!=nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetNext(m_posGeomlist);
			if(bEqual==true ? geom->m_strName==str : geom->m_strName.Find(str)!=-1)
			{
				m_pGeom = geom;

				POSITION pos = m_pDocument->GetFirstViewPosition();
				CView* pView = m_pDocument->GetNextView(pos);
				if(pView!=nullptr)
				{
					CPoint docPoint = geom->GetGeogroid();

					CRect cliRect;
					pView->GetClientRect(cliRect);
					CPoint cliPoint = cliRect.CenterPoint();

					pView->SendMessage(WM_FIXATEVIEW, (UINT)(&docPoint), (LONG)(&cliPoint));
				}

				((CButton*)GetDlgItem(IDC_REPLACE))->EnableWindow(TRUE);
				if(m_itLayertree==m_layertree.forwend()&&m_posGeomlist==nullptr)
					((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(FALSE);
				else
					((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(TRUE);

				return;
			}
		}
	}

	((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_REPLACE))->EnableWindow(FALSE);
}

void CReplaceDlg::OnChangeEdit1()
{
	m_pGeom = nullptr;

	if(m_pDocument!=nullptr)
	{
		m_itLayertree = m_layertree.forwbegin();
		while(m_itLayertree!=m_layertree.forwend())
		{
			const CLayer* layer = *m_itLayertree;
			if(layer->m_bVisible==false)
			{
				m_itLayertree.operator++();
			}
			else
			{
				m_posGeomlist = layer->m_geomlist.GetHeadPosition();
				break;
			}
		}
	}

	CString str;
	((CWnd*)GetDlgItem(IDC_SOURCE))->GetWindowText(str);
	if(str.IsEmpty()==FALSE)
	{
		((CButton*)GetDlgItem(IDC_REPLACEALL))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(TRUE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_REPLACEALL))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_NEXT))->EnableWindow(FALSE);
	}

	((CButton*)GetDlgItem(IDC_REPLACE))->EnableWindow(FALSE);
}
