#include "stdafx.h"

#include "Global.h"
#include "GisDoc.h"
#include "GeoDoc.h"
#include "GisView.h"
#include "GeoView.h"
#include "MainFrm.h"

#include "../Tool/PanTool.h"
#include "../Tool/Global.h"

#include "../Layer/Global.h"
#include "../Layer/Layer.h"

#include "../Public/RecordCtrl.h"
#include "../Public/PropertyPane.h"
#include "../Public/ODBCDatabase.h"

#include "../Geometry/Geom.h"
extern __declspec(dllimport) CPanTool panTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_GWDYNCREATE(CGeoView, CGisView)

BEGIN_MESSAGE_MAP(CGeoView, CGisView)
	//{{AFX_MSG_MAP(CGeoView)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEWGEOMDREW, OnNewGeomDrew)
	ON_MESSAGE(WM_NEWGEOMBRED, OnNewGeomBred)
	ON_MESSAGE(WM_PREDELETEGEOM, OnPreDeleteGeom)
	ON_MESSAGE(WM_GEOMACTIVATED, OnGeomActivated)
END_MESSAGE_MAP()

// CGeoView construction/destruction

CGeoView::CGeoView(CObject& parameter) noexcept
	: CGisView(parameter), m_document((CGeoDoc&)parameter)
{
}

CGeoView::~CGeoView()
{
}

void CGeoView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CGisView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(bActivate==TRUE&&pActivateView==this)
	{
		panTool.m_bContinuously = false;		
		CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
		if(pPropertyPane!=nullptr)
		{
			pPropertyPane->ValidateTab('R');
		}
	}
}

// CGeoView diagnostics
#ifdef _DEBUG
void CGeoView::AssertValid() const
{
	CGisView::AssertValid();
}

void CGeoView::Dump(CDumpContext& dc) const
{
	CGisView::Dump(dc);
}
#endif //_DEBUG

// CGeoView message handlers
/////////////////////////////////////////////////////////////////////////////
// GeoView message handlers

LONG CGeoView::OnNewGeomDrew(UINT WPARAM, LONG LPARAM)
{
	CGisView::OnNewGeomDrew(WPARAM, LPARAM);

	CGeom* pGeom = (CGeom*)WPARAM;
	if(CLayer* layer = GetActiveLayer(); layer!=nullptr)
	{
		layer->NewGeomDrew(pGeom);

		pGeom->m_bModified = true;

		layer->GeomActivated(pGeom);
		return TRUE;
	}
	else
	{
		AfxMessageBox(IDS_NOACTIVELAYER);
		return FALSE;
	}
}

LONG CGeoView::OnNewGeomBred(UINT WPARAM, LONG LPARAM)
{
	CGisView::OnNewGeomBred(WPARAM, LPARAM);

	CLayer* layer = (CLayer*)WPARAM;
	CGeom* pGeom = (CGeom*)LPARAM;
	if(layer!=nullptr&&pGeom!=nullptr)
	{
		layer->NewGeomBred(pGeom);

		pGeom->m_bModified = true;

		layer->GeomActivated(pGeom);
		return TRUE;
	}
	else
	{
		AfxMessageBox(IDS_NOACTIVELAYER);
		return FALSE;
	}
	return TRUE;
}

LONG CGeoView::OnPreDeleteGeom(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;
	CLayer* pLayer = (CLayer*)WPARAM;
	if(pGeom!=nullptr&&pLayer!=nullptr)
	{
		pLayer->PreRemoveGeom(pGeom);
	}

	return CGisView::OnPreDeleteGeom(WPARAM, LPARAM);
}

LONG CGeoView::OnGeomActivated(UINT WPARAM, LONG LPARAM)
{
	CLayer* pLayer = (CLayer*)WPARAM;
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return 0;
	if(pLayer==nullptr)
		return 0;
	CATTDatasource* pDatasource = pLayer->GetAttDatasource();
	if(pDatasource==nullptr)
		return 0;
	CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase==nullptr)
		return 0;
	if(pDatabase->IsOpen()==FALSE)
		return 0;

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr&&pPropertyPane->IsVisible()==TRUE)
	{
		CRecordCtrl* pRecordCtrl = (CRecordCtrl*)pPropertyPane->GetTabWnd('R');
		if(pRecordCtrl!=nullptr)
		{
			pPropertyPane->ActivateTab(_T('R'));
			const CATTHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
			const CString strTable = headerprofile.m_strKeyTable;
			pRecordCtrl->ActivateRec(pDatabase, strTable, headerprofile.m_strIdField, pGeom->m_attId);
		}
	}

	return CGisView::OnGeomActivated(WPARAM, LPARAM);
}

void CGeoView::InactivateAll()
{
	CGisView::InactivateAll();

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		CRecordCtrl* pRecordCtrl = (CRecordCtrl*)pPropertyPane->GetTabWnd('R');
		if(pRecordCtrl!=nullptr)
		{
			pRecordCtrl->DisposeRec();
		}
	}
}
