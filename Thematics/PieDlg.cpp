// DivisionalChart.cpp : implementation file
//

#include "stdafx.h"
#include "PieDlg.h"

using namespace std;
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPieDlg dialog

IMPLEMENT_DYNAMIC(CPieDlg, CChartDlg)
IMPLEMENT_DYNCREATE(CSymbolCell2, CGridCell)

CPieDlg::CPieDlg(CWnd* pParent, CPie& pie, CDatabase& database, CATTDatasource& datasource)
	: CChartDlg(CPieDlg::IDD, pParent, pie, database, datasource), m_pie(pie)
{
}

CPieDlg::~CPieDlg()
{
	m_StepsListCtrl.DeleteAllItems();
}

void CPieDlg::DoDataExchange(CDataExchange* pDX)
{
	CChartDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_StepsListCtrl);
}

BEGIN_MESSAGE_MAP(CPieDlg, CChartDlg)
	ON_WM_PAINT()
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_LIST1, OnGridEndEdit)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

BOOL CPieDlg::OnInitDialog()
{
	CChartDlg::OnInitDialog();

	try
	{
		m_StepsListCtrl.SetRowCount(1);
		m_StepsListCtrl.SetFixedRowCount(1);
		m_StepsListCtrl.SetColumnCount(5);
		m_StepsListCtrl.SetColumnWidth(0, 50);
		m_StepsListCtrl.SetColumnWidth(1, 50);
		m_StepsListCtrl.SetColumnWidth(2, 50);
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

	CMFCPropertyGridProperty* pGroupStyle = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Style"));
	if(pGroupStyle!=nullptr)
	{
		pGroupStyle->SetValue(m_pie.m_style==CPie::Style::TwoD ? _T("2D") : _T("3D"));
	}

	CMFCPropertyGridProperty* pGroupAngle = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Angle"));
	if(pGroupAngle!=nullptr)
	{
		CMFCPropertyGridProperty* pProp1 = new CMFCPropertyGridProperty(_T("Starting"), (_variant_t)m_pie.m_angleStarting, _T(""));
		pProp1->EnableSpinControl(TRUE, 0, 90);
		pGroupAngle->AddSubItem(pProp1);
	}

	m_PropertyGrid.ExpandAll();
	m_PropertyGrid.SetFocus();

	if(m_pie.m_divisions.size()>0)
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(2);
		const int count = m_pie.m_divisions.size();
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

	OnPropertyChanged(0, (DWORD)pGroupStyle);
	return TRUE;
}

void CPieDlg::OnContinuous()
{
	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck()!=BST_CHECKED)
		return;

	m_pie.m_divisions.clear();
	while(m_StepsListCtrl.GetRowCount()>1)
		m_StepsListCtrl.DeleteRow(m_StepsListCtrl.GetRowCount()-1);

	CMFCPropertyGridProperty* pSizeGroup = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Size"));
	if(pSizeGroup==nullptr)
		pSizeGroup = new CMFCPropertyGridProperty(_T("Size"));
	if(pSizeGroup!=nullptr)
	{
		CMFCPropertyGridProperty* pProp1 = new CMFCPropertyGridProperty(_T("Minimum"), (_variant_t)m_pie.m_continuous.m_minT, _T(""));
		pProp1->EnableSpinControl(TRUE, 1, 1000);
		pSizeGroup->AddSubItem(pProp1);
		CMFCPropertyGridProperty* pProp2 = new CMFCPropertyGridProperty(_T("Maximum"), (_variant_t)m_pie.m_continuous.m_maxT, _T(""));
		pProp2->EnableSpinControl(TRUE, 1, 1000);
		pSizeGroup->AddSubItem(pProp2);
	}
	m_PropertyGrid.AddProperty(pSizeGroup);

	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC4)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
}

void CPieDlg::OnDivisional()
{
	if(((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck()!=BST_CHECKED)
		return;

	CMFCPropertyGridProperty* pProperty = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Size"));
	if(pProperty!=nullptr)
	{
		m_PropertyGrid.DeleteProperty(pProperty);
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

	m_pie.RedivideSize(steps);

	int index = 1;
	for(std::list<CDivision<DWORD>>::iterator it = m_pie.m_divisions.begin(); it!=m_pie.m_divisions.end(); ++it)
	{
		CString str1;
		str1.Format(_T("%g"), it->m_minValue);
		CString str2;
		str2.Format(_T("%g"), it->m_maxValue);
		CString str3;
		str3.Format(_T("%d"), it->m_Value);

		m_StepsListCtrl.InsertRow(_T(""), -1);
		//	m_StepsListCtrl.SetCellColors(index,0,m_pie.m_colorFill,0XFFFFFF);
		m_StepsListCtrl.SetItemText(index, 1, str1);
		m_StepsListCtrl.SetItemText(index, 2, str2);
		m_StepsListCtrl.SetItemText(index, 3, str3);

		m_StepsListCtrl.SetCellType(index, 0, RUNTIME_CLASS(CSymbolCell2));
		CSymbolCell2* pSymbolCell = (CSymbolCell2*)m_StepsListCtrl.GetCell(index, 0);
		if(pSymbolCell!=nullptr)
		{
			pSymbolCell->m_pChartDlg = this;
			pSymbolCell->SetState(pSymbolCell->GetState()|GVIS_READONLY);
		}
		m_StepsListCtrl.SetRowHeight(index, max(15, it->m_Value+(m_pie.m_style==CPie::Style::ThreeD ? m_pie.m_deepth : 0)+4));
		index++;
	}
	this->Invalidate();
}

void CPieDlg::OnPaint()
{
	CPaintDC dc(this);
	const int size1 = m_pie.m_continuous.m_minT;
	const int size2 = m_pie.m_continuous.m_maxT;
	CRect crect1;
	CRect crect2;
	GetDlgItem(IDC_STATIC1)->GetClientRect(crect1);
	GetDlgItem(IDC_STATIC2)->GetClientRect(crect2);
	Gdiplus::Rect rect1(crect1.CenterPoint().x, crect1.CenterPoint().y, 0, 0);
	Gdiplus::Rect rect2(crect2.CenterPoint().x, crect2.CenterPoint().y, 0, 0);
	rect1.Inflate(size1/2, size1/2);
	rect2.Inflate(size2/2, size2/2);
	Gdiplus::Graphics g1(GetDlgItem(IDC_STATIC1)->m_hWnd);
	Gdiplus::Graphics g2(GetDlgItem(IDC_STATIC2)->m_hWnd);
	g1.SetPageUnit(Gdiplus::UnitPixel);
	g1.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g1.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g1.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	g2.SetPageUnit(Gdiplus::UnitPixel);
	g2.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g2.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g2.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	std::map<CString, double> elements1;
	for(std::map<CString, Gdiplus::ARGB>::iterator it = m_pie.m_fieldcolors.begin(); it!=m_pie.m_fieldcolors.end(); ++it)
	{
		elements1[it->first] = m_pie.m_minValue/m_pie.m_fieldcolors.size();
	}
	m_pie.Draw(g1, 1, Gdiplus::Point(rect1.X+rect1.Width/2, rect1.Y+rect1.Height/2), elements1);
	std::map<CString, double> elements2;
	for(std::map<CString, Gdiplus::ARGB>::iterator it = m_pie.m_fieldcolors.begin(); it!=m_pie.m_fieldcolors.end(); ++it)
	{
		elements2[it->first] = m_pie.m_maxValue/m_pie.m_fieldcolors.size();
	}
	m_pie.Draw(g2, 1, Gdiplus::Point(rect2.X+rect2.Width/2, rect2.Y+rect2.Height/2), elements2);
}

//
//void CPieDlg::OnColorchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	NM_LISTVIEW *pnmListView = (NM_LISTVIEW *)pNMHDR;
//	int index = pnmListView->iItem ;
//	if(index == -1)
//		return;
//
//	if(index<m_favcolors.size())
//	{
//		std::vector<DWORD> color = m_favcolors[index];	
//		for(int index=0;index<min(color.size(),m_FieldListCtrl.GetItemCount());index++)
//		{
//			m_FieldListCtrl.SetCellColors(index,0,color[index],0);
//		}
//		this->Invalidate();
//		OnPaint();
//	}
//	*pResult = TRUE;
//}

void CPieDlg::OnRemoveAllFields()
{
	CChartDlg::OnRemoveAllFields();

	while(m_StepsListCtrl.GetRowCount()>1)
		m_StepsListCtrl.DeleteRow(m_StepsListCtrl.GetRowCount()-1);

	OnDivisional();
}

LRESULT CPieDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProperty = (CMFCPropertyGridProperty*)lParam;
	if(pProperty==nullptr)
		return 0;
	const CString strName = pProperty->GetName();
	const COleVariant varValue = pProperty->GetValue();
	if(strName==_T("Style"))
	{
		CMFCPropertyGridProperty* pDeepthProp = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Deepth"));
		CMFCPropertyGridProperty* pAngleGroup = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Angle"));
		CMFCPropertyGridProperty* pInclineProp = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Incline"));

		if(varValue.bstrVal==_T("3D"))
		{
			m_pie.m_style = CPie::Style::ThreeD;

			if(pDeepthProp!=nullptr)
			{
			}
			if(pInclineProp==nullptr)
			{
			}
		}
		else if(varValue.bstrVal==_T("2D"))
		{
			m_pie.m_style = CPie::Style::TwoD;

			if(pDeepthProp!=nullptr)
			{
			}
			if(pInclineProp!=nullptr)
			{
			}
		}
	}
	else if(strName==_T("Minimum"))
	{
		m_pie.m_continuous.m_minT = varValue.intVal;
	}
	else if(strName==_T("Maximum"))
	{
		m_pie.m_continuous.m_maxT = varValue.intVal;
	}
	else if(strName==_T("Starting"))
	{
		m_pie.m_angleStarting = varValue.intVal;
	}

	int index = 1;
	for(std::list<CDivision<DWORD>>::iterator it = m_pie.m_divisions.begin(); it!=m_pie.m_divisions.end(); ++it)
	{
		m_StepsListCtrl.SetRowHeight(index++, max(15, it->m_Value+(m_pie.m_style==CPie::Style::ThreeD ? m_pie.m_deepth : 0)+4));
	}

	return CChartDlg::OnPropertyChanged(wParam, lParam);
}

void CPieDlg::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	*pResult = -1;

	const NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	if(pItem->iRow==0)
		return;
	if(pItem->iRow>m_pie.m_divisions.size())
		return;
	if(m_StepsListCtrl.GetRowCount()<m_pie.m_divisions.size()+1)
		return;

	std::list<CDivision<DWORD>>::iterator prev = m_pie.m_divisions.begin();
	std::list<CDivision<DWORD>>::iterator curr = m_pie.m_divisions.begin();
	std::list<CDivision<DWORD>>::iterator next = m_pie.m_divisions.begin();
	std::advance(prev, max(pItem->iRow-1-1, 0));
	std::advance(curr, pItem->iRow-1);
	std::advance(next, min(pItem->iRow-1+1, m_pie.m_divisions.size()));
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
		m_StepsListCtrl.SetRowHeight(pItem->iRow, max(15, value+(m_pie.m_style==CPie::Style::ThreeD ? m_pie.m_deepth : 0)+4));
		curr->m_Value = value;
		if(pItem->iRow==1)
			m_pie.m_continuous.m_minT = value;
		else if(pItem->iRow==m_StepsListCtrl.GetRowCount()-1)
		{
			m_pie.m_continuous.m_maxT = value;
			m_StepsListCtrl.SetColumnWidth(0, value);
		}
		*pResult = 0;
	}
	}
}

CSymbolCell2::CSymbolCell2()
{
}

CSymbolCell2::~CSymbolCell2()
{
}

BOOL CSymbolCell2::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	if(m_pChartDlg==nullptr)
		return FALSE;
	else if(m_pChartDlg->m_pie.m_divisions.size()<nRow)
		return FALSE;
	else
	{
		std::list<CDivision<DWORD>>::iterator it = m_pChartDlg->m_pie.m_divisions.begin();
		std::advance(it, nRow-1);
		const int oldDC = pDC->SaveDC();

		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rect);
		int oldRgn = pDC->SelectClipRgn(&rgn);

		Gdiplus::Rect rect1(rect.CenterPoint().x, rect.CenterPoint().y, 0, 0);
		const int size = it->m_Value;
		rect1.Inflate(size/2, size/2);

		Gdiplus::Graphics g(pDC->m_hDC);
		g.SetPageUnit(Gdiplus::UnitPixel);
		g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
		const double total = (it->m_minValue+it->m_maxValue)/2;
		std::map<CString, double> elements;
		for(std::map<CString, Gdiplus::ARGB>::iterator it = m_pChartDlg->m_pie.m_fieldcolors.begin(); it!=m_pChartDlg->m_pie.m_fieldcolors.end(); ++it)
		{
			elements[it->first] = total/m_pChartDlg->m_pie.m_fieldcolors.size();
		}
		m_pChartDlg->m_pie.Draw(g, 1, Gdiplus::Point(rect.CenterPoint().x, rect.CenterPoint().y), elements);

		pDC->SelectClipRgn(nullptr);
		rgn.DeleteObject();
		g.ReleaseHDC(pDC->m_hDC);
		pDC->RestoreDC(oldDC);
		return TRUE;
	}
}
