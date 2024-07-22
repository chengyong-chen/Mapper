#pragma once

#include "../Public/ODBCDatabase.h"
#include "../Information/POUList.h"
#include "../Information/POUListCtrl.h"
#include "../Style/Library.h"

#include "Catalog.h"
#include "Pyramid.h"

using namespace std;

class CPoly;
class CPOUListCtrl;
class CRoadTopo;
class CPOUPane;

class __declspec(dllexport) CAtlas : public CObject
{
public:
	CAtlas(CPOUPane* pPOUPane);
	~CAtlas() override;

public:
	CString m_strName;
	CString m_strTitle;
	CString m_strIndexHtml;
	CString m_strHomePage;

	BOOL m_bFree;
	BOOL m_bEdit;
	BOOL m_bRegionQuery;
	BOOL m_bNavigate;
	BOOL m_bHtml;

	CODBCDatabase m_databaseLAB;
	CODBCDatabase m_databasePOI;
public:
	CPOUList m_poulist;
	CPOUListCtrl m_POUListCtrl;

	CRoadTopo* m_pTopo;

public:
	CCatalog m_Catalog;
	CPyramid m_Pyramid;

public:
	CString m_strFilePath;

public:
	bool Open(CString strFile, DWORD dwDBOptions);

	bool Save();
	bool SaveAs(CString strFile);
	void Release();

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void SerializeWEB(CArchive& ar) const;

	void PublishPC();
	void PublishCE();
	void PublishWEB();

public:
	int KeyQuery(CListCtrl* listCtrl, CString strKey, DWORD dwClass, WORD wMap);
	//	int RgnQuery(CListCtrl* listCtrl,CPoly* poly,DWORD dwClass,WORD wMap);
};
