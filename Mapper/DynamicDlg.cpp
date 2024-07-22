#include "stdafx.h"
#include "DynamicDlg.h"
#include "afxdialogex.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDynamicDlg, CDialogEx)

CDynamicDlg::CDynamicDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DYNAMIC, pParent)
{

}

CDynamicDlg::~CDynamicDlg()
{
}

void CDynamicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDynamicDlg, CDialogEx)
END_MESSAGE_MAP()