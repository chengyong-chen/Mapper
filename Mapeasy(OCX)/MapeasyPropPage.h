#pragma once

// MapeasyPropPage.h : CMapeasyPropPage 属性页类的声明。


// CMapeasyPropPage : 有关实现的信息，请参阅 MapeasyPropPage.cpp。

class CMapeasyPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMapeasyPropPage)
	DECLARE_OLECREATE_EX(CMapeasyPropPage)

// 构造函数
public:
	CMapeasyPropPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_MAPEASY };

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持

// 消息映射
protected:
	DECLARE_MESSAGE_MAP()
};

