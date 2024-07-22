#include "stdafx.h"
#include <stdio.h>
#include "Geom.h"
#include "Widget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWidget, CGeom)

CWidget::CWidget()
{
}

CWidget::~CWidget()
{
}

//BEGIN_MESSAGE_MAP(CWidget, CCmdTarget)
//{{AFX_MSG_MAP(CWidget)   

//}}AFX_MSG_MAP   
//END_MESSAGE_MAP()

//BEGIN_DISPATCH_MAP(CWidget, CCmdTarget)
//{{AFX_DISPATCH_MAP(CWidget)   
//	DISP_FUNCTION(CWidget, "BeginConverter", BeginConverter, VT_BOOL, VTS_BSTR)
//	DISP_FUNCTION(CWidget, "EndConverter", EndConverter, VT_BOOL, VTS_NONE)
//	DISP_FUNCTION(CWidget, "ConverterIges2Kmg", ConverterIges2Kmg, VT_BOOL, VTS_BSTR VTS_BSTR)
//	DISP_FUNCTION(CWidget, "GetProgressInfo", GetProgressInfo, VT_BOOL, VTS_PR8 VTS_PBSTR)
//}}AFX_DISPATCH_MAP   
//END_DISPATCH_MAP()

//BEGIN_INTERFACE_MAP(CWidget, CCmdTarget)
//	INTERFACE_PART(CWidget, IID_IConvIges2Kmg, Dispatch)
//END_INTERFACE_MAP()
