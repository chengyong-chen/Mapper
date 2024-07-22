#pragma once
#include "LogToWnd.h"
#include <afxDockablePane.h>

class __declspec(dllexport) COutputPane : public CDockablePane
{
public:
	COutputPane();

	// Attributes
protected:
	CListBox m_messagebox;
	CLogToWnd m_logtownd;
	// Implementation
public:
	~COutputPane() override;
	BOOL DestroyWindow() override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditClear();
	DECLARE_MESSAGE_MAP()
	LONG OnLogMessage(WPARAM wParam, LPARAM lParam);
};
