#pragma once

#include <map>
#include <list>

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
					namespace Collection
					{
						static void Free_Ungrouped_Group(CDocument& document, CWnd* pWnd, CLayer* layer, std::pair<CGeom*, unsigned int>& geom)
						{
							CGeom* pGeom = geom.first;
							if(pGeom!=nullptr)
							{
								if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup))==TRUE)
								{
									CGroup* pGroup = (CGroup*)pGeom;
									pGroup->m_geomlist.RemoveAll();
								}
								else if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut))==TRUE)
								{
									CDoughnut* pDonut = (CDoughnut*)pGeom;
									pDonut->m_geomlist.RemoveAll();
								}
								else if(pGeom->IsKindOf(RUNTIME_CLASS(CClip))==TRUE)
								{
									CClip* pClip = (CClip*)pGeom;
									pClip->m_geomlist.RemoveAll();
								}
								else if(pGeom->IsKindOf(RUNTIME_CLASS(CMask))==TRUE)
								{
									CMask* pMask = (CMask*)pGeom;
									pMask->m_geomlist.RemoveAll();
								}
							}
							Free_Removed_Geom(document, pWnd, layer, geom);
						}
						static void Free_Ungroup_Groups(CDocument& document, CWnd* pWnd, CLayer* pLayer, std::list<std::pair<CGeom*, unsigned int>>& geoms)
						{
							for(std::list<std::pair<CGeom*, unsigned int>>::reverse_iterator it = geoms.rbegin(); it!=geoms.rend(); it++)
							{
								CGeom* pGeom = it->first;
								if(pGeom!=nullptr)
								{
									if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup))==TRUE)
									{
										CGroup* pGroup = (CGroup*)pGeom;
										pGroup->m_geomlist.RemoveAll();
									}
									else if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut))==TRUE)
									{
										CDoughnut* pDonut = (CDoughnut*)pGeom;
										pDonut->m_geomlist.RemoveAll();
									}
									else if(pGeom->IsKindOf(RUNTIME_CLASS(CClip))==TRUE)
									{
										CClip* pClip = (CClip*)pGeom;
										pClip->m_geomlist.RemoveAll();
									}
								}
							}
							Free_AppendRemove_Geoms(document, pWnd, pLayer, geoms);
						}
					}
				}
			}
		}
	}
}