#include "stdafx.h"

#include <io.h>
#include <cstdio>
#include <stdlib.h>

#include "Global.h"
#include "Geom.h"
#include "PRect.h"
#include "Poly.h"
#include "Imager.h"

#include "../Image/Dib.h"
#include "../Public/BinaryStream.h"
#include "../Public/Function.h"
#include "../DataView/viewinfo.h"
#include "../Pbf/writer.hpp"
#include "../Utility/Rect.hpp"
#include <boost/json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CImager, CPRect, 0)

CImager::CImager()
	: CPRect()
{
	m_bGroup = false;
	m_pDib = nullptr;
	m_nAlpha = 0;
	m_bEmbeded = false;
	m_bType = 3;
}

CImager::CImager(CRect rect, CDib* pDib)
	: CPRect(rect, nullptr, nullptr)
{
	m_bGroup = false;
	m_pDib = pDib;
	m_nAlpha = 0;
	m_bEmbeded = true;
	m_bType = 3;
}

CImager::CImager(const CPoint& origin, const CDatainfo* pDatainfo, CString lpszPathName)
	: CPRect()
{
	ASSERT_VALID(this);
	m_pDib = nullptr;

	if(lpszPathName.IsEmpty()==FALSE&&_taccess(lpszPathName, 00)!=-1)
	{
		m_pDib = CDib::FromFile(lpszPathName);
		if(m_pDib==nullptr)
			return;

		m_strFile = lpszPathName;
		const CSize size = m_pDib->GetSize();
		CSizeF DPI = m_pDib->GetDPI();

		{
			CString index = lpszPathName;
			index = index.Right(9);
			if(index.CompareNoCase(_T("Index.bmp"))==0)
			{
				DPI.cx = 152.7f;
				DPI.cy = 152.48f;
			}
		}
		const int cx = (size.cx/DPI.cx)*72.f*pDatainfo->m_zoomPointToDoc;
		const int cy = (size.cy/DPI.cy)*72.f*pDatainfo->m_zoomPointToDoc;
		m_Rect = CRect(origin, CSize(cx, -cy));
		m_Rect.NormalizeRect();
	}
	else
	{
		m_Rect = CRect(origin, CSize(0, 0));
	}
	m_bGroup = false;
	m_bType = 3;
	m_nAlpha = 0;
	m_bEmbeded = false;
}

CImager::~CImager()
{
	if(m_pDib!=nullptr)
	{
		delete m_pDib;
		m_pDib = nullptr;
	}
}
void CImager::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CGeom::Sha1(sha1);

	sha1.process_bytes(&m_Rect.left, sizeof(int));
	sha1.process_bytes(&m_Rect.top, sizeof(int));
	sha1.process_bytes(&m_Rect.right, sizeof(int));
	sha1.process_bytes(&m_Rect.bottom, sizeof(int));
	if(m_pDib!=nullptr)
	{
		m_pDib->Sha1(sha1);
	}
}
void CImager::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1, offset);

	LONG  left = m_Rect.left-offset.cx;
	LONG  top = m_Rect.top-offset.cy;
	LONG  rgiht = m_Rect.right-offset.cx;
	LONG  bottom = m_Rect.bottom-offset.cy;

	sha1.process_bytes(&left, sizeof(int));
	sha1.process_bytes(&top, sizeof(int));
	sha1.process_bytes(&right, sizeof(int));
	sha1.process_bytes(&bottom, sizeof(int));
	if(m_pDib!=nullptr)
	{
		m_pDib->Sha1(sha1);
	}
}
BOOL CImager::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom)==FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CImager))==FALSE)
		return FALSE;
	else
		return TRUE;
}

CGeom* CImager::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(CGeom::Clip(bound, clips, in, tolerance, bStroke) == nullptr)
		return nullptr;

	if(in)
	{
		CRect rect;
		rect.IntersectRect(bound, m_Rect);
		CImager* image = new CImager;
		image->m_strName = m_strName;
		image->m_attId = m_attId;
		image->m_Rect = rect;
		if(m_pDib!=nullptr)
		{
			rect.OffsetRect(-m_Rect.left, -m_Rect.top);
			const float xscale = (float)m_pDib->GetSize().cx/m_Rect.Width();
			const float yscale = (float)m_pDib->GetSize().cy/m_Rect.Height();
			rect.left = long(rect.left*xscale);
			rect.right = long(rect.right*xscale+0.99);
			rect.top = long(rect.top*yscale);
			rect.bottom = long(rect.bottom*yscale+0.99);
			image->m_pDib = m_pDib->GetSubDib(rect);
		}
		return image;
	}
	else
	{
		return (CGeom*)this;
	}
}

void CImager::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
}

void CImager::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	if(m_pDib==nullptr)
		return;

	Gdiplus::Rect drawRect;
	g.GetClipBounds(&drawRect);
	Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
	if(drawRect.IntersectsWith(rect)==FALSE)
		return;
	drawRect.Intersect(rect);

	if(m_bRegular==true)
	{
		m_pDib->Draw(g, CRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()), CRect(drawRect.GetLeft(), drawRect.GetTop(), drawRect.GetRight(), drawRect.GetBottom()), m_nAlpha);
	}
	else if(m_pPoints!=nullptr)
	{
		const double fSin = this->GetSin();
		const double fCos = this->GetCos();
		const double fTg = this->GetTg();

		CSize delta1;
		delta1.cx = m_pPoints[2].x-m_pPoints[3].x;
		delta1.cy = m_pPoints[2].y-m_pPoints[3].y;

		CSize delta2;
		delta2.cx = m_pPoints[0].x-m_pPoints[3].x;
		delta2.cy = m_pPoints[0].y-m_pPoints[3].y;

		CRect docRect;
		docRect.left = 0;
		docRect.bottom = 0;
		docRect.top = -sqrt((double)(delta2.cx*delta2.cx+delta2.cy*delta2.cy));
		docRect.right = sqrt((double)(delta1.cx*delta1.cx+delta1.cy*delta1.cy));
		docRect.top = -delta2.cx*fSin+fCos*delta2.cy;

		rect = viewinfo.DocToClient<Gdiplus::Rect>(docRect);
		if(rect.Equals(drawRect)==FALSE)
		{
			const Gdiplus::Matrix matrix1(fCos, -fSin, fSin, fCos, -rect.X, -rect.Y);

			Gdiplus::PointF point1(rect.GetLeft(), rect.GetTop());
			matrix1.TransformPoints(&point1, 1);
			Gdiplus::PointF point2(rect.GetRight(), rect.GetTop());
			matrix1.TransformPoints(&point2, 1);
			Gdiplus::PointF point3(rect.GetRight(), rect.GetBottom());
			matrix1.TransformPoints(&point3, 1);
			Gdiplus::PointF point4(rect.GetLeft(), rect.GetBottom());
			matrix1.TransformPoints(&point4, 1);

			drawRect.X = min(min(point1.X, point2.X), min(point3.X, point4.X));
			drawRect.Y = min(min(point1.Y, point2.Y), min(point3.Y, point4.Y));
			drawRect.Width = max(max(point1.X, point2.X), max(point3.X, point4.X))-drawRect.X;
			drawRect.Height = max(max(point1.Y, point2.Y), max(point3.Y, point4.Y))-drawRect.Y;
			drawRect.Intersect(rect);
		}
		const Gdiplus::GraphicsState state = g.Save();

		const Gdiplus::Matrix matrix(fCos, fSin, fTg*fCos-fSin, fTg*fSin+fCos, rect.X, rect.Y);
		g.MultiplyTransform(&matrix);

		m_pDib->Draw(g, CRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()), CRect(drawRect.GetLeft(), drawRect.GetTop(), drawRect.GetRight(), drawRect.GetBottom()), m_nAlpha);

		g.Restore(state);
	}
}

void CImager::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	return CImager::Draw(g, viewinfo, 1.f);
}

void CImager::Attribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	CImagerDlg dlg(nullptr, this);
	if(dlg.DoModal()==IDOK)
	{
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CImager::CopyTo(CGeom* pGeom, bool ignore) const
{
	CPRect::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CImager))==TRUE)
	{
		CImager* pImager = (CImager*)pGeom;

		if(m_strFile.IsEmpty()==FALSE)
		{
			pImager->m_strFile = m_strFile;
		}

		if(m_pDib!=nullptr)
		{
			delete pImager->m_pDib;
			pImager->m_pDib = m_pDib->Clone();
		}

		pImager->m_nAlpha = m_nAlpha;
		pImager->m_bEmbeded = m_bEmbeded;
	}
}

void CImager::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CImager))==TRUE)
	{
		CImager* pImager = (CImager*)pGeom;
	}

	CPRect::Swap(pGeom);
}

void CImager::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strFile = m_strFile;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		ar<<strFile;
		ar<<m_nAlpha;
		ar<<m_bEmbeded;
		if(m_bEmbeded==true)
		{
			m_pDib->Serialize(ar, dwVersion);
		}
	}
	else
	{
		ar>>m_strFile;
		AfxToFullPath(ar.m_strFileName, m_strFile);
		ar>>m_nAlpha;
		ar>>m_bEmbeded;
		if(m_bEmbeded==false)
		{
			if(_taccess(m_strFile, 0)==-1)
			{
				CString strMsg;
				strMsg.LoadString(IDS_NOTFINDIMAGE);
				strMsg.Replace(_T("%s"), m_strFile);

				if(AfxMessageBox(strMsg, MB_YESNO)==IDYES)
				{
					CString strExt = m_strFile.Right(m_strFile.GetLength()-m_strFile.ReverseFind(_T('.'))-1);
					CString strFilter;
					strFilter.Format(_T("Image file(*.%s)|*.%s||"), strExt, strExt);
					CString strTitle = _T("Find: ")+m_strFile;

					TCHAR CurrentDir[256];
					GetCurrentDirectory(255, CurrentDir);

					CFileDialog dlg(true, strExt, nullptr, OFN_HIDEREADONLY, strFilter, AfxGetMainWnd());
					dlg.m_ofn.lpstrTitle = strTitle;
					if(dlg.DoModal()==IDOK)
					{
						m_strFile = dlg.GetPathName();
						if(ar.m_pDocument!=nullptr)
						{
							ar.m_pDocument->SetModifiedFlag(TRUE);
						}

						CString strFile = m_strFile;
						strFile = strFile.Left(strFile.ReverseFind(_T('\\'))+1);
						if(AfxGetApp()!=nullptr)
						{
							//	AfxGetApp()->WriteProfileString(_T(""),_T("Image Path"),strFile);
						}
						m_pDib = CDib::FromFile(m_strFile);
					}
					SetCurrentDirectory(CurrentDir);
				}
			}
			else
			{
				m_pDib = CDib::FromFile(m_strFile);
			}
		}
		else
		{
			m_pDib = new CDib();
			m_pDib->Serialize(ar, dwVersion);
		}
	}
}

void CImager::ReleaseCE(CArchive& ar) const
{
	CPRect::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		CString strFile = m_strFile;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		//	ar << strFile;
	}
	else
	{
		//	m_pDib= CDib::Apply(m_strFile);
	}
}

void CImager::ReleaseDCOM(CArchive& ar)
{
	CPRect::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		const BYTE byte = m_pDib==nullptr ? (BYTE)0X00 : (BYTE)0X01;
		ar<<byte;
		if(m_pDib!=nullptr)
		{
			m_pDib->ReleaseDCOM(ar);
		}
	}
	else
	{
		delete m_pDib;
		m_pDib = nullptr;

		BYTE byte;
		ar>>byte;
		if(byte==0X01)
		{
			m_pDib = new CDib();
			m_pDib->ReleaseDCOM(ar);
		}
	}
}

DWORD CImager::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CGeom::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		const CString strFilePath = pFile->GetFilePath();
		CString strFile = m_strFile;
		AfxToRelatedPath(strFilePath, strFile);
		const unsigned short length = PackStrPC(strFile, pFile);
		size += length;

		return size;
	}
	else
	{
		CString strFile = UnpackStrPC(bytes);
		if(pFile!=nullptr)
		{
			const CString strFilePath = pFile->GetFilePath();
			AfxToFullPath(strFilePath, strFile);
		}

		if(strFile!=m_strFile)
		{
			delete m_pDib;
			m_pDib = CDib::FromFile(strFile);
			m_strFile = strFile;
		}
		return 0;
	}
}

DWORD CImager::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file, type);
	const CString strFilePath = file.GetFilePath();
	CString strFile = m_strFile;
	AfxToRelatedPath(strFilePath, strFile);
	const unsigned short length = PackStrCE(strFile, file);
	size += length;

	return size;
}
void CImager::ReleaseWeb(boost::json::object& json) const
{
	CPRect::ReleaseWeb(json);
			
	json["Alpha"] = boost::json::value_from(m_nAlpha);
	if(m_pDib!=nullptr)
	{
		m_pDib->ReleaseWeb(json, "base64");
	}
}
void CImager::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPRect::ReleaseWeb(writer, offset);

	writer.add_byte(m_nAlpha);
	if(m_pDib != nullptr)
	{
		writer.add_bool(true);
		m_pDib->ReleaseWeb(writer, offset);
	}
	else
	{
		writer.add_bool(false);
	}
}
void CImager::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
}

void CImager::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
}

void CImager::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
}

void CImager::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
}

IMPLEMENT_DYNAMIC(CImagerDlg, CDialog)

CImagerDlg::CImagerDlg(CWnd* pParent, CImager* imager)
	: CDialog(CImagerDlg::IDD, pParent), m_imager(imager)
{
}

CImagerDlg::~CImagerDlg()
{
}

void CImagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImagerDlg, CDialog)
	//{{AFX_MSG_MAP(CColorDlg)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EMBEDED, OnBnClickedEmbeded)
	ON_BN_CLICKED(IDC_LINKED, OnBnClickedLinked)
END_MESSAGE_MAP()

BOOL CImagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CScrollBar* pScrollBar = (CScrollBar*)GetDlgItem(IDC_SCROLL);
	if(pScrollBar!=nullptr)
	{
		pScrollBar->SetScrollRange(0, 100);
		pScrollBar->SetScrollPos(m_imager->m_nAlpha);
	}

	if(m_imager->m_pDib!=nullptr)
	{
		const CSize size = m_imager->m_pDib->GetSize();
		const CSizeF DPI = m_imager->m_pDib->GetDPI();

		TCHAR ch[4];
		_itot(m_imager->m_nAlpha, ch, 10);
		GetDlgItem(IDC_EDIT)->SetWindowText(ch);

		GetDlgItem(IDC_FILE)->SetWindowText(m_imager->m_strFile);
		CString string;
		string.Format(_T("%d"), size.cx);
		GetDlgItem(IDC_WIDTH)->SetWindowText(string);
		string.Format(_T("%d"), size.cy);
		GetDlgItem(IDC_HEIGHT)->SetWindowText(string);
		string.Format(_T("%.0f"), DPI.cx);
		GetDlgItem(IDC_DPI)->SetWindowText(string);
	}
	else
	{
	}

	if(m_imager->m_bEmbeded==true)
	{
		((CButton*)GetDlgItem(IDC_EMBEDED))->SetCheck(TRUE);
		GetDlgItem(IDC_FILENAME)->EnableWindow(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_LINKED))->SetCheck(TRUE);
		GetDlgItem(IDC_FILENAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_FILENAME)->SetWindowText(m_imager->m_strFile);
	}
	return TRUE; // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CImagerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	ASSERT(pScrollBar);

	if(nSBCode==8)
		return;
	const long oneLine = 1;
	const long onePage = 10;
	const long temp = pScrollBar->GetScrollPos();
	switch(nSBCode)
	{
	case SB_PAGELEFT:
		nPos = temp-onePage;
		break;
	case SB_PAGERIGHT:
		nPos = temp+onePage;
		break;
	case SB_LINERIGHT:
		nPos = temp+oneLine;
		break;
	case SB_LINELEFT:
		if(temp!=0)
			nPos = temp-oneLine;
		break;
	}

	int nMin, nMax;
	pScrollBar->GetScrollRange(&nMin, &nMax);
	if(nPos<nMin)
		nPos = nMin;
	if(nPos>nMax)
		nPos = nMax;

	CString string;
	string.Format(_T("%d"), nPos);
	GetDlgItem(IDC_EDIT)->SetWindowText(string);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImagerDlg::OnChangeEdit()
{
	CString str;
	GetDlgItem(IDC_EDIT)->GetWindowText(str);
	int nPos = _ttoi(str);

	CScrollBar* pScrollBar = (CScrollBar*)GetDlgItem(IDC_SCROLL);
	if(pScrollBar!=nullptr)
	{
		int nMin, nMax;
		pScrollBar->GetScrollRange(&nMin, &nMax);
		if(nPos<nMin)
		{
			nPos = nMin;

			CString string;
			string.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT)->SetWindowText(string);
		}
		else if(nPos>nMax)
		{
			nPos = nMax;

			CString string;
			string.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT)->SetWindowText(string);
		}

		pScrollBar->SetScrollPos(nPos);
	}
}

void CImagerDlg::OnOK()
{
	CDialog::OnOK();

	m_imager->m_bEmbeded = ((CButton*)GetDlgItem(IDC_EMBEDED))->GetCheck()==BST_CHECKED ? 1 : 0;

	CString str;
	GetDlgItem(IDC_EDIT)->GetWindowText(str);
	m_imager->m_nAlpha = _ttoi(str);

	CString strFile;
	GetDlgItem(IDC_FILENAME)->GetWindowText(strFile);
	m_imager->m_strFile = strFile;
}

void CImagerDlg::OnBnClickedEmbeded()
{
	GetDlgItem(IDC_FILENAME)->EnableWindow(FALSE);
}

void CImagerDlg::OnBnClickedLinked()
{
	TCHAR BASED_CODE szIM_IM_Filter[] =
		_T("Bitmap	File (*.bmp)	|*.bmp	|")
		_T("Bmb		File (*.bmb)	|*.bmb	|")
		_T("Gif		File (*.gif)	|*.gif	|")
		_T("Gib		File (*.gib)	|*.gib	|")
		_T("Tiff	File (*.tif)	|*.tif	|")
		_T("ICO		File (*.ICO)	|*.ICO	|")
		_T("JPG		File (*.JPG)	|*.JPG	|")
		_T("JPEG	File (*.JPEG)	|*.JPEG	|")
		_T("JNG		File (*.JNG	)	|*.JNG	|")
		_T("MNG		File (*.MNG	)	|*.MNG	|")
		_T("PCD		File (*.PCD	)	|*.PCD	|")
		_T("PCX		File (*.PCX	)	|*.PCX	|")
		_T("PGM		File (*.PGM	)	|*.PGM	|")
		_T("PNG		File (*.PNG	)	|*.PNG	|")
		_T("PPM		File (*.PPM	)	|*.PPM	|")
		_T("TIFF	File (*.TIFF)	|*.TIFF	|")
		_T("PSD		File (*.PSD	)	|*.PSD	|")
		_T("CUT		File (*.CUT	)	|*.CUT	|")
		_T("XBM		File (*.XBM	)	|*.XBM	|")
		_T("XPM		File (*.XPM	)	|*.XPM	|")
		_T("DDS		File (*.DDS	)	|*.DDS	|")
		_T("SGI		File (*.SGI	)	|*.SGI	|")
		_T("EXR		File (*.EXR	)	|*.EXR	|")
		_T("J2K		File (*.J2K	)	|*.J2K	|")
		_T("JP2		File (*.JP2	)	|*.JP2	|")
		_T("PFM		File (*.PFM	)	|*.PFM	|")
		_T("PICT	File (*.PICT)	|*.PICT	|")
		_T("RAW		File (*.RAW	)	|*.RAW	||");
	CFileDialog dlg(FALSE,
		nullptr,
		m_imager->m_strFile,
		OFN_CREATEPROMPT|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szIM_IM_Filter,
		nullptr
	);

	dlg.m_ofn.lpstrTitle = _T("Save as an Image file");
	if(dlg.DoModal()==IDOK)
	{
		CString strFile = dlg.GetPathName(); // Filename and path for the file to be created
		if(m_imager->m_pDib->SaveAs(CStringA(strFile))==true)
		{
			GetDlgItem(IDC_FILENAME)->SetWindowText(strFile);
			GetDlgItem(IDC_FILENAME)->EnableWindow(TRUE);
		}
		else
			((CButton*)GetDlgItem(IDC_EMBEDED))->SetCheck(TRUE);
	}
	else
		((CButton*)GetDlgItem(IDC_EMBEDED))->SetCheck(TRUE);
}
