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
					namespace PRect
					{	
						static void Undo_Modify_Anchors(CDocument& document,CWnd* pWnd,CPRect* pPRect,std::map<unsigned int,CPoint> original,std::map<unsigned int,CPoint> modified)
						{
							pPRect->Invalid(document);
							for(std::map<unsigned int,CPoint>::iterator it=original.begin(); it!=original.end(); it++)
							{
								pPRect->m_pPoints[it->first] = it->second;
							}
							pPRect->RecalRect();
							pPRect->m_bModified = true;
							pPRect->Invalid(document);
						}
						static void Redo_Modify_Anchors(CDocument& document,CWnd* pWnd,CPRect* pPRect,std::map<unsigned int,CPoint> original,std::map<unsigned int,CPoint> modified)
						{
							pPRect->Invalid(document);
							for(std::map<unsigned int,CPoint>::iterator it=modified.begin(); it!=modified.end(); it++)
							{
								pPRect->m_pPoints[it->first] = it->second;
							}
							pPRect->RecalRect();
							pPRect->m_bModified = true;
							pPRect->Invalid(document);
						}
					}
				}
			}
		}
	}
}