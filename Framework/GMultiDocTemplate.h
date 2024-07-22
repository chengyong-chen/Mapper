#pragma once

#include <afxwin.h>

class GRuntimeClass;

class __declspec(dllexport) GMultiDocTemplate : public CMultiDocTemplate
{
	DECLARE_DYNAMIC(GMultiDocTemplate)
public:
	GMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, GRuntimeClass* pViewClass, GRuntimeClass* pAidClass);
	~GMultiDocTemplate() override;

private:
	GRuntimeClass* m_pGWNewViewClass;
	GRuntimeClass* m_pGWAidViewClass;

	CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) override;
public:
	virtual void CloseAllDocumentsWithSaveModified(BOOL bEndSession);
};
