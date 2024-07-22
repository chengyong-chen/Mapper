#include "stdafx.h"
#include "JavascriptMinifier.h"
#include "JavascriptMinifierDlg.h"

#include <regex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CJavascriptMinifier::Invoke(int count,...)
{
	CJavascriptMinifierDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		AfxGetApp()->BeginWaitCursor();
		AfxGetApp()->EndWaitCursor();
	}
}
