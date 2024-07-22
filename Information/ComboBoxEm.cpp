#include "stdafx.h"
#include "ComboBoxEm.h"

#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBoxEm

CComboBoxEm::CComboBoxEm()
{
}

CComboBoxEm::~CComboBoxEm()
{
}

BEGIN_MESSAGE_MAP(CComboBoxEm, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxEm)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxEm message handlers

void CComboBoxEm::OnSelchange()
{
	CWnd* pParents = GetParent();
	if(pParents!=nullptr)
	{
		pParents = pParents->GetParent();
		if(pParents!=nullptr)
		{
			const int nIndex = GetCurSel();
			pParents->SendMessage(WM_COMBOBOXSELCHANGE, (UINT)this, nIndex);
		}
	}
}
