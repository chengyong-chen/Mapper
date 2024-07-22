#include "stdafx.h"
#include "Fast.h"

#include "FastDoc.h"

#include "..\Image\Dib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFastDoc

IMPLEMENT_DYNCREATE(CFastDoc, CDocument)

BEGIN_MESSAGE_MAP(CFastDoc, CDocument)
	//{{AFX_MSG_MAP(CFastDoc)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_EDIT_OR, OnEditOr)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OR, OnUpdateEditOr)
	ON_COMMAND(ID_EDIT_AND, OnEditAnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastDoc construction/destruction

CFastDoc::CFastDoc()
{
	m_Dib=nullptr;
}

CFastDoc::~CFastDoc()
{
	if(m_Dib !=nullptr)
		delete m_Dib ;
}

BOOL CFastDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL CFastDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	delete m_Dib;
	m_Dib = CDib::FromFile(lpszPathName);
	return m_Dib!=nullptr;
}


/////////////////////////////////////////////////////////////////////////////
// CFastDoc serialization

void CFastDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
	m_Dib->Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CFastDoc diagnostics

#ifdef _DEBUG
void CFastDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFastDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFastDoc commands

void CFastDoc::Draw(CDC* pDC,CFastView* pView)
{
	if(m_Dib!=nullptr)
	{
		CSize size=m_Dib->GetSize();
		
		Graphics g(pDC->m_hDC);
		g.SetPageUnit(UnitPixel);
		g.SetSmoothingMode(SmoothingModeHighQuality);
		g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

		g.TranslateTransform(0,size.cy);

		m_Dib->Draw(g,CRect(0,0,size.cx,size.cy),CRect(0,0,size.cx,size.cy),0);

		g.TranslateTransform(0,-size.cy);

		g.ReleaseHDC(pDC->m_hDC);
	}
}

void CFastDoc::OnFileExport() 
{
	TCHAR BASED_CODE szIM_IM_Filter[] = _T(
		"Bitmap  (*.bmp)|*.bmp|"
		);

	CFileDialog filedlg(FALSE, 
		                _T("bmp"), 
						nullptr, 
						OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, 
						szIM_IM_Filter, 
						nullptr
						);

	filedlg.m_ofn.lpstrTitle = _T("Export");
	if(filedlg.DoModal() != IDOK)
		return;

	CString pathname = filedlg.GetPathName();
	CString ext      = filedlg.GetFileExt();
	ext.MakeUpper();

	CFile file;
	CFileException fe;
	if(file.Open(pathname, CFile::modeCreate|CFile::modeWrite, &fe) == false)
	{
	
	}

	if(m_Dib != nullptr && ext == _T("BMP"))
	{
		m_Dib->ExportToBmp(file);
	}

	file.Close();	
}

void CFastDoc::OnEditOr() 
{
	CFileDialog  dlg(TRUE,
		_T("*.bmp"),
		nullptr,
		OFN_HIDEREADONLY,
		_T("Bitmap Files (*.bmp)|*.bmp||"),
		nullptr);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strPath= dlg.GetPathName();	

	CDib* pDib = CDib::FromFile(strPath);
	if(pDib!=nullptr)
	{
		m_Dib->Or(pDib);
		delete pDib;
	}
}

void CFastDoc::OnUpdateEditOr(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Dib!=nullptr);
}


void CFastDoc::OnEditAnd() 
{
	CFileDialog  dlg(TRUE,
		_T("*.bmp"),
		nullptr,
		OFN_HIDEREADONLY,
		_T("Bitmap Files (*.bmp)|*.bmp||"),
		nullptr);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strPath= dlg.GetPathName();	

	CDib* pDib = CDib::FromFile(strPath);
	if(pDib!=nullptr)
	{
		m_Dib->And(pDib);
		delete pDib;
	}	
}