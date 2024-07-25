#include "stdafx.h"
#include "Style.h"

#include "ColorSpot.h"
#include "ColorProcess.h"
#include "FontCombox.h"
#include "Hint.h"
#include "Type.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CStyleApp, CWinApp)
	//{{AFX_MSG_MAP(CStyleApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CStyleApp::CStyleApp()
{
	CColorSpot::LoadCMM();
	CColorProcess::LoadCMM();
	CFontDesc::LoadFontNames();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStyleApp object

CStyleApp theApp;

BOOL CStyleApp::InitInstance()
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	CType* pType = CType::Default();
	if(pType!=nullptr)
	{
		pType->m_fSize = 10;
		delete pType->m_pColor;
		pType->m_pColor = new CColorSpot(200, 200, 200, 255);
		pType->m_fontdesc.SetByFaceName(ncm.lfMessageFont.lfFaceName);
	}
	CHint* pHint = CHint::Default();
	if(pHint!=nullptr)
	{		
		pHint->m_fSize = 12;
		delete pHint->m_pColor;
		pHint->m_pColor = new CColorSpot(200, 200, 200, 255);
		pHint->m_fontdesc.SetByFaceName(ncm.lfMessageFont.lfFaceName);
	}
	//CColorSpot::Black = new CColorSpot(0, 0, 0, 0);
	return CWinApp::InitInstance();
}

int CStyleApp::ExitInstance()
{
	CColorSpot::ReleaseCMM();
	CColorProcess::ReleaseCMM();
	CType* pType = CType::Default();
	if(pType!=nullptr)
	{
		delete pType->m_pColor;
		pType->m_pColor = nullptr;
	}
	CHint* pHint = CHint::Default();
	if(pHint!=nullptr)
	{
		delete pHint->m_pColor;
		pHint->m_pColor = nullptr;		
	}
	//delete CColorSpot::Black;
	return CWinApp::ExitInstance();
}
