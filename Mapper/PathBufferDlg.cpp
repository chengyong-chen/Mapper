#include "stdafx.h"
#include "PathBufferDlg.h"


// CPathBufferDlg dialog

IMPLEMENT_DYNAMIC(CPathBufferDlg, CDialogEx)

CPathBufferDlg::CPathBufferDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PATHBUFFER, pParent)
	, distance(0.0)
{

}

CPathBufferDlg::~CPathBufferDlg()
{
}

void CPathBufferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BUFFERDELTA, distance);
	DDV_MinMaxFloat(pDX, distance, 0.1f, 1000.f);
}


BEGIN_MESSAGE_MAP(CPathBufferDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CPathBufferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_ENDBUTT))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_JOINROUND))->SetCheck(TRUE);
	return true;
}

void CPathBufferDlg::OnOK()
{
	CDialogEx::OnOK();
		
	if((((CButton*)GetDlgItem(IDC_ENDBUTT))->GetState()&0X0003)) endtype = Clipper2Lib::EndType::Butt;
	if((((CButton*)GetDlgItem(IDC_ENDROUND))->GetState()&0X0003)) endtype = Clipper2Lib::EndType::Round;
	if((((CButton*)GetDlgItem(IDC_ENDSQUARE))->GetState()&0X0003)) endtype = Clipper2Lib::EndType::Square;

	if((((CButton*)GetDlgItem(IDC_JOINBEVEL))->GetState()&0X0003)) jointype = Clipper2Lib::JoinType::Bevel;
	if((((CButton*)GetDlgItem(IDC_JOINROUND))->GetState()&0X0003)) jointype = Clipper2Lib::JoinType::Round;
	if((((CButton*)GetDlgItem(IDC_JOINMITER))->GetState()&0X0003)) jointype = Clipper2Lib::JoinType::Miter;
}
