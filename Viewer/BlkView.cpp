#include "stdafx.h"
#include <afxpriv.h>
#include <math.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "Viewer.h"
#include "Global.h"



#include "BlkView.h"
#include "BlkDoc.h"
#include "MainFrm.h"

#include "../Style/Global.h"
#include "../Style/Block.h"

#include "..\information\Global.h"

#include "../Tool/Global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBlkView, CGisView)

BEGIN_MESSAGE_MAP(CBlkView, CGisView)
	//{{AFX_MSG_MAP(CBlkView)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

CBlkView::CBlkView()
{
}

CBlkView::~CBlkView()
{			
}

#ifdef _DEBUG
void CBlkView::AssertValid() const
{
	CGisView::AssertValid();
}

void CBlkView::Dump(CDumpContext& dc) const
{
	CGisView::Dump(dc);
}

#endif//_DEBUG