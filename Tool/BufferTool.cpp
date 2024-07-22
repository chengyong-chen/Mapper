#include "stdafx.h"
#include "resource.h"

#include "../Geometry/Geom.h"

#include "BufferTool.h"

__declspec(dllexport) CBufferTool bufferTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CBufferTool::Extent = 10;

bool CBufferTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUND);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
