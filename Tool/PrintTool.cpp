#include "stdafx.h"
#include "Global.h"

#include <gdiplus.h>


#include "Tool.h"
#include "PrintTool.h"
#include "RectDlg.h"

#include "../Dataview/Viewinfo.h"

__declspec(dllexport) CPrintTool printTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CPrintTool::GetEncoderClsid(LPCWSTR format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;
	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size==0)
		return -1;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo==nullptr)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j<num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format)==0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

CString CPrintTool::EmueEncoderClsid()
{
	static char BASED_CODE szFilter[] = "Chart Files (*.xlc)|*.xlc|Worksheet Files (*.xls)|*.xls|Data Files (*.xlc;*.xls)|*.xlc; *.xls|All Files (*.*)|*.*||";

	CString string;

	UINT num = 0;
	UINT size = 0;
	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size==0)
		return string;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo==nullptr)
		return string;

	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j<num; ++j)
	{
		const CString strFormatDescription(pImageCodecInfo[j].FormatDescription);
		const CString strFilenameExtension(pImageCodecInfo[j].FilenameExtension);

		CString str;
		str.Format(_T("|%s Files (%s)|*.%s"), strFormatDescription, strFilenameExtension, strFormatDescription);

		string = string+str;
	}

	free(pImageCodecInfo);

	if(string.GetLength()>0)
		string = string.Mid(1);
	return string;
}

void CPrintTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);

		CRect docRect(c_docDown.x, c_docDown.y, c_docLast.x, c_docLast.y);
		docRect.NormalizeRect();
		Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
		dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());

		docRect.SetRect(c_docDown.x, c_docDown.y, docPoint.x, docPoint.y);
		docRect.NormalizeRect();
		cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
		dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CPrintTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(docPoint!=c_docDown)
	{
		CRect rect(c_docDown.x, c_docDown.y, c_docLast.x, c_docLast.y);
		rect.NormalizeRect();
		const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(rect);

		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);

		dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);

		if(pWnd->SendMessage(WM_SETPRINTARECT, 0, (LONG)&rect)==TRUE)
		{
			if(AfxMessageBox(IDS_PREVIEWPRINT, MB_YESNO)==IDYES)
				pWnd->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW, 0);
			else
				pWnd->SendMessage(WM_COMMAND, ID_FILE_PRINT, 0);
		}
	}
	else if(GetKeyState(VK_MENU)<0)
	{
		CRectDlg dlg(pWnd);
		if(dlg.DoModal()==IDOK)
		{
			CRect rect;
			rect.left = dlg.m_nLeft*10000;
			rect.top = dlg.m_nTop*10000;
			rect.right = dlg.m_nRight*10000;
			rect.bottom = dlg.m_nBottom*10000;

			pWnd->SendMessage(WM_SETPRINTARECT, 0, (LONG)&rect);
			if(dlg.m_bPrint==TRUE)
				pWnd->SendMessage(WM_COMMAND, ID_FILE_PRINT, 0);
			else
				pWnd->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW, 0);
		}
	}
}

void CPrintTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
{
	if((GetAsyncKeyState(VK_LBUTTON)&0X8000)==0X8000)// VK_LBUTTON is  down
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);

		CRect rect(c_docDown.x, c_docDown.y, c_docLast.x, c_docLast.y);
		rect.NormalizeRect();
		const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(rect);
		dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::Draw(pWnd, viewinfo, pDC);
}

bool CPrintTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
