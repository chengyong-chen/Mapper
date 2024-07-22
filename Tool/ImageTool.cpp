#include "stdafx.h"
#include "Global.h"
#include "Resource.h"

#include "Tool.h"
#include "ImageTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Imager.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CImageTool imageTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CImageTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_COMMAND, 33068, 0);

	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	TCHAR BASED_CODE szIM_IM_Filter[] = _T("All Image Files|*.bmp;*.bmb;*.gif;*.gib;*.tif;*.ICO;*.JPEG;*.JNG;*.MNG;*.PCD;*.PCX;*.PGM;*.PNG;*.PPM;*.TIFF;*.PSD;*.CUT;*.XBM;*.XPM;*.DDS;*.SGI;*.EXR;*.J2K;*.JP2;*.PFM;*.PICT;*.RAW|")
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
	const CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Image Path"), nullptr);
	CFileDialog dlg(TRUE,
		nullptr,
		strPath,
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
		szIM_IM_Filter,
		AfxGetMainWnd());
	if(IDOK!=dlg.DoModal())
	{
		SetCurrentDirectory(CurrentDir);
		return;
	}
	SetCurrentDirectory(CurrentDir);

	CString strFile = dlg.GetPathName();
	CImager* pImage = new CImager(docPoint, &viewinfo.m_datainfo, strFile);
	if(pImage->m_pDib==nullptr)
	{
		delete pImage;
		pImage = nullptr;
		return;
	}

	pImage->m_strFile = strFile;
	pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)pImage, 0);

	strFile = strFile.Left(strFile.ReverseFind(_T('\\'))+1);
	AfxGetApp()->WriteProfileString(_T(""), _T("Image Path"), strFile);
}

bool CImageTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_IMAGE);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
