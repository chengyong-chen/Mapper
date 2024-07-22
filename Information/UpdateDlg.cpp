#include "stdafx.h"
#include "information.h"
#include "UpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog


CUpdateDlg::CUpdateDlg(CWnd* pParent,COL* pCol, SWORD nCol)
	: CDialog(CUpdateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpdateDlg)
		
	//}}AFX_DATA_INIT
	m_pCol = pCol;
	m_nCol = nCol;
}


void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CUpdateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg message handlers
const int   xFIRST = 10;
const int   cxSEP  = 6;
const int   cySEP  = 3;
const int   cxNAME = 35;
const int   cyNAME = 8;
const int   cxEDIT = 180;
const int   cyEDIT = 10;

BOOL CUpdateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
#define DLGX(x)   (((x)*LOWORD(dwBaseUnits))/4)
#define DLGY(y)   (((y)*HIWORD(dwBaseUnits))/8)

//   HWND  hWnd;
   HFONT hfont;
   RECT  rc;
//   LPCOL lpcol;
   char  sz[512];
   DWORD dwBaseUnits;
   UINT  idName, idEdit;
   int      xName, yName;
   int      cxName, cyName;
   int      xEdit, yEdit;
   int      cxEdit, cyEdit;
   int      i;
   CDC* hdc;
   SIZE  size;

   // Determine basic characteristics and start position in dialog
   hfont = (HFONT)SendDlgItemMessage(IDC_STATIC1,WM_GETFONT, 0, 0L);

   dwBaseUnits = GetDialogBaseUnits();

   cxName = DLGX(cxNAME);
   cyName = DLGY(cyNAME);
   cxEdit = DLGX(cxEDIT);
   cyEdit = DLGY(cyEDIT);

   
   hdc = GetDC();

   if(hdc) 
   {
      for(i=0;i<m_nCol; i++)
         if(IsUpdateable(m_pCol[i].fSqlType)) 
		 {
            wsprintf((LPTSTR)sz, _T("%s:"), m_pCol[i].szName);
            GetTextExtentPoint(hdc->m_hDC, (LPCTSTR)sz, lstrlen((LPCTSTR)sz), &size);
            if(size.cx > cxName)
               cxName = size.cx;
         }
//      hdc->ReleaseDC();
   }

   GetDlgItem(IDOK)->GetWindowRect(&rc);

   xName = DLGX(xFIRST);
   yName = (4*DLGY(cySEP)) + (DLGY(cySEP)/2) + (2*(rc.bottom - rc.top));

   xEdit = xName + cxName + DLGX(cxSEP);

   idName = stc1;
   idEdit = edt1;

   // For each bound, updateable column, create and add an edit control
   for (i=0; i< m_nCol; i++) 
   {
      if(IsUpdateable(m_pCol[i].fSqlType)) 
	  {
         wsprintf((LPTSTR)sz, _T("%s:"), m_pCol[i].szName);

    	CStatic* sta = new CStatic;
		sta->Create((LPTSTR)sz,WS_CHILD | WS_VISIBLE,CRect(CPoint(xName, yName), CSize(cxName, cyName)), this);
	
     // FORWARD_WM_SETFONT(hWnd, hfont, 0, SendMessage);

         // Create (and intialize) edit control
         yEdit = yName - ((cyEDIT - cyNAME)/2);

         GetCurrentValue(sz, m_pCol+i);

		CEdit* edi = new CEdit;
		edi->Create(WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_LEFT,CRect(CPoint(xEdit, yEdit), CSize(cxEdit, cyEdit)),this,idEdit);
		edi->SetWindowText((LPTSTR)sz);	
//      FORWARD_WM_SETFONT(hWnd, hfont, 0, SendMessage);

         edi->LimitText(m_pCol[i].cbc-1);

         yName += cyEdit + DLGY(cySEP);

         idName++;
         idEdit++;
      }
   }

   // Grow dialog so that all controls are visible
   GetClientRect(&rc);

   rc.top    = 0;
   rc.bottom = yName + DLGY(cySEP);

   rc.left   = 0;
   rc.right  = (2*DLGX(xFIRST)) + DLGX(cxNAME) + DLGX(cxEDIT) + DLGX(cxSEP);

   AdjustWindowRect(&rc, DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU, FALSE);
   MoveWindow(0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);

   // Place OK and Cancel buttons appropriately
   GetClientRect(&rc);
   {  RECT  rcButton;
      int      x, y;

      GetDlgItem(IDOK)->GetWindowRect(&rcButton);

      x = rc.right - DLGX(cxSEP) - (rcButton.right - rcButton.left);
      y = DLGY(cySEP);

      GetDlgItem(IDOK)->MoveWindow(x, y, rcButton.right - rcButton.left, rcButton.bottom - rcButton.top, TRUE);

      y += rcButton.bottom - rcButton.top + (DLGY(cySEP)/2);

      GetDlgItem(IDCANCEL)->GetWindowRect(&rcButton);
      GetDlgItem(IDCANCEL)->MoveWindow(x, y, rcButton.right - rcButton.left, rcButton.bottom - rcButton.top, TRUE);
   }

   return TRUE;
}

void CUpdateDlg::OnOK() 
{
	CDialog::OnOK();

//	LPCOL lpcol;
	char  sz[512];
	UINT  idEdit;
	BOOL  fChangeMade;
	int   i;

   fChangeMade = FALSE;
   idEdit      = edt1;

   // For each bound, updateable column, retrieve the value
   for(i=0; i<m_nCol; i++) 
   {
      if(IsUpdateable(m_pCol[i].fSqlType)) 
	  {
         // Get the control value
         SendDlgItemMessage(idEdit, WM_GETTEXT,(WPARAM)sizeof(sz), (LPARAM)((LPSTR)sz));

         // Move into row-set buffer
         if(SetCurrentValue(sz, &m_pCol[i]) && !fChangeMade)
            fChangeMade = TRUE;

         idEdit++;
      }
   }

}
