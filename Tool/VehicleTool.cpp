#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "SelectTool.h"
#include "DispatchTool.h"

__declspec(dllexport) CDispatchTool vehicleTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDispatchTool::OnLButtonDown(CWnd* pWnd,CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd,viewinfo, nFlags, docPoint);	

	actMode = actVehicle;
}

bool CDispatchTool::SetCursor(CWnd* pWnd,CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUND);	
	if(hCursor != nullptr) 
	{
		::SetCursor(hCursor);
		return true;
	}		
	else
		return false;
}
