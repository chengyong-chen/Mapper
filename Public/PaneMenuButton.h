#pragma once

class __declspec(dllexport) CPaneMenuButton : public CMFCToolBarMenuButton
{
	DECLARE_SERIAL(CPaneMenuButton)

public:
	CPaneMenuButton(HMENU hMenu = nullptr)
		: CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE) override
	{
		pImages = CMFCToolBar::GetMenuImages();
		if(pImages!=nullptr)
		{
			CAfxDrawState ds;
			pImages->PrepareDrawImage(ds);

			CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

			pImages->EndDrawImage(ds);
		}
		else
			CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);
	}
};
