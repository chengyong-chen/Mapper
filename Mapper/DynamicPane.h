#pragma once

class CDynamicDlg;

class CDynamicPane : public CDockablePane
{
	DECLARE_DYNAMIC(CDynamicPane)

public:
	CDynamicPane();

	~CDynamicPane() override;

public:
	CDynamicDlg* m_DynamicDlg = nullptr;

protected:
	virtual BOOL CanAdjustLayout() const { return FALSE; }
	BOOL CanBeClosed() const override { return FALSE; }

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};