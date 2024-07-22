#pragma once

#include <afxwin.h>

struct GRuntimeClass;
struct CRuntimeClass;

class __declspec(dllexport) GSingleDocTemplate : public CSingleDocTemplate
{
	DECLARE_DYNAMIC(GSingleDocTemplate)
public:
	GSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, GRuntimeClass* pViewClass);
	GSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, GRuntimeClass* pFirstViewClass, GRuntimeClass* pSecondViewClass);
	~GSingleDocTemplate() override;

private:
	GRuntimeClass* m_pGWFirstViewClass;
	GRuntimeClass* m_pGWSecondViewClass;

	CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) override;
};
