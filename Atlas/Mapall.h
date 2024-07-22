#pragma once

#include "../../Thirdparty\libjson\7.6.1\libjson.h"

class CMapall
{
public:
	CMapall();	
	~CMapall();

public:
	unsigned int m_scaleMinimum;
	unsigned int m_scaleMaximum;
	
	CSize m_sizeMargin;
	CSize m_sizeCanvas;

	CSize m_sizeOffset;

	unsigned char m_bDecimal;

	CDatainfo  m_Datainfo;
	CProjection* m_pProjection;	

	CLibrary  m_library;
	CLayerTree m_layertree;


public:
	DWORD m_maxGeomId;
	WORD  m_wMapId;
	
	DWORD ApplyGeometryId();
	void  FreeGeometryId(CGeom* pGeom);
	

#ifdef MAPPER
	BOOL Setup(CDocument* pDoc,BOOL bSetWindow);
#endif

public:
	void Draw(CWnd* pWnd,Graphics& g,const CRect& inRect);

	void FitInWindow(CWnd* pWnd, float& doctoviewZoom);
	void ViewZoomChanged(CWnd* pWnd,CPoint docPoint,CPoint cliPoint,BOOL bDoScroll);
	
	void ZoomView(CWnd* pWnd, float& doctoviewZoom, float fRatio,CPoint docpoint,CPoint cliPoint);
	BOOL SetViewZoom(CWnd* pWnd, float& doctoviewZoom, float fZoom,CPoint docPoint);

	CSize GetCanvas(){return m_sizeCanvas;};
	CSize GetMargin(){return m_sizeMargin;};
	CSize GetOffset(){return m_sizeOffset;};
	unsigned long GetDecimal();

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	virtual void ReleaseCE(CArchive& ar) const override;
	virtual void ReleaseDCOM(CArchive& ar) override;
	virtual void ReleaseWeb(boost::json::object& json) const override;
	virtual void ReleaseWeb(pbf::writer& writer, CSize offset = CSize(0, 0)) const override;
};