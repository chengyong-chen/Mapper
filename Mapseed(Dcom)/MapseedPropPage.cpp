// MapseedPropPage.cpp : CMapseedPropPage 属性页类的实现。

#include "stdafx.h"
#include "Mapseed.h"
#include "MapseedPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMapseedPropPage, COlePropertyPage)



// 消息映射

BEGIN_MESSAGE_MAP(CMapseedPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CMapseedPropPage, "MAPSEED.MapseedPropPage.1",
	0xbbc3e2f8, 0x263e, 0x46eb, 0x8d, 0x4f, 0xb5, 0x5c, 0x8e, 0x95, 0x37, 0x2f)



// CMapseedPropPage::CMapseedPropPageFactory::UpdateRegistry -
// 添加或移除 CMapseedPropPage 的系统注册表项

BOOL CMapseedPropPage::CMapseedPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if(bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MAPSEED_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, nullptr);
}



// CMapseedPropPage::CMapseedPropPage - 构造函数

CMapseedPropPage::CMapseedPropPage() :
	COlePropertyPage(IDD, IDS_MAPSEED_PPG_CAPTION)
{
}



// CMapseedPropPage::DoDataExchange - 在页和属性间移动数据

void CMapseedPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CMapseedPropPage 消息处理程序
