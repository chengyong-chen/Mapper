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
					namespace Geom
					{							
						static void Undo_Modify_Rect(CDocument& document,CWnd* pWnd,CGeom* pGeom,CRect original,CRect modified)
						{
							pGeom->Invalid(document);
							pGeom->m_Rect = original;
							pGeom->Invalid(document);
						}
						static void Redo_Modify_Rect(CDocument& document,CWnd* pWnd,CGeom* pGeom,CRect original,CRect modified)
						{
							pGeom->Invalid(document);
							pGeom->m_Rect = modified;
							pGeom->Invalid(document);
						}
					}
				}
			}
		}
	}
}