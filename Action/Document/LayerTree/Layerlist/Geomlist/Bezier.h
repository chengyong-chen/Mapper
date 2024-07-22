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
					namespace Bezier
					{	
						static void Undo_Append_Anchor(CDocument& document,CWnd* pWnd,CBezier* pBezier,std::pair<std::list<CPoint>,bool>& anchor)
						{			
							pBezier->Invalid(document);
							pBezier->RemoveAnchor(anchor.second == true ? pBezier->GetAnchors() : 1);
							pBezier->Invalid(document);
						}
						static void Redo_Append_Anchor(CDocument& document,CWnd* pWnd,CBezier* pBezier,std::pair<std::list<CPoint>,bool>& anchor)
						{
							CPoint points[3];
							std::list<CPoint> pointlist = anchor.first;
							std::list<CPoint>::iterator it = pointlist.begin();
							for(int index=0;index<3;index++)
							{					 								
								points[index] = *it;
								it++;							
							}
							pBezier->Invalid(document);
							pBezier->AddAnchor(points,anchor.second);
							pBezier->Invalid(document);
						}
							
						static void Undo_Modify_Control(CDocument& document,CWnd* pWnd,std::pair<CGeom*,std::pair<unsigned int,unsigned char>> geom,CSize original,CSize modified)
						{			
							CGeom* pGeom = geom.first;
							std::pair<unsigned int,unsigned char> handle = geom.second;

							pGeom->Invalid(document);
							pGeom->ChangeContrl(handle,original);
							pGeom->Invalid(document);
						}
						static void Redo_Modify_Control(CDocument& document,CWnd* pWnd,std::pair<CGeom*,std::pair<unsigned int,unsigned char>> geom,CSize original,CSize modified)
						{			
							CGeom* pGeom = geom.first;
							std::pair<unsigned int,unsigned char> handle = geom.second;

							pGeom->Invalid(document);
							pGeom->ChangeContrl(handle,modified);
							pGeom->Invalid(document);
						}
						static void Undo_Modify_Close(CDocument& document,CWnd* pWnd,CBezier* pBezier,std::map<unsigned int,CPoint> original,std::map<unsigned int,CPoint> modified)
						{		
							pBezier->Invalid(document);
							Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Anchors(document,pWnd,pBezier,original,modified);
							pBezier->m_bClosed = false;
							pBezier->Invalid(document);
						}
						static void Redo_Modify_Close(CDocument& document,CWnd* pWnd,CBezier* pBezier,std::map<unsigned int,CPoint> original,std::map<unsigned int,CPoint> modified)
						{
							pBezier->Invalid(document);
							Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Anchors(document,pWnd,pBezier,original,modified);
							pBezier->m_bClosed = true;
							pBezier->Invalid(document);
						}

						static void Undo_Modify_ForceClose(CDocument& document,CWnd* pWnd,CBezier* pBezier,CPoint original,CPoint modified)
						{
							pBezier->Invalid(document);
							pBezier->m_nAnchors--;
							pBezier->m_bClosed = false;
							pBezier->Invalid(document);
						}
						static void Redo_Modify_ForceClose(CDocument& document,CWnd* pWnd,CBezier* pBezier,CPoint original,CPoint modified)
						{
							pBezier->Invalid(document);
							pBezier->m_pPoints[pBezier->m_nAnchors*3  ] = pBezier->m_pPoints[0];
							pBezier->m_pPoints[pBezier->m_nAnchors*3+1] = pBezier->m_pPoints[1];
							pBezier->m_pPoints[pBezier->m_nAnchors*3+2] = pBezier->m_pPoints[2];
							pBezier->m_nAnchors++;
							pBezier->m_bClosed = true;
							pBezier->Invalid(document);
						}
					}
				}
			}
		}
	}
}