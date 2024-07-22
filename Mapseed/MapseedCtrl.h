#pragma once

#include "../Atlas/Datainfo.h"
#include "../Atlas/Security.h"

#include "../Style/Library.h"

#include "..\Layer\LayerTree.h"

#include "../Information/ODBCDatabase.h"
#include "../Information/POU.h"

#include "..\Hyperlink\Link.h"

#include "..\Traffic\Result.h"

class CAtlas;
class CTool;
class CTrafficPane;
class CTraffic;
class CGridMapFile;
class CGeomMapFile;
class CProjection;

// MapseedCtrl.h : CMapseedCtrl ActiveX �ؼ����������

// CMapseedCtrl : �й�ʵ�ֵ���Ϣ������� MapseedCtrl.cpp��

class CMapseedCtrl : public COleControl
{
	ULONG_PTR m_gdiplusToken;

	DECLARE_DYNCREATE(CMapseedCtrl)

// ���캯��
public:
	CMapseedCtrl();

public:
	CAtlas* m_pAtlas;
	CSecurity m_Security;

// ��ͼ��Ա����
public:
	WORD m_wMapId;
	CGridMapFile* m_pGridMapFile;
	CGeomMapFile* m_pGeomMapFile;


	CPaper m_pPaper;
	CLibrary m_library;
	CLayerTree m_layertree;
	CDatainfo m_Datainfo;
	CProjection* m_pProjection;

protected:
	CSize  m_sizeView; 
	CPoint m_ptViewPos;
	unsigned int m_uZoom;	

	short m_nAngle;


protected:
	Matrix matrixDoctoView;
	Matrix matrixDoctoCli;
	Matrix matrixViewtoDoc;
	Matrix matrixClitoDoc;
	
public:
	CTrafficPane* m_pTrafficPane;
	CResultList m_Resultlist;
	CTraffic* m_pTraffic;

public:	
	CLinkList  m_linklist;

public:
	CODBCDatabase m_databaseOBJ;
	CODBCDatabase m_databasePOU;
	CPOUList  m_poulist;
	CLibrary  m_POUlib;
	CString m_strInfo;
public:
	CGeom* m_pFlash;
	int   m_nDelay;
public:
	CString GetProfileString(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault = nullptr);
	UINT GetProfileInt(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault);

public:
	void SetViewSize(const CPoint& docPoint, const CPoint& cliPoint,BOOL bScroll);

public:
	void ClientToDoc(const CPoint& point) const;
	void ClientToDoc(const CSize&  size) const;
	void ClientToDoc(const CRect&  rect) const;
	void WindowToDoc(const CRect&  rect) const;

	void DocToMap(const CPoint& docPoint,CPointF& mapPoint) const;
	void MapToGeo(const CPointF& mapPoint,CPointF& geoPoint) const;
	
	void GeoToMap(const CPointF& geoPoint,CPointF& mapPoint) const;
	void MapToDoc(const CPointF& mapPoint,CPoint& docPoint) const;

	void DocToClient(const CPoint& point) const;
	void DocToClient(const CSize&  size) const;
	void DocToClient(const CRect&  rect) const;

	void Draw(Gdiplus::Graphics& g,const unsigned int& uZoom,const CRect& inRect);
	void DrawTag(Gdiplus::Graphics& g,const unsigned int& uZoom,const CRect& inRect);

// ��д
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) override;
	virtual void DoPropExchange(CPropExchange* pPX) override;
	virtual void OnResetState() override;
	virtual DWORD GetControlFlags() override;
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) override;	  
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE) override;
	virtual BOOL KillTimer(UINT nIDEvent) override;

// ʵ��
protected:
	virtual ~CMapseedCtrl();

	BEGIN_OLEFACTORY(CMapseedCtrl)        // �๤���� guid
		virtual BOOL VerifyUserLicense() override;
		virtual BOOL GetLicenseKey(DWORD, BSTR FAR*) override;
	END_OLEFACTORY(CMapseedCtrl)

	DECLARE_OLETYPELIB(CMapseedCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMapseedCtrl)     // ����ҳ ID
	DECLARE_OLECTLTYPE(CMapseedCtrl)		// �������ƺ�����״̬

// ��Ϣӳ��
	//{{AFX_MSG(CMapseedCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
 	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LONG OnOpenDocumentFile(UINT WPARAM, LONG LPARAM);	
	afx_msg LONG OnPrepareDC(UINT WPARAM, LONG LPARAM);	  
	afx_msg LONG OnSetMatrix(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSetDrawStatus(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnClientToDoc(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnDocToMap(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnMapToGeo(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGeoToMap(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnMapToDoc(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnDocToClient(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFixateView(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFixateRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnScrollView(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetScale(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomOut(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnTrafficClick(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnShowInfo(UINT WPARAM, LONG LPARAM);	
	afx_msg LONG OnLookGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFlashGeom(UINT WPARAM, LONG LPARAM);
protected:
// Dispatch maps
	//{{AFX_DISPATCH(CMapseedCtrl)
	CString m_strFile;

	VARIANT_BOOL OpenAtlas(LPCTSTR strAtlas);
	void CloseAtlas(void);

	VARIANT_BOOL OpenMapByID(WORD wMapId);
	VARIANT_BOOL OpenMapFile(CString strFile);
	VARIANT_BOOL OpenMap(LPCTSTR strMap);
	void CloseMap(void);

	LONG GetViewPosX(void);
	LONG GetViewPosY(void);	

	USHORT GetZoom(void);
	void SetZoom(USHORT newVal);

	LONG GetViewSizeCX(void);
	LONG GetViewSizeCY(void);

	VARIANT_BOOL MatchMap(DOUBLE lng, DOUBLE lat);

	void ClientToDocPoint(LONG* x, LONG* y);
	void ClientToDocSize(LONG* cx, LONG* cy);
	void ClientToDocRect(LONG* left,LONG* top,LONG* right,LONG* bottom);

	void DocToClientPoint(LONG* x, LONG* y);
	void DocToClientSize(LONG* cx, LONG* cy);
	void DocToClientRect(LONG* left,LONG* top,LONG* right,LONG* bottom);

	void DocToMapPoint(LONG* doxX, LONG* doxY, DOUBLE* mapX, DOUBLE* mapY);
	void MapToDocPoint(DOUBLE* mapX, DOUBLE* mapY,LONG* doxX, LONG* doxY);

	void MapToGeoPoint(DOUBLE* mapX, DOUBLE* mapY,DOUBLE* geoX, DOUBLE* geoY);
	void GeoToMapPoint(DOUBLE* geoX, DOUBLE* geoY,DOUBLE* mapX, DOUBLE* mapY);
	
	void Scroll(LONG cx, LONG cy, VARIANT_BOOL bUpdate);	
	void AimatView(LONG docX, LONG docY, LONG cliX, LONG cliY);

	void SetInfoTool();
	void SetTrafficTool(USHORT status);
	void SetPanTool(void);
	void SetLengthTool(void);
	void SetAreaTool(void);
	void SetZoomInTool(void);
	void SetZoomOutTool(void);
	void SetNullTool(void);
	BSTR GetMapName(void);
	void GetMapSize(LONG* cx, LONG* cy);
	void SetViewZoom(USHORT uZoom);
	BSTR GetRoutelist(void);
	VARIANT_BOOL QueryRouteByStation(LPCTSTR strStart,LPCTSTR strEnd);
	void ShowTraffic(USHORT uZoom);
	BSTR GetInfo(void);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// �¼�ӳ��
	//{{AFX_EVENT(CMapseedCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// ���Ⱥ��¼� ID
public:
	enum {
		dispidGetInfo = 40L,
		dispidSetInfoTool = 39L,
		dispidShowTraffic = 38L,
		dispidQueryRouteByStation = 37L,
		dispidGetRoutelist = 36L,
		dispidSetTrafficTool = 35L,
		dispidSetViewZoom = 34L,	
		dispidGetMapSize = 33L,		
		dispidGetMapName = 32L,		
		dispidMatchIn = 31L,			
		dispidCloseMap = 30L,		
		dispidCloseAtlas = 29L,		
		dispidSetNullTool = 28L,		
		dispidOpenMap = 27L,		
		dispidOpenAtlas = 26L,		
		dispidSetZoomOutTool = 25L,		
		dispidSetZoomInTool = 24L,		
		dispidSetAreaTool = 23L,		
		dispidSetLengthTool = 22L,
		dispidSetPanTool = 21L,		
		dispidAimatView = 20L,		
		dispidScroll = 19L,		
		dispidGeoToMapPoint = 17L,	
		dispidMapToGeoPoint = 16L,

		dispidMapToDocPoint = 15L,
		dispidDocToMapPoint = 14L,
		
		dispidDocToClientRect = 13L,
		dispidDocToClientSize = 12L,
		dispidDocToClientPoint = 11L,
		
		dispidClientToDocRect = 10L,
		dispidClientToDocSize = 9L,
		dispidClientToDocPoint = 8L,

		dispidGetViewSizeCY = 7L,
		dispidGetViewSizeCX = 6L,		
		dispidZoom = 5L,		
		dispidGetViewPosY = 4L,		
		dispidGetViewPosX = 3L,	
		dispidFileName = 1L,
		
		eventidHasInfo = 201L,
		eventidSearchRouteOver = 200L,	
		eventidDrawOver = 1L
		//{{AFX_DISP_ID(CMapseedCtrl)
	//}}AFX_DISP_ID
	};
protected:

	void FireDrawOver(void)
	{
		FireEvent(eventidDrawOver, EVENT_PARAM(VTS_NONE));
	}
	void FireSearchRouteOver(void)
	{
		FireEvent(eventidSearchRouteOver, EVENT_PARAM(VTS_NONE));
	}
	void FireHasInfo(void)
	{
		FireEvent(eventidHasInfo, EVENT_PARAM(VTS_NONE));
	}
};