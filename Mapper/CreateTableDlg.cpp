// CreateTableDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Mapper.h"
#include "CreateTableDlg.h"

#include "..\Layer\Layer.h"
#include "../Information/FieldDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCreateTableDlg, CDialog)

	CCreateTableDlg::CCreateTableDlg(CDatabase* pDatabase,CObList* pLiayerlist,CWnd* pParent /*=nullptr*/)
	: CDialog(CCreateTableDlg::IDD, pParent)
{
	m_pDatabase = pDatabase;
	m_pLayerlist = pLiayerlist;

	d_pFieldCtrl = nullptr;
}
CCreateTableDlg::~CCreateTableDlg()
{
}

void CCreateTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAYERTREE, m_LayerTree);
}


BEGIN_MESSAGE_MAP(CCreateTableDlg, CDialog)
END_MESSAGE_MAP()


// CCreateTableDlg ��Ϣ�������

BOOL CCreateTableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pLayerlist == nullptr)
		return FALSE;

	POSITION pos = m_pLayerlist->GetHeadPosition();
	while(pos!=nullptr)
	{
		CLayer* layer = (CLayer*)m_pLayerlist->GetNext(pos);
		if(layer == nullptr)
			break;

		HTREEITEM pParentItem = GetItemById(layer->m_wParent);
		if(layer->m_wParent == 0 || pParentItem != nullptr)	
		{
			TV_INSERTSTRUCT	 tvstruct;
			tvstruct.hParent        = pParentItem;
			tvstruct.hInsertAfter   = TVI_LAST;
			tvstruct.item.mask      = TVIF_TEXT | TVIF_PARAM;
			tvstruct.item.pszText   = layer->m_strName.GetBuffer(layer->m_strName.GetLength());
			tvstruct.item.lParam    = (DWORD)layer;   
			m_LayerTree.InsertItem(&tvstruct);

			layer->m_strName.ReleaseBuffer();
		}
		else
		{
			TV_INSERTSTRUCT	 tvstruct;
			tvstruct.hParent        = pParentItem;
			tvstruct.hInsertAfter   = TVI_LAST;
			tvstruct.item.mask      = TVIF_TEXT | TVIF_PARAM;
			tvstruct.item.pszText   = layer->m_strName.GetBuffer(layer->m_strName.GetLength());
			tvstruct.item.lParam    = (DWORD)layer;   
			m_LayerTree.InsertItem(&tvstruct);

			layer->m_strName.ReleaseBuffer();
		}
	}

	HINSTANCE hInstOld = AfxGetResourceHandle();	
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	d_pFieldCtrl = new CFieldDlg(m_pDatabase,nullptr,this);
	if(d_pFieldCtrl->Create(IDD_FIELD, (CWnd*)this) == TRUE)
	{
		d_pFieldCtrl->ShowWindow(SW_SHOWNORMAL);
		CRect rect;
		GetDlgItem(IDC_TABLENAME)->GetWindowRect(rect);
		this->ScreenToClient(rect);
		d_pFieldCtrl->SetWindowPos(&wndTop, rect.left, rect.bottom, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

HTREEITEM CCreateTableDlg::GetItemById(WORD wItemID)
{
	if(wItemID == 0)
		return nullptr;

	HTREEITEM hItem = nullptr;
	hItem = this->GetDownItem(hItem);
	while(hItem != nullptr)
	{
		CLayer* layer = (CLayer*)m_LayerTree.GetItemData(hItem);
		if(layer->m_wId == wItemID)
		{
			return hItem;
		}
		hItem = this->GetDownItem(hItem);
	}

	return nullptr;
}


HTREEITEM CCreateTableDlg::GetDownItem(HTREEITEM hitem)
{
	if(m_LayerTree.GetChildItem(hitem) != nullptr)
	{
		hitem = m_LayerTree.GetChildItem(hitem);
		return hitem;
	}

	if(m_LayerTree.GetNextSiblingItem(hitem) != nullptr)
	{
		hitem = m_LayerTree.GetNextSiblingItem(hitem);
		return hitem;
	}

	while(m_LayerTree.GetParentItem(hitem) != nullptr)
	{
		hitem = m_LayerTree.GetParentItem(hitem);
		if(m_LayerTree.GetNextSiblingItem(hitem) != nullptr)
		{
			hitem = m_LayerTree.GetNextSiblingItem(hitem);
			return hitem;
		}
	}

	return nullptr;
}