#include "stdafx.h"
#include "Information.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInformationApp

BEGIN_MESSAGE_MAP(CInformationApp, CWinApp)
	//{{AFX_MSG_MAP(CInformationApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInformationApp construction

CInformationApp::CInformationApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CInformationApp object

CInformationApp theApp;

BOOL CInformationApp::InitInstance()
{
	::OleInitialize(nullptr);
	::AfxEnableControlContainer();

	return CWinApp::InitInstance();
}
