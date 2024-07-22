#include "StdAfx.h"
#include "GMultiDocTemplate.h"
#include "GRuntimeClass.h"
#include "GCreateContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(GMultiDocTemplate, CMultiDocTemplate)

GMultiDocTemplate::GMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, GRuntimeClass* pViewClass, GRuntimeClass* pAidClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, (CRuntimeClass*)pViewClass)
{
	m_pGWNewViewClass = pViewClass;
	m_pGWAidViewClass = pAidClass;
}

GMultiDocTemplate::~GMultiDocTemplate()
{
}

CFrameWnd* GMultiDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	if(pDoc!=nullptr)
		ASSERT_VALID(pDoc);

	ASSERT(m_nIDResource!=0); // must have a resource ID to load from
	GCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = this;
	context.m_pFirstViewClass = this->m_pGWNewViewClass;
	context.m_pSecondViewClass = this->m_pGWAidViewClass;

	if(m_pFrameClass==nullptr)
	{
		TRACE(traceAppMsg, 0, "Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);
		return nullptr;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if(pFrame==nullptr)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n", m_pFrameClass->m_lpszClassName);
		return nullptr;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if(context.m_pNewViewClass==nullptr)
		TRACE(traceAppMsg, 0, "Warning: creating frame with no default view.\n");

	if(!pFrame->LoadFrame(m_nIDResource, WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, nullptr, &context))
	{
		TRACE(traceAppMsg, 0, "Warning: CDocTemplate couldn't create a frame.\n");
		return nullptr;
	}

	return pFrame;
}

void GMultiDocTemplate::CloseAllDocumentsWithSaveModified(BOOL bEndSession)
{
	POSITION pos = GetFirstDocPosition();
	while(pos!=nullptr)
	{
		CDocument* pDoc = GetNextDoc(pos);
		pDoc->SaveModified();
		pDoc->OnCloseDocument();
	}
}
