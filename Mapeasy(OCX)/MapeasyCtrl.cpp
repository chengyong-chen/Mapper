// MapeasyCtrl.cpp : CMapeasyCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "Mapeasy.h"
#include "MapeasyCtrl.h"
#include "MapeasyPropPage.h"
#include "IMapdcom.h"

#include "comcat.h" 
#include "objsafe.h" 
#include <comdef.h>
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")


#include "../Viewer/Global.h"

#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"

#include "../Atlas/Lzari.h"

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

#include "../Projection/Peking54.h"
#include "../Projection/Gauss.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include ".\Mapeasyctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern __declspec(dllimport) CRectArray   d_oRectArray;

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

IMPLEMENT_DYNCREATE(CMapeasyCtrl, COleControl)

// 消息映射

BEGIN_MESSAGE_MAP(CMapeasyCtrl, COleControl)
//{{AFX_MSG_MAP(CMapeasyCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()	
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_PREPAREDC,     OnPrepareDC)
	ON_MESSAGE(WM_SETMATRIX,     OnSetMatrix)
	ON_MESSAGE(WM_SETDRAWSTATUS,OnSetDrawStatus)
	ON_MESSAGE(WM_CLIENTTODOC,   OnClientToDoc)
	ON_MESSAGE(WM_DOCTOMAP,      OnDocToMap)
	ON_MESSAGE(WM_MAPTOGEO,      OnMapToGeo)
	ON_MESSAGE(WM_GEOTOMAP,      OnGeoToMap)
	ON_MESSAGE(WM_MAPTODOC,      OnMapToDoc)
	ON_MESSAGE(WM_DOCTOCLIENT,   OnDocToClient)

	ON_MESSAGE(WM_CALLENGTH,   OnCalLength)
	ON_MESSAGE(WM_CALAREA,     OnCalArea)

	ON_MESSAGE(WM_GETSCALE,      OnGetScale)
	ON_MESSAGE(WM_ZOOMIN,        OnZoomIn)
	ON_MESSAGE(WM_ZOOMOUT,       OnZoomOut)
	ON_MESSAGE(WM_FIXATEVIEW,    OnFixateView)
	ON_MESSAGE(WM_SCROLLVIEW,    OnScrollView)

	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


// 调度映射
BEGIN_DISPATCH_MAP(CMapeasyCtrl, COleControl)
	DISP_FUNCTION_ID(CMapeasyCtrl, "OpenAtlas", dispidOpenAtlas, OpenAtlas, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CMapeasyCtrl, "OpenMap", dispidOpenMap, OpenMap, VT_BOOL, VTS_UI2 VTS_UI2)
	DISP_FUNCTION_ID(CMapeasyCtrl, "OpenMapByName", dispidOpenMapByName, OpenMapByName, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CMapeasyCtrl, "OpenMatchMap", dispidOpenMatchMap, OpenMatchMap, VT_BOOL, VTS_R8 VTS_R8)
	DISP_FUNCTION_ID(CMapeasyCtrl, "CloseAtlas", dispidCloseAtlas, CloseAtlas, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CMapeasyCtrl, "CloseMap", dispidCloseMap, CloseMap, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CMapeasyCtrl, "ScrollView", dispidScrollView, ScrollView, VT_EMPTY, VTS_I4 VTS_I4 VTS_BOOL)
	DISP_FUNCTION_ID(CMapeasyCtrl, "AimatView", dispidAimatView, AimatView, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CMapeasyCtrl, "SetTool", dispidSetTool, SetTool, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CMapeasyCtrl, "GetTool", dispidGetTool, GetTool, VT_UI1, VTS_NONE)
END_DISPATCH_MAP()



// 事件映射

BEGIN_EVENT_MAP(CMapeasyCtrl, COleControl)
END_EVENT_MAP()



// 属性页

// TODO: 按需要添加更多属性页。请记住增加计数！
BEGIN_PROPPAGEIDS(CMapeasyCtrl, 1)
	PROPPAGEID(CMapeasyPropPage::guid)
END_PROPPAGEIDS(CMapeasyCtrl)



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CMapeasyCtrl, "MAPEASY.MapeasyCtrl.1",
	0xb61d8175, 0x3ac4, 0x4071, 0xa3, 0xaf, 0xcd, 0x45, 0x17, 0x2a, 0x5c, 0x1c)



// 键入库 ID 和版本

IMPLEMENT_OLETYPELIB(CMapeasyCtrl, _tlid, _wVerMajor, _wVerMinor)



// 接口 ID

const IID BASED_CODE IID_DMapeasy =
		{ 0xB917C841, 0x4C11, 0x41A6, { 0x8C, 0x5B, 0x93, 0x45, 0xD7, 0x5C, 0x8C, 0x4F } };
const IID BASED_CODE IID_DMapeasyEvents =
		{ 0xEFECF2B1, 0x3F7A, 0x4674, { 0x8C, 0xB1, 0x79, 0xEF, 0x14, 0x21, 0x9C, 0x2D } };



// 控件类型信息

static const DWORD BASED_CODE _dwMapeasyOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
//	OLEMISC_IGNOREACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMapeasyCtrl, IDS_MAPEASY, _dwMapeasyOleMisc)



// CMapeasyCtrl::CMapeasyCtrlFactory::UpdateRegistry -
// 添加或移除 CMapeasyCtrl 的系统注册表项

BOOL CMapeasyCtrl::CMapeasyCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: 验证您的控件是否符合单元模型线程处理规则。
	// 有关更多信息，请参考 MFC 技术说明 64。
	// 如果您的控件不符合单元模型规则，则
	// 必须修改如下代码，将第六个参数从
	// afxRegApartmentThreading 改为 0。

	if(bRegister)
	{
		if(AfxOleRegisterControlClass(AfxGetInstanceHandle(),m_clsid,m_lpszProgID,IDS_MAPEASY,IDB_MAPEASY,afxRegApartmentThreading,_dwMapeasyOleMisc,_tlid,_wVerMajor,_wVerMinor) == FALSE)
			return FALSE;
		
		HRESULT hr1 = CreateComponentCategory(CATID_SafeForScripting,L"Controls that are safely scriptable");   
		HRESULT hr2 = CreateComponentCategory(CATID_SafeForInitializing,L"Controls safely initializable from persistent data");
		HRESULT hr3 = RegisterCLSIDInCategory (m_clsid, CATID_SafeForScripting);   
		HRESULT hr4 = RegisterCLSIDInCategory (m_clsid,CATID_SafeForInitializing);   
		return TRUE;
	}
	else
	{
		HRESULT hr1 = UnRegisterCLSIDInCategory(m_clsid,CATID_SafeForScripting);  
		HRESULT hr2 = UnRegisterCLSIDInCategory(m_clsid,CATID_SafeForInitializing) ;  
		
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
	}
}



// CMapeasyCtrl::CMapeasyCtrl - 构造函数

CMapeasyCtrl::CMapeasyCtrl()
{
	InitializeIIDs(&IID_DMapeasy, &IID_DMapeasyEvents);
	
	m_pIMapdcom = nullptr;

	m_wMapId = -1;
	pTool = nullptr;

	m_ptViewPos = CPoint(0, 0);
	m_sizeView = CSize(0,0);
	m_extView = CSize(72,72);
	m_uZoom	= 0;

	m_WindowRect.SetRectEmpty();
	m_validRect.SetRectEmpty();

	m_pProjection = nullptr;
	
	pTool = &zoomTool;
	zoomTool.m_bOut = false;
	panTool.m_bContinuously = false;

	try
	{
		m_pIMapdcom = new IMapdcom();
		if(m_pIMapdcom->Connect(_T("220.130.152.204"),1505,this) == false)
	//	if(m_pIMapdcom->Connect(_T("localhost"),1505,this) == false)
		{
			delete m_pIMapdcom;
			m_pIMapdcom = nullptr;
			AfxMessageBox(_T("同服务器建立连接失败!"));
		}
		else if(m_pIMapdcom->Longin(_T("fdsfs"),_T("")) == S_FALSE)
		{
			m_pIMapdcom->Close();
			delete m_pIMapdcom;
			m_pIMapdcom = nullptr;
			AfxMessageBox(_T("密码验证失败!"));
		}
		else
		{
			BSTR bstrAtlas = SysAllocString(L"China2005");
			if(m_pIMapdcom->OpenAtlas(bstrAtlas) != S_OK)
			{
				m_pIMapdcom->Close();
				delete m_pIMapdcom;
			//	m_pIMapdcom->Release();
				m_pIMapdcom = nullptr;
				AfxMessageBox(_T("打开工程错误!"));
			}
			SysFreeString(bstrAtlas);
		}
	}
	catch(CException* ex)
	{
		OutputDebugString(ex->m_strError+"\r\n");
		ex->Delete();
	}
}

// CMapeasyCtrl::~CMapeasyCtrl - 析构函数
CMapeasyCtrl::~CMapeasyCtrl()
{
	delete m_pProjection;
	
	if(m_pIMapdcom != nullptr)
	{
		m_pIMapdcom->Close();
		delete m_pIMapdcom;
		m_pIMapdcom = nullptr;	
	}
}



// CMapeasyCtrl::OnDraw - 绘图函数
void CMapeasyCtrl::OnDraw(CDC* pDC, const CRect& rcBounds, const CRect& rcInvalid)
{
	if(pDC == nullptr)
		return;

	if(m_pIMapdcom == nullptr)
		return;

	if(!IsOptimizedDraw())
	{
	}

	if(rcInvalid.IsRectNull())
		return;
	
//	unsigned long nDecimal = m_pPaper.GetDecimal();

	OnPrepareDC(pDC,nullptr);
	
	d_oRectArray.RemoveAll();

	CDC* pDrawDC = pDC;
	CDC dc;
	CBitmap bitmap;
	CBitmap* pOldBitmap = nullptr;		
	if(pDC->IsPrinting() == FALSE)
	{
		CRect cliRect = rcBounds; 
		cliRect.NormalizeRect();
		if(cliRect.IsRectNull())
			return;

		if(pTool != nullptr)
			pTool->Draw(this,pDC);

		if(dc.CreateCompatibleDC(pDC) == TRUE)
		{
			if(bitmap.CreateCompatibleBitmap(pDC, cliRect.Width(), cliRect.Height()) == TRUE)
			{
				pDC->SetMapMode(MM_TEXT);
				dc.SetMapMode(MM_TEXT);
				
				pOldBitmap = dc.SelectObject(&bitmap);

				CBrush* oldBrush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);
				CPen* oldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
				CRect rect = cliRect;
				rect.InflateRect(2,2);
				dc.Rectangle(rect);
				dc.SelectObject(oldBrush);
				dc.SelectObject(oldPen);

			//	dc.BitBlt(0, 0, cliRect.Width(), cliRect.Height(), pDC, cliRect.left, cliRect.top, WHITENESS);//98下有时又问题，不能全部变白，保留原来的东西
				
				pDrawDC = &dc;

				dc.SetMapMode(MM_ANISOTROPIC);
				dc.SetViewportExt(dc.GetDeviceCaps(LOGPIXELSX),dc.GetDeviceCaps(LOGPIXELSY));
				dc.SetWindowExt(72*nDecimal,-72*nDecimal);
				dc.SetViewportOrg(0,0);
				dc.SetWindowOrg(0,cliRect.Height()*72.0f*nDecimal/dc.GetDeviceCaps(LOGPIXELSY));

				dc.SetBrushOrg(cliRect.left%8, cliRect.top%8);
				dc.IntersectClipRect(0,0,cliRect.Width()*72.0f*nDecimal/dc.GetDeviceCaps(LOGPIXELSX),cliRect.Height()*72.0f*nDecimal/dc.GetDeviceCaps(LOGPIXELSY));
			}
			else
			{
				dc.DeleteDC();
			}
		}
		else
		{
		}

		CRect inRect = cliRect;
		ClientToDoc(inRect);		
		inRect.NormalizeRect();
		CPoint offset = CPoint(cliRect.left,cliRect.bottom);
		{
			offset.Offset(m_ptViewPos);
			offset.x =                         ((double)offset.x/m_extView.cx)*72*nDecimal; 
			offset.y = m_WindowRect.Height() - ((double)offset.y/m_extView.cy)*72*nDecimal; 
			offset.Offset(m_WindowRect.TopLeft());
		}

		float doctoviewZoom = (float)m_extView.cx/pDC->GetDeviceCaps(LOGPIXELSX);
		double m11 = doctoviewZoom;
		double m21 = 0;
		double m31 =-offset.x*doctoviewZoom;
		double m12 = 0;
		double m22 = doctoviewZoom;
		double m32 =-offset.y*doctoviewZoom;

		this->Draw(this,pDrawDC,inRect,m_uZoom,doctoviewZoom,m11,m21,m31,m12,m22,m32);

		if(pDrawDC == &dc)
		{
			pDC->SetMapMode(MM_TEXT);
			pDC->SetViewportOrg(0,0);
			pDC->SetWindowOrg(0,0);		
			dc.SetMapMode(MM_TEXT);
			dc.SetWindowOrg(0,0);
			dc.SetViewportOrg(0,0);

			pDC->BitBlt(cliRect.left, cliRect.top, cliRect.Width(), cliRect.Height(),&dc, 0, 0, SRCCOPY);

			dc.SelectObject(pOldBitmap);
			bitmap.DeleteObject();
			dc.DeleteDC();

			OnPrepareDC(pDC,nullptr);
		}	
	}
	if(pTool != nullptr)
		pTool->Draw(this,pDC);

	d_oRectArray.RemoveAll();
}

void CMapeasyCtrl::Draw(COleControl* pControl,CDC* pDC,const CRect& inRect,const unsigned int& uZoom,const float& doctoviewZoom,const double& m11,const double& m21,const long& m31,const double& m12,const double& m22,const long& m32)
{	
	if(m_layertree.m_layerlist.GetCount() < 1)
		return;

	if(inRect.IsRectNull() == TRUE)
		return;

	if(m_pIMapdcom == nullptr)
		return;

	if(inRect != m_validRect)
	{
		try
		{
			VARIANT varData;
			if(m_pIMapdcom->GetMapData(m_wMapId,m_uZoom,inRect.left,inRect.top,inRect.right,inRect.bottom,&varData) == S_OK)
			{
				if(varData.vt == (VT_ARRAY | VT_UI1))
				{
					int length = varData.parray->rgsabound[0].cElements;
					if(length>0)
					{
						BYTE* pData = nullptr;
						SafeArrayAccessData(varData.parray, (void**)&pData); // Get the data
						CLzari lzari;
						BYTE* pNewData = lzari.UnCompress(pData,length,length);
						SafeArrayUnaccessData(varData.parray);	    // Unaccess it.

						pData = pNewData;
						CMemFile file;
						file.Attach(pData,length);
						CArchive ar((CFile*)&file, CArchive::load);

						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend();++it)
						{
							CLayer* layer = *it;
							WORD vestigecount;
							ar >> vestigecount;
							CMap<DWORD,DWORD&,DWORD,DWORD&> existent;
							for(int i=0;i<vestigecount;i++)
							{
								DWORD id;
								ar >> id;
								existent.SetAt(id,id);
							}
				

							POSITION pos1 = layer->m_geomlist.GetHeadPosition();
							POSITION pos2;
							while((pos2=pos1) != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos1);
								DWORD id;
								if(existent.Lookup(pGeom->m_dwId,id) == FALSE)
								{
									delete pGeom;
									pGeom = nullptr;
									layer->m_geomlist.RemoveAt(pos2);
								}
							}
						//	layer->m_geomlist.RemoveAll();

							WORD nCount;
							ar >> nCount;
							for(int index=0;index<nCount;index++)
							{
								BYTE type;
								ar >> type;
								CGeom* pGeom = CGeom::Apply(type);
								if(pGeom != nullptr)
								{
									pGeom->ReleaseDCOM(ar);
									layer->m_geomlist.AddTail(pGeom);
									switch(pGeom->m_bType)
									{
									case 8:
										((CMark*)pGeom)->CalCorner(layer->GetSpot(),&m_library);
										break;
									default:
										pGeom->RecalRect();
										break;
									}
								}
							}
						}
					//	ar.Flush(); 
						ar.Close();
						
					//	file.Flush();
					//	file.Detach();
					//	file.Close();

					//	delete pData;
					//	pData = nullptr;
						lzari.Release();
					}
					m_validRect = inRect;
				}

				if(varData.parray != nullptr)
				{	
					SafeArrayDestroy(varData.parray);
					varData.parray = nullptr;
				}
			}
		}
		catch(_com_error e) 
		{ 
			OutputDebugString(ex->m_strError);
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}	

	Graphics g(pDC->m_hDC);
	g.SetPageUnit(UnitPixel);
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	unsigned long nDecimal = m_pPaper.GetDecimal();

	CMapeasyApp* pApp = (CMapeasyApp*)AfxGetApp();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend();++it)
	{
		CLayer* layer = *it;
		layer->GetDate(nullptr,nullptr,inRect,uZoom,m11,m21,m31,m12,m22,m32);
		layer->DrawPack(g,&m_library,inRect,uZoom,doctoviewZoom,nDecimal);
		
	//	layer->DrawSeed(pDC,&m_library,inRect,uZoom,doctoviewZoom);
	}

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend();++it)
	{
		CLayer* layer = *it;
		layer->DrawPackTag(g,inRect,uZoom,doctoviewZoom,nDecimal); 

		POSITION pos = layer->m_drawlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = layer->m_drawlist.GetNext(pos);
			delete pGeom;
		}
		layer->m_drawlist.RemoveAll();

//		layer->DrawSeedTag(pDC,inRect,uZoom,doctoviewZoom); 
	}

	g.ReleaseHDC(pDC->m_hDC);
}

// CMapeasyCtrl::DoPropExchange - 持久性支持

void CMapeasyCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 为每个持久的自定义属性调用 PX_ 函数。
}



// CMapeasyCtrl::GetControlFlags -
// 自定义 MFC 的 ActiveX 控件实现的标志。
//
DWORD CMapeasyCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	// 当前未剪辑控件的输出。
	// 控件保证它不会绘制到它的
	//矩形工作区之外。
	dwFlags &= ~clipPaintDC;

	// 在活动和不活动状态之间进行转换时，
	//不会重新绘制控件。
	dwFlags |= noFlickerActivate;

	// 控件在不活动时可以接收鼠标通知。
	// TODO: 如果编写 WM_SETCURSOR 和 WM_MOUSEMOVE 的处理程序，
	//		在使用 m_hWnd 成员变量之前应首先检查它的值
	//		是否非空。
	dwFlags |= pointerInactive;

	// 控件通过不还原设备上下文中的
	// 原始 GDI 对象，可以优化它的 OnDraw 方法。
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}



// CMapeasyCtrl::OnResetState - 将控件重置为默认状态

void CMapeasyCtrl::OnResetState()
{
	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	// TODO: 在此重置任意其他控件状态。
}



// CMapeasyCtrl 消息处理程序
// CMapeasyCtrl 消息处理程序

void CMapeasyCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonDown(nFlags,point);	
	
	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnLButtonDown(this, nFlags, point);
	}
}

void CMapeasyCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	COleControl::OnMouseMove(nFlags,point);
	
	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnMouseMove(this, nFlags, point);
	}
}

BOOL CMapeasyCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(::GetKeyState(VK_SHIFT)>=0)
		OnScrollBy(CSize(0,-zDelta/2), TRUE);
	else
		OnScrollBy(CSize(-zDelta/2,0), TRUE);
	
	return COleControl::OnMouseWheel(nFlags, zDelta, pt);
}

void CMapeasyCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonUp(nFlags,point);

	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnLButtonUp(this, nFlags, point);
	}
}

void CMapeasyCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	COleControl::OnLButtonDblClk(nFlags, point);

	if(pTool != nullptr)
	{	
		ClientToDoc(point);
		pTool->OnLButtonDblClk(this, nFlags, point);	
	}
}

void CMapeasyCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
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

void CMapeasyCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
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

/////////////////////////////////////////////////////////////////////////////
// CMapeasyCtrl diagnostics

BOOL CMapeasyCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(pWnd == this && nHitTest == HTCLIENT && pTool != nullptr)
		return pTool->SetCursor(this);
	else
		return COleControl::OnSetCursor(pWnd, nHitTest, message);	
}

void CMapeasyCtrl::SetViewSize(const CPoint& docPoint, const CPoint& cliPoint,BOOL bScroll)
{	
	m_ptViewPos = CPoint(0,0);
	
	float doctoviewZoom = (float)m_uZoom/100;
	CClientDC dc(this);
	m_extView.cx = round(dc.GetDeviceCaps(LOGPIXELSX)*doctoviewZoom); 
	m_extView.cy = round(dc.GetDeviceCaps(LOGPIXELSY)*doctoviewZoom); 
					
	unsigned long nDecimal = m_pPaper.GetDecimal();

	if(m_extView.cx<1)
		m_extView.cx = 1;
	if(m_extView.cy<1)
		m_extView.cy = 1;

	m_sizeView.cx = (m_WindowRect.Width() /(72.0f*nDecimal))*m_extView.cx ;
	m_sizeView.cy = (m_WindowRect.Height()/(72.0f*nDecimal))*m_extView.cy;
	m_levelCurrent = log(max(m_sizeView.cx, m_sizeView.cy)/256.f)/log(2.f);

	CRect cliRect;
	this->GetClientRect(&cliRect);
	
	if(m_sizeView.cx < cliRect.Width())
	{
		m_ptViewPos.x = (m_sizeView.cx - cliRect.Width())/2;
	}
	if(m_sizeView.cy < cliRect.Height())
	{
		m_ptViewPos.y = (m_sizeView.cy - cliRect.Height())/2;
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

	docPoint.y = m_WindowRect.Height() - docPoint.y;

	CSize sizeScroll;
	sizeScroll.cx = (docPoint.x/(72.0f*nDecimal))*m_extView.cx-cliPoint.x - m_ptViewPos.x;
	sizeScroll.cy = (docPoint.y/(72.0f*nDecimal))*m_extView.cy-cliPoint.y - m_ptViewPos.y;
	this->OnScrollBy(sizeScroll,bScroll);
}

BOOL CMapeasyCtrl::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	int xOrig, x;
	int yOrig, y;

	CRect cliRect;
	GetClientRect(&cliRect);
	
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
		GetClientRect(&rcBounds);
		
		CRect rcInvalid = rcBounds;

		CClientDC dc(this);
		OnPrepareDC(&dc,nullptr);

		OnDraw(&dc,rcBounds,rcInvalid);
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

LONG CMapeasyCtrl::OnFixateView(UINT WPARAM, LONG LPARAM)
{
	CPoint docPoint = *(CPoint*)WPARAM;
	CPoint cliPoint = *(CPoint*)LPARAM;

	unsigned long nDecimal = m_pPaper.GetDecimal();

	docPoint.y = m_WindowRect.Height() - docPoint.y;

	CSize sizeScroll;
	sizeScroll.cx = (docPoint.x/(72.0f*nDecimal))*m_extView.cx-cliPoint.x - m_ptViewPos.x;
	sizeScroll.cy = (docPoint.y/(72.0f*nDecimal))*m_extView.cy-cliPoint.y - m_ptViewPos.y;
			
	this->OnScrollBy(sizeScroll,TRUE);

	return 1L;
}

LONG CMapeasyCtrl::OnScrollView(UINT WPARAM, LONG LPARAM)
{
	CSize sizeScroll((int)WPARAM, (int)LPARAM);
	this->OnScrollBy(sizeScroll,TRUE);

	return 1L;
}

LONG CMapeasyCtrl::OnGetScale(UINT WPARAM, LONG LPARAM)
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

void CMapeasyCtrl::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CRect cliRect;
	GetClientRect(&cliRect);
	
	unsigned long nDecimal = m_pPaper.GetDecimal();

	if(m_sizeView.cx < cliRect.Width())
	{
		m_ptViewPos.x = -(cliRect.Width() - m_sizeView.cx)/2;
	}
	if(m_sizeView.cy < cliRect.Height())
	{
		m_ptViewPos.y = -(cliRect.Height() - m_sizeView.cy)/2;
	}

	CPoint ptWindowOrg;
	ptWindowOrg.x =                         ((double)m_ptViewPos.x/m_extView.cx)*72*nDecimal; 
	ptWindowOrg.y = m_WindowRect.Height() - ((double)m_ptViewPos.y/m_extView.cy)*72*nDecimal; 

	ASSERT_VALID(pDC);

	ASSERT(m_sizeView.cx >= 0 && m_sizeView.cy >= 0);
	
	pDC->SetMapMode(MM_ANISOTROPIC);

	pDC->SetViewportExt(m_extView);
	pDC->SetWindowExt(72*nDecimal,-72*nDecimal);
	pDC->SetWindowOrg(ptWindowOrg);
	pDC->SetViewportOrg(0, 0);
}

void CMapeasyCtrl::ClientToDoc(const CPoint& point)
{	
	unsigned long nDecimal = m_pPaper.GetDecimal();

	point.Offset(m_ptViewPos);
	point.x =                         ((double)point.x/m_extView.cx)*72*nDecimal; 
	point.y = m_WindowRect.Height() - ((double)point.y/m_extView.cy)*72*nDecimal; 
}

void CMapeasyCtrl::ClientToDoc(CSize& size)
{
	unsigned long nDecimal = m_pPaper.GetDecimal();
	
	size.cx = ((double)size.cx/m_extView.cx)*72*nDecimal;
	size.cy = ((double)size.cy/m_extView.cy)*72*nDecimal;
}

void CMapeasyCtrl::ClientToDoc(CRect& rect)
{
	rect.OffsetRect(m_ptViewPos);
	
	unsigned long nDecimal = m_pPaper.GetDecimal();

	rect.left   =                         ((double)rect.left  /m_extView.cx)*72*nDecimal; 
	rect.top    = m_WindowRect.Height() - ((double)rect.top   /m_extView.cy)*72*nDecimal; 
	rect.right  =                         ((double)rect.right /m_extView.cx)*72*nDecimal; 
	rect.bottom = m_WindowRect.Height() - ((double)rect.bottom/m_extView.cy)*72*nDecimal;
	rect.NormalizeRect();
}

void CMapeasyCtrl::DocToMap(const CPoint& docPoint,CPointF& mapPoint) const
{
	unsigned long nDecimal = m_pPaper.GetDecimal();
	mapPoint = m_Datainfo.DocToMap(docPoint,nDecimal);
}   

LONG CMapeasyCtrl::OnClientToDoc(UINT WPARAM, LONG LPARAM)
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

LONG CMapeasyCtrl::OnDocToMap(UINT WPARAM, LONG LPARAM)
{
	DocToMap(*(CPoint*)WPARAM,*(CPointF*)LPARAM);
	return 0L;
}

LONG CMapeasyCtrl::OnMapToGeo(UINT WPARAM, LONG LPARAM)
{
	CPointF* mapPoint = (CPointF*)WPARAM;
	CPointF* geoPoint = (CPointF*)LPARAM;

	*geoPoint = pDoc->m_Datainfo.MapToGeo(*mapPoint);
	return 0L;
}

void CMapeasyCtrl::GeoToMap(const CPointF& geoPoint,CPointF& mapPoint)
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

void CMapeasyCtrl::MapToDoc(const CPointF& mapPoint,CPoint& docPoint)
{   
	unsigned long nDecimal = m_pPaper.GetDecimal();
	docPoint = m_Datainfo.MapToDoc(mapPoint,nDecimal);
}   

LONG CMapeasyCtrl::OnGeoToMap(UINT WPARAM, LONG LPARAM)
{
	GeoToMap(*(CPointF*)WPARAM,*(CPointF*)LPARAM);
	return 0L;
}

LONG CMapeasyCtrl::OnMapToDoc(UINT WPARAM, LONG LPARAM)
{
	MapToDoc(*(CPointF*)WPARAM,*(CPoint*)LPARAM);
	return 0L;
}

LONG CMapeasyCtrl::OnDocToClient(UINT WPARAM, LONG LPARAM)
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

void CMapeasyCtrl::DocToClient(const CPoint& point)
{			
	unsigned long nDecimal = m_pPaper.GetDecimal();

	point.x = (point.x/(72.0f*nDecimal))*m_extView.cx;
	point.y = m_WindowRect.Height() - point.y;
	point.y = (point.y/(72.0f*nDecimal))*m_extView.cy;

	point.Offset(-m_ptViewPos);
}

void CMapeasyCtrl::DocToClient(CSize& size)
{	
	unsigned long nDecimal = m_pPaper.GetDecimal();

	size.cx = (size.cx/(72.0f*nDecimal))*m_extView.cx;
	size.cy =-(size.cy/(72.0f*nDecimal))*m_extView.cy;
}

void CMapeasyCtrl::DocToClient(CRect& rect)
{	
	unsigned long nDecimal = m_pPaper.GetDecimal();

	rect.left   = (rect.left  /(72.0f*nDecimal))*m_extView.cx;
	rect.top    = m_WindowRect.Height() - rect.top;
	rect.top    = (rect.top   /(72.0f*nDecimal))*m_extView.cy;
	rect.right  = (rect.right /(72.0f*nDecimal))*m_extView.cx;
	rect.bottom = m_WindowRect.Height() - rect.bottom;
	rect.bottom = (rect.bottom/(72.0f*nDecimal))*m_extView.cy;
	rect.OffsetRect(-m_ptViewPos);
	rect.NormalizeRect();
}

LONG CMapeasyCtrl::OnZoomIn(UINT WPARAM, LONG LPARAM)
{
	CMapeasyApp* pApp = (CMapeasyApp*)AfxGetApp();

	if(m_uZoom == m_pPaper.m_viewUpper)
	{
		CRect zoomRect = *(CRect*)WPARAM;	
		CPoint docPoint = zoomRect.CenterPoint();
		docPoint.x = zoomRect.left+zoomRect.Width()/2;//当zoomRect的值过大时，CenterPoint（）函数执行有误
		docPoint.y = zoomRect.top +zoomRect.Height()/2;
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
		
		if(m_pIMapdcom != nullptr && m_pProjection != nullptr)
		{
			WORD wSubMapId=0;
			if(m_pIMapdcom->GetSubMap(m_wMapId,geoPoint.x,geoPoint.y,&wSubMapId) == S_OK)
			{
				this->OpenMap(wSubMapId,0);
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
		docPoint.y = zoomRect.top +zoomRect.Height()/2;
		CPoint  cliPoint = docPoint;
		
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

LONG CMapeasyCtrl::OnZoomOut(UINT WPARAM, LONG LPARAM)
{
	CMapeasyApp* pApp = (CMapeasyApp*)AfxGetApp();
	if(m_uZoom <= m_pPaper.m_viewLower)
	{
		if(m_pIMapdcom != nullptr)
		{
			WORD wParentMapId=0; 
			if(m_pIMapdcom->GetParentMap(m_wMapId,&wParentMapId) == S_OK)
			{
				if(m_pProjection != nullptr)
				{		
					CRect cliRect;
					GetClientRect(&cliRect);
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

				return  this->OpenMap(wParentMapId,1000);
			}
		}
	}
	else
	{
		CRect zoomRect = *(CRect*)WPARAM;	
		
		CPoint  docPoint = zoomRect.CenterPoint();
		docPoint.x = zoomRect.left+zoomRect.Width()/2;//当zoomRect的值过大时，CenterPoint（）函数执行有误
		docPoint.y = zoomRect.top +zoomRect.Height()/2;
		CPoint  cliPoint = docPoint;
		
		DocToClient(cliPoint);
		DocToClient(zoomRect);
		
		m_pPaper.ZoomView(this,m_uZoom,docPoint,cliPoint,0.5);
	}

	return 0L;
}

LONG CMapeasyCtrl::OnPrepareDC(UINT WPARAM, LONG LPARAM)
{
	CDC* pDC = (CDC*)WPARAM;
	CPrintInfo* pInfo = (CPrintInfo*)LPARAM;

	OnPrepareDC(pDC,pInfo);

	return 1L;
}

LONG CGrfView::OnSetMatrix(UINT WPARAM, LONG LPARAM)
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

LONG CMapeasyCtrl::OnCalLength(UINT WPARAM, LONG LPARAM)
{
	static double length = 0.0f;
	if(m_pProjection != nullptr)
	{
		length = m_pProjection->CalLength(WPARAM);
	}
	else
	{
		length = CGauss::StaticCalLength(WPARAM);
	}


	return (LONG)(&length);
}

LONG CMapeasyCtrl::OnCalArea(UINT WPARAM, LONG LPARAM)
{
	static double area = 0.0f;
	if(m_pProjection != nullptr)
	{
		area = m_pProjection->CalArea(WPARAM);
	}
	else
	{
		area = CGauss::StaticCalArea(WPARAM);
	}
	return (LONG)&area;
}

VARIANT_BOOL CMapeasyCtrl::OpenAtlas(BSTR bstrAtlas)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pIMapdcom != nullptr)
	{
		if(m_pIMapdcom->OpenAtlas(bstrAtlas) == S_OK)
		{
			return VARIANT_TRUE;
		}
		else
		{
			return VARIANT_FALSE;
		}
	}
	else
		return VARIANT_FALSE;
}

VARIANT_BOOL CMapeasyCtrl::OpenMap(USHORT wMapId, USHORT nZoom)
{
	if(wMapId == m_wMapId)
		return VARIANT_TRUE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	delete m_pProjection;
	m_pProjection = nullptr;
	
	m_library.RemoveAll();
	m_layertree.RemoveAll();
	
	m_ptViewPos = CPoint(0, 0);
	m_sizeView = CSize(0,0);
	m_extView = CSize(72,72);

	if(m_pIMapdcom != nullptr)
	{
		VARIANT varData;
		varData.parray = nullptr;
		if(m_pIMapdcom->OpenMap(wMapId,&varData) == S_OK)
		{
			if(varData.vt == (VT_ARRAY | VT_UI1))
			{
				int length = varData.parray->rgsabound[0].cElements;
				if(length>0)
				{
					BYTE* pData = nullptr;
					SafeArrayAccessData(varData.parray, (void**)&pData); // Get the data
					CLzari lzari;
					BYTE* pNewData = lzari.UnCompress(pData,length,length);
					SafeArrayUnaccessData(varData.parray);	    // Unaccess it.
					
					pData = pNewData;
					CMemFile file;
					file.Attach(pData,length);
					CArchive ar((CFile*)&file, CArchive::load);

					ar >> m_wMapId;
					m_Datainfo.ReleaseDCOM(ar);
			
					BYTE type = m_pProjection == nullptr ? 0 : m_pProjection->Gettype();
					ar >> type;
					m_pProjection = CProjection::Apply(type);
					if(m_pProjection != nullptr)
					{
						m_pProjection->ReleaseDCOM(ar);		
					}

					m_library.ReleaseDCOM(ar);
					m_layertree.ReleaseDCOM(ar);
					m_pPaper.ReleaseDCOM(ar);	

					m_database.ReleaseDCOM(ar);
					int nCount;
					ar >> nCount;
					for(int index=0;index<nCount;index++)
					{
						BYTE type;
						ar >> type;
						CLink* link = CLink::Apply(type);
						if(link != nullptr)
						{
							link->ReleaseDCOM(ar);
							m_linklist.AddTail(link);
						}
					}
				
				//	ar.Flush(); 
					ar.Close();
					
				//	file.Flush();
				//	file.Detach();
				//	file.Close();

					lzari.Release();
				}
				if(varData.parray != nullptr)
				{
					SafeArrayDestroy(varData.parray);
					varData.parray = nullptr;
				}
			}
			else 
			{
				if(varData.parray != nullptr)
				{
					SafeArrayDestroy(varData.parray);
					varData.parray = nullptr;
				}

				return VARIANT_FALSE;
			}
		}
		else 
		{
			return VARIANT_FALSE;
		}
	}

	if(nZoom < m_pPaper.m_viewLower)
	{
		m_uZoom = m_pPaper.m_viewLower;
	}
	else if(nZoom > m_pPaper.m_viewUpper)
	{
		m_uZoom = m_pPaper.m_viewUpper;
	}
	else
	{
		m_uZoom = nZoom;
	}

	unsigned long nDecimal = m_pPaper.GetDecimal();
	m_WindowRect = CRect(0,0,m_pPaper.m_sizeCanvas.cx*nDecimal,m_pPaper.m_sizeCanvas.cy*nDecimal);

	CSize sizeView;
	sizeView.cx = ((double)m_WindowRect.Width() /nDecimal)*(m_uZoom/100.0f);
	sizeView.cy = ((double)m_WindowRect.Height()/nDecimal)*(m_uZoom/100.0f);

	CRect rect;
	this->GetClientRect(&rect);
	
	if(rect.Width() > sizeView.cx && rect.Height() > sizeView.cy)
	{
		CClientDC dc(this);
		float cx = (m_WindowRect.Width() /(nDecimal*72.0f))*dc.GetDeviceCaps(LOGPIXELSX);
		float cy = (m_WindowRect.Height()/(nDecimal*72.0f))*dc.GetDeviceCaps(LOGPIXELSY);
		float xscale = rect.Width() /cx;
		float yscale = rect.Height()/cy;

		float doctoviewZoom = min(xscale,yscale);
		if((long)(doctoviewZoom*100)<=m_pPaper.m_viewUpper)
		{
			m_uZoom = round(doctoviewZoom*100);
			m_pPaper.m_viewLower = m_uZoom;
		}
	}

	this->SetViewSize(CPoint(0,0),CPoint(0,0),FALSE);
	this->Invalidate(true);
	
	return VARIANT_TRUE;
}

VARIANT_BOOL CMapeasyCtrl::OpenMapByName(LPCTSTR bstrMap)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pIMapdcom != nullptr)
	{
		WORD wMapId=0;
		BSTR bstrMap = SysAllocString(bstrMap);

		if(m_pIMapdcom->GetMapId(bstrMap,&wMapId) == S_OK)
			return this->OpenMap(wMapId,0);
		else
			return VARIANT_FALSE;
	}
	else
		return VARIANT_FALSE;
}

VARIANT_BOOL CMapeasyCtrl::OpenMatchMap(DOUBLE lng, DOUBLE lat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPointF geoPoint = CPointF(lng,lat);
	if(m_pIMapdcom != nullptr)
	{
		WORD wMapId;
		if(m_pIMapdcom->MatchMap(geoPoint.x,geoPoint.y,&wMapId) == S_OK)
		{
			CRect cliRect;
			GetClientRect(&cliRect);
			CPoint cliPoint = cliRect.CenterPoint();

			CString stringX;
			CString stringY;
			stringX.Format(_T("%.5f"),geoPoint.x);
			stringY.Format(_T("%.5f"),geoPoint.y);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);

			return this->OpenMap(wMapId,100);
		}
	}
	else
		return FALSE;

	return VARIANT_TRUE;
}

VARIANT_BOOL CMapeasyCtrl::CloseAtlas(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码

	return VARIANT_TRUE;
}

VARIANT_BOOL CMapeasyCtrl::CloseMap(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	delete m_pProjection;
	m_pProjection = nullptr;
	
	m_library.RemoveAll();
	m_layertree.RemoveAll();
	
	m_ptViewPos = CPoint(0, 0);
	m_sizeView = CSize(0,0);
	m_extView = CSize(72,72);
	m_uZoom	= 0;

	this->Invalidate(true);

	return VARIANT_TRUE;
}

void CMapeasyCtrl::ScrollView(LONG cx, LONG cy, VARIANT_BOOL bUpdate)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	OnScrollBy(CSize(cx,cy),bUpdate);
}

void CMapeasyCtrl::AimatView(LONG docX, LONG docY, LONG cliX, LONG cliY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint docPoint = CPoint(docX,docY);
	CPoint cliPoint = CPoint(cliX,cliY);
	this->OnFixateView((UINT)&docPoint,(LONG)&cliPoint);
}

void CMapeasyCtrl::SetTool(BYTE bType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	switch(bType)
	{
	case 0:
		pTool = nullptr;
		break;
	case 1:
		zoomTool.m_bOut = false;
		pTool = &zoomTool;
		break;
	case 2:
		zoomTool.m_bOut = true;
		pTool = &zoomTool;
		break;
	case 3:
		pTool = &panTool;
		break;
	case 4:
		pTool = &lengthTool;
		break;
	case 5:
		pTool = &areaTool;
		break;
	case 6:
		break;
	case 7:
		break;
	}
}

BYTE CMapeasyCtrl::GetTool(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BYTE bType = 0;
	if(pTool == nullptr)
	{
		bType = 0;
	}
	else if(pTool == &zoomTool)
	{
		bType = zoomTool.m_bOut == false ? 1 : 2;
	}
	else if(pTool == &panTool)
	{
		bType = 3;
	}
	else if(pTool == &lengthTool)
	{
		bType = 4;
	}
	else if(pTool == &areaTool)
	{
		bType = 5;
	}

	return bType;
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
