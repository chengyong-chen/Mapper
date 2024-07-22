#pragma once

// MapseedPropPage.h : CMapseedPropPage 属性页类的声明。


// CMapseedPropPage : 有关实现的信息，请参阅 MapseedPropPage.cpp。

class CMapseedPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMapseedPropPage)
	DECLARE_OLECREATE_EX(CMapseedPropPage)

// 构造函数
public:
	CMapseedPropPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_MAPSEED };

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持

// 消息映射
protected:
	DECLARE_MESSAGE_MAP()
};

