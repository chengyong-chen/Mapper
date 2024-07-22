#pragma once

#include "Midtable.hpp"
#include "PatternMid.h"

class CSymbol;

template<class TPattern>
class TPatternMidDlg : public CDialog
{
public:
	TPatternMidDlg(UINT nId, CWnd* pParent, CMidtable<CPatternMid>& midtable, TPattern* pPattern)
		: CDialog(nId, pParent), m_midtable(midtable)
	{
		//{{AFX_DATA_INIT(TPatternMidDlg)
		// NOTE: the ClassWizard will add member initialization here
		m_nScale = 100;
		//}}AFX_DATA_INIT

		if(pPattern!=nullptr)
		{
			m_nScale = pPattern->m_nScale;
		}
		m_pPattern = pPattern;
	}

	// Dialog Data
	//{{AFX_DATA(TPatternMidDlg)
	CComboBox m_comboFile;
	CListBox m_ImageBox;
	long m_nScale;
	//}}AFX_DATA

	CMidtable<CPatternMid>& m_midtable;
	TPattern* m_pPattern;
	CImageList m_imagelist;
	int m_nHeight;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TPatternMidDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TPatternMidDlg)
		DDX_Control(pDX, IDC_COMBOFILE, m_comboFile);
		DDX_Text(pDX, IDC_SCALE, m_nScale);
		DDV_MinMaxInt(pDX, m_nScale, 10, 300);

		DDX_Control(pDX, IDC_IMAGELIST, m_ImageBox);
		//}}AFX_DATA_MAP
	}

	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TPatternMidDlg)
	BOOL OnInitDialog() override
	{
		CDialog::OnInitDialog();

		for(std::map<WORD, CPatternMid*>::const_iterator it = m_midtable.m_midlist.begin(); it!=m_midtable.m_midlist.end(); ++it)
		{
			CPatternMid* pMid = it->second;
			if(m_comboFile.FindString(0, pMid->m_strFile)==-1)
			{
				const int nItem = m_comboFile.AddString(pMid->m_strFile);
				m_comboFile.SetItemData(nItem, (DWORD)pMid);
			}
		}

		CPatternMid* pMid = m_pPattern==nullptr ? nullptr : m_midtable.GetMid(m_pPattern->m_libId);
		if(pMid!=nullptr)
		{
			const CString strFile = pMid->m_strFile;
			m_comboFile.SelectString(0, strFile);
			ResetImagelist(strFile, m_pPattern->m_nIndex);
		}
		else if(m_comboFile.GetCount()>0)
		{
			CString strFile;
			m_comboFile.GetLBText(0, strFile);
			m_comboFile.SetCurSel(0);
			ResetImagelist(strFile, -1);
		}

		return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void OnOK() override
	{
		CDialog::OnOK();

		if(m_pPattern!=nullptr)
		{
			const int indexFile = m_comboFile.GetCurSel();
			if(indexFile==-1)
				return;

			CString strFile;
			m_comboFile.GetLBText(indexFile, strFile);

			CPatternMid* pMid = (CPatternMid*)m_midtable.Find(strFile);
			if(pMid==nullptr)
			{
				pMid = new CPatternMid();
				pMid->Load(strFile);
				pMid->m_wId = m_midtable.GetMaxId()+1;
				pMid->m_nUsers = 1;
				m_midtable.m_midlist[pMid->m_wId] = pMid;
			}
			else
			{
				pMid->m_nUsers++;
			}
			m_pPattern->m_libId = pMid->m_wId;
			m_pPattern->m_nIndex = m_ImageBox.GetCurSel();
			m_pPattern->m_nScale = (unsigned short)m_nScale;
		}
	}

	afx_msg void OnPaint()
	{
		CPaintDC dc(this); // device context for painting

		CWnd* pWnd = GetDlgItem(IDC_PATTERNICON);
		if(pWnd==nullptr)
			return;
		const int nItem = m_ImageBox.GetCurSel();
		if(nItem!=-1)
		{
			CRect rect;
			pWnd->GetClientRect(rect);
			CPaintDC ImageDc(pWnd);
			ImageDc.FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

			CPoint point = rect.CenterPoint();
			point.x -= m_nHeight/2;
			point.y -= m_nHeight/2;

			if(m_imagelist.m_hImageList!=nullptr)
			{
				m_imagelist.Draw(&ImageDc, nItem, point, ILD_TRANSPARENT);
			}
		}
	}

	bool ResetImagelist(CString strFile, WORD nIndex)
	{
		m_ImageBox.ResetContent();

		if(m_imagelist.m_hImageList!=nullptr)
		{
			m_imagelist.DeleteImageList();
			m_imagelist.Detach();
		}
		if(_taccess(strFile, 00)==-1)
		{
			return FALSE;
		}
		CFile file;
		if(file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite)==false)
		{
			AfxMessageBox(_T("λͼ�ļ����ܱ���!"));
			return FALSE;
		}
		CPatternMid* pMid = new CPatternMid();
		if(pMid->Load(strFile)==TRUE)
		{
			m_nHeight = pMid->m_nHeight;
			m_imagelist.Create(&pMid->m_imagelist);
			m_imagelist.SetBkColor(RGB(192, 192, 192));
			m_ImageBox.SetItemHeight(0, m_nHeight+1);

			for(int index = 0; index<pMid->m_nCount; index++)
			{
				const int nCount = m_ImageBox.GetCount();
				m_ImageBox.InsertString(nCount, _T(""));
			}

			if(m_ImageBox.GetCount()>0)
			{
				CString string;

				string.Format(_T("%d"), pMid->m_nCount);
				GetDlgItem(IDC_COUNT)->SetWindowText(string);

				string.Format(_T("%d"), 72);
				GetDlgItem(IDC_RESOLUTION)->SetWindowText(string);

				string.Format(_T("%d"), m_nHeight);
				GetDlgItem(IDC_HEIGHT)->SetWindowText(string);

				string.Format(_T("%d"), 256);
				GetDlgItem(IDC_COLORS)->SetWindowText(string);
			}
			if(nIndex>=0)
			{
				m_ImageBox.SetCurSel(nIndex);
			}
			else
			{
				m_ImageBox.SetCurSel(0);
			}

			OnSelchangeImagelist();
		}
		delete pMid;
		pMid = nullptr;
		return true;
	}

	void OnSelchangeImagelist()
	{
		const int index = m_ImageBox.GetCurSel();
		if(index!=-1)
		{
			CWnd* pWnd = GetDlgItem(IDC_PATTERNICON);
			if(pWnd!=nullptr)
			{
				pWnd->Invalidate();
				CRect rect;
				pWnd->GetClientRect(rect);
				CPaintDC ImageDc(pWnd);
				ImageDc.FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

				CPoint point = rect.CenterPoint();
				point.x -= m_nHeight/2;
				point.y -= m_nHeight/2;
				if(m_imagelist.m_hImageList!=nullptr)
				{
					m_imagelist.Draw(&ImageDc, index, point, ILD_TRANSPARENT);
				}
			}
		}
	}

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		//	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);

		if(nIDCtl==IDC_IMAGELIST)
		{
			const int index = (int)lpDrawItemStruct->itemID;
			if(m_imagelist.m_hImageList==nullptr)
			{
			}
			else if(index<0)
			{
				// If there are no elements in the List Box 
				// based on whether the list box has Focus or not 
				// draw the Focus Gdiplus::Rect or Erase it,
				CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
				if((lpDrawItemStruct->itemAction&ODA_FOCUS)&&(lpDrawItemStruct->itemState&ODS_FOCUS))
				{
					pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
				}
				else if((lpDrawItemStruct->itemAction&ODA_FOCUS)&&!(lpDrawItemStruct->itemState&ODS_FOCUS))
				{
					pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
				}
			}
			else if(m_imagelist.GetImageCount()<index+1)
			{
			}
			else
			{
				CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

				CRect rcItem(lpDrawItemStruct->rcItem); // To draw the focus rect.
				CRect rClient(rcItem); // Gdiplus::Rect to highlight the Item

				rClient.left += m_nHeight;

				// If item selected, draw the highlight rectangle.
				// Or if item deselected, draw the rectangle using the window color.
				if((lpDrawItemStruct->itemState&ODS_SELECTED)&&(lpDrawItemStruct->itemAction&(ODA_SELECT|ODA_DRAWENTIRE)))
				{
					CBrush br(::GetSysColor(COLOR_HIGHLIGHT));
					pDC->FillRect(&rClient, &br);
				}
				else if(!(lpDrawItemStruct->itemState&ODS_SELECTED)&&(lpDrawItemStruct->itemAction&ODA_SELECT))
				{
					CBrush br(::GetSysColor(COLOR_WINDOW));
					pDC->FillRect(&rClient, &br);
				}

				// If the item has focus, draw the focus rect.
				// If the item does not have focus, erase the focus rect.
				if((lpDrawItemStruct->itemAction&ODA_FOCUS)&&(lpDrawItemStruct->itemState&ODS_FOCUS))
				{
					pDC->DrawFocusRect(&rcItem);
				}
				else if((lpDrawItemStruct->itemAction&ODA_FOCUS)&&!(lpDrawItemStruct->itemState&ODS_FOCUS))
				{
					pDC->DrawFocusRect(&rcItem);
				}
				m_imagelist.Draw(pDC, index, rcItem.TopLeft(), ILD_NORMAL);
			}
		}
	}

	afx_msg void OnLoad()
	{
		TCHAR CurrentDir[256];
		GetCurrentDirectory(255, CurrentDir);

		CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Pattern Path"), nullptr)+_T("*.bmp");
		CFileDialog dlg(TRUE,
			_T("bmp"),
			strPath,
			OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
			_T("Bitmap File (*.bmp)|*.bmp|"),
			this);
		if(IDOK!=dlg.DoModal())
		{
			SetCurrentDirectory(CurrentDir);
			return;
		}
		SetCurrentDirectory(CurrentDir);
		const CString strFile = dlg.GetPathName();
		if(m_comboFile.FindString(0, strFile)==-1)
		{
			m_comboFile.AddString(strFile);
		}
		m_comboFile.SelectString(0, strFile);
		ResetImagelist(strFile, -1);

		strPath = strFile.Left(strFile.ReverseFind(_T('\\'))+1);
		AfxGetApp()->WriteProfileString(_T(""), _T("Pattern Path"), strPath);
	}

public:
	afx_msg void OnDestroy()
	{
		if(m_imagelist.m_hImageList!=nullptr)
		{
			m_imagelist.DeleteImageList();
			m_imagelist.Detach();
		}

		CDialog::OnDestroy();
	}

	afx_msg void OnSelchangeFile()
	{
		const int nItem = m_comboFile.GetCurSel();
		if(nItem==-1)
			return;

		CString strFile;
		m_comboFile.GetLBText(nItem, strFile);
		ResetImagelist(strFile, -1);
	}

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_TEMPLATE_MESSAGE_MAP(TPatternMidDlg, TPattern, CDialog)
	//{{AFX_MSG_MAP(TSymbolMidDlg)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_CBN_SELCHANGE(IDC_COMBOFILE, OnSelchangeFile)
	ON_LBN_SELCHANGE(IDC_IMAGELIST, OnSelchangeImagelist)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
