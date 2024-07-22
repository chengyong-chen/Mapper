#pragma once

class __declspec(dllexport) CPaneToolBar : public CMFCToolBar
{
	void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler) override
	{
		CWnd* pOwner = GetOwner();
		if(pOwner!=nullptr&&::IsWindow(pOwner->m_hWnd)==TRUE)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)pOwner, bDisableIfNoHndler);
		}
	}

	BOOL AllowShowOnList() const override
	{
		return FALSE;
	}
};

class __declspec(dllexport) CTabToolBar : public CMFCToolBar
{
	void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler) override
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}

	BOOL AllowShowOnList() const override
	{
		return FALSE;
	}
};
