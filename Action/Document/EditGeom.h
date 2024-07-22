#pragma once
#include <map>

using namespace std;

namespace Undoredo
{
	namespace Document
	{
		namespace EditGeom
		{	
			static void Undo_Remove_Anchors(CDocument& document,CWnd* pWnd,CGeom* pGeom,std::map<unsigned int,CPoint>& anchors)
			{
				pGeom->Invalid(document);
				for(std::map<unsigned int,CPoint>::reverse_iterator it = anchors.rbegin(); it != anchors.rend(); it++)
				{
					unsigned int nAnchor = it->first;
					CPoint point = it->second;
					
					pGeom->InsertAnchor(nAnchor,point);
				}
				pGeom->Invalid(document);
			}

			static void Redo_Remove_Anchors(CDocument& document,CWnd* pWnd,CGeom* pGeom,std::map<unsigned int,CPoint>& anchors)
			{	
				pGeom->Invalid(document);
				for(std::map<unsigned int,CPoint>::iterator it = anchors.begin(); it != anchors.end(); it++)
				{
					unsigned int nAnchor = it->first;					
					pGeom->RemoveAnchor(nAnchor);
				}
				pGeom->Invalid(document);
			}
		}
	}
}

				