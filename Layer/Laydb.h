#pragma once

#include "Layer.h"
#include "../Database/Datasource.h"

class AFX_EXT_CLASS CLaydb abstract : public CLayer
{
public:
	CLaydb(CTree<CLayer>& tree);
	CLaydb(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);

	~CLaydb() override;

public:
	CGEODatasource m_geoDatasource;
	CGEODatasource& GetGeoDatasource() { return m_geoDatasource; };

public:
	mutable CRectF m_lastRect;

public:
	void AddHead(CWnd* pWnd, const CViewinfo& viewinfo, CLayer* from, Undoredo::CActionStack& actionstack) override
	{
	};

	void AddTail(CWnd* pWnd, const CViewinfo& viewinfo, CLayer* from, Undoredo::CActionStack& actionstack) override
	{
	};
	BYTE Gettype() const override { return 5; };

	virtual void Group(CWnd* pWnd, const CViewinfo& viewinfo)
	{
	};

	virtual void UnGroup(CWnd* pWnd, const CViewinfo& viewinfo)
	{
	};

	virtual bool Combine(CWnd* pWnd, Gdiplus::Matrix& matrixDoctoCli) { return false; };
	virtual bool Disband(CWnd* pWnd) { return false; };

public:
	bool CanActivateGeom(const CGeom* pGeom) const override;
	void PreRemoveGeom(CGeom* pGeom) const override;
	void PreInactivateGeom(const CDatainfo& datainfo, CGeom* pGeom) const override;
	void NewGeomDrew(CGeom* pGeom) override;
	void NewGeomBred(CGeom* pGeom) override;
	void GeomActivated(const CGeom* pGeom) const override;

	virtual void Dispose(CWnd* pWnd, const CDatainfo& datainfo, const CRectF& geoRect);
	void Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect) override;

public:
	virtual void Invalidate(CDocument* pDocument) const;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms = false) override;

public:
	static DWORD DuplicateRec(CDatabase* pDatabase, const CString& strTable, DWORD dwSourceID, DWORD dwTargetID);

	static DWORD GetMaxId(CDatabase* pDatabase, CString strTable);
};
