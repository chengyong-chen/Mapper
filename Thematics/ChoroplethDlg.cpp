// DivisionalChoroplethDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChoroplethDlg.h"
#include "Choropleth.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CChoroplethDlg dialog

IMPLEMENT_DYNAMIC(CChoroplethDlg, CClassificationDlg)
//IMPLEMENT_DYNAMIC(CContinuous<ARGB>,CClassification)

CChoroplethDlg::CChoroplethDlg(CWnd* pParent, CChoropleth& choropleth, CDatabase& database, CATTDatasource& datasource)
	: CClassificationDlg(CChoroplethDlg::IDD, pParent, database, datasource), m_choropleth(choropleth)
{
}

CChoroplethDlg::~CChoroplethDlg()
{
}

void CChoroplethDlg::DoDataExchange(CDataExchange* pDX)
{
	CClassificationDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_StepsListCtrl);
	DDX_Control(pDX, IDC_LIST2, m_ColorListCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_celledit);
}

BEGIN_MESSAGE_MAP(CChoroplethDlg, CClassificationDlg)
	ON_WM_PAINT()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST2, OnCustomdrawColorList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, OnColorchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclk)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON1, OnBnClickedMfccolorbutton)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON2, OnBnClickedMfccolorbutton)
END_MESSAGE_MAP()

BOOL CChoroplethDlg::OnInitDialog()
{
	CClassificationDlg::OnInitDialog();

	m_StepsListCtrl.InsertColumn(0, _T("Color"), LVCFMT_LEFT, 0, 0);
	m_StepsListCtrl.InsertColumn(1, _T("From"), LVCFMT_LEFT, 0, 0);
	m_StepsListCtrl.InsertColumn(2, _T("To"), LVCFMT_LEFT, 0, 0);
	m_StepsListCtrl.SetColumnWidth(0, 100);
	m_StepsListCtrl.SetColumnWidth(1, 50);
	m_StepsListCtrl.SetColumnWidth(2, 50);
	m_StepsListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_StepsListCtrl.SetColumnEditor(1, &m_celledit);
	m_StepsListCtrl.SetColumnEditor(2, &m_celledit);
	m_StepsListCtrl.SetColumnEditor(1, nullptr, &OnEndCellEdit, &m_celledit);
	m_StepsListCtrl.SetColumnEditor(2, nullptr, &OnEndCellEdit, &m_celledit);

	m_ColorListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	CClassificationDlg::LoadFavoriteColors(_T("Choropleth"), &m_ColorListCtrl, 2, m_favcolors);

	((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1))->SetColor(rand());
	((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2))->SetColor(rand());

	if(m_choropleth.m_divisions.size()>1)
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(2);
		CString str;
		str.Format(_T("%d"), m_choropleth.m_divisions.size());
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

void CChoroplethDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CWnd* pWnd = GetDlgItem(IDC_STATIC1);
	if(pWnd!=nullptr)
	{
		CRect rect1;
		pWnd->GetClientRect(rect1);
		const CClientDC dc(pWnd);
		Gdiplus::Graphics g(dc.m_hDC);
		g.SetPageUnit(Gdiplus::UnitPixel);
		const Gdiplus::Rect rect2 = Gdiplus::Rect(rect1.left, rect1.top, rect1.right-rect1.left, rect1.bottom-rect1.top);
		const COLORREF rgb1 = ((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1))->GetColor();
		const COLORREF rgb2 = ((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2))->GetColor();
		const Gdiplus::Color color1(255, GetRValue(rgb1), GetGValue(rgb1), GetBValue(rgb1));
		const Gdiplus::Color color2(255, GetRValue(rgb2), GetGValue(rgb2), GetBValue(rgb2));
		const Gdiplus::LinearGradientBrush brush(rect2, color1, color2, 90, true);
		g.FillRectangle(&brush, rect2);
		g.ReleaseHDC(dc.m_hDC);
	}
}

void CChoroplethDlg::OnFieldAdded(const CString& strField)
{
	if(m_choropleth.AddField(m_database, m_datasource, strField)==true)
	{
		OnDivisional();
		this->Invalidate();
	}
}

void CChoroplethDlg::OnFieldRemoved(const CString& strField)
{
	m_choropleth.RemoveField(m_database, m_datasource, strField);

	OnDivisional();
	this->Invalidate();
}

void CChoroplethDlg::OnRemoveAllFields()
{
	m_choropleth.RemoveAllFields();
	OnDivisional();
}

void CChoroplethDlg::OnContinuous()
{
	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck()!=BST_CHECKED)
		return;

	GetDlgItem(IDC_MFCCOLORBUTTON1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_MFCCOLORBUTTON2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);

	((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1))->SetColor(Gdiplus::Color(m_choropleth.m_continuous.m_minT).ToCOLORREF());
	((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2))->SetColor(Gdiplus::Color(m_choropleth.m_continuous.m_maxT).ToCOLORREF());

	m_choropleth.m_divisions.clear();
	LoadFavoriteColors(_T("Choropleth"), &m_ColorListCtrl, 2, m_favcolors);
}

void CChoroplethDlg::OnDivisional()
{
	if(((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck()!=BST_CHECKED)
		return;

	GetDlgItem(IDC_MFCCOLORBUTTON1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MFCCOLORBUTTON2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);

	CString str;
	GetDlgItem(IDC_CLASSES)->GetWindowText(str);
	const int steps = _ttoi(str);

	m_StepsListCtrl.DeleteAllItems();
	m_choropleth.RedivideARGB(steps);
	LoadFavoriteColors(_T("Choropleth"), &m_ColorListCtrl, steps, m_favcolors);

	int index = 0;
	for(std::list<CDivision<Gdiplus::ARGB>>::iterator it = m_choropleth.m_divisions.begin(); it!=m_choropleth.m_divisions.end(); ++it)
	{
		CString str1;
		str1.Format(_T("%g"), it->m_minValue);
		CString str2;
		str2.Format(_T("%g"), it->m_maxValue);;
		const COLORREF rgb = Gdiplus::Color(it->m_Value).ToCOLORREF();
		m_StepsListCtrl.InsertItem(index, _T(""));
		m_StepsListCtrl.SetCellColors(index, 0, rgb, 0XFFFFFF);
		m_StepsListCtrl.SetItemText(index, 1, str1);
		m_StepsListCtrl.SetItemText(index, 2, str2);
		index++;
	}
}

void CChoroplethDlg::OnColorchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pnmListView->iItem;
	if(index==-1)
		return;

	if(index<m_favcolors.size())
	{
		const std::vector<Gdiplus::ARGB> colors = m_favcolors[index];
		{
			if(colors.size()==m_StepsListCtrl.GetItemCount())
			{
				for(int index = 0; index<min(colors.size(), m_StepsListCtrl.GetItemCount()); index++)
				{
					const COLORREF rgb = Gdiplus::Color(colors[index]).ToCOLORREF();
					m_StepsListCtrl.SetCellColors(index, 0, rgb, 0X00000000);

					std::list<CDivision<Gdiplus::ARGB>>::iterator it = m_choropleth.m_divisions.begin();
					std::advance(it, index);
					it->m_Value = colors[index];
				}
			}
			m_StepsListCtrl.Invalidate();
		}
		{
			m_choropleth.m_continuous.m_minT = colors.front();
			m_choropleth.m_continuous.m_maxT = colors.back();
			((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1))->SetColor(Gdiplus::Color(m_choropleth.m_continuous.m_minT).ToCOLORREF());
			((CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2))->SetColor(Gdiplus::Color(m_choropleth.m_continuous.m_maxT).ToCOLORREF());
			OnPaint();
		}
	}
}

BOOL CChoroplethDlg::OnEndCellEdit(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate)
{
	ASSERT(pWnd);
	CEditableListCtrl* pListCtrl = reinterpret_cast<CEditableListCtrl*>(pThis);
	const CChoroplethDlg* pChoroplethDlg = (CChoroplethDlg*)pListCtrl->GetOwner();
	if(nRow>pChoroplethDlg->m_choropleth.m_divisions.size())
		return FALSE;
	if(pListCtrl->GetItemCount()<pChoroplethDlg->m_choropleth.m_divisions.size())
		return FALSE;

	std::list<CDivision<Gdiplus::ARGB>>::iterator prev = pChoroplethDlg->m_choropleth.m_divisions.begin();
	std::list<CDivision<Gdiplus::ARGB>>::iterator curr = pChoroplethDlg->m_choropleth.m_divisions.begin();
	std::list<CDivision<Gdiplus::ARGB>>::iterator next = pChoroplethDlg->m_choropleth.m_divisions.begin();
	std::advance(prev, nRow-1);
	std::advance(curr, nRow);
	std::advance(next, nRow+1);

	switch(nColumn)
	{
	case 1:
	{
		CEdit* pEdit = dynamic_cast<CEdit*>(*pWnd);
		if(pEdit!=nullptr)
		{
			CString strFrom;
			pEdit->GetWindowText(strFrom);
			const double valueFrom = _ttof(strFrom);
			const CString strTo = pListCtrl->GetItemText(nRow, nColumn+1);
			const double valueTo = _ttof(strTo);
			if(valueFrom>=valueTo)
				return FALSE;
			else if(nRow>0)
			{
				const CString strPrev = pListCtrl->GetItemText(nRow-1, nColumn);
				const double valuePrev = _ttof(strPrev);
				if(valueFrom<=valuePrev)
					return FALSE;
				else
				{
					strSubItemText = strFrom;
					pListCtrl->SetItemText(nRow-1, 2, strFrom);
					curr->m_minValue = valueFrom;
					prev->m_maxValue = valueFrom;
				}
			}
			else if(nRow==0)
			{
				curr->m_minValue = valueFrom;
				strSubItemText = strFrom;
			}
		}
	}
	break;
	case 2:
	{
		CEdit* pEdit = dynamic_cast<CEdit*>(*pWnd);
		if(pEdit!=nullptr)
		{
			CString strTo;
			pEdit->GetWindowText(strTo);
			const double valueTo = _ttof(strTo);
			const CString strFrom = pListCtrl->GetItemText(nRow, nColumn-1);
			const double valueFrom = _ttof(strFrom);
			if(valueTo<=valueFrom)
				return FALSE;
			else if(nRow<pListCtrl->GetItemCount()-1)
			{
				const CString strNext = pListCtrl->GetItemText(nRow+1, nColumn);
				const double valueNext = _ttof(strNext);
				if(valueTo>=valueNext)
					return FALSE;
				else
				{
					strSubItemText = strTo;
					pListCtrl->SetItemText(nRow+1, 1, strTo);
					curr->m_maxValue = valueTo;
					next->m_minValue = valueTo;
				}
			}
			else if(nRow==pListCtrl->GetItemCount()-1)
			{
				curr->m_maxValue = valueTo;
				strSubItemText = strTo;
			}
		}
	}
	break;
	}
	return TRUE;
}

void CChoroplethDlg::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView==nullptr)
		*pResult = FALSE;
	else
	{
		const int nItem = pNMListView->iItem;
		const int nSubItem = pNMListView->iSubItem;
		if(nSubItem==0)
		{
			CEditableListCtrl::CellInfo* pCellInfo = m_StepsListCtrl.GetCellInfo(nItem, 0);
			if(pCellInfo!=nullptr)
			{
				COLORREF rgb = pCellInfo->m_clrBack;
				CColorDialog dlg(rgb);
				if(dlg.DoModal()==IDOK)
				{
					rgb = dlg.GetColor();
					m_StepsListCtrl.SetCellColors(nItem, 0, rgb, 0);
					m_StepsListCtrl.Invalidate();

					std::list<CDivision<Gdiplus::ARGB>>::iterator it = m_choropleth.m_divisions.begin();
					std::advance(it, nItem);

					it->m_Value = Gdiplus::Color::MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
				}
				*pResult = TRUE;
			}
			else
				*pResult = FALSE;
		}
		else
			*pResult = FALSE;
	}
}

void CChoroplethDlg::OnBnClickedMfccolorbutton()
{
	OnPaint();
}
