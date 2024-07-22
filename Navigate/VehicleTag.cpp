#include "stdafx.h"
#include "VehicleTag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CVehicleTag, CWnd);

BEGIN_MESSAGE_MAP(CVehicleTag, CWnd)
	//{{AFX_MSG_MAP(CVehicleTag)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CVehicleTag::CVehicleTag(CString string)
{
	m_pParentWnd = nullptr;
	m_string = string;
}

BOOL CVehicleTag::Create(CWnd* pParentWnd)
{
	ASSERT(this!=nullptr);
	ASSERT(pParentWnd!=nullptr);

	CFont font;
	font.CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, _T("MS SansSerif"));

	CClientDC dc(nullptr);
	CFont* pOldFont = dc.SelectObject(&font);
	CSize size = dc.GetTextExtent(m_string);
	dc.LPtoDP(&size);

	m_pParentWnd = pParentWnd;
	return CreateEx(WS_EX_TOOLWINDOW, nullptr, nullptr, WS_POPUP|WS_CHILD|WS_CLIPSIBLINGS, CRect(0, 0, size.cx+7, size.cy+2), pParentWnd, 0, nullptr);
}

void CVehicleTag::Move(CPoint point)
{
	ASSERT(this!=nullptr);
	ASSERT(m_hWnd!=nullptr);

	CRect rect;
	m_pParentWnd->GetClientRect(rect);
	if(rect.PtInRect(point)==FALSE)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		m_pParentWnd->ClientToScreen(&point);
		SetWindowPos(&wndTop, point.x+20, point.y-30, 0, 0, SWP_SHOWWINDOW|SWP_NOOWNERZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
	}
}

//Close the tooltip
void CVehicleTag::Close()
{
	ASSERT(this!=nullptr);
	ASSERT(m_hWnd!=nullptr);

	ShowWindow(SW_HIDE);
}

void CVehicleTag::OnPaint()
{
	CPaintDC dc(this);

	CPen penBlack(PS_SOLID, 0, COLORREF(RGB(0, 0, 0)));
	CBrush brushToolTag(GetSysColor(COLOR_INFOBK));
	CPen* pOldPen = dc.SelectObject(&penBlack);
	CBrush* pOldBrush = dc.SelectObject(&brushToolTag);

	CRect rect;
	GetClientRect(rect);
	dc.Rectangle(rect);

	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);

	CFont font;
	font.CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, _T("MS SansSerif"));

	CFont* pOldFont = dc.SelectObject(&font);
	dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
	dc.SetTextAlign(TA_LEFT);
	dc.SetBkMode(TRANSPARENT);

	dc.TextOut(3, 1, m_string);

	dc.SelectObject(pOldFont);
	font.DeleteObject();
}
