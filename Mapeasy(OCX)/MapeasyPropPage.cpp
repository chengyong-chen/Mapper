// MapeasyPropPage.cpp : CMapeasyPropPage 属性页类的实现。

#include "stdafx.h"
#include "Mapeasy.h"
#include "MapeasyPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMapeasyPropPage, COlePropertyPage)



// 消息映射

BEGIN_MESSAGE_MAP(CMapeasyPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CMapeasyPropPage, "MAPEASY.MapeasyPropPage.1",
	0xbbc3e2f8, 0x263e, 0x46eb, 0x8d, 0x4f, 0xb5, 0x5c, 0x8e, 0x95, 0x37, 0x2f)



// CMapeasyPropPage::CMapeasyPropPageFactory::UpdateRegistry -
// 添加或移除 CMapeasyPropPage 的系统注册表项

BOOL CMapeasyPropPage::CMapeasyPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if(bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MAPEASY_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, nullptr);
}



// CMapeasyPropPage::CMapeasyPropPage - 构造函数

CMapeasyPropPage::CMapeasyPropPage() :
	COlePropertyPage(IDD, IDS_MAPEASY_PPG_CAPTION)
{
}



// CMapeasyPropPage::DoDataExchange - 在页和属性间移动数据

void CMapeasyPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CMapeasyPropPage 消息处理程序
