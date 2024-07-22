#include "stdafx.h"
#include "PyrDoc.h"

#include "../Project/Projection1.h"

IMPLEMENT_DYNCREATE(CPyrDoc,CGisDoc)

BEGIN_MESSAGE_MAP(CPyrDoc,CGisDoc)
	//{{AFX_MSG_MAP(CPyrDoc)
	//}}AFX_MSG_MAP)
END_MESSAGE_MAP()


CPyrDoc::CPyrDoc()
{
	m_Mapinfo.m_mapOrigin = CPointF(0,0);
	m_Mapinfo.m_mapCanvas = CSizeF(0,0);
	m_Mapinfo.m_scaleMinimum = 100000000;
	m_Mapinfo.m_scaleMaximum = 2000;
	m_Mapinfo.m_scaleSource  = 1000000;
	m_Mapinfo.m_pProjection = new CProjection();
	m_Mapinfo.m_pProjection->m_nGCSType = 1;
}

CPyrDoc::~CPyrDoc()
{
}

#ifdef _DEBUG
void CPyrDoc::AssertValid() const
{
	CGisDoc::AssertValid();
}

void CPyrDoc::Dump(CDumpContext& dc) const
{
	CGisDoc::Dump(dc);
}
#endif //_DEBUG


BOOL CPyrDoc::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{	
	if(pHandlerInfo == nullptr)
	{
	}

	return CGisDoc::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

