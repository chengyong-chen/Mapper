#include "stdafx.h"

#include "TopoCtrl.h"
#include "TopodbCtrl.h"

#include "Topo.h"
#include "RoadTopodb.h"

#include "RoadNodeForm.h"

#include "../Layer/Layer.h"

#include "../DataView/ViewMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopodbCtrl

CTopodbCtrl::CTopodbCtrl(CDocument& document, const CDatainfo& datainfo, std::list<CTopology*>& topolist, Undoredo::CActionStack& actionstack)
	: CTopoCtrl(document, datainfo, topolist, actionstack)
{
}

CTopodbCtrl::~CTopodbCtrl()
{
}

BEGIN_MESSAGE_MAP(CTopodbCtrl, CTopoCtrl)
	//{{AFX_MSG_MAP(CTopodbCtrl)
	ON_COMMAND(ID_TOPO_NEW_TRAFFIC, OnNewTrafficTopo)
	ON_COMMAND(ID_TOPO_NEW_REGION, OnNewRegionTopo)
	ON_COMMAND(ID_TOPO_SETTABLES, OnSetTables)
	ON_COMMAND(ID_TOPO_INITIALIZE, OnInitialize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopodbCtrl message handlers

CTopodb* CTopodbCtrl::GetSelTopo() const
{
	const long Item = CListCtrl::GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return nullptr;

	CTopodb* pTopology = (CTopodb*)CListCtrl::GetItemData(Item);
	return pTopology;
}
void CTopodbCtrl::OnNewRegionTopo()
{	
	/*	CTopology* pTopology = new CRegionTopodb(m_pDatabase);
		m_pTopolist->AddTail(pTopology);

		LV_ITEM lvi;
		lvi.mask      = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvi.iItem     = GetItemCount();
		lvi.iSubItem  = 0;
		lvi.pszText   = pTopology->m_strName.GetBuffer(100);
		lvi.stateMask = LVIS_STATEIMAGEMASK;
		lvi.state     = INDEXTOSTATEIMAGEMASK(pTopology->m_bVisible + 1);
		lvi.lParam    = (DWORD)pTopology;

		long hItem = InsertItem(&lvi);
		SetItemState(hItem, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);*/
}

void CTopodbCtrl::OnNewTrafficTopo()
{
	if(m_pDatabase==nullptr)
		return;

	if(m_pDatabase->IsOpen()==TRUE)
		return;

	CTopology* pTopology = new CRoadTopodb(m_pDatabase);
	m_topolist.push_back(pTopology);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT|LVIF_STATE|LVIF_PARAM;
	lvi.iItem = GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = pTopology->m_strName.GetBuffer(100);
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.state = INDEXTOSTATEIMAGEMASK(pTopology->m_bVisible+1);
	lvi.lParam = (DWORD)pTopology;
	const long hItem = InsertItem(&lvi);
	SetItemState(hItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	pTopology->m_strName.ReleaseBuffer();
}

void CTopodbCtrl::OnInitialize()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology!=nullptr)
	{
		CDatainfo datainfo;
		CViewMonitor viewinfo(datainfo);
		pTopology->Initialize(viewinfo);
	}
}

void CTopodbCtrl::OnSetTables()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology!=nullptr)
	{
		((CTopodb*)pTopology)->SetTables();
	}
}

BOOL CTopodbCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CTopoCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_TOPO_NEW_TRAFFIC:
			{
				if(m_pDatabase==nullptr)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				else if(m_pDatabase->IsOpen()==TRUE)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				else
				{
					pCmdUI->Enable(TRUE);
					return TRUE;
				}
			}
			break;
			case ID_TOPO_INITIALIZE:
			case ID_TOPO_SETTABLES:
			{
				CTopology* pTopology = GetSelTopo();
				if(pTopology==nullptr)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				if(m_pDatabase==nullptr)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				else if(m_pDatabase->IsOpen()==TRUE)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				else
				{
					pCmdUI->Enable(TRUE);
					return TRUE;
				}
			}
			break;
			case ID_TOPO_NEW_REGION:
			{
				pCmdUI->Enable(FALSE);
			}
			return TRUE;
			break;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CTopoCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
