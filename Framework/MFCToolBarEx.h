#pragma once

class CMFCToolBarEx : public CMFCToolBar
{
public:
	void OnFillBackground(CDC* pDC) override
	{
		if(pDC&&this->GetSafeHwnd()&&this->IsFloating())
		{
			CRect rc;
			this->GetClientRect(&rc);
			pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_WINDOW));
		}

		CMFCToolBar::OnFillBackground(pDC);
	}
};
