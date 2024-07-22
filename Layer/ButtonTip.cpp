#include "stdafx.h"
#include "ButtonTip.h"
#include "Type.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonTip

CButtonTip::CButtonTip()
{
	m_nHeight				= 0;
	m_nWidth				= 0;
	m_nFontSize				= 14;		// original size
	m_currentControlID		= 0;
	m_pParentWnd			= nullptr;

	m_aControlInfo.RemoveAll();
}

CButtonTip::~CButtonTip()
{
	ButtonInfo *pInfo = nullptr;
	int nSize = m_aControlInfo.GetSize();
	for(int i = 0; i < nSize; i++)
	{
		pInfo = (ButtonInfo*)m_aControlInfo.GetAt(i);
		delete pInfo;
	}

	m_aControlInfo.RemoveAll();
}

void CButtonTip::Create(CWnd* pWnd)
{
	m_pParentWnd = pWnd;
}

void CButtonTip::ErasePreviousToolTipDisplay(UINT nControlId)
{
	//This assertion fails because you did not call Create() first.
	//m_pParentWnd was not initialized.
	ASSERT(m_pParentWnd);

	//if erase already, do not do it again
	if((m_nHeight == 0) || (m_nWidth == 0))
		return;
	
	CRect		oInfoRect(0,0,0,0);

	CalculateInfoBoxRect(nControlId, &oInfoRect);
	m_pParentWnd->InvalidateRect(&oInfoRect);
	m_pParentWnd->UpdateWindow();

	m_nHeight = 0;
	m_nWidth = 0;
}


void CButtonTip::CalculateHeightAndWidth(CStringArray& straInfos)
{
	//This assertion fails because you did not call Create() first.
	//m_pParentWnd was not initialized.
	ASSERT(m_pParentWnd);

	int nMaxLength = 0;
	int nLength;
	int nLine = straInfos.GetSize();
	m_nTotalLine = nLine;		// holder for maximum line of information
	for(int i=0; i<nLine; i++)
	{
		nLength = (straInfos[i]).GetLength();
		if(nLength > nMaxLength)
			nMaxLength = nLength;
	}

	m_maxCharInLine = nMaxLength;		// holder for longest char info
	m_nHeight = 12 + nLine*(m_nFontSize - 1);	//m_nFontSize is the font size
	m_nWidth = 10+(int)(7*nMaxLength);	//aproximately 5 pixel per char

}


void CButtonTip::CalculateInfoBoxRect(UINT nControlId, CRect* pInfoRect)
{
	ASSERT(m_pParentWnd);
	
	CRect		oRect(0,0,0,0);
	CRect		oParentWindowRect(0,0,0,0);
	
	m_pParentWnd->GetWindowRect(&oParentWindowRect);
	m_pParentWnd->ScreenToClient(&oParentWindowRect);
	
	CWnd*		pWnd = m_pParentWnd->GetDlgItem(nControlId);
	ASSERT(pWnd);
	pWnd->GetWindowRect(&oRect);
	m_pParentWnd->ScreenToClient(&oRect);
	
	//check if the box fit in the parent dialog 
	SetFontSize(14);
	int nBottom = oRect.bottom - ((oRect.bottom - oRect.top)/2) + m_nHeight;
	if(nBottom <= oParentWindowRect.bottom)
	{
		pInfoRect->top = oRect.bottom - (oRect.bottom - oRect.top)/2;
		pInfoRect->bottom = pInfoRect->top + m_nHeight;
	}
	else
	{
		// change the size of the font as well as the info box if all
		// info data cannot be viewed
		if(m_nHeight > (oParentWindowRect.bottom - oParentWindowRect.top))
		{
			SetFontSize(12);
			m_nHeight = 12 + m_nTotalLine*(m_nFontSize - 1);	//m_nFontSize is the font size
			m_nWidth = 10 + (int)(7*m_maxCharInLine);	
		}
		// end
		pInfoRect->bottom = oParentWindowRect.bottom - 1;
		pInfoRect->top = pInfoRect->bottom - m_nHeight;
	}

	//check if the box fit in the parent dialog
	int nRight = (oRect.left + oRect.right)/2 + m_nWidth;
	if(nRight <= oParentWindowRect.right)
	{
		pInfoRect->left = (oRect.left + oRect.right)/2;
		pInfoRect->right = pInfoRect->left + m_nWidth;
	}
	else
	{
		int nLeft = (oRect.left + oRect.right)/2 - m_nWidth;
		if(nLeft <= oParentWindowRect.left)
		{
			pInfoRect->left = oParentWindowRect.left + 1;
			pInfoRect->right = pInfoRect->left + m_nWidth;
		}
		else
		{
			pInfoRect->right = (oRect.left + oRect.right)/2;
			pInfoRect->left = pInfoRect->right - m_nWidth;
		}
	}

	ASSERT(pInfoRect->top <= pInfoRect->bottom);
	ASSERT(pInfoRect->left <= pInfoRect->right);
}


void CButtonTip::ShowTag(UINT nControlId)
{						
	ButtonInfo *pToolTip = IsControlIDExisting(nControlId);
	if(pToolTip == nullptr)
		return;

	DisplayInfo(pToolTip);
}


void CButtonTip::ShowTag(const CPoint& point)
{						
	CWnd*		  pWnd = m_pParentWnd->ChildWindowFromPoint(point);
	if(pWnd)
	{
		UINT	nControlId = (UINT)pWnd->GetDlgCtrlID();
		if(m_currentControlID != nControlId)
		{
			ErasePreviousToolTipDisplay(m_currentControlID);
			ShowTag(nControlId);
		}
	}
}

void CButtonTip::DisplayInfo(ButtonInfo* pToolTip)
{
	if(pToolTip->nInfoSize <= 0)
		return;

	ASSERT(m_pParentWnd);

	CDC* pDC = m_pParentWnd->GetDC();

	CRect		oInfoRect;
	CBrush		oBrush, *pOldBrush, oBorderBrush;
	int			nX, nY;
	TEXTMETRIC	TM;
	int			nTextHigh;
	CFont		oFont, *pOldFont;
	CWnd*		pWnd = nullptr;

	pDC->SetBkMode(TRANSPARENT);
	
	oBrush.CreateSolidBrush(pToolTip->nBackColor);

	pOldBrush = pDC->SelectObject(&oBrush);
	pDC->SetTextColor(pToolTip->nTextColor);

	//calculate the width and height of the box dynamically
	CalculateHeightAndWidth(pToolTip->nControlInfo);
	CalculateInfoBoxRect(pToolTip->nControlId, &oInfoRect);

	oFont.CreateFont(m_nFontSize, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, CType::GetDefaultFont());
	pOldFont = pDC->SelectObject(&oFont);
	
	pDC->FillRect(&oInfoRect, &oBrush);
	pDC->SelectObject(pOldBrush);
	oBrush.DeleteObject();

	oBorderBrush.CreateSolidBrush(pToolTip->nTextColor);
	pOldBrush = pDC->SelectObject(&oBorderBrush);
	DrawEdge(pDC->m_hDC, &oInfoRect, BDR_RAISEDINNER | BDR_SUNKENOUTER, BF_RECT);
	
	pDC->SetTextAlign(TA_LEFT);
	pDC->GetTextMetrics(&TM);
	nTextHigh = TM.tmHeight + TM.tmExternalLeading - 2;
	
	nX = oInfoRect.left + 8;
	nY = oInfoRect.top + 5; 

	for(register UINT i = 0; i < pToolTip->nInfoSize; i++)
	{
		pDC->TextOut(nX, nY, pToolTip->nControlInfo[i]);
		nY += m_nFontSize - 1;
	}

	pDC->SelectObject(pOldBrush);
	oBorderBrush.DeleteObject();	
	m_pParentWnd->ReleaseDC(pDC);
}	

BOOL CButtonTip::AddControlInfo(UINT contolID, CStringArray& straInfo, COLORREF back, COLORREF text)
{
	ButtonInfo *pToolTip = new ButtonInfo;
	ASSERT(pToolTip != nullptr);

	int nSize = straInfo.GetSize();
	if(pToolTip <= 0)	// no information, don't add to the list and return FALSE
	{
		delete pToolTip;
		return FALSE;
	}

	pToolTip->nControlInfo.RemoveAll();
	pToolTip->nInfoSize  = nSize;
	pToolTip->nControlId = contolID;
	for(register int i = 0; i < nSize; i++)
	{
		pToolTip->nControlInfo.Add(straInfo[i]);
	}
	// initialize colors, use default if not supplied
	pToolTip->nBackColor = back;
	pToolTip->nTextColor = text;

	// add to the list
	m_aControlInfo.Add(pToolTip);

	return TRUE;
}

ButtonInfo* CButtonTip::IsControlIDExisting(UINT controlID)
{
	ButtonInfo *pToolTip = nullptr;
	int nSize = m_aControlInfo.GetSize();
	for(register int i = 0; i < nSize; i++)
	{
		pToolTip = (ButtonInfo*)m_aControlInfo.GetAt(i);
		if(pToolTip->nControlId == controlID)		// if found!
		{
			m_currentControlID = controlID;
			return pToolTip;
		}
	}
	m_currentControlID = 0;		// if not found, reset the current control ID to refresh the display

	return nullptr;
}

BOOL CButtonTip::RemoveControlInfo(UINT controlID)
{
	ButtonInfo *pToolTip = nullptr;
	int nSize = m_aControlInfo.GetSize();
	for(register int i = 0; i < nSize; i++)
	{
		pToolTip = (ButtonInfo*)m_aControlInfo.GetAt(i);
		if(pToolTip->nControlId == controlID)		// if found!
		{				
			if(m_currentControlID == controlID)
				ErasePreviousToolTipDisplay(m_currentControlID);

			m_aControlInfo.RemoveAt(i);	// remove the entry
			delete pToolTip;
			break;		// break from the loop
		}
	}

	return TRUE;
}

