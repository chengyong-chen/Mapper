#include "stdafx.h"
#include "StatusCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusCombo

extern CString a_strUnitType;

CStatusCombo::CStatusCombo()
{
	maxlen = 0;
}

CStatusCombo::~CStatusCombo()
{
}

BEGIN_MESSAGE_MAP(CStatusCombo, CComboBox)
	//{{AFX_MSG_MAP(CStatusCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusCombo message handlers
/****************************************************************************
*                          CStatusCombo::OnDropdown
* Result: void
*
* Effect:
*       Adjusts the combo box to be the right size for the number of items
****************************************************************************/

void CStatusCombo::OnDropdown()
{
	int n = GetCount();
	n = max(n, 2);
	const int ht = GetItemHeight(0);
	CRect r;
	GetWindowRect(&r);

	if(maxlen>0)
		n = max(maxlen, 2);

	CSize sz;
	sz.cx = r.Width();
	sz.cy = ht*(n+2);

	if(maxlen==0)
	{
		if(r.top-sz.cy < 0||r.bottom+sz.cy > ::GetSystemMetrics(SM_CYMAXTRACK))
		{
			const int k = max((r.top/ht), (::GetSystemMetrics(SM_CYMAXTRACK)-r.bottom)/ht);
			const int ht2 = ht*k;
			sz.cy = min(ht2, sz.cy);
		}
	}

	SetWindowPos(nullptr, 0, 0, sz.cx, sz.cy, SWP_NOMOVE|SWP_NOZORDER);
}

void CStatusCombo::OnSelchange()
{
	const int n = GetCurSel();
	if(CB_ERR!=n)
	{
		GetLBText(n, a_strUnitType);
	}
}
