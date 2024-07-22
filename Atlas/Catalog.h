#pragma once

#include "Page.h"

class __declspec(dllexport) CCatalog : public CObject
{
	DECLARE_SERIAL(CCatalog);
public:
	CCatalog();

	~CCatalog() override;

public:
	CPageList m_PageList;

public:
	void Release();

public:
	DWORD GetMaxItemId();

public:
	void Create(CPage* pParent, CString strFold);
	void Filter();

public:
	CPage* GetPage(WORD dwMapID);
	CPage* GetPageByMapFile(CString strMapFile);
	CPage* GetPageByMapName(CString strMapName);

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void SerializeWEB(CArchive& ar) const;

	static void PublishPC();

	static void PublishCE();

	static void PublishWEB();
};
