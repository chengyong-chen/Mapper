#pragma once

class CTriangle;
class CViewinfo;
class CDatum;
#define DllExport   __declspec( dllexport )

class DllExport CTin : public CObject
{
	DECLARE_SERIAL(CTin);
public:
	CTin();
	~CTin() override;

	void Create();
	CPoint Rectify(const CPoint& point);

public:
	CTypedPtrList<CObList, CDatum*> m_datumlist;
	CTypedPtrList<CObList, CTriangle*> m_trianglelist;

private:
	void RemoveIdentical();

public:
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo) const;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo);
public:
	void AddDatum(CDatum* pDatum);
	void RemoveDatum(CDatum* pDatum);
	void Clear();
	void ClearTins();
	CDatum* PickDatum(const CViewinfo& viewinfo, const CPoint& docPoint);

public:
	void Load();
	void Store();
};
