#pragma once

#import "..\Mapsvr\Mapsvr.tlb" no_namespace named_guids //raw_interfaces_only

#include "../Style/Library.h"
#include "..\Layer\LayerTree.h"
#include "../Atlas/Datainfo.h"

#include "..\HyperLink\Link.h"
#include "../Information/ODBCDatabase.h"

class CTool;
class CProjection;

// CMapseedCtrl : �й�ʵ�ֵ���Ϣ������� MapseedCtrl.cpp��

class CMapseedCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMapseedCtrl)

// ���캯��
public:
	CMapseedCtrl();

public:
	IMapdcom* m_pIMapdcom;

// ��ͼ��Ա����
public:
	WORD m_wMapId;

	CPaper m_pPaper;
	CLibrary m_library;
	CLayerTree m_layertree;
	CDatainfo m_Datainfo;
	CProjection* m_pProjection;
	
	unsigned int m_uZoom;
	CSize  m_sizeView; 
	CPoint m_ptViewPos;
	CSize  m_extView;

	CRect  m_WindowRect;
	CRect  m_validRect;

public:
	CLinkList m_linklist;
	CODBCDatabase m_database;

public:
	void SetViewSize(const CPoint& docPoint, const CPoint& cliPoint,BOOL bScroll);

public:
	void ClientToDoc(const CPoint& point) const;
	void ClientToDoc(const CSize&  size) const;
	void ClientToDoc(const CRect&  rect) const;

	void DocToMap(const CPoint& docPoint,CPointF& mapPoint) const;
	void MapToGeo(const CPointF& mapPoint,CPointF& geoPoint) const;
	
	void GeoToMap(const CPointF& geoPoint,CPointF& mapPoint) const;
	void MapToDoc(const CPointF& mapPoint,CPoint& docPoint) const;

	void DocToClient(const CPoint& point) const;
	void DocToClient(const CSize&  size) const;
	void DocToClient(const CRect&  rect) const;

	void Draw(COleControl* pControl,CDC* pDC,const CRect& inRect,const unsigned int& uZoom,const float& fZoom,const double& m11,const double& m21,const long& m31,const double& m12,const double& m22,const long& m32);

// ��д
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) override;
	virtual void DoPropExchange(CPropExchange* pPX) override;
	virtual void OnResetState() override;
	virtual DWORD GetControlFlags() override;
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) override;	  
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE) override;

// ʵ��
protected:
	virtual ~CMapseedCtrl();

	DECLARE_OLECREATE_EX(CMapseedCtrl)    // �๤���� guid
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
	
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
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
	afx_msg LONG OnScrollView(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetScale(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomOut(UINT WPARAM, LONG LPARAM);

	DECLARE_MESSAGE_MAP()

// ����ӳ��
protected:
	//{{AFX_DISPATCH(CMapseedCtrl)
	VARIANT_BOOL OpenAtlas(LPCTSTR bstrAtlas);
	VARIANT_BOOL OpenMap(USHORT wMapId, USHORT nZoom);
	VARIANT_BOOL OpenMapByName(LPCTSTR bstrMap);
	VARIANT_BOOL OpenMatchMap(DOUBLE lng, DOUBLE lat);
	VARIANT_BOOL CloseAtlas(void);
	VARIANT_BOOL CloseMap(void);
	void ScrollView(LONG cx, LONG cy, VARIANT_BOOL bUpdate);
	void AimatView(LONG docX, LONG docY, LONG cliX, LONG cliY);
	void SetTool(BYTE bType);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// �¼�ӳ��
	//{{AFX_EVENT(CMapseedCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// ���Ⱥ��¼� ID
public:
	enum {
		dispidGetTool = 10L,		dispidSetTool = 9L,		
		dispidAimatView = 8L,		
		dispidScrollView = 7L,		
		dispidCloseMap = 6L,		
		dispidCloseAtlas = 5L,		
		dispidOpenMatchMap = 4L,		
		dispidOpenMapByName = 3L,		
		dispidOpenMap = 2L,		
		dispidOpenAtlas = 1L
	};
protected:
	BYTE GetTool(void);
};

