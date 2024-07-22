// CMatrixCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Matrix.h"
#include "MatrixCtrl.h"
#include "Resource.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatrixCtrl
IMPLEMENT_DYNCREATE(CMatrixCtrl, CWnd)

//***************************************************************************
// CONSTRUCTOR: CMatrixCtrl::CMatrixCtrl
//***************************************************************************
CMatrixCtrl::CMatrixCtrl()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pMatrix = nullptr;
	m_nActiveRow = -1;

	if(m_imagelist.m_hImageList==nullptr)
	{
		m_imagelist.Create(IDB_FORBID, 16, 0, RGB(255, 255, 255));
	}
}

//***************************************************************************
// DESTRUCTOR: CMatrixCtrl::~CMatrixCtrl
//***************************************************************************
CMatrixCtrl::~CMatrixCtrl()
{
	if(m_imagelist.m_hImageList!=nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	m_pMatrix = nullptr;
	m_nActiveRow = -1;
}

BEGIN_MESSAGE_MAP(CMatrixCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMatrixCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	this->GetClientRect(rect);

	const COLORREF crWindow = IsWindowEnabled() ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_3DFACE);
	CBrush brush(crWindow);
	dc.FillRect(rect, &brush);

	if(m_pMatrix!=nullptr)
	{
		const int nColWidth = 15;
		const int nRowHeight = 20;

		CPen pen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
		CPen* oldpen = (CPen*)dc.SelectObject(&pen);
		for(int i = 0; i<=m_pMatrix->m_nDimension+1; i++)
		{
			dc.MoveTo(0, i*nRowHeight);
			dc.LineTo((m_pMatrix->m_nDimension+1)*nColWidth, i*nRowHeight);

			dc.MoveTo(i*nColWidth, 0);
			dc.LineTo(i*nColWidth, (m_pMatrix->m_nDimension+1)*nRowHeight);
		}
		dc.MoveTo(0, 0);
		dc.LineTo(nColWidth, nRowHeight);

		dc.SelectObject(oldpen);

		if(m_nActiveRow!=-1)
		{
			CRect rect(0, (m_nActiveRow+1)*20, 15, (m_nActiveRow+2)*20);
			rect.DeflateRect(1, 1);
			CBrush brush(RGB(255, 0, 0));
			dc.FillRect(rect, &brush);
		}

		CFont font;
		font.CreateFont(-12, 0, 0, 0, 400,
			FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_TT_PRECIS,
			CLIP_TT_ALWAYS|CLIP_LH_ANGLES,
			DEFAULT_QUALITY,
			FF_MODERN|TMPF_TRUETYPE,
			_T("Arial"));
		CFont* pOldFont = (CFont*)dc.SelectObject(&font);

		dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextAlign(TA_CENTER|TA_BOTTOM);
		for(int i = 1; i<=m_pMatrix->m_nDimension; i++)
		{
			CString str;
			str.Format(_T("%d"), i);
			dc.TextOut(nColWidth/2, i*nRowHeight+nRowHeight/2+8, str);

			dc.TextOut(i*nColWidth+nColWidth/2, nRowHeight/2+8, str);
		}

		for(int i = 0; i<m_pMatrix->m_nDimension; i++)
		{
			for(int j = 0; j<m_pMatrix->m_nDimension; j++)
			{
				CPoint point((j+1)*15+15/2, (i+1)*20+20/2);
				point.Offset(-8, -8);

				if(m_pMatrix->GetValue(i, j)==0X00)
				{
					m_imagelist.Draw(&dc, 0, point, ILD_TRANSPARENT);
				}
				else
				{
					m_imagelist.Draw(&dc, 1, point, ILD_TRANSPARENT);
				}
			}
		}

		dc.SelectObject(pOldFont);
		font.DeleteObject();
	}
}

//***************************************************************************
// METHOD: CMatrixCtrl::Initialize
//
//  Used to Initialize all of the drawing data used to draw the control.
//***************************************************************************
void CMatrixCtrl::Initialize(CMatrix* pMatrix)
{
	m_pMatrix = pMatrix;
	m_nActiveRow = -1;

	this->Invalidate();
}

//***************************************************************************
// METHOD: CMatrixCtrl::OnLButtonDown
//
//  Called when the left mouse button is pressed while on the control.
//  Determines what box tthe cursor is on and toggles a check mark on
//  or off of that box.
//***************************************************************************
void CMatrixCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	if(m_pMatrix!=nullptr)
	{
		int nXIndex = point.x/15;
		int nYIndex = point.y/20;

		if(nYIndex>=1&&nYIndex<=m_pMatrix->m_nDimension&&nXIndex>=0&&nXIndex<=m_pMatrix->m_nDimension)
		{
			nYIndex--;
			nXIndex--;

			if(nXIndex>=0)
			{
				CClientDC dc(this);
				CPoint point((nXIndex+1)*15+7, (nYIndex+1)*20+10);
				point.Offset(-8, -8);

				if(m_pMatrix->GetValue(nYIndex, nXIndex)==0X00)
				{
					m_imagelist.Draw(&dc, 1, point, ILD_TRANSPARENT);
					m_pMatrix->SetValue(nYIndex, nXIndex, true);
				}
				else
				{
					m_imagelist.Draw(&dc, 0, point, ILD_TRANSPARENT);
					m_pMatrix->SetValue(nYIndex, nXIndex, false);
				}

				CWnd* pParent = this->GetParent();
				if(pParent!=nullptr)
				{
					pParent->SendMessage(WM_MATRIXCHANGED, 0, 0);
				}
			}

			if(m_nActiveRow!=nYIndex)
			{
				CClientDC dc(this);

				CFont font;
				font.CreateFont(-12, 0, 0, 0, 400,
					FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_TT_ALWAYS|CLIP_LH_ANGLES,
					DEFAULT_QUALITY,
					FF_MODERN|TMPF_TRUETYPE,
					_T("Arial"));
				CFont* pOldFont = (CFont*)dc.SelectObject(&font);

				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkMode(TRANSPARENT);
				dc.SetTextAlign(TA_CENTER|TA_BOTTOM);

				if(m_nActiveRow!=-1)
				{
					CRect rect1(0, (m_nActiveRow+1)*20, 15, (m_nActiveRow+2)*20);
					rect1.DeflateRect(1, 1);
					CBrush brush1(RGB(255, 255, 255));
					dc.FillRect(rect1, &brush1);

					CString str1;
					str1.Format(_T("%d"), (m_nActiveRow+1));
					dc.TextOut(15/2, (m_nActiveRow+1)*20+20/2+8, str1);
				}

				if(nYIndex>=0)
				{
					CRect rect2(0, (nYIndex+1)*20, 15, (nYIndex+2)*20);
					rect2.DeflateRect(1, 1);
					CBrush brush2(RGB(255, 0, 0));
					dc.FillRect(rect2, &brush2);

					CString str2;
					str2.Format(_T("%d"), nYIndex+1);
					dc.TextOut(15/2, (nYIndex+1)*20+20/2+8, str2);
				}

				dc.SelectObject(pOldFont);
				font.DeleteObject();

				m_nActiveRow = nYIndex;
			}
			CWnd* pWnd = this->GetParent();
			if(pWnd!=nullptr)
			{
				pWnd->Invalidate();
			}
		}
	}
}
