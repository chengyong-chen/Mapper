#include "stdafx.h"
#include "Resource.h"

#include "Global.h"

#include "ShowTool.h"
#include "../Geometry/Global.h"

__declspec(dllexport) CShowTool showTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CShowTool::OnLButtonDown(CWnd* pWnd,CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd,viewinfo, nFlags, docPoint);
	
	DWORD dwRec = pWnd->SendMessage(WM_PICKSPEINFO,nFlags,(LONG)&docPoint);
	if(dwRec >= 0)
	{
		pWnd->SendMessage(WM_SHOWSPEINFO,dwRec,0);
	}	
}


bool CShowTool::SetCursor(CWnd* pWnd,CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_INFO);	
	if(hCursor!=nullptr) 
	{
		::SetCursor(hCursor);
		return true;
	}		
	else
		return false;
}
