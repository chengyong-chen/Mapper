#include "stdafx.h"
#include "TypeCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTypeCombo

CTypeCombo::CTypeCombo()
{
	maxlen = 0;
}

CTypeCombo::~CTypeCombo()
{
}

BEGIN_MESSAGE_MAP(CTypeCombo, CComboBox)
	//{{AFX_MSG_MAP(CTypeCombo)    
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTypeCombo message handlers
/****************************************************************************
*                          CTypeCombo::OnDropdown
* Result: void
*
* Effect:
*       Adjusts the combo box to be the right size for the number of items
****************************************************************************/

void CTypeCombo::OnDropdown()
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

void CTypeCombo::OnSelchange()
{
	CWnd* pParents = GetParent();
	if(pParents!=nullptr)
	{
		pParents = pParents->GetParent();
		if(pParents!=nullptr)
		{
			//			pParents->SendMessage(WM_COMBOBOXSELCHANGE,(UINT)this,0);
		}
	}
}