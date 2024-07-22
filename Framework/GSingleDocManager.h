#pragma once

#include <afxwin.h>

struct GRuntimeClass;
struct CRuntimeClass;

class __declspec(dllexport) GSingleDocManager : public CDocManager
{
	DECLARE_DYNAMIC(GSingleDocManager)

public:
	void OnFileNew() override;

	CDocument* OpenDocumentFile(LPCTSTR lpszFileName) override; // open named file
};
