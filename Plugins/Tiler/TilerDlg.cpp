// TilerDlg.cpp : implementation file
//
#include "stdafx.h"
#include "TilerDlg.h"
#include "afxdialogex.h"

#include <cmath>
#include <io.h>
#include <MagickWand/MagickWand.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTilerDlg, CDialogEx)

CTilerDlg::CTilerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CTilerDlg::IDD, pParent)
{
	m_minLevel = 0;
	m_maxLevel = 10;
}

CTilerDlg::~CTilerDlg()
{
}

void CTilerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MINILEVEL, m_minLevel);
	DDX_Text(pDX, IDC_MAXLEVEL, m_maxLevel);
}

BEGIN_MESSAGE_MAP(CTilerDlg, CDialogEx)
	ON_EN_CHANGE(IDC_IMAGEFILE, OnEnChangeImageFile)
END_MESSAGE_MAP()

// CTilerDlg message handlers
BOOL CTilerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	static TCHAR BASED_CODE szFilter[] =
			_T("Supported image files  |*.tif;*.tiff;*.jp2;*.j2k;*.jpg;*.jpeg;*.png;*.gif;*.bmp|")
			_T("TIFF/BigTIFF/GeoTIFF (.tif)|*.tif;*.tiff|")
			_T("JPEG2000 - JPEG 2000 (.jp2, .j2k)|*.jp2;*.j2k|")
			_T("JPEG - Joint Photographic Experts Group JFIF (.jpg)|*.jpg;*.jpeg|")
			_T("PNG - Portable Network Gdiplus::Graphics (.png)|*.png|")
			_T("GIF - Gdiplus::Graphics Interchange Format (.gif)|*.gif|")
			_T("BMP - Microsoft Windows Device Independent Gdiplus::Bitmap (.bmp)|*.bmp|")
			_T("All files (*.*)|*.*");

	((CMFCEditBrowseCtrl*)GetDlgItem(IDC_IMAGEFILE))->EnableFileBrowseButton(nullptr, szFilter);
	return TRUE;
}

void CTilerDlg::OnEnChangeImageFile()
{
	CString strFile;
	GetDlgItem(IDC_IMAGEFILE)->GetWindowText(strFile);
	if(_taccess(strFile, 00) == -1)
		return;

	MagickWandGenesis();
	MagickWand* pWand = NewMagickWand();
	if(MagickReadImage(pWand, CStringA(strFile)) != MagickFalse)
	{
		int cxSource = MagickGetImageWidth(pWand);
		int cySource = MagickGetImageHeight(pWand);
		const int squareSource = max(cxSource,cySource);
		const int levelSource = std::lround(std::log(squareSource/256.f)/std::log(2.f));
		CString str;
		str.Format(_T("%d"), levelSource);
		GetDlgItem(IDC_SOURCELEVEL)->SetWindowText(str);

		DestroyMagickWand(pWand);
	}
	MagickWandTerminus();
}

void CTilerDlg::OnOK()
{
	GetDlgItem(IDC_IMAGEFILE)->GetWindowText(m_imagefile);
	GetDlgItem(IDC_OUTPUTPATH)->GetWindowText(m_outputpath);

	CDialogEx::OnOK();
}
