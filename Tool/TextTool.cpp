#include "stdafx.h"
#include "Global.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Text.h"
#include "../Geometry/TextEditDlg.h"

#include "Resource.h"
#include "Tool.h"
#include "TextTool.h"

__declspec(dllexport) CTextTool textTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CTextTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	CTextEditDlg dlg(pWnd);
	if(dlg.DoModal()==IDOK&&dlg.m_String.IsEmpty()==FALSE)
	{
		pWnd->SendMessage(WM_COMMAND, 33068, 0);
		CText* text = new CText(docPoint);
		text->m_strName = dlg.m_String;
		pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)text, 0);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

bool CTextTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_TEXT);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
