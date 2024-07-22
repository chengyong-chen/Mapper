#pragma once

#include <map>
#include <list>

using namespace std;

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
					namespace Poly						
					{	
						static void Undo_Append_Anchor(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::pair<CPoint,bool>& anchor)
						{			
							pPoly->Invalid(document);
							pPoly->RemoveAnchor(anchor.second == true ? pPoly->GetAnchors() : 1);	
							pPoly->Invalid(document);
						}
						static void Redo_Append_Anchor(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::pair<CPoint,bool>& anchor)
						{
							pPoly->Invalid(document);
							pPoly->AddAnchor(anchor.first,anchor.second);
							pPoly->Invalid(document);
						}

						static void Undo_Modify_Anchors(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::map<unsigned int,CPoint> original,std::map<unsigned int,CPoint> modified)
						{
							pPoly->Invalid(document);
							for(std::map<unsigned int,CPoint>::iterator it=original.begin(); it!=original.end(); it++)
							{
								pPoly->m_pPoints[it->first] = it->second;
							}
							pPoly->RecalRect();
							pPoly->m_bModified = true;
							pPoly->Invalid(document);
						}
						static void Redo_Modify_Anchors(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::map<unsigned int,CPoint> original,std::map<unsigned int,CPoint> modified)
						{
							pPoly->Invalid(document);
							for(std::map<unsigned int,CPoint>::iterator it=modified.begin(); it!=modified.end(); it++)
							{
								pPoly->m_pPoints[it->first] = it->second;
							}
							pPoly->RecalRect();
							pPoly->m_bModified = true;
							pPoly->Invalid(document);
						}

						static void Undo_Modify_Close(CDocument& document,CWnd* pWnd,CPoly* pPoly,CPoint original,CPoint modified)
						{
							pPoly->Invalid(document);
							pPoly->m_pPoints[pPoly->m_nAnchors-1] = original;
							pPoly->m_bClosed = false;
							pPoly->Invalid(document);
						}
						static void Redo_Modify_Close(CDocument& document,CWnd* pWnd,CPoly* pPoly,CPoint original,CPoint modified)
						{
							pPoly->Invalid(document);
							pPoly->m_pPoints[pPoly->m_nAnchors-1] = modified;
							pPoly->m_bClosed = true;
							pPoly->Invalid(document);
						}

						static void Undo_Modify_ForceClose(CDocument& document,CWnd* pWnd,CPoly* pPoly,CPoint original,CPoint modified)
						{
							pPoly->Invalid(document);
							pPoly->m_nAnchors--;
							pPoly->m_bClosed = false;
							pPoly->Invalid(document);
						}
						static void Redo_Modify_ForceClose(CDocument& document,CWnd* pWnd,CPoly* pPoly,CPoint original,CPoint modified)
						{
							pPoly->Invalid(document);
							pPoly->m_pPoints[pPoly->m_nAnchors  ] = pPoly->m_pPoints[0]; 
							pPoly->m_nAnchors++;
							pPoly->m_bClosed = true;
							pPoly->Invalid(document);
						}

						static void Undo_Modify_Points(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::pair<unsigned int,CPoint*> original,std::pair<unsigned int,CPoint*> modified)
						{			
							pPoly->Invalid(document);
							pPoly->m_nAnchors = original.first;
							pPoly->m_nAllocs = original.first;
							pPoly->m_pPoints = original.second;	
							pPoly->RecalRect();
							pPoly->Invalid(document);
						}
						static void Redo_Modify_Points(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::pair<unsigned int,CPoint*> original,std::pair<unsigned int,CPoint*> modified)
						{			
							pPoly->Invalid(document);
							pPoly->m_nAnchors = modified.first;
							pPoly->m_nAllocs = modified.first;
							pPoly->m_pPoints = modified.second;	
							pPoly->RecalRect();
							pPoly->Invalid(document);
						}
						static void Free_Modify_Points(CDocument& document,CWnd* pWnd,CPoly* pPoly,std::pair<unsigned int,CPoint*> points)
						{	
							CPoint* pPoints = points.second;
							if(pPoints != nullptr)
							{
								delete pPoints;
								pPoints = nullptr;
							}
						}
					}
				}
			}
		}
	}
}