#pragma once

#include "../Dataview/Global.h"

#include <list>

using namespace std;

class CLayer;
namespace Undoredo
{
	namespace Document
	{
		namespace LayerTreeCtrl
		{
			namespace Layerlist
			{
				static void Undo_Append_Layer(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayerTree& layertree,std::pair<std::pair<CLayer*,CLayer*>,CLayer*>& layer)
				{
					HTREEITEM hItem = layertreectrl.GetItem((DWORD)layer.second);
					layer.first.first->DetachChild(layer.second);
					layertreectrl.DeleteItem(hItem);
					document.UpdateAllViews(nullptr);
					document.SetModifiedFlag(TRUE);
				}
				static void Redo_Append_Layer(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayerTree& layertree,std::pair<std::pair<CLayer*,CLayer*>,CLayer*>& layer)
				{
					HTREEITEM itemParent = layertreectrl.GetItem((DWORD)layer.first.first);
					HTREEITEM itemAfter = layertreectrl.GetItem((DWORD)layer.first.second);					
					layer.first.first->AddChild(layer.first.second,layer.second);
					layertreectrl.AddNode(itemParent,itemAfter,layer.second);
					document.UpdateAllViews(nullptr);
					document.SetModifiedFlag(TRUE);
				}
				static void Free_Append_Layer(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayerTree& layertree,std::pair<std::pair<CLayer*,CLayer*>,CLayer*>& layer)
				{
					POSITION pos = document.GetFirstViewPosition();
					CView* pView = document.GetNextView(pos);
					pView->SendMessage(WM_PREDELETELAYER,(UINT)layer.second,0);	
					
					layer.second->Relieve();
				}

				static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,std::pair<CLayer*,CLayer*> original,std::pair<CLayer*,CLayer*> midified)
				{
					pLayer->Parent()->DetachChild(pLayer);
					HTREEITEM hItem = layertreectrl.GetItem((DWORD)pLayer);
					if(hItem != nullptr)
					{
						layertreectrl.DeleteItem(hItem);
					}
					
					original.first->AddChild(original.second,pLayer);
					HTREEITEM hItemParent = layertreectrl.GetItem((DWORD)original.first);
					HTREEITEM hItemAfter = layertreectrl.GetItem((DWORD)original.second);
					layertreectrl.AddNode(hItemParent,hItemAfter,pLayer);
				
					document.UpdateAllViews(nullptr);
					document.SetModifiedFlag(TRUE);		
				}
				static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,std::pair<CLayer*,CLayer*> original,std::pair<CLayer*,CLayer*> midified)
				{
					pLayer->Parent()->DetachChild(pLayer);
					HTREEITEM hItem = layertreectrl.GetItem((DWORD)pLayer);
					if(hItem != nullptr)
					{
						layertreectrl.DeleteItem(hItem);
					}
					
					midified.first->AddChild(midified.second,pLayer);
					HTREEITEM hItemParent = layertreectrl.GetItem((DWORD)midified.first);
					HTREEITEM hItemAfter = layertreectrl.GetItem((DWORD)midified.second);
					layertreectrl.AddNode(hItemParent,hItemAfter,pLayer);

				
					document.UpdateAllViews(nullptr);
					document.SetModifiedFlag(TRUE);		
				}
			}
			namespace LayerName
			{
				static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CString original,CString modified)
				{
					pLayer->m_strName = original;
					HTREEITEM hItem = layertreectrl.GetItem((DWORD)pLayer);
					if(hItem != nullptr)
					{
						layertreectrl.SetItemText(hItem,original);
					}
					document.SetModifiedFlag(TRUE);		
				}
				static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CString original,CString modified)
				{
					pLayer->m_strName = modified;
					HTREEITEM hItem = layertreectrl.GetItem((DWORD)pLayer);
					if(hItem != nullptr)
					{
						layertreectrl.SetItemText(hItem,modified);
					}
					document.SetModifiedFlag(TRUE);		
				}
			}
			namespace Layer
			{
				namespace Spot
				{
					static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CSpot* original,CSpot* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pSpot = original;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CSpot* original,CSpot* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pSpot = modified;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Free_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CSpot* pSpot)
					{
						if(pSpot != nullptr)
						{
							pSpot->Decrease(layertreectrl.m_layertree.m_library);
							delete pSpot;
							pSpot = nullptr;
						}
					}
				}
				namespace Line
				{
					static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CLine* original,CLine* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pLine = original;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CLine* original,CLine* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pLine = modified;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Free_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CLine* pLine)
					{
						if(pLine != nullptr)
						{
							pLine->Decrease(layertreectrl.m_layertree.m_library);
							delete pLine;
							pLine = nullptr;
						}
					}
				}
				namespace Fill
				{
					static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CFill* original,CFill* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pFill = (CFillGeneral*)original;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CFill* original,CFill* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pFill = (CFillGeneral*)modified;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Free_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CFill* pFill)
					{
						if(pFill != nullptr)
						{
							pFill->Decrease(layertreectrl.m_layertree.m_library);
							delete pFill;
							pFill = nullptr;
						}
					}
				}
				namespace Type
				{
					static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CType* original,CType* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pType = original;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CType* original,CType* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pType = modified;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Free_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CType* pType)
					{
						delete pType;
						pType = nullptr;
					}
				}
				namespace Hint
				{	  
					static void Undo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CHint* original,CHint* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pHint = original;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Redo_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CHint* original,CHint* modified)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO,0,0);

						pLayer->Invalid(document);
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->m_pHint = modified;
						pLayer->StyleChanged(pView,*pViewinfo);
						pLayer->Invalid(document);
					}
					static void Free_Modify(CDocument& document,CLayerTreeCtrl& layertreectrl,CLayer* pLayer,CHint* pHint)
					{
						delete pHint;
						pHint = nullptr;
					}		   
				}
			}
		}
	}
}