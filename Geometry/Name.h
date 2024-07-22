#pragma once

class CPRect;

class CName : public CObject
{
protected:
	DECLARE_SERIAL(CName);
	CName();

	~CName() override;

	// Constructors
public:
	CName(LPCTSTR lpszAnno, CPoint* points, DWORD dwPosCount, DWORD dwWriteStyle,DWORD dwFontStyle, DWORD dwFontSize, COLORREF colFontColor, LPCTSTR lpctFontName, BYTE bCharSet, BYTE bPitchAndFamily);
	CName(CPoint* points, DWORD dwPosCount, const ANNOTATIONFORMAT& af);

	void AddPoint(const CPoint& point, CGrfView* pView = nullptr, bool bIsRePaint = TRUE);
	bool RecalcBounds(CGrfView* pView = nullptr, bool bIsRePaint = TRUE);

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void Draw(CDC* pDC, const COLORREF* pColor = nullptr) const override;
	virtual void DrawTransparent(CGrfView* pView) override;
	virtual void Mark(CDC* pDC, UINT state = 0) override;
	virtual bool Intersects(const CRect& rect, CGrfView* pView) const override;
	virtual bool IntersectsWithRect(const CRect& rect, CGrfView* pView) const override;

	virtual CPoint* GetLocationData();
	virtual unsigned short GetLocationDataCount();
	virtual DWORD GetFontStyle() override { return m_dwFontStyle; }
	virtual DWORD GetWriteStyle() override { return m_dwWriteStyle; }

	virtual unsigned short GetAnnotationCount() { return m_strAnno.GetLength()/2; }
	virtual CString GetGeomName() override { return m_strAnno; };

	bool GetAnnotationRgn(CRgn& rgn);
	bool DrawAnnotationFrame(CDC* pDC);
#ifdef _DEBUG
	void AssertValid() const override;
#endif

	// Attributes
public:
	unsigned short m_nPoints;
	long m_nAllocs;
	CPoint* m_points;
};
