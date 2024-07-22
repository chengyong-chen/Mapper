#include "stdafx.h"

#include "TopoCtrl.h"
#include "TopoerCtrl.h"

#include "Topo.h"
#include "RegionTopoer.h"
#include "RoadTopoer.h"
#include "RoadNodeForm.h"
#include "Layon.h"

#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"
#include "../Layer/LayerTreeCtrl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopoerCtrl
CTopoerCtrl::CTopoerCtrl(CDocument& document, const CDatainfo& datainfo, std::list<CTopology*>& topolist, CLayerTree& layertree, CLayerTreeCtrl& layerTreeCtrl, Undoredo::CActionStack& actionstack)
	: CTopoCtrl(document,datainfo, topolist, actionstack), m_layertree(layertree), m_layerTreeCtrl(layerTreeCtrl)
{
}

CTopoerCtrl::~CTopoerCtrl()
{
}

BEGIN_MESSAGE_MAP(CTopoerCtrl, CTopoCtrl)
	//{{AFX_MSG_MAP(CTopoerCtrl)
	ON_COMMAND(ID_TOPO_DELETE, OnDelete)
	ON_COMMAND(ID_TOPO_NEW_TRAFFIC, OnNewTrafficTopo)
	ON_COMMAND(ID_TOPO_NEW_REGION, OnNewRegionTopo)
	ON_COMMAND(ID_TOPO_CREATEPOLYGONS, OnCreatePolygons)
	ON_COMMAND(ID_TOPO_CREATELINESTRING, OnCreateLinestrings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopoerCtrl message handlers

CTopoer* CTopoerCtrl::GetSelTopo() const
{
	const long Item = CListCtrl::GetNextItem(-1, LVNI_SELECTED);
	if(Item < 0)
		return nullptr;

	CTopoer* pTopology = (CTopoer*)CListCtrl::GetItemData(Item);
	return pTopology;
}
void CTopoerCtrl::OnDelete()
{
	CTopoCtrl::OnDelete();

	m_layerTreeCtrl.BuildTree();
}
void CTopoerCtrl::OnNewRegionTopo()
{
	CLayon* pLayon1 = new CLayon(m_layertree);
	pLayon1->m_wId = m_layertree.ApplyId();
	pLayon1->m_bVisible = false;
	pLayon1->m_strName = _T("polygons");
	m_layertree.m_root.AddChild(nullptr, pLayon1);

	CLayon* pLayon2 = new CLayon(m_layertree);
	pLayon2->m_wId = m_layertree.ApplyId();
	pLayon2->m_bVisible = false;
	pLayon2->m_strName = _T("linestrings");
	m_layertree.m_root.AddChild(nullptr, pLayon2);
	std::list<CLayon*> layonlist;
	layonlist.push_back(pLayon1);
	layonlist.push_back(pLayon2);

	CRegionTopoer* pTopology = new CRegionTopoer(m_layertree, layonlist);
	m_topolist.push_back(pTopology);
	pLayon1->m_pRegionTopoer = pTopology;
	pLayon2->m_pRegionTopoer = pTopology;

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = pTopology->m_strName.GetBuffer(100);
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.state = INDEXTOSTATEIMAGEMASK(pTopology->m_bVisible + 1);
	lvi.lParam = (DWORD)pTopology;

	const long hItem = InsertItem(&lvi);
	SetItemState(hItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	pTopology->m_strName.ReleaseBuffer();
}

void CTopoerCtrl::OnNewTrafficTopo()
{
	CTopology* pTopology = new CRoadTopoer(m_layertree);
	m_topolist.push_back(pTopology);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = pTopology->m_strName.GetBuffer(100);
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.state = INDEXTOSTATEIMAGEMASK(pTopology->m_bVisible + 1);
	lvi.lParam = (DWORD)pTopology;
	const long hItem = InsertItem(&lvi);
	SetItemState(hItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	pTopology->m_strName.ReleaseBuffer();
}

void CTopoerCtrl::OnCreateLinestrings()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology != nullptr && pTopology->Gettype() == 3)
	{
		const POSITION pos = m_Document.GetFirstViewPosition();
		if(pos != nullptr)
		{
			((CRegionTopoer*)pTopology)->CreateLinestrings(m_Datainfo, nullptr);
			m_Document.SetModifiedFlag(TRUE);
			m_Document.UpdateAllViews(nullptr);
		}
	}
}

void CTopoerCtrl::OnCreatePolygons()
{
	CTopology* pTopology = GetSelTopo();
	if(pTopology != nullptr && pTopology->Gettype() == 3)
	{
		const POSITION pos = m_Document.GetFirstViewPosition();
		if(pos != nullptr)
		{
			((CRegionTopoer*)pTopology)->CreatePolygons(m_Datainfo,nullptr);
			m_Document.SetModifiedFlag(TRUE);
			m_Document.UpdateAllViews(nullptr);
		}
	}
}

BOOL CTopoerCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo == nullptr)
	{
		if(nCode == CN_UPDATE_COMMAND_UI)
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
						pCmdUI->Enable(FALSE);
					}
					return TRUE;
					break;
				case ID_TOPO_CREATEPOLYGONS:
					{
						CTopology* pTopology = GetSelTopo();
						if(pTopology == nullptr)
							pCmdUI->Enable(FALSE);
						else if(pTopology->Gettype() == 1)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);
					}
					return TRUE;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode == CN_COMMAND)
		{
		}
	}

	return CTopoCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
