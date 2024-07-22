#pragma once

#include "afxwin.h"

class __declspec(dllexport) GMultiDocManager : public CDocManager
{
	DECLARE_DYNAMIC(GMultiDocManager)

public:
	GMultiDocManager();

	// Operations
public:

	BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate) override;
	virtual void CloseAllDocumentsWithSaveModified(BOOL bEndSession);
	// Implementation
public:
	~GMultiDocManager() override;
};