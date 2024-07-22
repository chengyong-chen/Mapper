#pragma once
#include <list>
#include <map>

#include "../../../../Layer/Global.h"
using namespace std;

class CLayer;
namespace Undoredo
{
	namespace Document
	{
		namespace LayerTree
		{
			namespace Layer
			{
				namespace Geomlist
				{
					static void Remove_Geom(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::pair<CGeom*, unsigned int>& geom)//geom's index is from big to small in geoms
					{
						if(pLayer==nullptr)
							return;

						POSITION pos1 = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos1);

						CGeom* pGeom = geom.first;
						unsigned int index = geom.second;

						POSITION pos2 = pLayer->m_geomlist.FindIndex(index);
						if(pos2==nullptr)
							pos2 = pLayer->m_geomlist.Find(pGeom);
						if(pos2!=nullptr)
						{
							if(pLayer->m_geomlist.GetAt(pos2)==pGeom)
							{
								pLayer->Invalidate(document, pGeom);
								pLayer->m_geomlist.RemoveAt(pos2);

								pView->SendMessage(WM_PREREMOVEGEOM, (LONG)pLayer, (UINT)pGeom);
								pLayer->SetModifiedFlag(true);
								if(pGeom->m_bActive==true)
									pLayer->m_nActiveCount--;
							}
							else
							{
								//something wrong
							}
						}
					}

					static void Remove_Geoms(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<std::pair<CGeom*, unsigned int>>& geoms)//geom's index is from big to small in geoms
					{
						for(std::list<std::pair<CGeom*, unsigned int>>::iterator it = geoms.begin(); it!=geoms.end(); it++)
						{
							Geomlist::Remove_Geom(document, pWnd, pLayer, *it);
						}
					}
					static void Append_Geom(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::pair<CGeom*, unsigned int>& geom)
					{
						if(pLayer==nullptr)
							return;

						CGeom* pGeom = geom.first;
						unsigned int index = geom.second;
						if(index==0)
							pLayer->m_geomlist.AddHead(pGeom);
						else if(index==-1)
							pLayer->m_geomlist.AddTail(pGeom);
						else
						{
							POSITION pos = pLayer->m_geomlist.FindIndex(index);
							if(pos==nullptr)
								pLayer->m_geomlist.AddTail(pGeom);
							else
								pLayer->m_geomlist.InsertBefore(pos, pGeom);
						}
						if(pGeom->m_bActive==true)
							pLayer->m_nActiveCount++;
						pLayer->Invalidate(document, pGeom);
						pLayer->SetModifiedFlag(true);
					}
					static void Append_Geoms(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<std::pair<CGeom*, unsigned int>>& geoms)
					{
						for(std::list<std::pair<CGeom*, unsigned int>>::reverse_iterator it = geoms.rbegin(); it!=geoms.rend(); it++)
						{
							Append_Geom(document, pWnd, pLayer, *it);
						}
					}
					static void Free_Removed_Geom(CDocument& document, CWnd* pWnd, CLayer* layer, std::pair<CGeom*,unsigned int>& geom)
					{
						CGeom* pGeom = geom.first;
						if(pGeom!=nullptr)
						{
							if(pGeom->m_bActive==true)
								layer->m_nActiveCount--;

							POSITION pos = document.GetFirstViewPosition();
							CView* pView = document.GetNextView(pos);
							pView->SendMessage(WM_PREDELETEGEOM, (UINT)layer, (LONG)pGeom);

							delete pGeom;
						}
					}
					static void Free_AppendRemove_Geoms(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<std::pair<CGeom*, unsigned int>>& geoms)
					{
						for(std::list<std::pair<CGeom*, unsigned int>>::reverse_iterator it = geoms.rbegin(); it!=geoms.rend(); it++)
						{
							Free_Removed_Geom(document, pWnd, pLayer, *it);
						}
						geoms.clear();
					}

					static void Undo_Modify_Move(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::pair<CGeom*, CSize> original, std::pair<CGeom*, CSize> modified)
					{
						CGeom* pGeom = original.first;
						CSize delta = original.second;
						if(pGeom!=nullptr)
						{
							pLayer->Invalidate(document, pGeom);
							pGeom->Move(-delta);
							pLayer->Invalidate(document, pGeom);
							pLayer->SetModifiedFlag(true);
						}
					}
					static void Redo_Modify_Move(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::pair<CGeom*, CSize> original, std::pair<CGeom*, CSize> modified)
					{
						CGeom* pGeom = original.first;
						CSize delta = original.second;
						if(pGeom!=nullptr)
						{
							pLayer->Invalidate(document, pGeom);
							pGeom->Move(delta);
							pLayer->Invalidate(document, pGeom);
							pLayer->SetModifiedFlag(true);
						}
					}

					static void Undo_Modify_Position(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<int> original, std::list<int> modified)
					{
						for(std::list<int>::reverse_iterator it1 = original.rbegin(), it2 = modified.rbegin(); it1!=original.rend()&&it2!=modified.rend(); it1++, it2++)
						{
							int indexOriginal = *it1;
							int indexModified = *it2;
							if(indexOriginal==indexModified)
								continue;

							POSITION originalpos = pLayer->m_geomlist.FindIndex(indexOriginal);
							POSITION modifiedpos = pLayer->m_geomlist.FindIndex(indexModified);
							if(originalpos==nullptr||modifiedpos==nullptr)
								continue;

							CGeom* pGeom = pLayer->m_geomlist.GetAt(modifiedpos);
							pLayer->m_geomlist.RemoveAt(modifiedpos);
							pLayer->m_geomlist.InsertBefore(originalpos, pGeom);

							pGeom->Invalid(document);
							pLayer->SetModifiedFlag(true);
						}
					}
					static void Redo_Modify_Position(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<int> original, std::list<int> modified)
					{
						for(std::list<int>::iterator it1 = original.begin(), it2 = modified.begin(); it1!=original.end()&&it2!=modified.end(); it1++, it2++)
						{
							int indexOriginal = *it1;
							int indexModified = *it2;
							if(indexOriginal==indexModified)
								continue;
							POSITION originalpos = pLayer->m_geomlist.FindIndex(indexOriginal);
							POSITION modifiedpos = pLayer->m_geomlist.FindIndex(indexModified);
							if(originalpos==nullptr||modifiedpos==nullptr)
								continue;

							CGeom* pGeom = pLayer->m_geomlist.GetAt(originalpos);
							pLayer->m_geomlist.RemoveAt(originalpos);
							pLayer->m_geomlist.InsertAfter(modifiedpos, pGeom);

							pGeom->Invalid(document);
							pLayer->SetModifiedFlag(true);
						}
					}

					static void Undo_Modify_Replace(CDocument& document, CWnd* pWnd, std::pair<CLayer*, unsigned int> inlayer, CGeom* original, CGeom* modified)
					{
						int index = inlayer.second;
						CLayer* pLayer = inlayer.first;
						POSITION pos = pLayer->m_geomlist.FindIndex(inlayer.second);
						if(pos!=nullptr)
						{
							pLayer->m_geomlist.SetAt(pos, original);
							pLayer->Invalidate(document, original);
							pLayer->Invalidate(document, modified);
							pLayer->SetModifiedFlag(true);

						}
					}
					static void Redo_Modify_Replace(CDocument& document, CWnd* pWnd, std::pair<CLayer*, unsigned int> inlayer, CGeom* original, CGeom* modified)
					{
						int index = inlayer.second;
						CLayer* pLayer = inlayer.first;
						POSITION pos = pLayer->m_geomlist.FindIndex(inlayer.second);
						if(pos!=nullptr)
						{
							pLayer->m_geomlist.SetAt(pos, modified);
							pLayer->Invalidate(document, original);
							pLayer->Invalidate(document, modified);
							pLayer->SetModifiedFlag(true);

						}
					}
					static void Free_Modify_Replace(CDocument& document, CWnd* pWnd, std::pair<CLayer*, unsigned int> inlayer, CGeom* pGeom)
					{
						int index = inlayer.second;
						CLayer* pLayer = inlayer.first;

						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);
						pView->SendMessage(WM_PREDELETEGEOM, (UINT)pLayer, (LONG)pGeom);
						delete pGeom;
						pGeom = nullptr;
					}

					static void Both_Modify_Swap(CDocument& document, CWnd* pWnd, CLayer* pLayer, CGeom* original, CGeom* modified)
					{
						if(modified!=nullptr&&original!=nullptr)
						{
							pLayer->Invalidate(document, modified);
							pLayer->Invalidate(document, original);
							modified->Swap(original);
						}
						if(pLayer==nullptr)
						{
							pLayer->SetModifiedFlag(true);
						}
					}
					static void Free_Modify_Swap(CDocument& document, CWnd* pWnd, CLayer* pLayer, CGeom* pGeom)
					{
						delete pGeom;
						pGeom = nullptr;
					}
					static void Undo_Modify_Lock(CDocument& document, CWnd* pWnd, std::list<CGeom*> geoms, bool original, bool modified)
					{
						for(std::list<CGeom*>::iterator it = geoms.begin(); it!=geoms.end(); it++)
						{
							CGeom* pGeom = *it;
							pGeom->m_bLocked = original;
						}
					}
					static void Redo_Modify_Lock(CDocument& document, CWnd* pWnd, std::list<CGeom*> geoms, bool original, bool modified)
					{
						for(std::list<CGeom*>::iterator it = geoms.begin(); it!=geoms.end(); it++)
						{
							CGeom* pGeom = *it;
							pGeom->m_bLocked = modified;
						}
					}

					static void Undo_Modify_ReplaceAll(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<CGeom*> original, std::list<CGeom*> modified)
					{
						pLayer->m_geomlist.RemoveAll();
						for(std::list<CGeom*>::iterator it = original.begin(); it!=original.end(); it++)
						{
							CGeom* pGeom = *it;
							pLayer->m_geomlist.AddTail(pGeom);
						}
						pLayer->Invalid(document);
					}
					static void Redo_Modify_ReplaceAll(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<CGeom*> original, std::list<CGeom*> modified)
					{
						pLayer->m_geomlist.RemoveAll();
						for(std::list<CGeom*>::iterator it = modified.begin(); it!=modified.end(); it++)
						{
							CGeom* pGeom = *it;
							pLayer->m_geomlist.AddTail(pGeom);
						}
						pLayer->Invalid(document);
					}
					static void Free_CreateOrDelete_Geoms(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<CGeom*>& geoms)
					{
						POSITION pos = document.GetFirstViewPosition();
						CView* pView = document.GetNextView(pos);

						for(std::list<CGeom*>::iterator it = geoms.begin(); it!=geoms.end(); it++)
						{
							CGeom* pGeom = *it;
							
							pView->SendMessage(WM_PREDELETEGEOM, (UINT)pLayer, (LONG)pGeom);
							delete pGeom;
							pGeom = nullptr;
						}
					}
				}
			}
		}
	}
}