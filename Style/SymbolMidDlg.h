#pragma once

#ifndef __AFXWIN_H__
#include "afxwin.h"
#endif

#include "Global.h"
#include "Symbol.h"
#include "SymbolMid.hpp"
#include "SpotSymbol.h"
#include "LineSymbol.h"
#include "FillSymbol.h"
#include "../Style/MidTable.hpp"

template<class TSymbolX, class TXSymbol>
class TSymbolMidDlg : public CDialog
{
public:
	TSymbolMidDlg(UINT IDD, CWnd* pParent, CMidtable<CSymbolMid<TSymbolX>>& midtable, TXSymbol* pXSymbol)
		: CDialog(IDD, pParent), m_midtable(midtable)
	{
		//{{AFX_DATA_INIT(TSymbolMidDlg)
		m_nScale=100;
		//}}AFX_DATA_INIT

		if(pXSymbol != nullptr)
		{
			m_nScale=pXSymbol->m_nScale;
		}
		m_pXSymbol=pXSymbol;
	}

	// Dialog Data
	//{{AFX_DATA(TSymbolMidDlg)
	CComboBox m_comboFile;
	CListCtrl m_listctrl;
	long m_nScale;;
	//}}AFX_DATA

	CMidtable<CSymbolMid<TSymbolX>>& m_midtable;
	CSymbolMid<TSymbolX> m_symbolmid;
	TXSymbol* m_pXSymbol;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TSymbolMidDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TSymbolMidDlg)
		DDX_Control(pDX, IDC_COMBOFILE, m_comboFile);
		DDX_Text(pDX, IDC_SCALE, m_nScale);
		DDV_MinMaxInt(pDX, m_nScale, 20, 1000);
		DDX_Control(pDX, IDC_SYMBOLLIST, m_listctrl);
		//}}AFX_DATA_MAP
	}

	BOOL OnInitDialog() override
	{
		CDialog::OnInitDialog();

		CRect rect;
		LV_COLUMN lvcolumn;
		m_listctrl.GetClientRect(rect);

		CString wordName;
		CString wordSymbol;
		wordName.LoadString(IDS_NAME);
		wordSymbol.LoadString(IDS_SYMBOL);

		//Add column
		lvcolumn.mask=LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt=LVCFMT_LEFT;
		lvcolumn.pszText=wordName.GetBuffer(0);
		lvcolumn.iSubItem=0;
		lvcolumn.cx=rect.Width()/2;
		m_listctrl.InsertColumn(0, &lvcolumn);
		wordName.ReleaseBuffer();

		lvcolumn.fmt=LVCFMT_CENTER;
		lvcolumn.pszText=wordSymbol.GetBuffer(0);
		lvcolumn.iSubItem=1;
		lvcolumn.cx=rect.Width()/2;
		m_listctrl.InsertColumn(1, &lvcolumn);
		wordSymbol.ReleaseBuffer();

		m_listctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		for(typename std::map<WORD, CSymbolMid<TSymbolX>*>::const_iterator it=m_midtable.m_midlist.begin(); it != m_midtable.m_midlist.end(); ++it)
		{
			CSymbolMid<TSymbolX>* pMid=it->second;
			if(m_comboFile.FindString(0, pMid->m_strFile) == -1)
			{
				m_comboFile.AddString(pMid->m_strFile);
			}
		}

		CSymbolMid<TSymbolX>* pMid=m_pXSymbol == nullptr ? nullptr : m_midtable.GetMid(m_pXSymbol->m_libId);
		if(pMid != nullptr)
		{
			const CString strFile=pMid->m_strFile;
			m_comboFile.SelectString(0, strFile);
			ResetSymbollist(strFile, m_pXSymbol->m_symId);
		}
		else if(m_comboFile.GetCount() > 0)
		{
			CString strFile;
			m_comboFile.GetLBText(0, strFile);
			m_comboFile.SetCurSel(0);
			ResetSymbollist(strFile, -1);
		}

		return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void OnOK() override
	{
		CDialog::OnOK();

		if(m_pXSymbol != nullptr)
		{
			const int indexFile=m_comboFile.GetCurSel();
			if(indexFile == -1)
				return;

			CString strFile;
			m_comboFile.GetLBText(indexFile, strFile);
			POSITION pos=m_listctrl.GetFirstSelectedItemPosition();
			if(pos == nullptr)
				return;
			const long indexSymbol=m_listctrl.GetNextSelectedItem(pos);
			CSymbol* symbol=(CSymbol*)(m_listctrl.GetItemData(indexSymbol));
			if(symbol == nullptr)
				return;

			CSymbolMid<TSymbolX>* pMid=m_midtable.Find(strFile);
			if(pMid == nullptr)
			{
				pMid=new CSymbolMid<TSymbolX>();
				pMid->Load(strFile);
				pMid->m_wId=m_midtable.GetMaxId() + 1;
				pMid->m_nUsers=1;
				m_midtable.m_midlist[pMid->m_wId]=pMid;
			}
			else
			{
				pMid->m_nUsers++;
			}

			m_pXSymbol->m_libId=pMid->m_wId;
			m_pXSymbol->m_symId=symbol->m_wId;
			m_pXSymbol->m_nScale=(unsigned short)m_nScale;
		}
	}

	virtual void DrawPreview()
	{
		POSITION pos=m_listctrl.GetFirstSelectedItemPosition();
		if(pos == nullptr)
		{
			TRACE0("No items were selected!\n");
			return;
		}
		const long nItem=m_listctrl.GetNextSelectedItem(pos);
		CSymbol* symbol=(CSymbol*)(m_listctrl.GetItemData(nItem));
		if(symbol == nullptr)
			return;

		CRect box;
		CWnd* pWnd=GetDlgItem(IDC_PREVIEW);
		if(pWnd != nullptr)
		{
			pWnd->GetClientRect(box);

			CRect rect(box);
			rect.InflateRect(-1, -1);
			CClientDC dc(pWnd);
			dc.FillSolidRect(box, GetSysColor(COLOR_3DFACE));

			CRgn rgn;
			rgn.CreateRectRgnIndirect(rect);
			dc.SelectClipRgn(&rgn);
			rgn.DeleteObject();

			dc.SetMapMode(MM_ANISOTROPIC);
			const long diversion=rect.bottom;
			rect.bottom=rect.top;
			rect.top=diversion;
			dc.DPtoLP(rect);
			const CPoint point=rect.CenterPoint();

			Gdiplus::Graphics g(dc.m_hDC);
			g.TranslateTransform(point.x, point.y);
			symbol->Draw(g, 0.f, 1.f, 1.f, CSize(72, 72));
			g.TranslateTransform(-point.x, -point.y);
			g.ReleaseHDC(dc.m_hDC);
		}
		GetParent()->SendMessage(WM_REDRAWPREVIEW);
	}

	bool ResetSymbollist(CString strFile, WORD symID)
	{
		m_listctrl.DeleteAllItems();
		m_symbolmid.Clear();

		if(m_symbolmid.Load(strFile) == TRUE)
		{
			POSITION pos=m_symbolmid.m_Symbollist.GetHeadPosition();
			while(pos != nullptr)
			{
				TSymbolX* pSymbolX=m_symbolmid.m_Symbollist.GetNext(pos);
				long nItem=m_listctrl.GetItemCount();
				nItem=m_listctrl.InsertItem(nItem, nullptr, 1);
				m_listctrl.SetItemData(nItem, (DWORD)pSymbolX);
				m_listctrl.SetItemText(nItem, 0, pSymbolX->m_strName);
				if(pSymbolX->m_wId == symID)
				{
					m_listctrl.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
			}
			return TRUE;
		}
		else
		{
			AfxMessageBox(_T("��״���ſ��ļ����ܱ���!"));
			return FALSE;
		}
	}

	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TSymbolMidDlg)
	afx_msg void OnPaint()
	{
		CPaintDC dc(this); // device context for painting

		DrawPreview();
	}

	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NM_LISTVIEW* pNMListView=(NM_LISTVIEW*)pNMHDR;

		if(pNMListView->uNewState == 0)
		{
			*pResult=0;
			return;
		}

		DrawPreview();

		*pResult=0;
	}

	afx_msg void OnLoad()
	{
		CString strExt;
		CString strFilter;
		if(RUNTIME_CLASS(TXSymbol) == RUNTIME_CLASS(CSpotSymbol))
		{
			strExt=_T("*.mym");
			strFilter.LoadString(IDS_SPOTSYMBOLFILTER);
		}
		else if(RUNTIME_CLASS(TXSymbol) == RUNTIME_CLASS(CFillSymbol))
		{
			strExt=_T("*.fym");
			strFilter.LoadString(IDS_FILLSYMBOLFILTER);
		}
		else if(RUNTIME_CLASS(TXSymbol) == RUNTIME_CLASS(CLineSymbol))
		{
			strExt=_T("*.lym");
			strFilter.LoadString(IDS_LINESYMBOLFILTER);
		}

		TCHAR CurrentDir[256];
		GetCurrentDirectory(255, CurrentDir);

		CString strPath=AfxGetApp()->GetProfileString(_T(""), _T("Symbol Path"), nullptr) + strExt;
		CFileDialog dlg(TRUE,
			strExt,
			strPath,
			OFN_HIDEREADONLY,
			strFilter,
			this);

		if(dlg.DoModal() != IDOK)
		{
			SetCurrentDirectory(CurrentDir);
			return;
		}
		SetCurrentDirectory(CurrentDir);

		strExt=strExt.Mid(2);
		if(dlg.GetFileExt().CompareNoCase(strExt) != 0)
		{
			AfxMessageBox(_T("��Ч�ķ��ſ��ļ�!"));
			return;
		}

		strPath=dlg.GetPathName();

		if(m_comboFile.FindString(0, strPath) == -1)
			m_comboFile.AddString(strPath);
		m_comboFile.SelectString(0, strPath);
		ResetSymbollist(strPath, -1);

		strPath=strPath.Left(strPath.ReverseFind(_T('\\')) + 1);
		CWinApp* pApp=AfxGetApp();
		//	AfxGetApp()->WriteProfileString(_T(""),_T("Spot Symbol Path"),strPath);	
	}

	afx_msg void OnSelchangeFile()
	{
		const int nItem=m_comboFile.GetCurSel();
		if(nItem == -1)
			return;

		CString strFile;
		m_comboFile.GetLBText(nItem, strFile);
		ResetSymbollist(strFile, -1);
	}

	afx_msg void OnChangeScale()
	{
		// TODO: Add your control notification handler code here
		CString str;
		GetDlgItem(IDC_SCALE)->GetWindowText(str);
		m_nScale=_ttoi(str);
		DrawPreview();
	}

	afx_msg void OnDestroy()
	{
		CDialog::OnDestroy();

		m_symbolmid.Clear();
	}

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define BEGIN_TEMPLATE_MESSAGE_MAP_2(theClass, class1,class2, baseClass)	\
	PTM_WARNING_DISABLE														\
	template < typename class1,typename class2 >							\
	const AFX_MSGMAP* theClass< class1,class2>::GetMessageMap() const		\
{ return GetThisMessageMap(); }												\
	template < typename class1,typename class2 >							\
	const AFX_MSGMAP* PASCAL theClass< class1,class2>::GetThisMessageMap()	\
{																			\
	typedef theClass<class1,class2> ThisClass;								\
	typedef baseClass TheBaseClass;											\
	static const AFX_MSGMAP_ENTRY _messageEntries[] =						\
{
BEGIN_TEMPLATE_MESSAGE_MAP_2(TSymbolMidDlg, TXSymbol, TSymbolX, CDialog)
//{{AFX_MSG_MAP(TSymbolMidDlg)
ON_WM_PAINT()
ON_CBN_SELCHANGE(IDC_COMBOFILE, OnSelchangeFile)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_SYMBOLLIST, OnItemchanged)
ON_EN_CHANGE(IDC_SCALE, OnChangeScale)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_LOAD, OnLoad)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
