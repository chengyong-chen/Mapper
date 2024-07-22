#include "stdafx.h"
#include "FieldsPickerDlg.h"
#include <afxdialogex.h>
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CFieldsPickerDlg, CDialogEx)

CFieldsPickerDlg::CFieldsPickerDlg(CWnd* pParent, std::list<std::pair<CString, CString>> fileds, CString fieldAid, CString fieldName, CString fieldCode, CString fieldCentroidX, CString fieldCentroidY, std::list<std::list<CString>> samples, CString caption)
	: CDialogEx(IDD_FIELDPICKER, pParent), m_fileds(fileds), m_strFieldAid(fieldAid), m_strFieldAnno(fieldName), m_strFieldCode(fieldCode), m_strFieldCentroidX(fieldCentroidX), m_strFieldCentroidY(fieldCentroidY), m_samples(samples), m_caption(caption)
{
}

CFieldsPickerDlg::~CFieldsPickerDlg()
{
	m_DBGrid.DeleteAllItems();
}

void CFieldsPickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DBGRID, m_DBGrid);
}


BEGIN_MESSAGE_MAP(CFieldsPickerDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CFieldsPickerDlg::OnInitDialog()
{
	const BOOL ret=CDialogEx::OnInitDialog();

	m_DBGrid.SetRowCount(1);
	m_DBGrid.SetFixedRowCount(1);
	m_DBGrid.SetColumnCount(m_fileds.size());
	int index=0;
	for(std::list<std::pair<CString, CString>>::iterator it=m_fileds.begin(); it != m_fileds.end(); ++it)
	{
		std::pair<CString, CString> pair=(std::pair<CString, CString>)*it;
		if(pair.second == _T("integer")){
			((CComboBox*)GetDlgItem(IDC_FIELDAID))->AddString(pair.first);
			((CComboBox*)GetDlgItem(IDC_GEOCODEFIELD))->AddString(pair.first);
			((CComboBox*)GetDlgItem(IDC_CENTROIDXFIELD))->AddString(pair.first);
			((CComboBox*)GetDlgItem(IDC_CENTROIDYFIELD))->AddString(pair.first);
		}
		else if(pair.second == _T("string")){
			((CComboBox*)GetDlgItem(IDC_NAMEFIELD))->AddString(pair.first);
			((CComboBox*)GetDlgItem(IDC_GEOCODEFIELD))->AddString(pair.first);
		}
		else if(pair.second == _T("double")) {
			((CComboBox*)GetDlgItem(IDC_CENTROIDXFIELD))->AddString(pair.first);
			((CComboBox*)GetDlgItem(IDC_CENTROIDYFIELD))->AddString(pair.first);
		}
		m_DBGrid.SetColumnWidth(index, 100);
		GV_ITEM Item;
		Item.mask=GVIF_TEXT | GVIF_FORMAT;
		Item.nFormat=DT_LEFT | DT_WORDBREAK;
		Item.row=0;
		Item.col=index++;
		Item.strText=pair.first;
		m_DBGrid.SetItem(&Item);
	}
	if(m_strFieldAid != _T(""))
	{
		const int nIndex=((CComboBox*)GetDlgItem(IDC_FIELDAID))->FindStringExact(0, m_strFieldAid);
		((CComboBox*)GetDlgItem(IDC_FIELDAID))->SetCurSel(nIndex);
	}
	if(m_strFieldAnno != _T(""))
	{
		const int nIndex=((CComboBox*)GetDlgItem(IDC_NAMEFIELD))->FindStringExact(0, m_strFieldAnno);
		((CComboBox*)GetDlgItem(IDC_NAMEFIELD))->SetCurSel(nIndex);
	}
	if(m_strFieldCode != _T(""))
	{
		const int nIndex=((CComboBox*)GetDlgItem(IDC_GEOCODEFIELD))->FindStringExact(0, m_strFieldCode);
		((CComboBox*)GetDlgItem(IDC_GEOCODEFIELD))->SetCurSel(nIndex);
	}
	if(m_strFieldCentroidX != _T(""))
	{
		const int nIndex=((CComboBox*)GetDlgItem(IDC_CENTROIDXFIELD))->FindStringExact(0, m_strFieldCentroidX);
		((CComboBox*)GetDlgItem(IDC_CENTROIDXFIELD))->SetCurSel(nIndex);
	}
	if(m_strFieldCentroidY != _T(""))
	{
		const int nIndex=((CComboBox*)GetDlgItem(IDC_CENTROIDYFIELD))->FindStringExact(0, m_strFieldCentroidY);
		((CComboBox*)GetDlgItem(IDC_CENTROIDYFIELD))->SetCurSel(nIndex);
	}
	int index1=1;
	for(std::list<std::list<CString>>::iterator it1=m_samples.begin(); it1 != m_samples.end(); ++it1)
	{
		m_DBGrid.InsertRow(_T(""), -1);
		int index2=0;
		for(std::list<CString>::iterator it2=it1->begin(); it2 != it1->end(); ++it2)
		{
			m_DBGrid.SetItemText(index1, index2++, *it2);
		}
		index1++;
	}
	SetWindowText(_T("Pick key field names for ") + m_caption);

	return ret;
}
void CFieldsPickerDlg::OnOK()
{
	CDialogEx::OnOK();
	const int index1=((CComboBox*)GetDlgItem(IDC_FIELDAID))->GetCurSel();
	if(index1 != -1)
		((CComboBox*)GetDlgItem(IDC_FIELDAID))->GetLBText(index1, m_strFieldAid);
	else
		m_strFieldAid="";
	const int index2=((CComboBox*)GetDlgItem(IDC_NAMEFIELD))->GetCurSel();
	if(index2 != -1)
		((CComboBox*)GetDlgItem(IDC_NAMEFIELD))->GetLBText(index2, m_strFieldAnno);
	else
		m_strFieldAnno="";
	const int index3=((CComboBox*)GetDlgItem(IDC_GEOCODEFIELD))->GetCurSel();
	if(index3 != -1)
		((CComboBox*)GetDlgItem(IDC_GEOCODEFIELD))->GetLBText(index3, m_strFieldCode);
	else
		m_strFieldCode="";

	const int index4=((CComboBox*)GetDlgItem(IDC_CENTROIDXFIELD))->GetCurSel();
	if(index4 != -1)
		((CComboBox*)GetDlgItem(IDC_CENTROIDXFIELD))->GetLBText(index4, m_strFieldCentroidX);
	else
		m_strFieldCentroidX="";

	const int index5=((CComboBox*)GetDlgItem(IDC_CENTROIDYFIELD))->GetCurSel();
	if(index5 != -1)
		((CComboBox*)GetDlgItem(IDC_CENTROIDYFIELD))->GetLBText(index5, m_strFieldCentroidY);
	else
		m_strFieldCentroidY="";
}