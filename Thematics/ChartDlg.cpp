// DivisionalChart.cpp : implementation file
//

#include "stdafx.h"
#include "ChartDlg.h"
#include "Chart.h"
#include "afxdialogex.h"
#include "Resource.h"

using namespace std;
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CChartDlg dialog

IMPLEMENT_DYNAMIC(CChartDlg, CClassificationDlg)

CChartDlg::CChartDlg(UINT IDD, CWnd* pParent, CChart& chart, CDatabase& database, CATTDatasource& datasource)
	: CClassificationDlg(IDD, pParent, database, datasource), m_chart(chart)
{
}

CChartDlg::~CChartDlg()
{
}

void CChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CClassificationDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_FieldListCtrl);
	DDX_Control(pDX, IDC_LIST3, m_ColorListCtrl);
}

BEGIN_MESSAGE_MAP(CChartDlg, CClassificationDlg)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST3, OnCustomdrawColorList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST3, OnColorchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnNMDblclk)
END_MESSAGE_MAP()

BOOL CChartDlg::OnInitDialog()
{
	CClassificationDlg::OnInitDialog();

	m_FieldListCtrl.InsertColumn(0, _T("Color"), LVCFMT_LEFT, 0, 0);
	m_FieldListCtrl.InsertColumn(1, _T("Field"), LVCFMT_LEFT, 0, 0);
	m_FieldListCtrl.SetColumnWidth(0, 50);
	m_FieldListCtrl.SetColumnWidth(1, 100);
	m_FieldListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	for(std::map<CString, Gdiplus::ARGB>::iterator it = m_chart.m_fieldcolors.begin(); it!=m_chart.m_fieldcolors.end(); ++it)
	{
		const int item = m_FieldListCtrl.InsertItem(m_FieldListCtrl.GetItemCount(), _T(""));
		const COLORREF rgb = Gdiplus::Color(it->second).ToCOLORREF();
		m_FieldListCtrl.SetCellColors(item, 0, rgb, rgb);
		m_FieldListCtrl.SetItemText(item, 1, it->first);
	}

	m_ColorListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	CClassificationDlg::LoadFavoriteColors(_T("Chart"), &m_ColorListCtrl, m_chart.m_fieldcolors.size(), m_favcolors);

	CRect rect;
	GetDlgItem(IDC_CUSTOM)->GetClientRect(rect);
	GetDlgItem(IDC_CUSTOM)->MapWindowPoints(this, &rect);
	m_PropertyGrid.Create(WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP, rect, this, 1231);
	m_PropertyGrid.SetVSDotNetLook(TRUE);
	m_PropertyGrid.MarkModifiedProperties();

	CMFCPropertyGridProperty* pProp1 = new CMFCPropertyGridProperty(_T("Style") _T(""), _T(""));
	pProp1->AddOption(_T("2D"));
	pProp1->AddOption(_T("3D"));
	m_PropertyGrid.AddProperty(pProp1);

	CMFCPropertyGridProperty* pGroupAngle = new CMFCPropertyGridProperty(_T("Angle"));
	{
		CMFCPropertyGridProperty* pProp2 = new CMFCPropertyGridProperty(_T("Incline"), (_variant_t)m_chart.m_angleIncline, _T(""));
		pProp2->EnableSpinControl(TRUE, 0, 60);
		pGroupAngle->AddSubItem(pProp2);
	}
	m_PropertyGrid.AddProperty(pGroupAngle);

	CMFCPropertyGridProperty* pGroupBorder = new CMFCPropertyGridProperty(_T("Border"));
	{
		CMFCPropertyGridProperty* pProp3 = new CMFCPropertyGridProperty(_T("Border Width"), (_variant_t)m_chart.m_widthBorder, _T(""));
		//	pProp3->EnableSpinControl(TRUE, 0, 90);
		pGroupBorder->AddSubItem(pProp3);

		CMFCPropertyGridColorProperty* pProp1 = new CMFCPropertyGridColorProperty(_T("Border Color"), m_chart.m_colorBorder);
		pProp1->EnableOtherButton(_T("Other..."));
		pProp1->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
		pGroupBorder->AddSubItem(pProp1);
	}
	m_PropertyGrid.AddProperty(pGroupBorder);

	CMFCPropertyGridProperty* pGroupAxis = new CMFCPropertyGridProperty(_T("Axis"));
	{
		CMFCPropertyGridProperty* pProp4 = new CMFCPropertyGridProperty(_T("Axis Width"), (_variant_t)m_chart.m_widthBorder, _T(""));
		//	pProp4->EnableSpinControl(TRUE, 0, 90);
		pGroupAxis->AddSubItem(pProp4);

		CMFCPropertyGridColorProperty* pProp2 = new CMFCPropertyGridColorProperty(_T("Axis Color"), m_chart.m_colorAxis);
		pProp2->EnableOtherButton(_T("Other..."));
		pProp2->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
		pGroupAxis->AddSubItem(pProp2);
	}
	m_PropertyGrid.AddProperty(pGroupAxis);

	CMFCPropertyGridProperty* pProp2 = new CMFCPropertyGridProperty(_T("Deepth"), (_variant_t)m_chart.m_deepth, _T(""));
	//	pProp2->EnableSpinControl(TRUE, 5, 1000);
	m_PropertyGrid.AddProperty(pProp2);

	return TRUE;
}

void CChartDlg::OnFieldAdded(const CString& strField)
{
	if(m_chart.AddField(m_database, m_datasource, strField)==true)
	{
		const int item = m_FieldListCtrl.InsertItem(m_FieldListCtrl.GetItemCount(), _T(""));
		const COLORREF rgb = Gdiplus::Color(m_chart.m_fieldcolors[strField]).ToCOLORREF();
		m_FieldListCtrl.SetCellColors(item, 0, rgb, rgb);
		m_FieldListCtrl.SetItemText(item, 1, strField);

		OnDivisional();
		CClassificationDlg::LoadFavoriteColors(_T("Chart"), &m_ColorListCtrl, m_chart.m_fieldcolors.size(), m_favcolors);
		this->Invalidate();
	}
}

void CChartDlg::OnFieldRemoved(const CString& strField)
{
	for(int index = m_FieldListCtrl.GetItemCount()-1; index>=0; index--)
	{
		CString str = m_FieldListCtrl.GetItemText(index, 1);
		if(str.CompareNoCase(strField)==0)
		{
			m_FieldListCtrl.DeleteItem(index);
		}
	}
	m_chart.RemoveField(m_database, m_datasource, strField);

	OnDivisional();
	CClassificationDlg::LoadFavoriteColors(_T("Chart"), &m_ColorListCtrl, m_chart.m_fieldcolors.size(), m_favcolors);
	this->Invalidate();
}

void CChartDlg::OnRemoveAllFields()
{
	m_FieldListCtrl.DeleteAllItems();
	m_chart.RemoveAllFields();
}

void CChartDlg::OnColorchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pnmListView->iItem;
	if(index==-1)
		return;

	if(index<m_favcolors.size())
	{
		const std::vector<Gdiplus::ARGB> colors = m_favcolors[index];
		{
			if(colors.size()==m_FieldListCtrl.GetItemCount())
			{
				for(int index = 0; index<min(colors.size(), m_FieldListCtrl.GetItemCount()); index++)
				{
					const COLORREF rgb = Gdiplus::Color(colors[index]).ToCOLORREF();
					m_FieldListCtrl.SetCellColors(index, 0, rgb, 0X00000000);
					CString strField = m_FieldListCtrl.GetItemText(index, 1);
					m_chart.m_fieldcolors[strField] = colors[index];
				}
			}
			m_FieldListCtrl.Invalidate();
		}
		SendMessage(WM_PAINT, 0, 0);
	}
}

void CChartDlg::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView==nullptr)
		*pResult = FALSE;
	else if(pNMListView->iItem<0)
		*pResult = FALSE;
	else if(pNMListView->iSubItem!=0)
		*pResult = FALSE;
	else if(pNMListView->iItem<m_chart.m_fieldcolors.size())
	{
		const CString strField = m_FieldListCtrl.GetItemText(pNMListView->iItem, 1);
		CEditableListCtrl::CellInfo* pCellInfo = m_FieldListCtrl.GetCellInfo(pNMListView->iItem, 0);
		if(pCellInfo!=nullptr)
		{
			const COLORREF rgb = pCellInfo->m_clrBack;
			CColorDialog dlg(rgb);
			if(dlg.DoModal()==IDOK)
			{
				const COLORREF rgb = dlg.GetColor();
				m_FieldListCtrl.SetCellColors(pNMListView->iItem, 0, rgb, 0);
				m_chart.m_fieldcolors[strField] = Gdiplus::Color::MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
				this->Invalidate();
			}
			*pResult = TRUE;
		}
		else
			*pResult = FALSE;
	}
	else
		*pResult = FALSE;
}

LRESULT CChartDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam) const
{
	CMFCPropertyGridProperty* pProperty = (CMFCPropertyGridProperty*)lParam;
	if(pProperty==nullptr)
		return 0;
	const CString strName = pProperty->GetName();
	const COleVariant varValue = pProperty->GetValue();
	if(strName==_T("Deepth"))
	{
		m_chart.m_deepth = varValue.fltVal;
	}
	else if(strName==_T("Incline"))
	{
		m_chart.m_angleIncline = varValue.intVal;
	}
	else if(strName==_T("Border Color"))
	{
		m_chart.m_colorBorder = varValue.uintVal;
	}
	else if(strName==_T("Border Width"))
	{
		m_chart.m_widthBorder = varValue.fltVal;
	}
	else if(strName==_T("Axis Width"))
	{
		m_chart.m_widthAxis = varValue.fltVal;
	}
	else if(strName==_T("Axis Color"))
	{
		m_chart.m_colorAxis = varValue.uintVal;
	}
	SendMessage(WM_PAINT, 0, 0);
	return 0;
}
