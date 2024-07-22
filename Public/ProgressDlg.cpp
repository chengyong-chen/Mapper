// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"
#include "resource.h"


IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGRESSBAR, pParent)
{

}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESSCTRL, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
END_MESSAGE_MAP()

void CProgressDlg::SetRange(int start, int end)
{
	m_ProgressCtrl.SetRange(start, end);
}

void CProgressDlg::UpdateProgressPosition(int pos)
{
	m_ProgressCtrl.SetPos(pos);
}

