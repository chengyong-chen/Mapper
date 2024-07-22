#include "stdafx.h"
#include "Line.h"
#include "LineCtrl.h"

#include "LineAloneDlg.h"
#include "LineSymbol.h"
#include "LineSymbolDlg.h"
#include "LineRiver.h"
#include "LineRiverDlg.h"
#include "Library.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLineCtrl::CLineCtrl(CWnd* pParent, bool bComplex, CLibrary& library)
	: CDialog(CLineCtrl::IDD, pParent), m_library(library), m_bComplex(bComplex)
{
	//{{AFX_DATA_INIT(CLineCtrl)

	d_pLine = nullptr;
	d_pLinedlg = nullptr;
	//}}AFX_DATA_INIT	
}

CLineCtrl::~CLineCtrl()
{
	delete d_pLine;
}

void CLineCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineCtrl)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLineCtrl, CDialog)
	//{{AFX_MSG_MAP(CLineCtrl)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineCtrl message handlers

BOOL CLineCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();

	CBitmap bmp;
	BITMAP bitmapData;
	bmp.LoadBitmap(MAKEINTRESOURCE(IDB_STROKETYPES));
	bmp.GetBitmap(&bitmapData);
	m_imagelist.Create(18, 18, bitmapData.bmBitsPixel, bmp.GetBitmapDimension().cx/18, 0);
	m_imagelist.Add(&bmp, RGB(254, 254, 254));
	m_imagelist.SetBkColor(RGB(255, 255, 255));

	CComboBoxEx* pComboBoxEx = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO));
	if(pComboBoxEx!=nullptr)
	{
		pComboBoxEx->SetImageList(&m_imagelist);

		COMBOBOXEXITEM cbi;
		cbi.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT|CBEIF_LPARAM;// | CBEIF_INDENT;
		cbi.iItem = -1;

		CString string;
		if(string.LoadString(IDS_LINEINHERIT)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 0;
			cbi.iSelectedImage = 0;
			cbi.lParam = CLine::LINETYPE::Inherit;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_LINEEMPTY)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 1;
			cbi.iSelectedImage = 1;
			cbi.lParam = CLine::LINETYPE::Blank;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_LINEALONE)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 2;
			cbi.iSelectedImage = 2;
			cbi.lParam = CLine::LINETYPE::Alone;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}

		if(m_bComplex==true)
		{
			if(string.LoadString(IDS_LINESYMBOL)==TRUE)
			{
				cbi.pszText = (LPTSTR)(LPCTSTR)string;
				cbi.iImage = 3;
				cbi.iSelectedImage = 3;
				cbi.lParam = CLine::LINETYPE::Symbol;
				const int index = pComboBoxEx->InsertItem(&cbi);
				pComboBoxEx->SetItemHeight(index, 20);
			}
			if(string.LoadString(IDS_LINERIVER)==TRUE)
			{
				cbi.pszText = (LPTSTR)(LPCTSTR)string;
				cbi.iImage = 4;
				cbi.iSelectedImage = 4;
				cbi.lParam = CLine::LINETYPE::River;
				const int index = pComboBoxEx->InsertItem(&cbi);
				pComboBoxEx->SetItemHeight(index, 20);
			}
		}
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLineCtrl::OnTypeSelchange()
{
	const int index = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	const DWORD data = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetItemData(index);

	CLine* line = CLine::Apply(data);
	Reset(line);
}

void CLineCtrl::OnOK()
{
	if(d_pLinedlg!=nullptr&&d_pLinedlg->m_hWnd!=nullptr)
	{
		d_pLinedlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	if(d_pLine!=nullptr)
		d_pLine->SetWidth(m_library.m_libLineSymbol);

	CDialog::OnOK();
}

void CLineCtrl::OnCancel()
{
	delete d_pLine;

	CDialog::OnCancel();
}

void CLineCtrl::Reset(CLine* pline)
{
	delete d_pLine;
	d_pLine = nullptr;

	if(d_pLinedlg!=nullptr)
	{
		d_pLinedlg->PostMessage(WM_DESTROY, 0, 0);
		d_pLinedlg->DestroyWindow();
		delete d_pLinedlg;
		d_pLinedlg = nullptr;
	}

	d_pLine = pline;
	const CLine::LINETYPE type = d_pLine==nullptr ? CLine::LINETYPE::Inherit : d_pLine->Gettype();
	switch(type)
	{
	case CLine::LINETYPE::Inherit:
	case CLine::LINETYPE::Blank:
		break;
	case CLine::LINETYPE::Alone:
	{
		d_pLinedlg = new CLineAloneDlg((CWnd*)this, (CLine*)d_pLine);
		d_pLinedlg->Create(IDD_LINEALONE, (CWnd*)this);
		d_pLinedlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case CLine::LINETYPE::Symbol:
	{
		d_pLinedlg = new CLineSymbolDlg((CWnd*)this, m_library.m_libLineSymbol, (CLineSymbol*)d_pLine);
		d_pLinedlg->Create(IDD_LINESYMBOL, (CWnd*)this);
		d_pLinedlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case CLine::LINETYPE::River:
	{
		d_pLinedlg = new CLineRiverDlg((CWnd*)this, (CLineRiver*)d_pLine);
		d_pLinedlg->Create(IDD_LINERIVER, (CWnd*)this);
		d_pLinedlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	}
	for(int nIndex = 0; nIndex<((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetCount(); nIndex++)
	{
		const int data = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetItemData(nIndex);
		if(data==type)
		{
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
			break;
		}
	}
}

void CLineCtrl::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_imagelist.m_hImageList!=nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	if(d_pLinedlg!=nullptr)
	{
		d_pLinedlg->PostMessage(WM_DESTROY, 0, 0);
		d_pLinedlg->DestroyWindow();
		delete d_pLinedlg;
		d_pLinedlg = nullptr;
	}
}
