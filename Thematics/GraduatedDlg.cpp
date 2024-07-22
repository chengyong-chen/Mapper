#include "stdafx.h"
#include "GraduatedDlg.h"
#include "afxdialogex.h"
#include <stdlib.h>

#include "Resource.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGraduatedDlg, CClassificationDlg)
IMPLEMENT_DYNCREATE(CSymbolCell, CGridCell)

CGraduatedDlg::CGraduatedDlg(CWnd* pParent, CGraduated& graduated, CDatabase& database, CATTDatasource& datasource)
	: CClassificationDlg(CGraduatedDlg::IDD, pParent, database, datasource), m_graduated(graduated)
{
}

CGraduatedDlg::~CGraduatedDlg()
{
	m_StepsListCtrl.DeleteAllItems();
}

void CGraduatedDlg::DoDataExchange(CDataExchange* pDX)
{
	CClassificationDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_StepsListCtrl);
	DDX_Control(pDX, IDC_COMBO, m_SymbolCombo);
}

BEGIN_MESSAGE_MAP(CGraduatedDlg, CClassificationDlg)
	ON_WM_PAINT()
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_LIST1, OnGridEndEdit)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_CBN_SELCHANGE(IDC_COMBO, &CGraduatedDlg::OnCbnSelchangeCombo)
END_MESSAGE_MAP()

BOOL CGraduatedDlg::OnInitDialog()
{
	CClassificationDlg::OnInitDialog();

	m_font.CreatePointFont(120, _T("Map Symbols"));
	m_SymbolCombo.SetFont(&m_font);
	for(int index = 0; index<100; index++)
	{
		CString strShape;
		strShape.Format(_T("%c"), 0X20+index);
		m_SymbolCombo.AddString(strShape);
	}
	m_SymbolCombo.SetCurSel(m_graduated.m_symbol);

	try
	{
		m_StepsListCtrl.SetRowCount(1);
		m_StepsListCtrl.SetFixedRowCount(1);
		m_StepsListCtrl.SetColumnCount(5);
		m_StepsListCtrl.SetColumnWidth(0, 50);
		m_StepsListCtrl.SetColumnWidth(1, 40);
		m_StepsListCtrl.SetColumnWidth(2, 40);
		m_StepsListCtrl.SetColumnWidth(3, 35);
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nFormat = DT_LEFT|DT_WORDBREAK;
		Item.row = 0;
		Item.col = 0;
		Item.strText = _T("Symbol");
		m_StepsListCtrl.SetItem(&Item);
		Item.col = 1;
		Item.strText = _T("From");
		m_StepsListCtrl.SetItem(&Item);
		Item.col = 2;
		Item.strText = _T("To");
		m_StepsListCtrl.SetItem(&Item);
		Item.col = 3;
		Item.strText = _T("Size");
		m_StepsListCtrl.SetItem(&Item);

		m_StepsListCtrl.SetEditable(TRUE);
		//		m_StepsListCtrl.EnableD2DSupport();
		m_StepsListCtrl.EnableSelection();
		m_StepsListCtrl.EnableWindow();
		m_StepsListCtrl.SetHeaderSort(FALSE);
		m_StepsListCtrl.SetRowResize(FALSE);
		m_StepsListCtrl.SetSingleRowSelection(FALSE);
		m_StepsListCtrl.SetSingleColSelection(FALSE);
		m_StepsListCtrl.EnableDragAndDrop(FALSE);
		m_StepsListCtrl.EnableDragRowMode(FALSE);
		m_StepsListCtrl.EnableTitleTips(FALSE);
		m_StepsListCtrl.EnableToolTips(FALSE);
		m_StepsListCtrl.EnableTrackingToolTips(FALSE);
		m_StepsListCtrl.SetFixedColumnCount(1);
	}
	catch(CMemoryException*ex)
	{
		OutputDebugString(_T("Error when create StepsListCtrl"));
		ex->ReportError();
		ex->Delete();
	}

	CRect rect;
	GetDlgItem(IDC_CUSTOM)->GetClientRect(rect);
	GetDlgItem(IDC_CUSTOM)->MapWindowPoints(this, &rect);
	m_PropertyGrid.Create(WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP, rect, this, 1231);
	m_PropertyGrid.SetVSDotNetLook(TRUE);
	m_PropertyGrid.MarkModifiedProperties();

	/*{
		CMFCPropertyGridProperty* pProp1 = new CMFCPropertyGridProperty( _T( "Symbol" ) _T(""), _T(""));
		for(int index=0;index<100;index++)
		{
			CString strShape;
			strShape.Format(_T("%c"), 0X20+index);
			pProp1->AddOption(strShape);
		}
		m_PropertyGrid.AddProperty(pProp1);
	}*/
	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Color"));
	{
		CMFCPropertyGridColorProperty* pProp1 = new CMFCPropertyGridColorProperty(_T("Border"), m_graduated.m_colorBorder);
		pProp1->EnableOtherButton(_T("Other..."));
		pProp1->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
		pGroup2->AddSubItem(pProp1);
		CMFCPropertyGridColorProperty* pProp2 = new CMFCPropertyGridColorProperty(_T("Fill"), m_graduated.m_colorFill);
		pProp2->EnableOtherButton(_T("Other..."));
		pProp2->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
		pGroup2->AddSubItem(pProp2);
		CMFCPropertyGridProperty* pProp3 = new CMFCPropertyGridProperty(_T("Transparency"), (_variant_t)((short)m_graduated.m_Alpha), _T(""));
		pProp3->EnableSpinControl(TRUE, 0, 255);
		pGroup2->AddSubItem(pProp3);
		m_PropertyGrid.AddProperty(pGroup2);
	}
	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border Width"), (_variant_t)m_graduated.m_widthBorder, _T(""));
	m_PropertyGrid.AddProperty(pProp);

	m_PropertyGrid.ExpandAll();
	m_PropertyGrid.SetFocus();

	if(m_graduated.m_divisions.size()>1)
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(2);
		const int count = m_graduated.m_divisions.size();
		CString str;
		str.Format(_T("%d"), count);
		GetDlgItem(IDC_CLASSES)->SetWindowText(str);
		OnDivisional();
	}
	else
	{
		GetDlgItem(IDC_CLASSES)->SetWindowText(_T("6"));
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		OnContinuous();
	}
	return TRUE;
}

void CGraduatedDlg::OnContinuous()
{
	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck()!=BST_CHECKED)
		return;

	m_graduated.m_divisions.clear();
	while(m_StepsListCtrl.GetRowCount()>1)
		m_StepsListCtrl.DeleteRow(m_StepsListCtrl.GetRowCount()-1);

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Size"));
	{
		CMFCPropertyGridProperty* pProp1 = new CMFCPropertyGridProperty(_T("Minimal"), (_variant_t)m_graduated.m_continuous.m_minT, _T(""));
		pProp1->EnableSpinControl(TRUE, 1, 1000);
		pGroup1->AddSubItem(pProp1);
		CMFCPropertyGridProperty* pProp2 = new CMFCPropertyGridProperty(_T("Maximal"), (_variant_t)m_graduated.m_continuous.m_maxT, _T(""));
		pProp2->EnableSpinControl(TRUE, 1, 1000);
		pGroup1->AddSubItem(pProp2);
		m_PropertyGrid.AddProperty(pGroup1);
	}

	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC4)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
}

void CGraduatedDlg::OnDivisional()
{
	if(((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck()!=BST_CHECKED)
		return;

	CMFCPropertyGridProperty* pProp = CClassificationDlg::GetPropertyByName(&m_PropertyGrid, _T("Size"));
	if(pProp!=nullptr)
	{
		;
		m_PropertyGrid.DeleteProperty(pProp);
	}

	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);

	CString str;
	GetDlgItem(IDC_CLASSES)->GetWindowText(str);
	const int steps = _ttoi(str);

	while(m_StepsListCtrl.GetRowCount()>1)
		m_StepsListCtrl.DeleteRow(m_StepsListCtrl.GetRowCount()-1);

	m_graduated.RedivideSize(steps);

	int index = 1;
	for(std::list<CDivision<DWORD>>::iterator it = m_graduated.m_divisions.begin(); it!=m_graduated.m_divisions.end(); ++it)
	{
		CString str1;
		str1.Format(_T("%g"), it->m_minValue);
		CString str2;
		str2.Format(_T("%g"), it->m_maxValue);
		CString str3;
		str3.Format(_T("%d"), it->m_Value);
		m_StepsListCtrl.InsertRow(_T(""), -1);
		//	m_StepsListCtrl.SetCellColors(index,0,m_graduated.m_colorFill,0XFFFFFF);
		m_StepsListCtrl.SetItemText(index, 1, str1);
		m_StepsListCtrl.SetItemText(index, 2, str2);
		m_StepsListCtrl.SetItemText(index, 3, str3);

		m_StepsListCtrl.SetCellType(index, 0, RUNTIME_CLASS(CSymbolCell));
		CSymbolCell* pSymbolCell = (CSymbolCell*)m_StepsListCtrl.GetCell(index, 0);
		if(pSymbolCell!=nullptr)
		{
			pSymbolCell->m_pGraduatedDlg = this;
			pSymbolCell->SetState(pSymbolCell->GetState()|GVIS_READONLY);
		}
		m_StepsListCtrl.SetRowHeight(index, max(15, it->m_Value+4));
		index++;
	}
	this->Invalidate();
}

void CGraduatedDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawShape(GetDlgItem(IDC_STATIC1), m_graduated.m_symbol, m_graduated.m_continuous.m_minT, m_graduated.m_colorBorder, m_graduated.m_colorFill);
	DrawShape(GetDlgItem(IDC_STATIC2), m_graduated.m_symbol, m_graduated.m_continuous.m_maxT, m_graduated.m_colorBorder, m_graduated.m_colorFill);
}

void CGraduatedDlg::OnFieldAdded(const CString& strField)
{
	if(m_graduated.AddField(m_database, m_datasource, strField)==true)
	{
		OnDivisional();
		this->Invalidate();
	}
}

void CGraduatedDlg::OnFieldRemoved(const CString& strField)
{
	m_graduated.RemoveField(m_database, m_datasource, strField);

	OnDivisional();
	this->Invalidate();
}

void CGraduatedDlg::OnRemoveAllFields()
{
	m_graduated.RemoveAllFields();
	OnDivisional();
}

void CGraduatedDlg::DrawShape(CWnd* pWnd, int dwId, int size, COLORREF rgbBorder, COLORREF rgbFill)
{
	if(pWnd==nullptr)
		return;

	CRect rect;
	pWnd->GetClientRect(rect);

	CClientDC dc(pWnd);
	CFont font;
	font.CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Map Symbols"));
	//	font.CreatePointFont(size.cy*10,_T("Map Symbols"));
	dc.SelectObject(font);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(GetRValue(rgbFill), GetGValue(rgbFill), GetBValue(rgbFill)));
	dc.SetTextAlign(TA_BOTTOM|TA_CENTER);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);
	dc.SelectClipRgn(&rgn);
	rgn.DeleteObject();
	dc.FillSolidRect(rect, 0XFFFFFF);

	CString strChar;
	strChar.Format(_T("%c"), 0X20+dwId);
	dc.TextOut((rect.left+rect.right)/2, (rect.bottom+rect.top+size)/2, strChar);

	font.DeleteObject();
}

LRESULT CGraduatedDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProperty = (CMFCPropertyGridProperty*)lParam;
	if(pProperty==nullptr)
		return 0;
	const CString strName = pProperty->GetName();
	const COleVariant varValue = pProperty->GetValue();
	if(strName==_T("Symbol"))
	{
		const CString str = varValue.bstrVal;
		m_graduated.m_symbol = str.GetAt(0);
	}
	else if(strName==_T("Border"))
	{
		m_graduated.m_colorBorder = varValue.intVal;
	}
	else if(strName==_T("Border Width"))
	{
		m_graduated.m_widthBorder = varValue.fltVal;
	}
	else if(strName==_T("Fill"))
	{
		m_graduated.m_colorFill = varValue.intVal;
	}
	else if(strName==_T("Minimal"))
	{
		m_graduated.m_continuous.m_minT = varValue.intVal;
	}
	else if(strName==_T("Maximal"))
	{
		m_graduated.m_continuous.m_maxT = varValue.intVal;
	}

	this->Invalidate();
	return 0;
}

void CGraduatedDlg::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	*pResult = -1;

	const NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	if(pItem->iRow==0)
		return;
	if(pItem->iRow>m_graduated.m_divisions.size())
		return;
	if(m_StepsListCtrl.GetRowCount()<m_graduated.m_divisions.size()+1)
		return;

	std::list<CDivision<DWORD>>::iterator prev = m_graduated.m_divisions.begin();
	std::list<CDivision<DWORD>>::iterator curr = m_graduated.m_divisions.begin();
	std::list<CDivision<DWORD>>::iterator next = m_graduated.m_divisions.begin();
	std::advance(prev, max(pItem->iRow-1-1, 0));
	std::advance(curr, pItem->iRow-1);
	std::advance(next, min(pItem->iRow-1+1, m_graduated.m_divisions.size()));
	const CString strValue = m_StepsListCtrl.GetItemText(pItem->iRow, pItem->iColumn);
	switch(pItem->iColumn)
	{
	case 1:
	{
		const double valueFrom = _ttof(strValue);
		const CString strTo = m_StepsListCtrl.GetItemText(pItem->iRow, pItem->iColumn+1);
		const double valueTo = _ttof(strTo);
		if(valueFrom>=valueTo)
			return;
		else if(pItem->iRow>1)
		{
			const CString strPrev = m_StepsListCtrl.GetItemText(pItem->iRow-1, pItem->iColumn);
			const double valuePrev = _ttof(strPrev);
			if(valueFrom<=valuePrev)
				return;
			else
			{
				m_StepsListCtrl.SetItemText(pItem->iRow-1, 2, strValue);
				m_StepsListCtrl.RedrawCell(pItem->iRow-1, 2);
				curr->m_minValue = valueFrom;
				prev->m_maxValue = valueFrom;
				*pResult = 0;
			}
		}
		else if(pItem->iRow==1)
		{
			curr->m_minValue = valueFrom;
			*pResult = 0;
		}
	}
	break;
	case 2:
	{
		const double valueTo = _ttof(strValue);
		const CString strFrom = m_StepsListCtrl.GetItemText(pItem->iRow, pItem->iColumn-1);
		const double valueFrom = _ttof(strFrom);
		if(valueTo<=valueFrom)
			return;
		else if(pItem->iRow<(m_StepsListCtrl.GetRowCount()-1))
		{
			const CString strNext = m_StepsListCtrl.GetItemText(pItem->iRow+1, pItem->iColumn);
			const double valueNext = _ttof(strNext);
			if(valueTo>=valueNext)
				return;
			else
			{
				m_StepsListCtrl.SetItemText(pItem->iRow+1, 1, strValue);
				m_StepsListCtrl.RedrawCell(pItem->iRow+1, 1);
				curr->m_maxValue = valueTo;
				next->m_minValue = valueTo;
				*pResult = 0;
			}
		}
		else if(pItem->iRow==m_StepsListCtrl.GetRowCount()-1)
		{
			curr->m_maxValue = valueTo;
			*pResult = 0;
		}
	}
	break;
	case 3:
	{
		const double value = _ttof(strValue);

		if(pItem->iRow>1)
		{
			const CString strPrev = m_StepsListCtrl.GetItemText(pItem->iRow-1, 3);
			const double valuePrev = _ttof(strPrev);
			if(value<=valuePrev)
				return;
		}
		if(pItem->iRow<m_StepsListCtrl.GetRowCount()-1)
		{
			const CString strNext = m_StepsListCtrl.GetItemText(pItem->iRow+1, pItem->iColumn);
			const double valueNext = _ttof(strNext);
			if(value>=valueNext)
				return;
		}
		m_StepsListCtrl.SetRowHeight(pItem->iRow, max(15, value+4));
		curr->m_Value = value;
		if(pItem->iRow==1)
			m_graduated.m_continuous.m_minT = value;
		else if(pItem->iRow==m_StepsListCtrl.GetRowCount()-1)
		{
			m_graduated.m_continuous.m_maxT = value;
			m_StepsListCtrl.SetColumnWidth(0, value);
		}
		*pResult = 0;
	}
	break;
	}
}

CSymbolCell::CSymbolCell()
{
}

CSymbolCell::~CSymbolCell()
{
}

BOOL CSymbolCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	if(m_pGraduatedDlg==nullptr)
		return FALSE;
	else if(m_pGraduatedDlg->m_graduated.m_divisions.size()<nRow)
		return FALSE;
	else
	{
		std::list<CDivision<DWORD>>::iterator it = m_pGraduatedDlg->m_graduated.m_divisions.begin();
		std::advance(it, nRow-1);
		const int nShape = m_pGraduatedDlg->m_graduated.m_symbol;
		const COLORREF rgb = m_pGraduatedDlg->m_graduated.m_colorFill;
		CFont font;
		font.CreateFont(it->m_Value, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Map Symbols"));
		CFont* oldFont = (CFont*)pDC->SelectObject(font);
		const COLORREF oldrgb = pDC->SetTextColor(rgb);
		const int oldalign = pDC->SetTextAlign(TA_BOTTOM|TA_CENTER);

		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rect);
		pDC->SelectClipRgn(&rgn);

		CString strChar;
		strChar.Format(_T("%c"), 0X20+nShape);
		pDC->TextOut((rect.left+rect.right)/2, (rect.bottom+rect.top+it->m_Value)/2, strChar);

		pDC->SetTextColor(oldrgb);
		pDC->SetTextAlign(oldalign);
		//	pDC->SelectObject(oldFont);
		font.DeleteObject();
		pDC->SelectClipRgn(nullptr);
		rgn.DeleteObject();
		return TRUE;
	}
}

void CGraduatedDlg::OnCbnSelchangeCombo()
{
	m_graduated.m_symbol = m_SymbolCombo.GetCurSel();
	this->Invalidate();
}
