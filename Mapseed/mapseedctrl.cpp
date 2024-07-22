// MapseedCtrl.cpp : CMapseedCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "Mapseed.h"
#include "MapseedCtrl.h"
#include "MapseedPropPage.h"


#include "../Viewer/Global.h"

#include "../Style/Color.h"
#include "../Style/Line.h"
#include "../Style/LineNormal.h"
#include "../Style/Fill.h"
#include "../Style/Type.h"
#include "..\Layer\GridMapFile.h"
#include "..\Layer\GeomMapFile.h"

#include "../Tool/Tool.h"
#include "../Tool/Global.h"
#include "../Tool/Tool.h"
#include "../Tool/SelectTool.h"
#include "../Tool/TextTool.h"
#include "../Tool/PRectTool.h"
#include "../Tool/RRectTool.h"
#include "../Tool/EllipseTool.h"
#include "../Tool/PolyTool.h"
#include "../Tool/MarkTool.h"
#include "../Tool/ImageTool.h"
#include "../Tool/ZoomTool.h"
#include "../Tool/PrintTool.h"
#include "../Tool/panTool.h"
#include "../Tool/MarqueeTool.h"
#include "../Tool/RadiusTool.h"
#include "../Tool/BoundTool.h"
#include "../Tool/RegionTool.h"
#include "../Tool/LengthTool.h"
#include "../Tool/AreaTool.h"
#include "../Tool/DispatchTool.h"
#include "../Tool/TrafficTool.h"
#include "../Tool/InfoTool.h"

#include "../Projection/Mercator.h"
#include "../Projection/TMercator.h"
#include "../Projection/Gauss.h"

#include "..\Traffic\TrafficPane.h"
#include "..\Traffic\Result.h"

#include "../Atlas/Ruler.h"
#include "../Atlas/PageItem.h"
#include "../Atlas/Atlas1.h"

#include "../Information/Global.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include "Mapseedctrl.h"

#include <gdiplus.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern __declspec(dllimport) CTool*     pTool;

extern __declspec(dllimport) CArray<RectF, Gdiplus::RectF&> d_oRectArray;

extern __declspec(dllimport) CTool*       pTool;
extern __declspec(dllimport) CSelectTool  selectTool;
extern __declspec(dllimport) CTextTool    textTool;
extern __declspec(dllimport) CPRectTool   prectTool;
extern __declspec(dllimport) CRRectTool   rrectTool;
extern __declspec(dllimport) CEllipseTool ellipseTool;
extern __declspec(dllimport) CPolyTool    polyTool;
extern __declspec(dllimport) CMarkTool    markTool;
extern __declspec(dllimport) CImageTool   imageTool;

extern __declspec(dllimport) CZoomTool    zoomTool;
extern __declspec(dllimport) CPrintTool   printTool;
extern __declspec(dllimport) CPanTool     panTool;
extern __declspec(dllimport) CMarqueeTool marqueeTool;
extern __declspec(dllimport) CRadiusTool  radiusTool;
extern __declspec(dllimport) CBoundTool   boundTool;
extern __declspec(dllimport) CRegionTool  regionTool;
extern __declspec(dllimport) CLengthTool  lengthTool;
extern __declspec(dllimport) CAreaTool    areaTool;
extern __declspec(dllimport) CDispatchTool dispatchTool;
extern __declspec(dllimport) CTrafficTool trafficTool;
extern __declspec(dllimport) CInfoTool infoTool;

IMPLEMENT_DYNCREATE(CMapseedCtrl, COleControl)


// 消息映射
BEGIN_MESSAGE_MAP(CMapseedCtrl, COleControl)
	//{{AFX_MSG_MAP(CMapseedCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()	
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_OPENDOCUMENTFILE,OnOpenDocumentFile)	
	ON_MESSAGE(WM_PREPAREDC,     OnPrepareDC)
	ON_MESSAGE(WM_SETMATRIX,     OnSetMatrix)
	ON_MESSAGE(WM_CLIENTTODOC,   OnClientToDoc)
	ON_MESSAGE(WM_DOCTOMAP,      OnDocToMap)
	ON_MESSAGE(WM_MAPTOGEO,      OnMapToGeo)
	ON_MESSAGE(WM_GEOTOMAP,      OnGeoToMap)
	ON_MESSAGE(WM_MAPTODOC,      OnMapToDoc)
	ON_MESSAGE(WM_DOCTOCLIENT,   OnDocToClient)

	ON_MESSAGE(WM_GETSCALE,      OnGetScale)
	ON_MESSAGE(WM_ZOOMIN,        OnZoomIn)
	ON_MESSAGE(WM_ZOOMOUT,       OnZoomOut)
	ON_MESSAGE(WM_FIXATEVIEW,    OnFixateView)
	ON_MESSAGE(WM_FIXATERECT,    OnFixateRect)
	ON_MESSAGE(WM_SCROLLVIEW,    OnScrollView)
	ON_MESSAGE(WM_TRAFFICCLICK,OnTrafficClick)
	ON_MESSAGE(WM_SHOWINFO,      OnShowInfo)
	ON_MESSAGE(WM_LOOKGEOM,    OnLookGeom)
	ON_MESSAGE(WM_FLASHGEOM,   OnFlashGeom)
END_MESSAGE_MAP()


// 调度映射
BEGIN_DISPATCH_MAP(CMapseedCtrl, COleControl)
	DISP_FUNCTION_ID(CMapseedCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_ID(CMapseedCtrl, "FileName", dispidFileName, m_strFile, VT_BSTR)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetViewPosX", dispidGetViewPosX, GetViewPosX, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetViewPosY", dispidGetViewPosY, GetViewPosY, VT_I4, VTS_NONE)
	DISP_PROPERTY_ID(CMapseedCtrl, "Zoom", dispidZoom, m_uZoom, VT_UI2)
	DISP_PROPERTY_EX_ID(CMapseedCtrl, "ZoomView", dispidZoom, GetZoom, SetZoom, VT_UI2)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetViewSizeCX", dispidGetViewSizeCX, GetViewSizeCX, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetViewSizeCY", dispidGetViewSizeCY, GetViewSizeCY, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "ClientToDocPoint", dispidClientToDocPoint, ClientToDocPoint, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "ClientToDocSize", dispidClientToDocSize, ClientToDocSize, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "ClientToDocRect", dispidClientToDocRect, ClientToDocRect, VT_EMPTY, VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "DocToClientPoint",dispidDocToClientPoint, DocToClientPoint, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "DocToClientSize", dispidDocToClientSize, DocToClientSize, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "DocToClientRect", dispidDocToClientRect, DocToClientRect, VT_EMPTY, VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)

	DISP_FUNCTION_ID(CMapseedCtrl, "DocToMapPoint", dispidDocToMapPoint, DocToMapPoint, VT_EMPTY, VTS_PI4 VTS_PI4 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapseedCtrl, "MapToDocPoint", dispidMapToDocPoint, MapToDocPoint, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "MapToGeoPoint", dispidMapToGeoPoint, MapToGeoPoint, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapseedCtrl, "GeoToMapPoint", dispidGeoToMapPoint, GeoToMapPoint, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapseedCtrl, "Scroll", dispidScroll, Scroll, VT_EMPTY, VTS_I4 VTS_I4 VTS_BOOL)
	DISP_FUNCTION_ID(CMapseedCtrl, "AimatView", dispidAimatView, AimatView, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4)

	DISP_FUNCTION_ID(CMapseedCtrl, "SetPanTool", dispidSetPanTool, SetPanTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetLengthTool", dispidSetLengthTool, SetLengthTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetAreaTool", dispidSetAreaTool, SetAreaTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetZoomInTool", dispidSetZoomInTool, SetZoomInTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetZoomOutTool", dispidSetZoomOutTool, SetZoomOutTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "OpenAtlas", dispidOpenAtlas, OpenAtlas, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CMapseedCtrl, "OpenMap", dispidOpenMap, OpenMap, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetNullTool", dispidSetNullTool, SetNullTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "CloseAtlas", dispidCloseAtlas, CloseAtlas, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "CloseMap", dispidCloseMap, CloseMap, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "MatchMap", dispidMatchIn, MatchMap, VT_BOOL, VTS_R8 VTS_R8)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetMapName", dispidGetMapName, GetMapName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetMapSize", dispidGetMapSize, GetMapSize, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetViewZoom", dispidSetViewZoom, SetViewZoom, VT_EMPTY, VTS_UI2)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetTrafficTool", dispidSetTrafficTool, SetTrafficTool, VT_EMPTY, VTS_UI2)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetRoutelist", dispidGetRoutelist, GetRoutelist, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "QueryRouteByStation", dispidQueryRouteByStation, QueryRouteByStation, VT_BOOL, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CMapseedCtrl, "ShowTraffic", dispidShowTraffic, ShowTraffic, VT_EMPTY, VTS_UI2)
	DISP_FUNCTION_ID(CMapseedCtrl, "SetInfoTool", dispidSetInfoTool, SetInfoTool, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapseedCtrl, "GetInfo", dispidGetInfo, GetInfo, VT_BSTR, VTS_NONE)
END_DISPATCH_MAP()

// 事件映射
BEGIN_EVENT_MAP(CMapseedCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSmileCtrl)
	EVENT_CUSTOM("Outside", FireOutside, VTS_NONE)
	EVENT_CUSTOM("Inside", FireInside, VTS_XPOS_PIXELS  VTS_YPOS_PIXELS)
	//}}AFX_EVENT_MAP
	EVENT_CUSTOM_ID("DrawOver", eventidDrawOver, FireDrawOver, VTS_NONE)
	EVENT_CUSTOM_ID("SearchRouteOver", eventidSearchRouteOver, FireSearchRouteOver, VTS_NONE)
	EVENT_CUSTOM_ID("HasInfo", eventidHasInfo, FireHasInfo, VTS_NONE)
	EVENT_STOCK_MOUSEMOVE()
	EVENT_STOCK_MOUSEDOWN()
	EVENT_STOCK_CLICK()
	EVENT_STOCK_DBLCLICK()
END_EVENT_MAP()



// 属性页

// TODO: 按需要添加更多属性页。请记住增加计数！
BEGIN_PROPPAGEIDS(CMapseedCtrl, 1)
	PROPPAGEID(CMapseedPropPage::guid)
END_PROPPAGEIDS(CMapseedCtrl)



// 初始化类工厂和 guid
IMPLEMENT_OLECREATE_EX(CMapseedCtrl, "MAPSEED.MapseedCtrl.1",0x2a5cca81, 0xdf7f, 0x4a80, 0xbb, 0x90, 0xa8, 0xda, 0xa, 0x28, 0xbf, 0x31)



// 键入库 ID 和版本
IMPLEMENT_OLETYPELIB(CMapseedCtrl, _tlid, _wVerMajor, _wVerMinor)



// 接口 ID
const IID BASED_CODE IID_DMapseed =	{ 0xF119C744, 0xE94A, 0x4DC7, { 0x9A, 0x81, 0xB3, 0xF9, 0x6A, 0x1E, 0x72, 0x11 } };
const IID BASED_CODE IID_DMapseedEvents = { 0xC40570B4, 0xC981, 0x4492, { 0x89, 0x78, 0x76, 0xD8, 0xFF, 0x21, 0x25, 0xEF } };



// 控件类型信息
static const DWORD BASED_CODE _dwMapseedOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMapseedCtrl, IDS_MAPSEED, _dwMapseedOleMisc)



// CMapseedCtrl::CMapseedCtrlFactory::UpdateRegistry -
// 添加或移除 CMapseedCtrl 的系统注册表项
BOOL CMapseedCtrl::CMapseedCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: 验证您的控件是否符合单元模型线程处理规则。
	// 有关更多信息，请参考 MFC 技术说明 64。
	// 如果您的控件不符合单元模型规则，则
	// 必须修改如下代码，将第六个参数从
	// afxRegApartmentThreading 改为 0。

	if(bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MAPSEED,
			IDB_MAPSEED,
			afxRegApartmentThreading,
			_dwMapseedOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// 授权字符串
static const TCHAR BASED_CODE _szLicFileName[] = _T("Mapseed.lic");
static const WCHAR BASED_CODE _szLicString[] =	L"Copyright (c) 2005 ";

// CMapseedCtrl::CMapseedCtrlFactory::VerifyUserLicense -
// 检查是否存在用户许可证
BOOL CMapseedCtrl::CMapseedCtrlFactory::VerifyUserLicense()
{
	return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName,	_szLicString);
}



// CMapseedCtrl::CMapseedCtrlFactory::GetLicenseKey -
// 返回运行时授权密钥
BOOL CMapseedCtrl::CMapseedCtrlFactory::GetLicenseKey(DWORD dwReserved,	BSTR FAR* pbstrKey)
{
	if(pbstrKey == nullptr)
		return FALSE;

	*pbstrKey = SysAllocString(_szLicString);
	return (*pbstrKey != nullptr);
}



// CMapseedCtrl::CMapseedCtrl - 构造函数
CMapseedCtrl::CMapseedCtrl()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
	
	InitializeIIDs(&IID_DMapseed, &IID_DMapseedEvents);

	m_pAtlas = nullptr;;

	m_wMapId = 0;
	pTool = nullptr;

	m_ptViewPos = CPoint(0, 0);
	m_sizeView = CSize(0,0);
	m_uZoom	= 100;

	m_nAngle = 0;

	m_pProjection = nullptr;
	m_pGridMapFile = nullptr;
	m_pGeomMapFile = nullptr;
	
	pTool = &lengthTool;
	pTool = &zoomTool;
	pTool = &panTool;
	zoomTool.m_bOut = false;

	m_pTrafficPane = nullptr;
	m_pTraffic = nullptr;

	m_pFlash = nullptr; 
	m_nDelay = 0;
}

CMapseedCtrl::~CMapseedCtrl()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	CloseMap();

	if(m_pAtlas != nullptr)
	{
		delete m_pAtlas->m_pTopo;
		m_pAtlas->m_pTopo = nullptr;
		
		delete m_pAtlas;
		m_pAtlas = nullptr;
	}

	m_pTraffic = nullptr;

	if(m_pTrafficPane != nullptr)
	{
		m_pTrafficPane->DestroyWindow();
		delete m_pTrafficPane;
		m_pTrafficPane = nullptr;
	}

	POSITION pos = m_Resultlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CTraffic* result = m_Resultlist.GetNext(pos);
		delete result;
	}
	m_Resultlist.RemoveAll();
}
// CMapseedCtrl::OnDraw - 绘图函数

void CMapseedCtrl::OnDraw(CDC* pDC, const CRect& rcBounds, const CRect& rcInvalid)
{
	if(pDC == nullptr)
		return;

	if(this->m_hWnd == nullptr)
		return;

	if(!IsOptimizedDraw())
	{
		// 容器不支持优化绘图。

		// TODO: 如果将任何 GDI 对象选入到设备上下文 *pdc 中，
		//		请在此处恢复先前选定的对象。
	}

	if(rcInvalid.IsRectNull())
		return;
	
	OnPrepareDC(pDC,nullptr);
	
	d_oRectArray.RemoveAll();
	
	if(pDC->IsPrinting() == FALSE)
	{
		CRect cliRect = rcBounds; 
		cliRect.NormalizeRect();
		if(cliRect.IsRectNull())
			return;

		if(pTool != nullptr)
			pTool->Draw(this,pDC);

		unsigned long nDecimal = m_pPaper.GetDecimal();
		
		CRect inRect = ClientToDoc(cliRect);		
		inRect.NormalizeRect();

		Bitmap bufferBmp(cliRect.Width(),cliRect.Height());
		Graphics g(&bufferBmp);
		g.SetPageUnit(UnitPixel);
		g.SetSmoothingMode(SmoothingModeHighQuality);
		g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
		g.SetTransform(&matrixDoctoCli);
		g.Clear(Color::White);

		this->Draw(g,m_uZoom,inRect);
		this->DrawTag(g,m_uZoom,inRect);

		CClientDC dc(this);
		Graphics g1(dc.m_hDC);
		g1.DrawImage(bufferBmp,0,0);
		g1.ReleaseHDC(dc.m_hDC);
	}

	if(pTool != nullptr)
		pTool->Draw(this,pDC);

	if(m_pProjection != nullptr)
	{
		CClientDC dc(this);
		if(m_pTraffic != nullptr)
		{	
			m_pTraffic->Draw(this,&dc);
		}

		if(m_pTrafficPane != nullptr)
		{
			m_pTrafficPane->DrawMark(this);
		}
	}
	d_oRectArray.RemoveAll();
		
	FireDrawOver();
}

void CMapseedCtrl::Draw(Gdiplus::Graphics& g,const unsigned int& uZoom,const CRect& inRect)
{	
	if(m_layertree.m_layerlist.GetCount() < 1)
		return;

	if(inRect.IsRectNull() == TRUE)
		return;

	d_oRectArray.RemoveAll();
	unsigned long nDecimal = m_pPaper.GetDecimal();

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend();++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == FALSE)
			continue;

		layer->Draw(g,&m_library,inRect,uZoom,nDecimal,m_pGridMapFile,m_pGeomMapFile,true);
	}
}

void CMapseedCtrl::DrawTag(Gdiplus::Graphics& g,const unsigned int& uZoom,const CRect& inRect)
{
	unsigned long nDecimal = m_pPaper.GetDecimal();

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend();++it)
	{
		CLayer* layer = *it;
		layer->DrawTag(g,&m_library,inRect,uZoom,nDecimal,true); 
	}
}

// CMapseedCtrl::DoPropExchange - 持久性支持

void CMapseedCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 为每个持久的自定义属性调用 PX_ 函数。
}

// CMapseedCtrl::GetControlFlags -
// 自定义 MFC 的 ActiveX 控件实现的标志。
//
DWORD CMapseedCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// 在活动和不活动状态之间进行转换时，
	//不会重新绘制控件。
	dwFlags |= noFlickerActivate;

	// 控件通过不还原设备上下文中的
	// 原始 GDI 对象，可以优化它的 OnDraw 方法。
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}



// CMapseedCtrl::OnResetState - 将控件重置为默认状态

void CMapseedCtrl::OnResetState()
{
	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	// TODO: 在此重置任意其他控件状态。
}

// CMapseedCtrl::AboutBox - 向用户显示“关于”框
void CMapseedCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_MAPSEED);
	dlgAbout.DoModal();
}

void CMapseedCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonDown(nFlags,point);	
	
	if(pTool != nullptr)
	{	
		CPoint docPoint = ClientToDoc(point);
		pTool->OnLButtonDown(this, nFlags, point);
	}
}

void CMapseedCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	COleControl::OnMouseMove(nFlags,point);
	
	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnMouseMove(this, nFlags, point);
	}
}

BOOL CMapseedCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(::GetKeyState(VK_SHIFT)>=0)
		OnScrollBy(CSize(0,-zDelta/2), TRUE);
	else
		OnScrollBy(CSize(-zDelta/2,0), TRUE);
	
	return COleControl::OnMouseWheel(nFlags, zDelta, pt);
}

void CMapseedCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonUp(nFlags,point);
	
	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnLButtonUp(this, nFlags, point);
	}
}

void CMapseedCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	COleControl::OnLButtonDblClk(nFlags, point);

	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnLButtonDblClk(this, nFlags, point);	
	}
}

void CMapseedCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	COleControl::OnRButtonDown(nFlags, point);
	
	if((nFlags & MK_LBUTTON) == MK_LBUTTON)
		return;

	if(pTool != nullptr && pTool->CanPan() == true)
	{
		CTool::oldTool = pTool;
		pTool = &panTool;
		pTool->SetCursor(this);
	
		ClientToDoc(point);
		pTool->OnLButtonDown(this, nFlags, point);
	}
}

void CMapseedCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	COleControl::OnRButtonUp(nFlags, point);

	if(pTool == &panTool)
	{
		ClientToDoc(point);
		pTool->OnLButtonUp(this, nFlags, point);	

		pTool->Recover(this);
		pTool->SetCursor(this);
	}
}

void CMapseedCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(pTool != nullptr)
	{	
		pTool->OnKeyDown(this,nChar,nRepCnt,nFlags);	
	}

	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CMapseedCtrl diagnostics

BOOL CMapseedCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(pWnd == this && nHitTest == HTCLIENT && pTool != nullptr)
		return pTool->SetCursor(this);
	else
		return COleControl::OnSetCursor(pWnd, nHitTest, message);	
}

// CMapseedCtrl 消息处理程序
void CMapseedCtrl::SetViewSize(const CPoint& docPoint, const CPoint& cliPoint,BOOL bScroll)
{	
	CClientDC dc(this);
	int xDPI = dc.GetDeviceCaps(LOGPIXELSX); 
	int yDPI = dc.GetDeviceCaps(LOGPIXELSY); 

	unsigned long nDecimal = m_pPaper.GetDecimal();
	CRect rect = CRect(0,0,m_pPaper.m_sizeCanvas.cx*nDecimal,m_pPaper.m_sizeCanvas.cy*nDecimal);

	double fViewRotateSin = sin(m_nAngle*3.1415926535/180);
	double fViewRotateCos = cos(m_nAngle*3.1415926535/180);
	
	if(m_nAngle != 0)
	{
		CSize size = rect.CSize();
		CPoint point1;
		point1.x = long(fViewRotateCos*size.cx);
		point1.y = long(fViewRotateSin*size.cx);

		CPoint point2;
		point2.x = long(fViewRotateCos*size.cx-fViewRotateSin*size.cy);
		point2.y = long(fViewRotateSin*size.cx+fViewRotateCos*size.cy);

		CPoint point3;
		point3.x = long(-fViewRotateSin*size.cy);
		point3.y = long( fViewRotateCos*size.cy);

		rect.left = min(min(0,point1.x),min(point2.x,point3.x));
		rect.top = min(min(0,point1.y),min(point2.y,point3.y));
		rect.right = max(max(0,point1.x),max(point2.x,point3.x));
		rect.bottom = max(max(0,point1.y),max(point2.y,point3.y));
	}

	{
		float fScale = ((m_uZoom/100.0f)*xDPI)/(72.0f*nDecimal);

		double m11 = fScale*fViewRotateCos;
		double m21 =-fScale*fViewRotateSin;
		double m12 =-fScale*fViewRotateSin;
		double m22 =-fScale*fViewRotateCos;
		
		float dx =-rect.left  *fScale;
		float dy = rect.bottom*fScale;
		matrixDoctoView.SetElements(m11,m21,m12,m22,dx,dy);
	}

	{
		float fScale = (72.0f*nDecimal)/((m_uZoom/100.0f)*xDPI);

		double m11 = fScale*fViewRotateCos;
		double m21 =-fScale*fViewRotateSin;
		double m12 =-fScale*fViewRotateSin;
		double m22 =-fScale*fViewRotateCos;
		
		float dx = rect.left*fViewRotateCos + rect.bottom*fViewRotateSin;
		float dy =-rect.left*fViewRotateSin + rect.bottom*fViewRotateCos;
		matrixViewtoDoc.SetElements(m11,m21,m12,m22,dx,dy);
	}

	m_ptViewPos = CPoint(0,0);

	float doctoviewZoom = (float)m_uZoom/100;

	m_sizeView.cx = (rect.Width() /(72.0f*nDecimal))*(xDPI*doctoviewZoom);
	m_sizeView.cy = (rect.Height()/(72.0f*nDecimal))*(yDPI*doctoviewZoom);
	m_levelCurrent = log(max(m_sizeView.cx, m_sizeView.cy)/256.f)/log(2.f);

	CRect cliRect;
	GetClientRect(cliRect);
	if(m_sizeView.cx < cliRect.Width())
	{
		m_ptViewPos.x = (m_sizeView.cx - cliRect.Width())/2;
	}
	else
	{
		m_ptViewPos.x = 0;
	}

	if(m_sizeView.cy < cliRect.Height())
	{
		m_ptViewPos.y = (m_sizeView.cy - cliRect.Height())/2;
	}
	else
	{
		m_ptViewPos.y = 0;
	}

	if(docPoint == CPoint(0,0))
	{
		docPoint.x = AfxGetApp()->GetProfileInt(_T("NewView"), _T("DocX"),0);
		docPoint.y = AfxGetApp()->GetProfileInt(_T("NewView"), _T("DocY"),0);

		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocX"),0);
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocY"),0);
	}

	if(docPoint == CPoint(0,0))
	{
		docPoint = CPoint(m_pPaper.m_sizeCanvas.cx*nDecimal/2,m_pPaper.m_sizeCanvas.cy*nDecimal/2);
	}
	if(cliPoint == CPoint(0,0))
	{
		cliPoint=cliRect.CenterPoint();
	}

	CString stringX = AfxGetApp()->GetProfileString(_T("NewView"), _T("GeoX"),nullptr);
	CString stringY = AfxGetApp()->GetProfileString(_T("NewView"), _T("GeoY"),nullptr);
	if(stringX.IsEmpty() == FALSE && stringY.IsEmpty() == FALSE)
	{
		CPointF geoPoint;
		CPointF mapPoint;
		geoPoint.x = _tcstod(stringX,nullptr);
		geoPoint.y = _tcstod(stringY,nullptr);

		this->SendMessage(WM_GEOTOMAP,(UINT)(&geoPoint),(LONG)(&mapPoint));
		this->SendMessage(WM_MAPTODOC,(UINT)(&mapPoint),(LONG)(&docPoint));

		cliPoint.x = AfxGetApp()->GetProfileInt(_T("NewView"), _T("CliX"),0);
		cliPoint.y = AfxGetApp()->GetProfileInt(_T("NewView"), _T("CliY"),0);

		AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),nullptr);
		AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),nullptr);
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),0);
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),0);
	}

	CPoint point(docPoint.x,docPoint.y);
	matrixDoctoView.TransformPoints(&point);

	CSize sizeScroll;
	sizeScroll.cx = point.x - cliPoint.x - m_ptViewPos.x;
	sizeScroll.cy = point.y - cliPoint.y - m_ptViewPos.y;		
	this->OnScrollBy(sizeScroll,bScroll);
}

BOOL CMapseedCtrl::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	if(this->m_hWnd == nullptr)
		return FALSE;

	int xOrig, x;
	int yOrig, y;

	CRect cliRect;
	this->GetClientRect(cliRect);
	
	// adjust current x position
	xOrig = x = m_ptViewPos.x;
	int xMax = m_sizeView.cx - cliRect.Width() + 1;
	if(xMax > 0)
	{
		x += sizeScroll.cx;
		if(x < 0)
			x = 0;
		else if(x > xMax)
			x = xMax;
	}

	// adjust current y position
	yOrig = y = m_ptViewPos.y;
	int yMax = m_sizeView.cy - cliRect.Height() + 1;
	if(yMax > 0)
	{
		y += sizeScroll.cy;
		if(y < 0)
			y = 0;
		else if(y > yMax)
			y = yMax;
	}

	if(bDoScroll == TRUE)
	{
	}
	// did anything change?
	if(x != xOrig || y != yOrig)
	{
		m_ptViewPos.x = x;
		m_ptViewPos.y = y;			
	}

	if(bDoScroll == TRUE)
	{
		CRect rcBounds;
		this->GetClientRect(rcBounds);
		CRect rcInvalid = rcBounds;

		CClientDC pDC(this);
		OnPrepareDC(&pDC,nullptr);

		OnDraw(&pDC,rcBounds,rcInvalid);
	}
	
	if(sizeScroll.cy != 0)
	{
	}

//	if(pTool != nullptr)
	{
	}

	if(x != xOrig || y != yOrig)
		return TRUE;
	else
		return FALSE;
}

LONG CMapseedCtrl::OnFixateView(UINT WPARAM, LONG LPARAM)
{
	unsigned long nDecimal = m_pPaper.GetDecimal();

	CPoint docPoint = *(CPoint*)WPARAM;
	CPoint cliPoint = *(CPoint*)LPARAM;

	CPoint point = CPoint(docPoint.x,docPoint.y);
	matrixDoctoView.TransformPoints(&point);
	
	CSize sizeScroll;
	sizeScroll.cx = point.x-cliPoint.x - m_ptViewPos.x;
	sizeScroll.cy = point.y-cliPoint.y - m_ptViewPos.y;

	return this->OnScrollBy(sizeScroll,TRUE);
}

LONG CMapseedCtrl::OnFixateRect(UINT WPARAM, LONG LPARAM)
{
	CRect docRect = *(CRect*)WPARAM;
	CRect cliRect = docRect;
	DocToClient(cliRect);

	CPoint docPoint = docRect.CenterPoint();
	docPoint.x = docRect.left+docRect.Width()/2;//当zoomRect的值过大时，CenterPoint（）函数执行有误
	docPoint.y = docRect.top+docRect.Height()/2;
	CPoint cliPoint = docPoint;
	DocToClient(cliPoint);

	CRect rect;
	this->GetClientRect(rect);
	cliPoint = rect.CenterPoint();
	float xScale = (float)cliRect.Width()/rect.Width();
	float yScale = (float)cliRect.Height()/rect.Height();
	float fScale = max(xScale,yScale);

	unsigned int uZoom = (unsigned short)(m_uZoom/fScale);
	if(uZoom > m_pPaper.m_viewUpper)
	{
		uZoom = m_pPaper.m_viewUpper;
		uZoom -= 1;
	}
	if(uZoom<1)
	{
		uZoom = 1;
	}

	m_uZoom = uZoom;
	this->SetViewSize(docPoint,cliPoint,TRUE);

	return 1L;
}

LONG CMapseedCtrl::OnScrollView(UINT WPARAM, LONG LPARAM)
{
	CSize sizeScroll((int)WPARAM, (int)LPARAM);
	this->OnScrollBy(sizeScroll,TRUE);

	return 1L;
}

LONG CMapseedCtrl::OnGetScale(UINT WPARAM, LONG LPARAM)
{
	switch(LPARAM)
	{
	case 0:
		return m_currentScale;
		break;
	case 1:
		return m_pPaper.m_scaleMinimum;
		break;
	case 2:
		return m_pPaper.m_scaleMaximum;
		break;
	default:
		return m_currentScale;
		break;
	}
}

void CMapseedCtrl::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if(this->m_hWnd != nullptr)
	{
		CRect cliRect;
		this->GetClientRect(cliRect);
		if(m_sizeView.cx < cliRect.Width())
		{
			m_ptViewPos.x = -(cliRect.Width() - m_sizeView.cx)/2;
		}
		if(m_sizeView.cy < cliRect.Height())
		{
			m_ptViewPos.y = -(cliRect.Height() - m_sizeView.cy)/2;
		}
	}

	pDC->SetMapMode(MM_TEXT);
	pDC->SetViewportOrg(-m_ptViewPos.x,-m_ptViewPos.y);

	int yDPI = pDC->GetDeviceCaps(LOGPIXELSX);
	int xDPI = pDC->GetDeviceCaps(LOGPIXELSY);

	CSize sizeCanvas = m_pPaper.GetCanvas();
	unsigned long nDecimal = m_pPaper.GetDecimal();
	CRect rect = CRect(0,0,sizeCanvas.cx*nDecimal,sizeCanvas.cy*nDecimal);

	double fViewRotateSin = 0.0f;
	double fViewRotateCos = 1.0f;
	if(m_nAngle != 0)
	{
		fViewRotateSin = sin(m_nAngle*3.1415926535/180);
		fViewRotateCos = cos(m_nAngle*3.1415926535/180);

		CSize size = rect.CSize();
		CPoint point1;
		point1.x = long(fViewRotateCos*size.cx);
		point1.y = long(fViewRotateSin*size.cx);

		CPoint point2;
		point2.x = long(fViewRotateCos*size.cx-fViewRotateSin*size.cy);
		point2.y = long(fViewRotateSin*size.cx+fViewRotateCos*size.cy);

		CPoint point3;
		point3.x = long(-fViewRotateSin*size.cy);
		point3.y = long( fViewRotateCos*size.cy);

		rect.left   = min(min(0,point1.x),min(point2.x,point3.x));
		rect.top    = min(min(0,point1.y),min(point2.y,point3.y));
		rect.right  = max(max(0,point1.x),max(point2.x,point3.x));
		rect.bottom = max(max(0,point1.y),max(point2.y,point3.y));
	}

	{
		float fScale = ((m_uZoom/100.0f)*xDPI)/(72.0f*nDecimal);

		double m11 = fScale*fViewRotateCos;
		double m21 =-fScale*fViewRotateSin;
		double m12 =-fScale*fViewRotateSin;
		double m22 =-fScale*fViewRotateCos;
		
		CPoint pos = pDC->GetViewportOrg();
		float dx =-rect.left  *fScale + pos.x;
		float dy = rect.bottom*fScale + pos.y;

		matrixDoctoCli.SetElements(m11,m21,m12,m22,dx,dy);
	}

	{
		float fScale = (72.0f*nDecimal)/((m_uZoom/100.0f)*xDPI);
	
		double m11 = fScale*fViewRotateCos;
		double m21 =-fScale*fViewRotateSin;
		double m12 =-fScale*fViewRotateSin;
		double m22 =-fScale*fViewRotateCos;
	
		CPoint pos = pDC->GetViewportOrg();
		float dx = (rect.left-pos.x*fScale)*fViewRotateCos + (rect.bottom+pos.y*fScale)*fViewRotateSin;
		float dy =-(rect.left-pos.x*fScale)*fViewRotateSin + (rect.bottom+pos.y*fScale)*fViewRotateCos;

		matrixClitoDoc.SetElements(m11,m21,m12,m22,dx,dy);
	}
}

void CMapseedCtrl::ClientToDoc(const CPoint& point)
{
	CPoint point1(point.x,point.y);

	matrixClitoDoc.TransformPoints(&point1);
	
	point.x = point1.x;
	point.y = point1.y;
}

void CMapseedCtrl::ClientToDoc(CSize& size)
{
	PointF point1(0,0);
	PointF point2(size.cx,size.cy);
	
	matrixClitoDoc.TransformPoints(&point1);
	matrixClitoDoc.TransformPoints(&point2);

	size.cx = point2.x - point1.x;
	size.cy = point2.y - point1.y;
}

void CMapseedCtrl::ClientToDoc(CRect& rect)
{
	PointF point1(rect.left,rect.top);
	PointF point2(rect.right,rect.top);
	PointF point3(rect.right,rect.bottom);
	PointF point4(rect.left,rect.bottom);

	matrixClitoDoc.TransformPoints(&point1);
	matrixClitoDoc.TransformPoints(&point2);
	matrixClitoDoc.TransformPoints(&point3);
	matrixClitoDoc.TransformPoints(&point4);

	rect.left   = min(min(point1.x,point2.x),min(point3.x,point4.x));
	rect.top    = min(min(point1.y,point2.y),min(point3.y,point4.y));
	rect.right  = max(max(point1.x,point2.x),max(point3.x,point4.x));
	rect.bottom = max(max(point1.y,point2.y),max(point3.y,point4.y));
	rect.NormalizeRect();
}

void CMapseedCtrl::WindowToDoc(CRect& rect)
{
	PointF point1(rect.left,rect.top);
	PointF point2(rect.right,rect.bottom);
	
	matrixViewtoDoc.TransformPoints(&point1);
	matrixViewtoDoc.TransformPoints(&point2);

	rect.left = point1.x;
	rect.top = point1.y;
	rect.right = point2.x;
	rect.bottom = point2.y;
	rect.NormalizeRect();
}

void CMapseedCtrl::DocToMap(const CPoint& docPoint,CPointF& mapPoint) const
{
	unsigned long nDecimal = m_pPaper.GetDecimal();

	mapPoint = m_Datainfo.DocToMap(docPoint,nDecimal);
}   

LONG CMapseedCtrl::OnClientToDoc(UINT WPARAM, LONG LPARAM)
{
	switch(LPARAM)
	{
	case 1:	
		{
			CPoint* point = (CPoint*)WPARAM;
			ClientToDoc(*point);
		}
		break;
	case 2:
		{
			CSize* size = (CSize*)WPARAM;
			ClientToDoc(*size);
		}
		break;
	case 3:
		{
			CRect* rect = (CRect*)WPARAM;
			ClientToDoc(*rect);
		}
		break;
	}
	
	return 0L;
}

LONG CMapseedCtrl::OnDocToMap(UINT WPARAM, LONG LPARAM)
{
	DocToMap(*(CPoint*)WPARAM,*(CPointF*)LPARAM);
	return 0L;
}

LONG CMapseedCtrl::OnMapToGeo(UINT WPARAM, LONG LPARAM)
{
	CPointF* mapPoint = (CPointF*)WPARAM;
	CPointF* geoPoint = (CPointF*)LPARAM;

	*geoPoint = pDoc->m_Datainfo.MapToGeo(*mapPoint);
	return 0L;
}

void CMapseedCtrl::GeoToMap(const CPointF& geoPoint,CPointF& mapPoint)
{
	if(m_pProjection != nullptr)
	{
		double Z = 0;
		CPointF point = geoPoint;
		m_pProjection->GeoToMap(point.x,point.y,0,mapPoint.x,mapPoint.y,Z);
	}
	else
	{
		mapPoint.x = geoPoint.x;
		mapPoint.y = geoPoint.y;
	}
}

void CMapseedCtrl::MapToDoc(const CPointF& mapPoint,CPoint& docPoint)
{   
	unsigned long nDecimal = m_pPaper.GetDecimal();

	docPoint = m_Datainfo.MapToDoc(mapPoint,nDecimal);
}   

LONG CMapseedCtrl::OnGeoToMap(UINT WPARAM, LONG LPARAM)
{
	GeoToMap(*(CPointF*)WPARAM,*(CPointF*)LPARAM);
	return 0L;
}

LONG CMapseedCtrl::OnMapToDoc(UINT WPARAM, LONG LPARAM)
{
	MapToDoc(*(CPointF*)WPARAM,*(CPoint*)LPARAM);
	return 0L;
}

LONG CMapseedCtrl::OnDocToClient(UINT WPARAM, LONG LPARAM)
{
	switch(LPARAM)
	{
	case 1:	
		{
			CPoint* point = (CPoint*)WPARAM;
			DocToClient(*point);
		}
		break;
	case 2:
		{
			CSize* size = (CSize*)WPARAM;
			DocToClient(*size);
		}
		break;
	case 3:
		{
			CRect* rect = (CRect*)WPARAM;
			DocToClient(*rect);
		}
		break;
	}
	
	return 0L;
}

void CMapseedCtrl::DocToClient(const CPoint& point)
{
	CPoint point1(point.x,point.y);

	matrixDoctoCli.TransformPoints(&point1);

	point.x = point1.x;
	point.y = point1.y;
}

void CMapseedCtrl::DocToClient(CSize& size)
{
	CPoint point1(0,0);
	CPoint point2(size.cx,size.cy);
	
	matrixDoctoCli.TransformPoints(&point1);
	matrixDoctoCli.TransformPoints(&point2);

	size.cx = abs(point2.x - point1.x);
	size.cy = abs(point2.y - point1.y);
}

void CMapseedCtrl::DocToClient(CRect& rect)
{
	PointF point1(rect.left,rect.top);
	PointF point2(rect.right,rect.top);
	PointF point3(rect.right,rect.bottom);
	PointF point4(rect.left,rect.bottom);

	matrixDoctoCli.TransformPoints(&point1);
	matrixDoctoCli.TransformPoints(&point2);
	matrixDoctoCli.TransformPoints(&point3);
	matrixDoctoCli.TransformPoints(&point4);

	rect.left   = min(min(point1.x,point2.x),min(point3.x,point4.x));
	rect.top    = min(min(point1.y,point2.y),min(point3.y,point4.y));
	rect.right  = max(max(point1.x,point2.x),max(point3.x,point4.x));
	rect.bottom = max(max(point1.y,point2.y),max(point3.y,point4.y));
	rect.NormalizeRect();
}

LONG CMapseedCtrl::OnZoomIn(UINT WPARAM, LONG LPARAM)
{
	CMapseedApp* pApp = (CMapseedApp*)AfxGetApp();

	if(m_uZoom == m_pPaper.m_viewUpper)
	{
		CRect zoomRect = *(CRect*)WPARAM;	
		CPoint docPoint = zoomRect.CenterPoint();
		
	docPoint.x = zoomRect.left+zoomRect.Width()/2;//当zoomRect的值过大时，CenterPoint（）函数执行有误
	docPoint.y = zoomRect.top+zoomRect.Height()/2;
		CPointF mapPoint;
		CPointF geoPoint;
		
		if(m_pProjection != nullptr)
		{
			CPoint cliPoint = docPoint;
			DocToClient(zoomRect);
			DocToClient(cliPoint);
		
			this->SendMessage(WM_DOCTOMAP,(UINT)(&docPoint),(LONG)(&mapPoint));
			this->SendMessage(WM_MAPTOGEO,(UINT)(&mapPoint),(LONG)(&geoPoint));
			
			CString stringX;
			CString stringY;
			stringX.Format(_T("%.5f"),geoPoint.x);
			stringY.Format(_T("%.5f"),geoPoint.y);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);
		}
		
		if(m_pAtlas != nullptr && m_pProjection != nullptr)
		{
			CPageItem* pPageItem = m_pAtlas->GetSubItem(m_wMapId,geoPoint);
			if(pPageItem != nullptr)
			{
				return (LONG)pPageItem->Open(this,0);
			}
		}

		AfxGetApp()->WriteProfileString(_T("NewView"),_T("GeoX"),nullptr);
		AfxGetApp()->WriteProfileString(_T("NewView"),_T("GeoY"),nullptr);
		AfxGetApp()->WriteProfileInt(_T("NewView"),_T("CliX"),0);
		AfxGetApp()->WriteProfileInt(_T("NewView"),_T("CliY"),0);
	}
	else if(LPARAM != 0)
	{
		if(LPARAM > m_pPaper.m_viewUpper)
			return 0;	
		if(LPARAM < m_pPaper.m_viewLower)
			return 0;	

		m_pPaper.SetZoom(this,LPARAM,m_uZoom);
	}
	else
	{
		CRect zoomRect = *(CRect*)WPARAM;	
		
		CPoint  docPoint = zoomRect.CenterPoint() ;
		
	docPoint.x = zoomRect.left+zoomRect.Width()/2;//当zoomRect的值过大时，CenterPoint（）函数执行有误
	docPoint.y = zoomRect.top+zoomRect.Height()/2;
		CPoint  cliPoint = docPoint ;
		
		DocToClient(cliPoint);
		DocToClient(zoomRect);

		if(zoomRect.Width() < 5 || zoomRect.Height() < 5)
		{	
			m_pPaper.ZoomView(this,m_uZoom,docPoint,cliPoint,2);
		}
		else
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			cliPoint = clientRect.CenterPoint();
			
			float scaleH = (float) clientRect.Width() /zoomRect.Width();
			float scaleV = (float) clientRect.Height()/zoomRect.Height();
			float scale = min(scaleH, scaleV);

			m_pPaper.ZoomView(this,m_uZoom,docPoint,cliPoint,scale);
		}
	}

	return 0L;
}

LONG CMapseedCtrl::OnZoomOut(UINT WPARAM, LONG LPARAM)
{
	CMapseedApp* pApp = (CMapseedApp*)AfxGetApp();
	if(m_uZoom <= m_pPaper.m_viewLower)
	{
		if(m_pAtlas != nullptr)
		{
			CPageItem* pPageItem = m_pAtlas->GetParentMap(m_wMapId);
			if(pPageItem != nullptr && pPageItem->m_wType == CPageItem::typeMap)
			{
				if(m_pProjection != nullptr)
				{		
					CRect cliRect;
					this->GetClientRect(cliRect);
					CPoint cliPoint = cliRect.CenterPoint();
					CPoint docPoint = cliPoint;
					CPointF mapPoint;
					CPointF geoPoint;
					
					ClientToDoc(docPoint);
					this->SendMessage(WM_DOCTOMAP,(UINT)(&docPoint),(LONG)(&mapPoint));
					this->SendMessage(WM_MAPTOGEO,(UINT)(&mapPoint),(LONG)(&geoPoint));
					
					CString stringX;
					CString stringY;
					stringX.Format(_T("%.5f"),geoPoint.x);
					stringY.Format(_T("%.5f"),geoPoint.y);
					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);
				}

				return  (LONG)pPageItem->Open(this,1000);
			}
		}
	}
	else
	{
		CRect zoomRect = *(CRect*)WPARAM;	
		
		CPoint  docPoint = zoomRect.CenterPoint();
		
	docPoint.x = zoomRect.left+zoomRect.Width()/2;//当zoomRect的值过大时，CenterPoint（）函数执行有误
	docPoint.y = zoomRect.top+zoomRect.Height()/2;
		CPoint  cliPoint = docPoint;
		
		DocToClient(cliPoint);
		DocToClient(zoomRect);
		
		m_pPaper.ZoomView(this,m_uZoom,docPoint,cliPoint,0.5);
	}
	
	return 0L;
}

LONG CMapseedCtrl::OnPrepareDC(UINT WPARAM, LONG LPARAM)
{
	CDC* pDC = (CDC*)WPARAM;
	CPrintInfo* pInfo = (CPrintInfo*)LPARAM;

	OnPrepareDC(pDC,pInfo);

	return 1L;
}

LONG CMapseedCtrl::OnSetMatrix(UINT WPARAM, LONG LPARAM)
{
	Matrix* matrix = (Gdiplus::Matrix*)WPARAM;
	if(matrix == nullptr)
		return 0;

	switch(LPARAM)
	{
	case 0:
		{
			REAL m[6];
			matrixDoctoCli.GetElements(m);
			matrix->SetElements(m[0],m[1],m[2],m[3],m[4],m[5]) ;
		}
		break;
	case 1:
		{
			REAL m[6];
			matrixDoctoView.GetElements(m);
			matrix->SetElements(m[0],m[1],m[2],m[3],m[4],m[5]) ;
		}
		break;
	case 2:
		{
			REAL m[6];
			matrixClitoDoc.GetElements(m);
			matrix->SetElements(m[0],m[1],m[2],m[3],m[4],m[5]) ;
		}
		break;	
	}
	return 1L;
}


LONG CMapseedCtrl::GetViewPosX(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_ptViewPos.x;
}

LONG CMapseedCtrl::GetViewPosY(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_ptViewPos.y;
}

USHORT CMapseedCtrl::GetZoom(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_uZoom;
}

void CMapseedCtrl::SetZoom(USHORT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	m_uZoom = newVal;
}

LONG CMapseedCtrl::GetViewSizeCX(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_sizeView.cx;
}

LONG CMapseedCtrl::GetViewSizeCY(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_sizeView.cy;
}

void CMapseedCtrl::ClientToDocPoint(LONG* x, LONG* y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint point = CPoint(*x,*y);  

	ClientToDoc(point);

	*x = point.x;
	*y = point.y;
}

void CMapseedCtrl::ClientToDocSize(LONG* cx, LONG* cy)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CSize size = CSize(*cx,*cy);  

	ClientToDoc(size);

	*cx = size.cx;
	*cy = size.cy;
}
void CMapseedCtrl::ClientToDocRect(LONG* left,LONG* top,LONG* right,LONG* bottom)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect = CRect(*left,*top,*right,*bottom);  
	
	ClientToDoc(rect);

	*left = rect.left;
	*top = rect.top;
	*right = rect.right;
	*bottom = rect.bottom;

}

void CMapseedCtrl::DocToClientPoint(LONG* x, LONG* y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint point = CPoint(*x,*y);  

	DocToClient(point);

	*x = point.x;
	*y = point.y;
}

void CMapseedCtrl::DocToClientSize(LONG* cx, LONG* cy)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CSize size = CSize(*cx,*cy);  

	DocToClient(size);

	*cx = size.cx;
	*cy = size.cy;
}

void CMapseedCtrl::DocToClientRect(LONG* left,LONG* top,LONG* right,LONG* bottom)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect = CRect(*left,*top,*right,*bottom);  
	DocToClient(rect);

	*left = rect.left;
	*top = rect.top;
	*right = rect.right;
	*bottom = rect.bottom;
}

void CMapseedCtrl::DocToMapPoint(LONG* docX, LONG* docY, DOUBLE* mapX, DOUBLE* mapY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint docPoint = CPoint(*docX,*docY);  
	CPointF mapPoint = CPointF(*mapX,*mapY);  

	DocToMap(docPoint,mapPoint);

	*mapX = mapPoint.x;
	*mapY = mapPoint.y;
}

void CMapseedCtrl::MapToDocPoint(DOUBLE* mapX, DOUBLE* mapY,LONG* docX, LONG* docY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPointF mapPoint = CPointF(*mapX,*mapY);  
	CPoint docPoint = CPoint(*docX,*docY);  
	
	MapToDoc(mapPoint,docPoint);

	*docX = docPoint.x;
	*docY = docPoint.y;
}

void CMapseedCtrl::MapToGeoPoint(DOUBLE* mapX, DOUBLE* mapY,DOUBLE* geoX, DOUBLE* geoY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPointF mapPoint = CPointF(*mapX,*mapY);  
	CPointF geoPoint = m_Datainfo.MapToGeo(mapPoint);

	*geoX = geoPoint.x;
	*geoY = geoPoint.y;
}

void CMapseedCtrl::GeoToMapPoint(DOUBLE* geoX, DOUBLE* geoY,DOUBLE* mapX, DOUBLE* mapY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPointF geoPoint = CPointF(*geoX,*geoY);  
	CPointF mapPoint = CPointF(*mapX,*mapY);  
	
	GeoToMap(geoPoint,mapPoint);

	*mapX = mapPoint.x;
	*mapY = mapPoint.y;
}


void CMapseedCtrl::Scroll(LONG cx, LONG cy, VARIANT_BOOL bUpdate)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	OnScrollBy(CSize(cx,cy),bUpdate);
}

void CMapseedCtrl::AimatView(LONG docX, LONG docY, LONG cliX, LONG cliY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint docPoint = CPoint(docX,docY);
	CPoint cliPoint = CPoint(cliX,cliY);
	this->OnFixateView((UINT)&docPoint,(LONG)&cliPoint);
}

void CMapseedCtrl::SetPanTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	pTool = &panTool;
}

void CMapseedCtrl::SetLengthTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	pTool = &lengthTool;
}

void CMapseedCtrl::SetAreaTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	pTool = &areaTool;
}

void CMapseedCtrl::SetZoomInTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	zoomTool.m_bOut = false;
	pTool = &zoomTool;
}

void CMapseedCtrl::SetZoomOutTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect;
	GetClientRect(rect);
	this->ClientToDoc(rect);
	this->SendMessage(WM_ZOOMOUT,(LONG)(&rect),0);

//	zoomTool.m_bOut = true;
//	pTool = &zoomTool;
}

void CMapseedCtrl::SetTrafficTool(USHORT status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if(status == 1)
	{
		CTrafficTool::m_bStart = true;
	}
	else
		CTrafficTool::m_bStart = false;
	
	pTool = &trafficTool;
}

void CMapseedCtrl::SetInfoTool()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	pTool = &infoTool;
}

void CMapseedCtrl::SetNullTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	
	pTool = nullptr;
}

VARIANT_BOOL CMapseedCtrl::OpenAtlas(LPCTSTR lpcstrAtlas)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CloseAtlas();
	
	CString strAtlas = lpcstrAtlas;
	strAtlas.MakeUpper();
	if(strAtlas.Right(4) != _T(".ATL"))
	{
		CString strMapPath = this->GetProfileString(strAtlas,_T("Map"),_T("Path"),nullptr);
		if(strMapPath.IsEmpty() == FALSE)
		{
			if(_taccess(strMapPath,00) == -1)
			{
				CString string;
				string.Format(_T("The directory %s wasn't found!\nUnable to use this software!"),strMapPath);
				AfxMessageBox(string);
				return VARIANT_FALSE;
			}
		}

		strAtlas = strMapPath + strAtlas + _T(".Atl");
		
		
		CMapseedApp* pApp = (CMapseedApp*)AfxGetApp();
		CString strProject = lpcstrAtlas;	
		pApp->SetRegister(strProject);
	}
	else if(_taccess(strAtlas,00) != -1)
	{
		CString strProject = strAtlas;
		strProject =strProject.Left(strProject.GetLength()-4);
		strProject = strProject.Mid(strProject.ReverseFind(_T('\\'))+1);
		CMapseedApp* pApp = (CMapseedApp*)AfxGetApp();
		pApp->SetRegister(strProject);
	}
	else
		return VARIANT_FALSE;
		
	m_pAtlas = new CAtlas;
	if(m_pAtlas->Open(strAtlas,CDatabase::openReadOnly) == false)
	{
		delete m_pAtlas;
		m_pAtlas = nullptr;

		AfxMessageBox(_T("Can't open the Atlas file!"));
		return VARIANT_FALSE;
	}
	else
	{
		m_Security.SetEditRight(m_pAtlas->m_bEdit);
		m_Security.SetRegionQueryRight(m_pAtlas->m_bRegionQuery);
		m_Security.SetBroadcastRight(m_pAtlas->m_bBroadcast);
		m_Security.SetTrafficRight(m_pAtlas->m_bTraffic);
		m_Security.SetNavigateRight(m_pAtlas->m_bNavigate);
		m_Security.SetHtmlRight(m_pAtlas->m_bHtml);
		m_Security.SetHtmlRight(true);
		m_Security.SetDatabaseRight(m_pAtlas->m_bDatabase);

		CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"),_T("GPS Receiver"));
		if(strPort.CompareNoCase(_T("Socket")) == 0)
		{
			m_Security.SetBroadcastRight(false);
			m_Security.SetTracarRight(true);
			m_Security.SetHtmlRight(false);
			m_Security.SetDatabaseRight(false);
			m_Security.SetTrafficRight(false);
		}
	}

	return VARIANT_TRUE;
}

LONG CMapseedCtrl::OnOpenDocumentFile(UINT WPARAM, LONG LPARAM)
{
	CString strFile = *(CString*)WPARAM;
	if(strFile == m_strFile)
	{
	
	}
	else if(_taccess(strFile,00) == -1)
	{
		AfxMessageBox(_T("Map file dosen't exist!"));
		return nullptr;
	}
	else 
	{
		m_uZoom = LPARAM;
		OpenMap(strFile);
	}

	return nullptr;
}

VARIANT_BOOL CMapseedCtrl::OpenMapByID(WORD wMapId)
{
	if(m_pAtlas == nullptr)
		return FALSE;

	if(m_wMapId == wMapId)
		return TRUE;

	CPageItem* pPageItem = m_pAtlas->GetItem(wMapId);
	if(pPageItem != nullptr)
	{
		m_wMapId = wMapId;
		CString strFile = pPageItem->m_strFile;

		return OpenMapFile(strFile);
	}
	else
		return  VARIANT_FALSE;
}

VARIANT_BOOL CMapseedCtrl::OpenMap(LPCTSTR strMap)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CloseMap();

	CString strFile = strMap;
	if(m_pAtlas != nullptr)
	{
		if(_taccess(strFile,00) == -1)
		{
			CPageItem* pPageItem = m_pAtlas->GetItemByMapName(strMap);
			if(pPageItem != nullptr)
			{
				m_wMapId = pPageItem->m_wId;
				strFile = pPageItem->m_strFile;
			}
			else
				return  VARIANT_FALSE;
		}
		else
		{
			CPageItem* pPageItem = m_pAtlas->GetItemByMapFile(strMap);
			if(pPageItem != nullptr)
			{
				m_wMapId = pPageItem->m_wId;
				strFile = pPageItem->m_strFile;
			}
			else
				return  VARIANT_FALSE;
		}
	}
	else
	{
		strFile = strMap;
	}

	return OpenMapFile(strFile);
}

VARIANT_BOOL CMapseedCtrl::OpenMapFile(CString strFile)
{
	if(_taccess(strFile,00) == -1)
		return  VARIANT_FALSE;

	m_strFile = strFile;

	CFile file;
	if(file.Open(strFile,CFile::modeRead | CFile::shareDenyWrite) == FALSE)
		return VARIANT_FALSE;

	CArchive ar(&file,CArchive::load | CArchive::bNoFlushOnDelete);			

	m_Datainfo.Serialize(ar);

	

	BYTE type;
	ar >> type;
	delete m_pProjection;
	m_pProjection = CProjection::Apply(type);
	if(m_pProjection != nullptr)
	{
		m_pProjection->Serialize(ar);	
	}

	BYTE byte;
	DWORD maxObjID;
	ar >> byte;
	ar >> maxObjID;

	m_library.Serialize(ar);
	m_layertree.Serialize(ar);
	m_pPaper.Serialize(ar);	

	CPoint point;
	ar >> byte;;
	ar >> point;

	m_databaseOBJ.Serialize(ar);
	m_databasePOU.Serialize(ar);
	
	CLink::SerializeList(ar,m_linklist);

	m_databaseOBJ.Open(CDatabase::noOdbcDialog);
	m_databasePOU.Open(CDatabase::noOdbcDialog);

	CString strPOUFile = strFile;
	strPOUFile.MakeLower();
	strPOUFile.Replace(_T(".gis"),_T(".Pou"));
	if(_taccess(strPOUFile,0) != -1)
	{
		CFile  file;
		if(file.Open(strPOUFile,CFile::modeRead | CFile::shareDenyWrite) == TRUE)
		{
			CArchive POUAr(&file, CArchive::load);
			
			CPOU::SerializeList(POUAr,m_poulist);
			m_POUlib.Serialize(POUAr);
			
			POUAr.Close();
			file.Close();
		}
	}

	ar.Close();	
	file.Close();

	CString strGrid = strFile;
	CString strGeom  = strFile;
	if(strFile.Find(strFile) != -1)
	{
		int pos = strFile.Find(_T("\\USERMAP\\"));
		if(pos != -1)
		{
			strFile = strFile.Mid(pos+9);
			strGrid = strFile + strFile;
			strGeom = strFile + strFile;
		}
	}

	strGrid = strGrid.Left(strGrid.ReverseFind(_T('.')));
	strGeom = strGeom.Left(strGeom.ReverseFind(_T('.')));
	strGrid = strGrid +_T(".Grd");
	strGeom = strGeom +_T(".Geo");
	if(_taccess(strGrid,00) != -1  || _taccess(strGeom,00) != -1)
	{
		m_pGridMapFile = new CGridMapFile;
		m_pGeomMapFile = new CGeomMapFile;
		if(m_pGridMapFile->Open(strGrid) == false || m_pGeomMapFile->Open(strGeom) == false)
		{
			delete m_pGridMapFile;
			m_pGridMapFile = nullptr;
			delete m_pGeomMapFile;
			m_pGeomMapFile = nullptr;
		}
	}

	if(m_uZoom < m_pPaper.m_viewLower)
	{
		m_uZoom = m_pPaper.m_viewLower;
	}
	else if(m_uZoom > m_pPaper.m_viewUpper)
	{
		m_uZoom = m_pPaper.m_viewUpper;
	}

	CClientDC dc(this);
	int xDPI = dc.GetDeviceCaps(LOGPIXELSX);
	int yDPI = dc.GetDeviceCaps(LOGPIXELSY);

	long cx = m_pPaper.m_sizeCanvas.cx;
	long cy = m_pPaper.m_sizeCanvas.cy;
	if(m_nAngle != 0)
	{
		double fViewRotateSin = sin(m_nAngle*3.1415926535/180);
		double fViewRotateCos = cos(m_nAngle*3.1415926535/180);

		CPoint point1;
		point1.x = long(fViewRotateCos*cx);
		point1.y = long(fViewRotateSin*cx);

		CPoint point2;
		point2.x = long(fViewRotateCos*cx-fViewRotateSin*cy);
		point2.y = long(fViewRotateSin*cx+fViewRotateCos*cy);

		CPoint point3;
		point3.x = long(-fViewRotateSin*cy);
		point3.y = long( fViewRotateCos*cy);
		
		CRect rect;
		rect.left = min(min(0,point1.x),min(point2.x,point3.x));
		rect.top = min(min(0,point1.y),min(point2.y,point3.y));
		rect.right = max(max(0,point1.x),max(point2.x,point3.x));
		rect.bottom = max(max(0,point1.y),max(point2.y,point3.y));

		cx = rect.Width();
		cy = rect.Height();
	}

	CSize sizeView;
	sizeView.cx = (cx/72.0f)*(xDPI*m_uZoom/100.0f);
	sizeView.cy = (cy/72.0f)*(yDPI*m_uZoom/100.0f);
	
	CRect rect;
	GetClientRect(rect);
	if(rect.Width() > sizeView.cx && rect.Height() > sizeView.cy)
	{
		float xscale = rect.Width() *72.0f/(cx*xDPI);
		float yscale = rect.Height()*72.0f/(cy*yDPI);

		float doctoviewZoom = min(xscale,yscale);
		if((long)(doctoviewZoom*100)<=m_pPaper.m_viewUpper)
		{
			m_uZoom = round(doctoviewZoom*100);
			m_pPaper.m_viewLower = m_uZoom;
		}
	}
	
	this->SetViewSize(CPoint(0,0),CPoint(0,0),FALSE);
	
	if(this->m_hWnd != nullptr)
	{
		this->Invalidate(true);
	}
	return VARIANT_TRUE;                           
}

void CMapseedCtrl::CloseAtlas(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CloseMap();

	CMapseedApp* pApp = (CMapseedApp*)AfxGetApp();
	if(m_pAtlas != nullptr) 
	{
		delete m_pAtlas->m_pTopo;
		m_pAtlas->m_pTopo = nullptr;
		
		delete m_pAtlas;
		m_pAtlas = nullptr;;
	}

	if(m_pTrafficPane != nullptr)
	{
		m_pTrafficPane->DestroyWindow();
		delete m_pTrafficPane;
		m_pTrafficPane = nullptr;
	}

	POSITION pos = m_Resultlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CTraffic* result = m_Resultlist.GetNext(pos);
		delete result;
	}
	m_Resultlist.RemoveAll();

	if(this->m_hWnd != nullptr)
	{
		this->Invalidate(TRUE);
	}
}

void CMapseedCtrl::CloseMap(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	delete m_pProjection;
	m_pProjection = nullptr;
	
	m_library.RemoveAll();
	m_layertree.RemoveAll();
	
	if(m_pGridMapFile != nullptr)
	{	
		m_pGridMapFile->Close();
		delete m_pGridMapFile;
		m_pGridMapFile = nullptr;
	}
	if(m_pGeomMapFile != nullptr)
	{	
		m_pGeomMapFile->Close();
		delete m_pGeomMapFile;
		m_pGeomMapFile = nullptr;
	}

	m_pFlash = nullptr; 

	m_ptViewPos = CPoint(0, 0);
	m_sizeView = CSize(0,0);
	m_uZoom	= 0;

	m_strFile.Empty();

	if(this->m_hWnd != nullptr)
	{
		this->Invalidate(TRUE);
	}

	m_uZoom = 0;
}

VARIANT_BOOL CMapseedCtrl::MatchMap(DOUBLE lng, DOUBLE lat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPointF geoPoint = CPointF(lng,lat);

	CMapseedApp* pApp = (CMapseedApp*)AfxGetApp();
	if(m_pAtlas != nullptr) 
	{
		CPageItem* pPageItem = m_pAtlas->MatchMap(nullptr,geoPoint);
		if(pPageItem != nullptr && pPageItem->m_wId != m_wMapId)
		{
			CRect cliRect;
			this->GetClientRect(cliRect);
			CPoint cliPoint = cliRect.CenterPoint();

			CString stringX;
			CString stringY;
			stringX.Format(_T("%.5f"),geoPoint.x);
			stringY.Format(_T("%.5f"),geoPoint.y);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);

			return (LONG)pPageItem->Open(this,100);
		}
		else
		{
			return FALSE;
		}
	}
	else
		return FALSE;
}

BSTR CMapseedCtrl::GetMapName(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult = m_strFile;

	// TODO: 在此添加调度处理程序代码

	return strResult.AllocSysString();
}

void CMapseedCtrl::GetMapSize(LONG* cx, LONG* cy)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*cx = m_pPaper.m_sizeCanvas.cx;
	*cy = m_pPaper.m_sizeCanvas.cy;
}

void CMapseedCtrl::SetViewZoom(USHORT uZoom)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(uZoom > m_pPaper.m_viewUpper)
	{
		AfxMessageBox(_T("你所设置的显示比例大于该图的最大比例！"));
		uZoom = m_pPaper.m_viewUpper;
	}
	else if(uZoom < m_pPaper.m_viewLower)
	{
		AfxMessageBox(_T("你所设置的显示比例小于该图的最小比例！"));
		uZoom = m_pPaper.m_viewLower;
	}
	
	m_pPaper.SetZoom(this,uZoom,m_uZoom);
}

CString CMapseedCtrl::GetProfileString(LPCTSTR lpszProject, LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault)
{
	if(lpszSection != nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"),lpszProject);

		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strBase + lpszSection,0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr,nullptr, &dwSize) == ERROR_SUCCESS)
			{
				CString strValue;
				LONG lRet = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr,(LPBYTE)strValue.GetBuffer(dwSize/sizeof(TCHAR)), &dwSize);
				strValue.ReleaseBuffer();
				if(lRet == ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return strValue;
				}
			}

			RegCloseKey(hKey);
		}
	}

	return lpszDefault;
}

UINT CMapseedCtrl::GetProfileInt(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault)
{
	if(lpszSection != nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"),lpszProject);

		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strBase + lpszSection,0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr,nullptr, &dwSize) == ERROR_SUCCESS)
			{
				DWORD dwValue;
				DWORD dwType;
				DWORD dwCount = sizeof(DWORD);
				LONG lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, &dwType,(LPBYTE)&dwValue, &dwCount);

				if(lResult == ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return dwValue;
				}
			}

			RegCloseKey(hKey);
		}
	}

	return nDefault;
}


LONG CMapseedCtrl::OnTrafficClick(UINT WPARAM, LONG LPARAM)
{
	if(m_pAtlas == nullptr)
		return 0;

	if(m_pTrafficPane == nullptr)
	{
		HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Traffic.dll"));
		AfxSetResourceHandle(hInst);

		CTrafficPane* pTrafficPane = new CTrafficPane(this);
		if(pTrafficPane->Create(IDD_TRAFFIC,this) == TRUE)
		{
			pTrafficPane->ShowWindow(SW_HIDE);
			m_pTrafficPane = pTrafficPane;
		}
		else
		{
			delete pTrafficPane;
			pTrafficPane = nullptr;
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}


	CPointF geoPoint = *(CPointF*)WPARAM;

	if(LPARAM == 1)
	{
		if(m_pTrafficPane != nullptr)
		{
			m_pTrafficPane->Clear(this);
			m_pTrafficPane->SetStart(this,geoPoint);
		}
	}
	else if(LPARAM == 2)
	{
		if(m_pTrafficPane != nullptr)
		{
			m_pTrafficPane->SetEnd(this,geoPoint);
		}
	}

	if(m_pTrafficPane != nullptr)
	{
		if(m_pTrafficPane->m_ptStart != CPointF(0.0f,0.0f) && m_pTrafficPane->m_ptEnd != CPointF(0.0f,0.0f))
		{
			CStation* startStation = m_pTrafficPane->GetStation(m_pTrafficPane->m_ptStart);
			CStation* endStation = m_pTrafficPane->GetStation(m_pTrafficPane->m_ptEnd);
			
			m_pTraffic = nullptr;

			POSITION pos = m_Resultlist.GetHeadPosition();
			while(pos != nullptr)
			{
				CTraffic* result = m_Resultlist.GetNext(pos);
				delete result;
			}
			m_Resultlist.RemoveAll();

			m_pTrafficPane->Query(m_Resultlist,startStation,endStation);
			
			FireSearchRouteOver();

			SetZoomInTool();
		}
	}
	
	return 0;
}

BSTR CMapseedCtrl::GetRoutelist(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strReturn;

	int nCount=0;
	int index = -1;
	CString strLastResult=_T("");
	
	POSITION pos1 = m_Resultlist.GetHeadPosition();
	while(pos1 != nullptr)
	{		
		index++;
		CTraffic* result = m_Resultlist.GetNext(pos1);
		CString string;
		CString strLastRoad=_T("");
		POSITION pos2 = result->m_interroutelist.GetHeadPosition();
		while(pos2 != nullptr)
		{
			CInterroute* interroute = result->m_interroutelist.GetNext(pos2);
			if(strLastRoad!= interroute->road->m_strName)
			{
				string = string + _T(" 转 ") +  interroute->road->m_strName;
				
				strLastRoad= interroute->road->m_strName;
			}
		}
		string = string.Mid(4);
		if(strLastResult!=string)
		{
			nCount++;
			if(nCount>40)
				break;
			CString str;
			str.Format(_T("<a href='#' onclick='document.Mapseed1.ShowTraffic(%d)'>%s</a><br>"),index,string);
			strReturn += str;
			
			strLastResult=string;
		}
	}	

	if(nCount>0)
	{
		CString str;
		str.Format(_T("<a href='#' onclick='document.Mapseed1.ShowTraffic(%d)'>%s</a><br>"),32768,_T("清除显示"));
		strReturn += str;
	}

	return strReturn.AllocSysString();
}

BSTR CMapseedCtrl::GetInfo(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strReturn;
	CString string = m_strInfo;
	while(string.IsEmpty() == FALSE)
	{
		CString str = string;
		int pos = string.Find(_T('\n'),0);
		if(pos != -1)
		{
			str = string.Left(pos);
			string = string.Mid(pos+1);
		}
		else
		{
			str = string;
			string.Empty();
		}
		
		CString str1 = str.Left(str.Find(_T(':'))+1);
		strReturn += _T("<font color='#ff0000'>") + str1 + _T("<font>");
		CString str2 = str.Mid(str.Find(_T(':'))+1);
		strReturn += _T("<font color='#000000'>") + str2 + _T("<font>");
		strReturn += _T("<br>");
	}
	
	return strReturn.AllocSysString();
}

VARIANT_BOOL CMapseedCtrl::QueryRouteByStation(LPCTSTR strStart,LPCTSTR strEnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pAtlas == nullptr)
		return 0;

	if(m_pTrafficPane == nullptr)
	{
		HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Traffic.dll"));
		AfxSetResourceHandle(hInst);

		CTrafficPane* pTrafficPane = new CTrafficPane(this);
		if(pTrafficPane->Create(IDD_TRAFFIC,this) == TRUE)
		{
			pTrafficPane->ShowWindow(SW_HIDE);
			m_pTrafficPane = pTrafficPane;
		}
		else
		{
			delete pTrafficPane;
			pTrafficPane = nullptr;
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_pTrafficPane != nullptr)
	{
		CStation* startStation = m_pTrafficPane->GetStation(strStart);
		CStation* endStation = m_pTrafficPane->GetStation(strEnd);
	
		if(startStation == nullptr || endStation == nullptr)
		{
			AfxMessageBox(_T("起始站为空或终点站为空!"));
			return VARIANT_FALSE;
		}
		if(startStation == endStation)
		{			
			AfxMessageBox(_T("起始站与终点站相同!"));
			return VARIANT_FALSE;
		}

		m_pTrafficPane->m_ptStart = CPointF(0.0f,0.0f);
		m_pTrafficPane->m_ptEnd = CPointF(0.0f,0.0f);
		
		m_pTraffic = nullptr;


		POSITION pos = m_Resultlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CTraffic* result = m_Resultlist.GetNext(pos);
			delete result;
		}
		m_Resultlist.RemoveAll();
		m_pTrafficPane->Query(m_Resultlist,startStation,endStation);

		FireSearchRouteOver();

		SetZoomInTool();
		return VARIANT_TRUE;
	}
	else
	{
		return VARIANT_FALSE;
	}
}


void CMapseedCtrl::ShowTraffic(USHORT index)
{	
	if(m_pTraffic != nullptr)
	{
		m_pTraffic = nullptr;
		this->InvalidateControl();
	}
	
	CTraffic* pTraffic = nullptr;
	if(m_Resultlist.GetCount()>index)
	{
		POSITION pos = m_Resultlist.FindIndex(index);
		if(pos != nullptr)
		{
			pTraffic = m_Resultlist.GetAt(pos);
		}
	}
	
	if(pTraffic == nullptr)
		return;

	CRectF dRect = pTraffic->GetRect();
	if(dRect.IsRectEmpty() == true)
	{
	//	delete pTraffic;
		pTraffic = nullptr;
		AfxMessageBox(_T("公交线路无法显示!"));
		return;
	}
	pTraffic->CalLength();

	if(m_pAtlas != nullptr)
	{
		CRectF maxRect;
		CPageItem* pPageItem = nullptr;
		POSITION pos = m_pAtlas->m_PageItemList.GetHeadPosition();
		while(pos != nullptr)
		{
			CPageItem* pageItem = m_pAtlas->m_PageItemList.GetNext(pos);
			if(pageItem->m_bLogistic == TRUE)
				continue;
			if(pageItem->m_wType != CPageItem::typeMap)
				continue;

			CRectF rect = pageItem->m_Rect;
			rect.IntersectRect(dRect);
			if(rect.Width() >= maxRect.Width() && rect.Height() >= maxRect.Height())
			{
				maxRect = rect;
				pPageItem = pageItem;
			}
		}

		if(pPageItem != nullptr)
		{		
			pPageItem->Open(this,100);
		}

		CPointF geoPoint = dRect.TopLeft();
		CPointF mapPoint;
		CPoint  docPoint;
					
		this->SendMessage(WM_GEOTOMAP,(UINT)(&geoPoint),(LONG)(&mapPoint));
		this->SendMessage(WM_MAPTODOC,(UINT)(&mapPoint),(LONG)(&docPoint));
		
		CPoint lefttop = docPoint;

		geoPoint = dRect.BottomRight();
		this->SendMessage(WM_GEOTOMAP,(UINT)(&geoPoint),(LONG)(&mapPoint));
		this->SendMessage(WM_MAPTODOC,(UINT)(&mapPoint),(LONG)(&docPoint));

		CPoint bottomright = docPoint;

		CRect docRect(lefttop,bottomright);
		docRect.NormalizeRect();
		this->SendMessage(WM_DOCTOCLIENT,(UINT)&docRect,3);
		docRect.InflateRect(50,50);
		this->SendMessage(WM_CLIENTTODOC,(UINT)&docRect,3);

		if(docRect.IsRectEmpty() == FALSE)
		{
			this->SendMessage(WM_FIXATERECT,(UINT)&docRect,0);	
		}
	}

	m_pTraffic = pTraffic;
}

LONG CMapseedCtrl::OnShowInfo(UINT WPARAM, LONG LPARAM)
{
	m_strInfo.Empty();
	FireHasInfo();

	unsigned long nDecimal = m_pPaper.GetDecimal();
	if(m_databasePOU.IsOpen() == TRUE)
	{
		CPoint point(WPARAM,LPARAM);
		this->SendMessage(WM_DOCTOCLIENT,(UINT)&point,1);

		POSITION pos = m_poulist.GetHeadPosition();
		while(pos != nullptr)
		{
			CPOU* pPOU = (CPOU*)m_poulist.GetNext(pos);
			if(pPOU->m_bVisible == false)
				continue;
			
			DWORD dwId = pPOU->Pick(this,&m_databasePOU,&m_POUlib,nDecimal,point);
			if(dwId != -1)
			{
				m_strInfo = m_databasePOU.GetTip(pPOU->m_strTable,_T(""),dwId);
				if(m_strInfo.IsEmpty() == FALSE)
					FireHasInfo();
				return 1;
			}
		}
	}

	if(m_pAtlas != nullptr && m_pAtlas->m_databasePOU.IsOpen() == TRUE)
	{
		CPoint point(WPARAM,LPARAM);
		this->SendMessage(WM_DOCTOCLIENT,(UINT)&point,1);

		POSITION Pos3 = m_pAtlas->m_poulist.GetTailPosition();
		while(Pos3 != nullptr)
		{
			CPOU* pPOU = (CPOU*)m_pAtlas->m_poulist.GetPrev(Pos3);
			if(pPOU->m_bVisible == false)
				continue;

			DWORD dwId = pPOU->Pick(this,&m_pAtlas->m_databasePOU,&m_pAtlas->m_library,nDecimal,point);
			if(dwId != -1)
			{
				m_strInfo = m_pAtlas->m_databasePOU.GetTip(pPOU->m_strTable,_T(""),dwId);
				if(m_strInfo.IsEmpty() == FALSE)
					FireHasInfo();
				return 1;
			}
		}
	}
	
	if(m_databaseOBJ.IsOpen() == TRUE)
	{
		CPoint point(WPARAM,LPARAM);
		CGeom* pGeom = (CGeom*)SendMessage(WM_LOOKGEOM,(LONG)&point,1);
		if(pGeom != nullptr)	
		{
			CLayer* layer = m_layertree.GetLayerByGeom(pGeom);
			if(layer == nullptr)
				return 0;

			CString strTable = layer->m_attTable;
			if(strTable.IsEmpty() == TRUE)
				return 0;

			m_strInfo = m_databaseOBJ.GetTip(strTable,_T(""),pGeom->m_dwId);
			if(m_strInfo.IsEmpty() == FALSE)
				FireHasInfo();

			this->KillTimer(TIMER_FLASHGEOM);
			m_pFlash = pGeom;
			this->SetTimer(TIMER_FLASHGEOM,500, nullptr);
		}
	}

	return 1;
}

LONG CMapseedCtrl::OnLookGeom(UINT WPARAM, LONG LPARAM)
{
	CPoint point = *(CPoint*)WPARAM;
	
	CClientDC dc(this);
	OnPrepareDC(&dc,nullptr);

	CSize sizeExtView = dc.GetViewportExt();
	float doctoviewZoom = (float)sizeExtView.cx/dc.GetDeviceCaps(LOGPIXELSX);
	unsigned long nDecimal = m_pPaper.GetDecimal();

	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend();++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == FALSE)
			continue;
	
		if(LPARAM == 1 && layer->m_attTable.IsEmpty() == TRUE)
			continue;

		CGeom* pGeom = layer->Look(nDecimal,matrixClitoDoc,point);
		if(pGeom != nullptr)
			return (LONG)pGeom;
	} 
	
	return 0;
}

LONG CMapseedCtrl::OnFlashGeomtry(UINT WPARAM, LONG LPARAM)
{
	return 0;
}

void CMapseedCtrl::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case TIMER_FLASHGEOM:
		{
			if(m_pFlash != nullptr && m_nDelay < 20)
			{
				m_pFlash->Flash(this,matrixDoctoCli,m_nDelay++);
			}
			else
				this->KillTimer(nIDEvent);
			break;
		}
	default:
		break;	
	}		

	COleControl::OnTimer(nIDEvent);
}

BOOL CMapseedCtrl::KillTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_FLASHGEOM:
		{
			if(m_pFlash != nullptr)
			{
				m_pFlash->Invalidate(this,matrixDoctoCli,9);
				m_pFlash = nullptr;
			}
			m_nDelay = 0;
		}
		break;
	default:
		break;
	}

	return COleControl::KillTimer(nIDEvent);
}

void CGrfView::ZoomView(float fRatio,CPoint docPoint,CPoint cliPoint)
{
	float newZoom = m_doctoviewZoom*fRatio;
	unsigned int nScale = pDoc->m_Datainfo.GetScaleByZoom(newZoom,(docPoint);

	if(nScale < m_scaleMaximum)
	{
		newZoom = pDoc->m_Datainfo.GetZoomByScale(m_scaleMaximum,docPoint);
	}
	else if(nScale > m_scaleMinimum)
	{
		newZoom = pDoc->m_Datainfo.GetZoomByScale(m_scaleMinimum,docPoint);
	}
	else
	{
		/*	
	    #ifdef MAPPER
		scale = (double)Zoom/100;
		scale = log10(scale)/log10(2.f);
		if(Zoom>10)
		{
			if(scale>0)
				scale += 0.5;
			else
				scale -= 0.5;
		}
		scale = powf(2,(int)scale);

		Zoom = (int)(scale*100);
		if(Zoom < 1)
			Zoom = 1;
		#endif
		*/
	}
	
	if(newZoom != doctoviewZoom)
	{
		m_doctoviewZoom = newZoom;
		this->SetViewSize(docPoint,cliPoint,TRUE);
	}
}
