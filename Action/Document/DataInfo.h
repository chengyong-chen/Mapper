#pragma once

#include <list>
using namespace std;

namespace Undoredo
{
	namespace Document
	{
		namespace Datainfo
		{
			static void Undo_Modify(CDocument& document,CWnd* pWnd,CDatainfo& datainfo,CDatainfo original,CDatainfo modified)
			{
				datainfo = original;
				document.UpdateAllViews(nullptr);
				document.SetModifiedFlag(TRUE);		
			}
			static void Redo_Modify(CDocument& document,CWnd* pWnd,CDatainfo& datainfo,CDatainfo original,CDatainfo modified)
			{
				datainfo = modified;
				document.UpdateAllViews(nullptr);
				document.SetModifiedFlag(TRUE);		
			}
		}
	}
}
