#include "Stdafx.h"

#include <afxpriv.h>
#include "GSplitterWnd.h"
#include "GRuntimeClass.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(GSplitterWnd, CSplitterWndEx)

GSplitterWnd::GSplitterWnd()
	: m_pSavedColInfo(nullptr), m_pSavedRowInfo(nullptr)
{
}

BEGIN_MESSAGE_MAP(GSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(GSplitterWnd)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

GSplitterWnd::~GSplitterWnd()
{
	delete[] m_pSavedColInfo;
	delete[] m_pSavedRowInfo;
}

void GSplitterWnd::Init()
{
	m_panwnds.Init(m_nMaxRows, m_nMaxCols);

	for(int i = 0; i<m_nMaxRows; i++)
	{
		m_shown_rows.push_back(i);
	}

	for(int j = 0; j<m_nMaxCols; j++)
	{
		m_shown_cols.push_back(j);
	}

	m_pSavedColInfo = new CRowColInfo[m_nMaxCols];
	m_pSavedRowInfo = new CRowColInfo[m_nMaxRows];
}

BOOL GSplitterWnd::CreateStatic(CWnd* pParentWnd, int nRows, int nCols, DWORD dwStyle, UINT nId)
{
	const BOOL ret = CSplitterWnd::CreateStatic(pParentWnd, nRows, nCols, dwStyle, nId);
	Init();
	return ret;
}

BOOL GSplitterWnd::CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(row>=0&&row<m_nRows);
	ASSERT(col>=0&&col<m_nCols);
	ASSERT(pViewClass!=nullptr);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));

	if(GetDlgItem(IdFromRowCol(row, col))!=nullptr)
	{
		TRACE(traceAppMsg, 0, "Error: CreateView - pane already exists for row %d, col %d.\n", row, col);
		ASSERT(FALSE);
		return FALSE;
	}
#endif

	// set the initial size for that pane
	m_pColInfo[col].nIdealSize = sizeInit.cx;
	m_pRowInfo[row].nIdealSize = sizeInit.cy;

	BOOL bSendInitialUpdate = FALSE;

	CCreateContext contextT;
	if(pContext==nullptr)
	{
		// if no context specified, generate one from the currently selected
		//  client if possible
		CView* pOldView = (CView*)GetActivePane();
		if(pOldView!=nullptr&&pOldView->IsKindOf(RUNTIME_CLASS(CView)))
		{
			// set info about last pane
			ASSERT(contextT.m_pCurrentFrame==nullptr);
			contextT.m_pLastView = pOldView;
			contextT.m_pCurrentDoc = pOldView->GetDocument();
			if(contextT.m_pCurrentDoc!=nullptr)
				contextT.m_pNewDocTemplate = contextT.m_pCurrentDoc->GetDocTemplate();
		}
		pContext = &contextT;
		bSendInitialUpdate = TRUE;
	}

	CWnd* pWnd;
	try
	{
		pWnd = (CWnd*)((GRuntimeClass*)pViewClass)->CreateObject(*pContext->m_pCurrentDoc);
		if(pWnd==nullptr)
			AfxThrowMemoryException();
	}
	catch(CException* ex)
	{
		TRACE(traceAppMsg, 0, "Out of memory creating a splitter pane.\n");
		ex->Delete();
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}

	ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd==nullptr); // not yet created
	const DWORD dwStyle = AFX_WS_DEFAULT_VIEW&~WS_BORDER;

	// Create with the right size (wrong position)
	const CRect rect(CPoint(0, 0), sizeInit);
	if(!pWnd->Create(nullptr, nullptr, dwStyle, rect, this, IdFromRowCol(row, col), pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: couldn't create client pane for splitter.\n");
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}
	ASSERT((int)::GetDlgCtrlID(pWnd->m_hWnd)==IdFromRowCol(row, col));

	// send initial notification message
	if(bSendInitialUpdate)
		pWnd->SendMessage(WM_INITIALUPDATE);

	m_panwnds(row, col) = pWnd;
	return TRUE;
}

void GSplitterWnd::ShowColumn(int col)
{
	ASSERT_VALID(this);
	ASSERT(m_nCols<m_nMaxCols);
	const auto col_pos = std::find(m_shown_cols.begin(), m_shown_cols.end(), col);
	if(col_pos!=m_shown_cols.end())
	{
		ASSERT(0);
		return;
	}
	const int place = AbsToRelPosition(m_shown_cols, col);

	m_nCols++; // add a column

	m_shown_cols.push_back(col);
	m_shown_cols.sort();
	m_hiden_cols.remove(col);

	RenumeratePanes();

	RestoreColInfo(place, col);
	RecalcLayout();
}

int GSplitterWnd::AbsToRelPosition(std::list<int>& list, int value) const
{
	unsigned int index = -1;
	const auto pos = std::find(list.begin(), list.end(), value);
	if(pos!=list.end())
	{
		index = std::distance(list.begin(), pos);
	}
	else
	{
		index = list.size();
	}

	return index;
}

void GSplitterWnd::RemoveColInfo(int place, int col) const
{
	ASSERT(m_nCols<=m_nMaxCols);

	m_pSavedColInfo[col] = m_pColInfo[place];

	for(int i = place; i<m_nCols; i++)
	{
		m_pColInfo[i] = m_pColInfo[i+1];
	}
}

void GSplitterWnd::RestoreColInfo(int place, int col) const
{
	ASSERT(m_nCols<=m_nMaxCols);

	for(int i = m_nCols-1; i>=place+1; i--)
	{
		m_pColInfo[i] = m_pColInfo[i-1];
	}

	m_pColInfo[place] = m_pSavedColInfo[col];
}

void GSplitterWnd::HideColumn(int colHide)
{
	ASSERT_VALID(this);
	ASSERT(m_nCols>1);
	ASSERT(colHide<m_nMaxCols);
	const auto col_pos = std::find(m_shown_cols.begin(), m_shown_cols.end(), colHide);
	if(col_pos==m_shown_cols.end())
	{
		ASSERT(0);
		return;
	}
	const int place = std::distance(m_shown_cols.begin(), col_pos);

	// if the column has an active window -- change it
	int rowActive, colActive;
	if(GetActivePane(&rowActive, &colActive)!=nullptr&&
		colActive==place)
	{
		if(++colActive>=m_nCols)
			colActive = 0;
		SetActivePane(rowActive, colActive);
	}

	m_shown_cols.remove(colHide);
	m_hiden_cols.push_back(colHide);

	m_nCols--;

	RenumeratePanes();

	RemoveColInfo(place, colHide);

	RecalcLayout();
}

CPoint GSplitterWnd::RelToAbsPosition(int row, int col)
{
	CPoint pos;
	pos.x = RelToAbsPosition(m_shown_rows, m_hiden_rows, row);
	pos.y = RelToAbsPosition(m_shown_cols, m_hiden_cols, col);
	return pos;
}

int GSplitterWnd::RelToAbsPosition(std::list<int>& vis_list, std::list<int>& hid_list, int cur_index)
{
	unsigned int org_index;
	if(cur_index<vis_list.size())
	{
		auto it = vis_list.begin();
		for(int i = 0; i<cur_index; i++)
		{
			it++;
		}

		org_index = *it;
	}
	else
	{
		auto it = hid_list.begin();
		for(unsigned int i = 0; i<cur_index-vis_list.size(); i++)
		{
			it++;
		}

		org_index = *it;
	}

	return org_index;
}

BOOL GSplitterWnd::IsPaneVisible(int row, int col)
{
	ASSERT(m_nRows<=m_nMaxCols);
	const BOOL bRow = m_shown_rows.end()!=std::find(m_shown_rows.begin(), m_shown_rows.end(), row);
	ASSERT(m_nCols<=m_nMaxCols);
	const BOOL bCol = m_shown_cols.end()!=std::find(m_shown_cols.begin(), m_shown_cols.end(), col);

	return bRow&&bCol;
}

void GSplitterWnd::RenumeratePanes()
{
	int i, j, id;

	for(i = 0; i<m_nMaxRows; i++)
	{
		for(j = 0; j<m_nMaxCols; j++)
		{
			const CPoint pos = RelToAbsPosition(i, j);
			CWnd* pPane = (CWnd*)m_panwnds(pos.x, pos.y);
			ASSERT(pPane!=nullptr);

			id = AFX_IDW_PANE_FIRST+i*16+j;
			const int r = pPane->SetDlgCtrlID(id);
			ASSERT(r);

			if(IsPaneVisible(pos.x, pos.y))
				pPane->ShowWindow(SW_SHOW);
			else
				pPane->ShowWindow(SW_HIDE);
		}
	}
}

void GSplitterWnd::HideRow(int rowHide)
{
	ASSERT_VALID(this);
	ASSERT(m_nRows>1);
	ASSERT(rowHide<m_nMaxCols);
	const auto pos = std::find(m_shown_rows.begin(), m_shown_rows.end(), rowHide);
	if(pos==m_shown_rows.end())
	{
		ASSERT(0);
		return;
	}
	const int place = std::distance(m_shown_rows.begin(), pos);

	// if the column has an active window -- change it
	int rowActive, colActive;
	if(GetActivePane(&rowActive, &colActive)!=nullptr&&
		rowActive==place)
	{
		if(++rowActive>=m_nRows)
			rowActive = 0;
		SetActivePane(rowActive, colActive);
	}

	m_shown_rows.remove(rowHide);
	m_hiden_rows.push_back(rowHide);

	m_nRows--;

	RenumeratePanes();

	RemoveRowInfo(place, rowHide);

	RecalcLayout();
}

void GSplitterWnd::RemoveRowInfo(int place, int col) const
{
	ASSERT(m_nRows<=m_nMaxRows);

	m_pSavedRowInfo[col] = m_pRowInfo[place];
	for(int i = place; i<m_nRows; i++)
	{
		m_pRowInfo[i] = m_pRowInfo[i+1];
	}
}

void GSplitterWnd::ShowRow(int row)
{
	ASSERT_VALID(this);
	ASSERT(m_nRows<m_nMaxRows);
	const auto pos = std::find(m_shown_rows.begin(), m_shown_rows.end(), row);
	if(pos!=m_shown_rows.end())
	{
		ASSERT(0);
		return;
	}
	const int place = AbsToRelPosition(m_shown_rows, row);

	m_nRows++; // add a column

	m_shown_rows.push_back(row);
	m_shown_rows.sort();
	m_hiden_rows.remove(row);

	RenumeratePanes();

	RestoreRowInfo(place, row);
	RecalcLayout();
}

void GSplitterWnd::RestoreRowInfo(int place, int col) const
{
	ASSERT(m_nRows<=m_nMaxRows);

	for(int i = m_nRows-1; i>=place+1; i--)
	{
		m_pRowInfo[i] = m_pRowInfo[i-1];
	}

	m_pRowInfo[place] = m_pSavedRowInfo[col];
}
