#pragma once

#include "TreeNode.h"

class __declspec(dllexport) CPage : public CTreeNode1
{
public:
	enum
	{
		typeFolder = 0,
		typeMap,
		typeHtml
	};

	DECLARE_SERIAL(CPage);
public:
	CPage();

	~CPage() override;

public:
	CDocument* Open(CWnd* pWnd, unsigned int nScale) const;

public:
	CString m_strTarget;
	CPoint m_Anchor;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
};

typedef CTypedPtrList<CObList, CPage*> CPageList;
