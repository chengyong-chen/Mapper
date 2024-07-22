#include "stdafx.h"

#include "PropertyDlg.h"
#include <optional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC (CPropertyDlg, CDialog)

CPropertyDlg::CPropertyDlg (CWnd* pParent, const DWORD& dwGeomId, const DWORD& dwAttId, CStringA& strGeoCode, std::optional<bool> bClockwise)
	: CDialog (CPropertyDlg::IDD, pParent), m_dwGeomId (dwGeomId), m_dwAttId (dwAttId), m_strGeoCode (strGeoCode), m_strClockwise (bClockwise==true ? _T ("clockwise") : (bClockwise==false ? _T("anticlockwise ") : _T ("")))
{
}

CPropertyDlg::~CPropertyDlg ()
{
}

void CPropertyDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	DDX_Text (pDX, IDC_GEOMID, m_dwGeomId);
	DDX_Text (pDX, IDC_ATTID, m_dwAttId);
	DDX_Text (pDX, IDC_GEOCODE, m_strGeoCode);
	DDX_Text (pDX, IDC_CLOCKWISE, m_strClockwise);
}

BEGIN_MESSAGE_MAP (CPropertyDlg, CDialog)
END_MESSAGE_MAP ()

