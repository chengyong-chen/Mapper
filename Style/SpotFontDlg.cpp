#include "stdafx.h"

#include "SpotFont.h"
#include "SpotFontDlg.h"
#include "FontDesc.h"
#include "Color.h"
#include "ColorDlg.h"
#include "../Public/Global.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CSpotFontDlg::COLUMN = 5;
long CSpotFontDlg::ROW = 3;

CSpotFontDlg::CSpotFontDlg(CWnd* pParent, CSpotFont* pSpot)
	: CDialog(CSpotFontDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpotFontDlg)
	m_nSize = 0;
	//}}AFX_DATA_INIT
	d_pSpot = pSpot;
	m_nSize = pSpot->m_nSize / 10;

	m_nFontCount = 0;
	m_nLineCount = 0;
}

void CSpotFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotFontDlg)
	DDX_Control(pDX, IDC_FONTCOMBO, m_fontcombo);
	DDX_Text(pDX, IDC_CHARSIZE, m_nSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpotFontDlg, CDialog)
	//{{AFX_MSG_MAP(CSpotFontDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_BN_DOUBLECLICKED(IDC_COLOR, OnDCColor)
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_CHARSIZE, OnChangeSize)
	ON_CBN_SELCHANGE(IDC_FONTCOMBO, OnSelchangeFontcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotFontDlg message handlers

BOOL CSpotFontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_fontcombo.LoadFontList(SYMBOL_CHARSET);

	CStringA strFolder = GetExeDirectory() + "Font\\";
	if(m_fontcombo.MatchFont(d_pSpot->m_strFontFace) == true)
	{
		memset(&m_logFont, 0, sizeof(LOGFONT));
		wsprintf(m_logFont.lfFaceName, d_pSpot->m_strFontFace);
		m_logFont.lfCharSet = SYMBOL_CHARSET;
		m_logFont.lfHeight = -32;
		m_logFont.lfWidth = -16;
		m_nFontCount = 255 - 0X20 - 1;

		m_nSelected = d_pSpot->m_wId - 0X20;
	}

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	OnSelchangeFontcombo();

	return TRUE;
}

void CSpotFontDlg::DrawFont()
{
	CRect rect;
	GetDlgItem(IDC_CHARLIST)->GetClientRect(rect);
	if(m_nFontCount <= COLUMN * ROW)
	{
		GetDlgItem(IDC_SCROLLBAR)->ShowWindow(SW_HIDE);
	}
	else
	{
		CRect rectBar;
		GetDlgItem(IDC_SCROLLBAR)->GetClientRect(rectBar);
		GetDlgItem(IDC_SCROLLBAR)->ShowWindow(SW_SHOW);
		rect.right -= rectBar.Width();
	}

	CClientDC dc(GetDlgItem(IDC_CHARLIST));

	CBrush brush;
	CFont font;
	CFont* pFont = nullptr;
	for(long I = m_nLineCount * COLUMN, J = 0; J < COLUMN * ROW; I++, J++)
	{
		CRect box(rect.left + (J % COLUMN) * rect.Width() / COLUMN,
			rect.top + (J / COLUMN) * rect.Height() / ROW,
			rect.left + (J % COLUMN + 1) * rect.Width() / COLUMN,
			rect.top + (J / COLUMN + 1) * rect.Height() / ROW);
		if(m_nLineCount * COLUMN + J >= m_nFontCount)
		{
			dc.FillSolidRect(&box, GetSysColor(COLOR_3DFACE));
			continue;
		}

		box.InflateRect(-1, -1);
		if(I == m_nSelected)
			dc.Rectangle(&box);
		else
			dc.DrawFrameControl(&box, DFC_BUTTON, DFCS_CHECKED | DFCS_BUTTONPUSH);

		box.InflateRect(-2, -2);
		brush.CreateSolidBrush(RGB(255, 255, 255));
		if(I == m_nSelected)
		{
			CBrush* pBrush = dc.SelectObject(&brush);
			dc.Rectangle(&box);
			ASSERT(pBrush);
			dc.SelectObject(pBrush);
		}
		else
		{
			dc.FillRect(&box, &brush);
		}

		CString strChar;
		strChar.Format(_T("%c"), 0X20 + I);

		if(I == m_nLineCount * COLUMN)
		{
			if(box.Width() > box.Height())
			{
				m_logFont.lfHeight = box.Height();
				m_logFont.lfWidth = 0;
			}
			else
			{
				m_logFont.lfHeight = box.Width();
				m_logFont.lfWidth = 0;
			}

			if(font.CreateFontIndirect(&m_logFont) == FALSE)
			{
				brush.DeleteObject();
				AfxMessageBox(_T("Cann't create the Font!"), MB_ICONSTOP);
				return;
			}

			pFont = dc.SelectObject(&font);
		}

		ASSERT(pFont);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(0, 0, 0));
		const UINT oldAlign = dc.SetTextAlign(TA_BOTTOM | TA_CENTER);
		dc.TextOut((box.left + box.right) / 2, box.bottom, strChar);
		dc.SetTextAlign(oldAlign);
		brush.DeleteObject();
	}

	if(pFont != nullptr)
	{
		dc.SelectObject(pFont);
		font.DeleteObject();
	}
}

void CSpotFontDlg::DrawFontUnit(long nUnitNum, long nType)
{
	if(nUnitNum < m_nLineCount * COLUMN || nUnitNum >= m_nLineCount * COLUMN + COLUMN * ROW)
		return;

	CRect rect;
	GetDlgItem(IDC_CHARLIST)->GetClientRect(rect);
	if(m_nFontCount <= COLUMN * ROW)
		GetDlgItem(IDC_SCROLLBAR)->ShowWindow(SW_HIDE);
	else
	{
		CRect rectBar;
		GetDlgItem(IDC_SCROLLBAR)->GetClientRect(rectBar);
		GetDlgItem(IDC_SCROLLBAR)->ShowWindow(SW_SHOW);
		rect.right -= rectBar.Width();
	}

	CClientDC dc(GetDlgItem(IDC_CHARLIST));

	CBrush brush;
	const long J = nUnitNum - m_nLineCount * COLUMN;
	CRect box(rect.left + (J % COLUMN) * rect.Width() / COLUMN,
		rect.top + (J / COLUMN) * rect.Height() / ROW,
		rect.left + (J % COLUMN + 1) * rect.Width() / COLUMN,
		rect.top + (J / COLUMN + 1) * rect.Height() / ROW);
	if(m_nLineCount * COLUMN + J >= m_nFontCount)
		dc.FillSolidRect(&box, GetSysColor(COLOR_3DFACE));

	box.InflateRect(-1, -1);
	if(nType == 1) //Selected
		dc.Rectangle(&box);
	else
		dc.DrawFrameControl(&box, DFC_BUTTON, DFCS_CHECKED | DFCS_BUTTONPUSH);

	box.InflateRect(-2, -2);
	brush.CreateSolidBrush(RGB(255, 255, 255));
	if(nType == 1) //Selected
	{
		CBrush* pBrush = dc.SelectObject(&brush);
		dc.Rectangle(&box);
		dc.SelectObject(pBrush);
	}
	else
	{
		dc.FillRect(&box, &brush);
	}
	brush.DeleteObject();

	CString strChar;
	strChar.Format(_T("%c"), 0X20 + nUnitNum);

	CFont font;
	if(box.Width() > box.Height())
	{
		m_logFont.lfHeight = box.Height();
		m_logFont.lfWidth = 0;
	}
	else
	{
		m_logFont.lfHeight = box.Width();
		m_logFont.lfWidth = 0;
	}

	if(font.CreateFontIndirect(&m_logFont) == FALSE)
	{
		AfxMessageBox(_T("Cann't create the Font!"), MB_ICONSTOP);
		return;
	}

	CFont* pFont = dc.SelectObject(&font);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 0, 0));
	const UINT oldAlign = dc.SetTextAlign(TA_BOTTOM | TA_CENTER);
	dc.TextOut((box.left + box.right) / 2, box.bottom, strChar);
	dc.SetTextAlign(oldAlign);
	dc.SelectObject(pFont);
	font.DeleteObject();
}

void CSpotFontDlg::DrawPreview()
{
	if(m_nSelected == -1)
		return;

	CRect box;
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW);
	if(pWnd == nullptr)
		return;

	pWnd->GetClientRect(box);

	CRect rect(box);
	rect.InflateRect(-1, -1);
	CClientDC dc(pWnd);
	dc.FillSolidRect(box, GetSysColor(COLOR_3DFACE));

	CString strChar;
	strChar.Format(_T("%c"), 0X20 + m_nSelected);

	CFont font;
	m_logFont.lfHeight = m_nSize;
	m_logFont.lfWidth = 0;
	if(font.CreateFontIndirect(&m_logFont) == FALSE)
	{
		AfxMessageBox(_T("Cann't create the Font!"), MB_ICONSTOP);
		return;
	}

	CFont* pFont = dc.SelectObject(&font);
	dc.SetBkMode(TRANSPARENT);
	if(d_pSpot->m_pColor != nullptr)
		dc.SetTextColor(d_pSpot->m_pColor->GetMonitorRGB());
	else
		dc.SetTextColor(RGB(0, 0, 0));
	const UINT oldAlign = dc.SetTextAlign(TA_BOTTOM | TA_CENTER);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&box);
	dc.SelectClipRgn(&rgn);
	rgn.DeleteObject();
	dc.TextOut((box.left + box.right) / 2, (box.bottom + box.top + m_logFont.lfHeight) / 2, strChar);
	dc.SetTextAlign(oldAlign);
	dc.SelectObject(pFont);
	font.DeleteObject();
}

void CSpotFontDlg::OnPaint()
{
	CPaintDC dc(this);

	DrawFont();
	DrawPreview();
}

void CSpotFontDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar);
	const long nCtrlID = pScrollBar->GetDlgCtrlID();
	if(nCtrlID != IDC_SCROLLBAR)
	{
		CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	const long nMaxLineCount = ((m_nFontCount - 1) / COLUMN + 1) - ROW;
	switch(nSBCode)
	{
		case SB_BOTTOM: // Scroll to bottom.
			m_nLineCount = nMaxLineCount;
			break;
		case SB_TOP: // Scroll to top.
			m_nLineCount = 0;
			break;
		case SB_LINEDOWN: // Scroll one line down.
			if(m_nLineCount == nMaxLineCount)
				return;
			m_nLineCount++;
			break;
		case SB_LINEUP: // Scroll one line up.
			if(m_nLineCount == 0)
				return;
			m_nLineCount--;
			break;
		case SB_PAGEDOWN: // Scroll one page down.
			m_nLineCount += ROW - 1;
			break;
		case SB_PAGEUP: // Scroll one page up.
			m_nLineCount -= ROW - 1;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos.
			m_nLineCount = nPos;
			break;
		default:
			return;
	}

	if(m_nLineCount < 0)
		m_nLineCount = 0;
	else if(m_nLineCount > nMaxLineCount)
		m_nLineCount = nMaxLineCount;

	pScrollBar->SetScrollPos(m_nLineCount);
	DrawFont();

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSpotFontDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	GetDlgItem(IDC_CHARLIST)->ScreenToClient(&point);

	CRect rect;
	GetDlgItem(IDC_CHARLIST)->GetClientRect(rect);
	if(m_nFontCount > COLUMN * ROW)
	{
		CRect rectBar;
		GetDlgItem(IDC_SCROLLBAR)->GetClientRect(rectBar);
		rect.right -= rectBar.Width();
	}

	for(long I = 0; I < COLUMN * ROW; I++)
	{
		CRect box(rect.left + (I % COLUMN) * rect.Width() / COLUMN,
			rect.top + (I / COLUMN) * rect.Height() / ROW,
			rect.left + (I % COLUMN + 1) * rect.Width() / COLUMN,
			rect.top + (I / COLUMN + 1) * rect.Height() / ROW);
		box.InflateRect(-4, -4);
		if(box.PtInRect(point))
		{
			if(m_nSelected != m_nLineCount * COLUMN + I && m_nLineCount * COLUMN + I < m_nFontCount)
			{
				DrawFontUnit(m_nSelected, 0);
				m_nSelected = m_nLineCount * COLUMN + I;
				DrawFontUnit(m_nSelected, 1);
				DrawPreview();
			}
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CSpotFontDlg::OnDCColor()
{
	// TODO: Add your control notification handler code here
	CColorDlg dlg(nullptr, d_pSpot->m_pColor, RGB(0, 0, 0));
	if(dlg.DoModal() == IDOK)
	{
		delete d_pSpot->m_pColor;
		d_pSpot->m_pColor = dlg.d_pColor;
		dlg.d_pColor = nullptr;
		GetDlgItem(IDC_COLOR)->Invalidate();
		DrawPreview();
	}
}

void CSpotFontDlg::OnOK()
{
	if(m_nSelected >= 0)
	{
		CFontDesc* pDesc = m_fontcombo.GetCurSelDesc();
		if(pDesc != nullptr)
		{
			d_pSpot->m_strFontReal = pDesc->GetReal();
			d_pSpot->m_strFontFace = pDesc->GetFace();
			d_pSpot->m_wId = (unsigned short)(0X20 + m_nSelected);
			d_pSpot->m_nSize = m_nSize * 10;
		}
	}
}

void CSpotFontDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if(lpDrawItemStruct->itemID == LB_ERR)
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;

	if(lpDrawItemStruct->CtlID == IDC_COLOR)
	{
		rect.InflateRect(-1, -1);
		if(d_pSpot->m_pColor != nullptr)
			pDC->FillSolidRect(rect, d_pSpot->m_pColor->GetMonitorRGB());
		else
			pDC->FillSolidRect(rect, RGB(0, 0, 0));
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSpotFontDlg::OnChangeSize()
{
	CString str;
	GetDlgItem(IDC_CHARSIZE)->GetWindowText(str);
	m_nSize = _ttoi(str);
	DrawPreview();
}

void CSpotFontDlg::OnSelchangeFontcombo()
{
	CFontDesc* pDesc = m_fontcombo.GetCurSelDesc();
	if(pDesc == nullptr)
		return;

	d_pSpot->m_strFontReal = pDesc->GetReal();
	d_pSpot->m_strFontFace = CFontDesc::GetFaceByReal(d_pSpot->m_strFontReal);

	memset(&m_logFont, 0, sizeof(LOGFONT));
	wsprintf(m_logFont.lfFaceName, d_pSpot->m_strFontFace);
	m_logFont.lfCharSet = SYMBOL_CHARSET;
	m_logFont.lfHeight = 32;
	m_nFontCount = 255 - 0X20 - 1;

	if(m_nFontCount > COLUMN * ROW)
	{
		((CScrollBar*)GetDlgItem(IDC_SCROLLBAR))->SetScrollRange(0, ((m_nFontCount - 1) / COLUMN + 1) - ROW);
		m_nLineCount = ((m_nSelected - 1) / COLUMN + 1) - ROW;
		m_nLineCount = max(m_nLineCount, 0);
		((CScrollBar*)GetDlgItem(IDC_SCROLLBAR))->SetScrollPos(m_nLineCount);
	}

	DrawFont();
	DrawPreview();
}
