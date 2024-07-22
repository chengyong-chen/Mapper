#include "StdAfx.h"
#include "GSingleDocTemplate.h"
#include "GRuntimeClass.h"
#include "GCreateContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(GSingleDocTemplate, CSingleDocTemplate)

GSingleDocTemplate::GSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, GRuntimeClass* pViewClass)
	: CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, (CRuntimeClass*)pViewClass)
{
	m_pGWFirstViewClass = pViewClass;
	m_pGWSecondViewClass = nullptr;
}

GSingleDocTemplate::GSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, GRuntimeClass* pFirstViewClass, GRuntimeClass* pSecondViewClass)
	: CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, (CRuntimeClass*)pFirstViewClass)
{
	m_pGWFirstViewClass = pFirstViewClass;
	m_pGWSecondViewClass = pSecondViewClass;
}

GSingleDocTemplate::~GSingleDocTemplate()
{
}

CFrameWnd* GSingleDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	if(pDoc!=nullptr)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	ASSERT(m_nIDResource!=0); // must have a resource ID to load from
	GCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = this;
	context.m_pFirstViewClass = this->m_pGWFirstViewClass;
	context.m_pSecondViewClass = this->m_pGWSecondViewClass;

	if(m_pFrameClass==nullptr)
	{
		TRACE(traceAppMsg, 0, "Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);
		return nullptr;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if(pFrame==nullptr)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return nullptr;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if(context.m_pNewViewClass==nullptr)
		TRACE(traceAppMsg, 0, "Warning: creating frame with no default view.\n");

	// create new from resource
	if(!pFrame->LoadFrame(m_nIDResource,
		WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, // default frame styles
		nullptr, &context))
	{
		TRACE(traceAppMsg, 0, "Warning: CDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return nullptr;
	}

	// it worked !
	return pFrame;
}
