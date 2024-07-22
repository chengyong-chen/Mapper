// ClassificationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClassificationDlg.h"
#include "../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"
#include <string.h>

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CClassificationDlg

CClassificationDlg::CClassificationDlg(UINT IDD, CWnd* pParent, CDatabase& database, CATTDatasource& datasource)
	: CDialogEx(IDD, pParent), m_database(database), m_datasource(datasource)
{
}

CClassificationDlg::~CClassificationDlg()
{
}

void CClassificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClassificationDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, OnContinuous)
	ON_BN_CLICKED(IDC_RADIO2, OnDivisional)
END_MESSAGE_MAP()

BOOL CClassificationDlg::OnInitDialog()
{
	return CDialogEx::OnInitDialog();
}

// CClassificationDlg member functions
void CClassificationDlg::OnContinuous()
{
}

void CClassificationDlg::OnDivisional()
{
}

void CClassificationDlg::AddFields(std::list<CString> fields)
{
	for(std::list<CString>::iterator it = fields.begin(); it!=fields.end(); ++it)
	{
		OnFieldAdded(*it);
	}
}

void CClassificationDlg::LoadFavoriteColors(CString strType, CListCtrl* pListCtrl, int subs, std::vector<std::vector<Gdiplus::ARGB>>& favcolors)
{
	pListCtrl->DeleteAllItems();
	while(pListCtrl->DeleteColumn(0));
	favcolors.clear();

	if(subs==0)
		return;

	CRect rect;
	pListCtrl->GetClientRect(rect);
	int width = (rect.Width()-17)/subs;
	for(int index = 0; index<subs; index++)
	{
		pListCtrl->InsertColumn(index, _T(""), LVCFMT_LEFT, width, -1);
	}

	char strFile[MAX_PATH];
	memset(strFile, 0, MAX_PATH);
	::GetModuleFileNameA(nullptr, strFile, MAX_PATH);
	char* last = strrchr(strFile, '\\');
	memcpy(last, "\\Colors.xml", sizeof("\\Colors.xml"));
	tinyxml2::XMLDocument xml;
	if(xml.LoadFile(strFile)==tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pColors = xml.FirstChildElement("colors");
		while(pColors!=nullptr)
		{
			int group = atoi(pColors->Attribute("group"));
			if(group==subs)
			{
				int index = 0;
				tinyxml2::XMLElement* pColor = pColors->FirstChildElement("color");
				while(pColor!=nullptr)
				{
					char seps[] = " ";

					std::vector<Gdiplus::ARGB> colors;
					const char* value1 = pColor->FirstChild()->Value();
					char* value2 = _strdup(value1);
					char* token = strtok(value2, seps);
					while(token!=nullptr&&colors.size()<group)
					{
						Gdiplus::ARGB rgb;
						sscanf(token, "%x", &rgb);
						colors.push_back(rgb|0XFF000000);
						free(token);
						token = strtok(nullptr, seps);
					}
					free(value2);

					if(colors.size()==group)
					{
						favcolors.push_back(colors);
						pListCtrl->InsertItem(index++, _T(""));
					}

					pColor = pColor->NextSiblingElement();
				}
				break;
			}

			pColors = pColors->NextSiblingElement();
		}
	}
	else
	{
		for(int index = 0; index<100; index++)
		{
			std::vector<Gdiplus::ARGB> colors;
			for(int step = 0; step<subs; step++)
			{
				colors.push_back(rand()|0XFF000000);
			}
			favcolors.push_back(colors);
			pListCtrl->InsertItem(index, _T(""));
		}
	}
}

void CClassificationDlg::OnCustomdrawColorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	const NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	switch(pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT: // First thing - check the draw stage. If it's the control's prepaint stage, then tell Windows we want messages for every item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT: // This is the notification message for an item.  We'll request notifications before each subitem's prepaint stage.:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
	{
		const int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		if(nItem<m_favcolors.size())
		{
			Gdiplus::Graphics g(pLVCD->nmcd.hdc);
			g.SetPageUnit(Gdiplus::UnitPixel);
			Gdiplus::Rect rect = Gdiplus::Rect(pLVCD->nmcd.rc.left, pLVCD->nmcd.rc.top, pLVCD->nmcd.rc.right-pLVCD->nmcd.rc.left, pLVCD->nmcd.rc.bottom-pLVCD->nmcd.rc.top);
			rect.Inflate(-1, -1);

			const std::vector<Gdiplus::ARGB> colors = m_favcolors[nItem];
			const Gdiplus::SolidBrush brush(colors[pLVCD->iSubItem]);
			g.FillRectangle(&brush, rect);
			g.ReleaseHDC(pLVCD->nmcd.hdc);
		}
		*pResult = CDRF_SKIPDEFAULT;
	}
	break;
	default:
		*pResult = 0;
		break;
	}
}

CMFCPropertyGridProperty* CClassificationDlg::GetPropertyByName(CMFCPropertyGridCtrl* pPropertyGrid, CString strName)
{
	if(pPropertyGrid==nullptr)
		return nullptr;
	for(int index = 0; index<pPropertyGrid->GetPropertyCount(); index++)
	{
		CMFCPropertyGridProperty* pProp = pPropertyGrid->GetProperty(index);
		if(pProp==nullptr)
			continue;
		if(pProp->GetName()==strName)
			return pProp;
		CMFCPropertyGridProperty* pSub = GetPropertyByName(pProp, strName);
		if(pSub!=nullptr)
			return pSub;
	}

	return nullptr;
}

CMFCPropertyGridProperty* CClassificationDlg::GetPropertyByName(CMFCPropertyGridProperty* pProperty, CString strName) const
{
	if(pProperty==nullptr)
		return nullptr;

	for(int index = 0; index<pProperty->GetSubItemsCount(); index++)
	{
		CMFCPropertyGridProperty* pProp = pProperty->GetSubItem(index);
		if(pProp==nullptr)
			continue;
		if(pProp->GetName()==strName)
			return pProp;
		CMFCPropertyGridProperty* pSub = GetPropertyByName(pProp, strName);
		if(pSub!=nullptr)
			return pSub;
	}

	return nullptr;
}

COleVariant CClassificationDlg::GetPropertyValue(CMFCPropertyGridCtrl* pPropertyGrid, CString strName)
{
	CMFCPropertyGridProperty* pProperty = CClassificationDlg::GetPropertyByName(pPropertyGrid, strName);
	if(pProperty==nullptr)
		return COleVariant();
	else
		return pProperty->GetValue();
}
