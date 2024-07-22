#include "stdafx.h"
#include "Topo.h"
#include "Topoer.h"
#include "TopoCtrl.h"
#include "RoadNodeForm.h"

#include "../Layer/Layer.h"
#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopoCtrl

CTopoCtrl::CTopoCtrl(CDocument& document, const CDatainfo& datainfo, std::list<CTopology*>& topolist, Undoredo::CActionStack& actionstack)
	: m_Document(document), m_Datainfo(datainfo), m_topolist(topolist), m_actionStack(actionstack)
{
}

CTopoCtrl::~CTopoCtrl()
{
}

BEGIN_MESSAGE_MAP(CTopoCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CTopoCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_TOPO_STYLE, OnStyle)
	ON_COMMAND(ID_TOPO_DELETE, OnDelete)
	ON_COMMAND(ID_TOPO_FILTENODE, OnRemoveOrphanNodes)
	ON_COMMAND(ID_TOPO_FILTEEDGE, OnRemoveOrphanEdges)
	ON_COMMAND(ID_TOPO_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopoCtrl message handlers

int CTopoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
	AfxSetResourceHandle(hInst);
	
	CImageList stateImages;
	if(stateImages.Create(IDB_TOPOSTATE, 16, 0, RGB(255, 0, 0))==TRUE)
	{
		stateImages.SetOverlayImage(0, 1);
		SetImageList(&stateImages, LVSIL_STATE);
		stateImages.Detach();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);	
	return 0;
}

void CTopoCtrl::Create(CWnd* parent)
{
	DWORD dwStyle = LVS_LIST|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_EDITLABELS|WS_CHILD|WS_VISIBLE;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;

	CListCtrl::EnableToolTips(FALSE);

	CListCtrl::Create(dwStyle, CRect(0, 0, 0, 0), parent, 0);
}

void CTopoCtrl::Built()
{
	for(std::list<CTopology*>::iterator it=m_topolist.begin(); it != m_topolist.end(); it++)
	{
		CTopology* pTopology=*it;

		LV_ITEM lvi;
		lvi.mask=LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvi.iItem=GetItemCount();
		lvi.iSubItem=0;
		lvi.pszText=pTopology->m_strName.GetBuffer(100);
		lvi.stateMask=LVIS_STATEIMAGEMASK;
		lvi.state=INDEXTOSTATEIMAGEMASK(pTopology->m_bVisible + 1);
		lvi.lParam=(DWORD)pTopology;
		CListCtrl::InsertItem(&lvi);
		pTopology->m_strName.ReleaseBuffer();
	}

	if(m_topolist.size()>0)
	{
		CListCtrl::SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CTopoCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	const CPoint client = point;
	UINT uFlags = 0;
	long hItem;

	hItem = CListCtrl::HitTest(client, &uFlags);

	if(uFlags&LVHT_ONITEMSTATEICON)
	{
		const long iImage = GetItemState(hItem, TVIS_STATEIMAGEMASK)>>12;
		CListCtrl::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(iImage==1 ? 2 : 1), TVIS_STATEIMAGEMASK);

		CTopology* pTopology = (CTopology*)GetItemData(hItem);

		pTopology->m_bVisible = !pTopology->m_bVisible;
		pTopology->Invalidate(&m_Document);
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CTopoCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(pDispInfo->item.pszText!=nullptr)
	{
		pDispInfo->item.mask = TVIF_TEXT;
		CListCtrl::SetItem(&pDispInfo->item);

		CTopology* pTopology = (CTopology*)GetItemData(pDispInfo->item.iItem);
		pTopology->m_strName = pDispInfo->item.pszText;
	}

	*pResult = 0;
}

CTopology* CTopoCtrl::GetSelTopo() const
{
	const long Item=CListCtrl::GetNextItem(-1, LVNI_SELECTED);
	return Item < 0 ? nullptr : (CTopology*)CListCtrl::GetItemData(Item);
}

void CTopoCtrl::OnStyle()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology!=nullptr)
	{
		if(pTopology->SetStyle()==TRUE)
		{
			pTopology->Invalidate(&m_Document);

			//	CCommand* undo = new CCommand("None");
			//	m_pView->SendMessage(WM_SUBMITUNDO,0,(LONG)undo);
		}
	}
}

void CTopoCtrl::OnDelete()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CTopology* pTopology = (CTopology*)GetItemData(Item);
	if(pTopology!=nullptr)
	{	
		pTopology->PreDelete();
		m_topolist.remove(pTopology);
		delete pTopology;

		m_Document.SetModifiedFlag(TRUE);
		m_Document.UpdateAllViews(nullptr);
	}

	DeleteItem(Item);
}

BOOL CTopoCtrl::DestroyWindow()
{
	DeleteAllItems();

	return CListCtrl::DestroyWindow();
}

void CTopoCtrl::OnRemoveOrphanNodes()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology!=nullptr)
	{
		pTopology->FilterNode(m_Datainfo);
		m_Document.SetModifiedFlag(TRUE);
		m_Document.UpdateAllViews(nullptr);
	}
}

void CTopoCtrl::OnRemoveOrphanEdges()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology!=nullptr)
	{
		pTopology->FilterEdge(m_Datainfo);

		m_Document.SetModifiedFlag(TRUE);
		m_Document.UpdateAllViews(nullptr);
	}
}

void CTopoCtrl::OnExport()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology!=nullptr)
	{
		CFile file;
		if(file.Open(_T("c:\\1.top"), CFile::modeCreate|CFile::modeWrite)==TRUE)
		{
			CArchive ar(&file, CArchive::store);
			DWORD dwVersion = AfxGetCurrentArVersion();
			ar << dwVersion;
			BYTE type = pTopology->Gettype();
			ar << type;
			pTopology->Serialize(ar, dwVersion);

			ar.Close();
			file.Close();
		}
	}
}

BOOL CTopoCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CListCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_TOPO_FILTENODE:
			case ID_TOPO_FILTEEDGE:
				CTopology* pTopology = GetSelTopo();
				if(pTopology==nullptr)
					pCmdUI->Enable(FALSE);
				else if(pTopology->Gettype() == 1==FALSE)
					pCmdUI->Enable(TRUE);
				else 
					pCmdUI->Enable(TRUE);
				return TRUE;
				break;
			}
		}
	}

	return CListCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
