#include "stdafx.h"
#include "Grid.h"

#include <algorithm>

// OLE stuff for clipboard operations
#include <afxadv.h>            // For CSharedFile
#include <afxconv.h>           // For LPTSTR -> LPSTR macros

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGridCtrl::CGridCtrl(int nRows, int nCols, int nFixedRows, int nFixedCols)
{
    // Store the system colours in case they change. The gridctrl uses
    // these colours, and in OnSysColorChange we can check to see if
    // the gridctrl colours have been changed from the system colours.
    // If they have, then leave them, otherwise change them to reflect
    // the new system colours.
    m_crWindowText        = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour      = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace            = ::GetSysColor(COLOR_3DFACE);
    m_crShadow            = ::GetSysColor(COLOR_3DSHADOW);
    m_crGridLineColour    = RGB(192,192,192);

    m_nRows               = 0;
    m_nCols               = 0;
    m_nFixedRows          = 0;
    m_nFixedCols          = 0;

    m_nVScrollMax         = 0;          // Scroll position
    m_nHScrollMax         = 0;
                                                   // per mouse wheel notch to scroll
    m_nBarState           = GVL_NONE;
    m_MouseMode           = MOUSE_NOTHING;
    m_nGridLines          = GVL_BOTH;


    m_bSingleRowSelection = FALSE;
    m_bSingleColSelection = FALSE;
    m_bLMouseButtonDown   = FALSE;
    m_bRMouseButtonDown   = FALSE;
    m_bAllowDraw          = TRUE;       // allow draw updates
    m_bEnableSelection    = TRUE;
    m_bFixedColumnSelection = TRUE;
    m_bFixedRowSelection  = TRUE;
    m_bTitleTips          = TRUE;       // show cell title tips



    m_nAutoSizeColumnStyle = GVS_BOTH;  // Autosize grid using header and data info

    m_pImageList          = nullptr;       // Images in the grid
    m_bTrackFocusCell     = TRUE;       // Track Focus cell?
    m_bFrameFocus         = TRUE;       // Frame the selected cell?

    m_pRtcDefault = RUNTIME_CLASS(CGridCell);

    SetupDefaultCells();
    SetGridBkColor(m_crShadow);

    // Set up the initial grid size
    SetRowCount(nRows);
    SetColumnCount(nCols);
    SetFixedRowCount(nFixedRows);
    SetFixedColumnCount(nFixedCols);

    SetTitleTipTextClr(CLR_DEFAULT);  //FNA
    SetTitleTipBackClr(CLR_DEFAULT); 
}

CGridCtrl::~CGridCtrl()
{
}
// creates the control - use like any other window create control
BOOL CGridCtrl::Create()
{
    try
    {
        m_arRowHeights.SetSize(m_nRows);    // initialize row heights
        m_arColWidths.SetSize(m_nCols);     // initialize column widths
    }
    catch(CMemoryException* ex)
    {
        ex->ReportError();
        ex->Delete();
        return FALSE;
    }
	catch(CException* ex)
	{
		ex->Delete();
	}

    int i;
    for (i = 0; i < m_nRows; i++)
        m_arRowHeights[i] = m_cellDefault.GetHeight();
    for (i = 0; i < m_nCols; i++)
        m_arColWidths[i] = m_cellDefault.GetWidth();

    return TRUE;
}




void CGridCtrl::SetupDefaultCells()
{
    m_cellDefault.SetGrid(this);            // Normal editable cell
    m_cellFixedColDef.SetGrid(this);        // Cell for fixed columns
    m_cellFixedRowDef.SetGrid(this);        // Cell for fixed rows
    m_cellFixedRowColDef.SetGrid(this);     // Cell for area overlapped by fixed columns/rows

    m_cellDefault.SetTextClr(m_crWindowText);   
    m_cellDefault.SetBackClr(m_crWindowColour); 
    m_cellFixedColDef.SetTextClr(m_crWindowText);
    m_cellFixedColDef.SetBackClr(m_cr3DFace);
    m_cellFixedRowDef.SetTextClr(m_crWindowText);
    m_cellFixedRowDef.SetBackClr(m_cr3DFace);
    m_cellFixedRowColDef.SetTextClr(m_crWindowText);
    m_cellFixedRowColDef.SetBackClr(m_cr3DFace);
}
void CGridCtrl::OnDraw(CDC* pDC)
{
    if (!m_bAllowDraw)
        return;

    CRect clipper;
    if (pDC->GetClipBox(&clipper) == ERROR)
        return;

    CRect rect;
    int row, col;
    CGridCellBase* pCell;

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth  = GetFixedColumnWidth();

    CCellID idTopLeft = GetTopleftNonFixedCell();
    int minVisibleRow = idTopLeft.row,
        minVisibleCol = idTopLeft.col;

    CRect VisRect;
    CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
    int maxVisibleRow = VisCellRange.GetMaxRow(),
        maxVisibleCol = VisCellRange.GetMaxCol();

    CPen pen;
    pen.CreatePen(PS_SOLID, 0, m_crGridLineColour);
    pDC->SelectObject(&pen);

    // draw vertical lines (drawn at ends of cells)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
    {
        int x = nFixedColWidth;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            x += GetColumnWidth(col);
            pDC->MoveTo(x-1, nFixedRowHeight);
            pDC->LineTo(x-1, VisRect.bottom);
        }
    }

    // draw horizontal lines (drawn at bottom of each cell)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
    {
        int y = nFixedRowHeight;
        for (row = minVisibleRow; row <= maxVisibleRow; row++)
        {
            if (GetRowHeight(row) <= 0) continue;

            y += GetRowHeight(row);
            pDC->MoveTo(nFixedColWidth, y-1);
            pDC->LineTo(VisRect.right,  y-1);
        }
    }

    pDC->SelectStockObject(NULL_PEN);

    // draw rest of non-fixed cells
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipper.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipper.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipper.right)
                break;        // gone past cliprect
            if (rect.right < clipper.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
            // TRACE(_T("Cell %d,%d type: %s\n"), row, col, pCell->GetRuntimeClass()->m_lpszClassName);
            if (pCell)
			{
				//Used for merge cells
				//by Huang Wei

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, rect, FALSE);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
						if(GetCellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
							pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, TRUE);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

					if(GetCellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, mergerect, TRUE);
					}
				}
			}
        }
    }

    // draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipper.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipper.top)
            continue;             // Reached cliprect yet?

        rect.right = -1;
        for (col = 0; col < m_nFixedCols; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipper.right)
                break;            // gone past cliprect
            if (rect.right < clipper.left)
                continue;         // Reached cliprect yet?

            pCell = GetCell(row, col);
   //         if (pCell)
			//{
			//	pCell->SetCoords(row,col);
   //             pCell->Draw(pDC, row, col, rect, TRUE);
			//}
            if (pCell)
			{
				//Used for merge cells
				//by Huang Wei

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, rect, FALSE);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
						if(GetCellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
							pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, TRUE);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

					if(GetCellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, mergerect, TRUE);
					}
				}
			}
        }
    }

    // draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipper.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipper.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipper.right)
                break;        // gone past cliprect
            if (rect.right < clipper.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
   //         if (pCell)
			//{
			//	pCell->SetCoords(row,col);
   //             pCell->Draw(pDC, row, col, rect, TRUE);
			//}
            if (pCell)
			{
				//Used for merge cells
				//by Huang Wei

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, rect, FALSE);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
						if(GetCellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
							pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, TRUE);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

					if(GetCellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, mergerect, TRUE);
					}
				}
			}
        }
    }
/*
    // draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;
        rect.right = -1;

        for (col = 0; col < m_nFixedCols; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            pCell = GetCell(row, col);
            if (pCell)
			{
				pCell->SetCoords(row,col);
                pCell->Draw(pDC, row, col, rect, FALSE);
			}
        }
    }
*/
	// draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
	rect.bottom = -1;
	for (row = 0; row < m_nFixedRows; row++)
	{
		if (GetRowHeight(row) <= 0) continue;

		rect.top = rect.bottom+1;
		rect.bottom = rect.top + GetRowHeight(row)-1;
		rect.right = -1;

		for (col = 0; col < m_nFixedCols; col++)
		{
			if (GetColumnWidth(col) <= 0) continue;

			rect.left = rect.right+1;
			rect.right = rect.left + GetColumnWidth(col)-1;

			pCell = GetCell(row, col);
			/* if (pCell)
			{
			pCell->SetCoords(row,col);
			pCell->Draw(pDC, row, col, rect, FALSE);
			}*/
			if (pCell)
			{
				//Used for merge cells by Huang Wei
				//bugfix by Luther Bruck

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, rect, FALSE);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
						if(GetCellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
							pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, TRUE);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

					if(GetCellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
						pCell->Draw(pDC, row, col, mergerect, TRUE);
					}
				}
			}
		}
	}
}
// Is a given cell designation valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridCtrl::IsValid(int nRow, int nCol) const
{
    return (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols);
}

BOOL CGridCtrl::IsValid(const CCellID& cell) const
{
    return IsValid(cell.row, cell.col);
}

// Is a given cell range valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridCtrl::IsValid(const CCellRange& range) const
{
    return (range.GetMinRow() >= 0 && range.GetMinCol() >= 0 &&
        range.GetMaxRow() >= 0 && range.GetMaxCol() >= 0 &&
        range.GetMaxRow() < m_nRows && range.GetMaxCol() < m_nCols &&
        range.GetMinRow() <= range.GetMaxRow() && range.GetMinCol() <= range.GetMaxCol());
}
// Forces a redraw of a cell immediately (using a direct DC construction,
// or the supplied dc)
BOOL CGridCtrl::RedrawCell(const CCellID& cell, CDC* pDC /* = nullptr */)
{
    return RedrawCell(cell.row, cell.col, pDC);
}

BOOL CGridCtrl::RedrawCell(int nRow, int nCol, CDC* pDC /* = nullptr */)
{
    BOOL bResult = TRUE;
    BOOL bMustReleaseDC = FALSE;

    if (!m_bAllowDraw || !IsCellVisible(nRow, nCol))
        return FALSE;

    CRect rect;
    if (!GetCellRect(nRow, nCol, rect))
        return FALSE;

    if (pDC)
    {
        // Redraw cells directly
        if (nRow < m_nFixedRows || nCol < m_nFixedCols)
        {
            CGridCellBase* pCell = GetCell(nRow, nCol);
            if (pCell)
                bResult = pCell->Draw(pDC, nRow, nCol, rect, TRUE);
        }
        else
        {
            CGridCellBase* pCell = GetCell(nRow, nCol);
            if (pCell)
                bResult = pCell->Draw(pDC, nRow, nCol, rect, TRUE);

            // Since we have erased the background, we will need to redraw the gridlines
            CPen pen;
            pen.CreatePen(PS_SOLID, 0, m_crGridLineColour);

            CPen* pOldPen = (CPen*) pDC->SelectObject(&pen);
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                pDC->MoveTo(rect.left,    rect.bottom);
                pDC->LineTo(rect.right + 1, rect.bottom);
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                pDC->MoveTo(rect.right, rect.top);
                pDC->LineTo(rect.right, rect.bottom + 1);
            }
            pDC->SelectObject(pOldPen);
        }
    } else
        InvalidateRect(rect, TRUE);     // Could not get a DC - invalidate it anyway
    // and hope that OnPaint manages to get one

    return bResult;
}
// Sets the currently selected cell, returning the previous current cell
CCellID CGridCtrl::SetFocusCell(int nRow, int nCol)
{
    return SetFocusCell(CCellID(nRow, nCol));
}

CCellID CGridCtrl::SetFocusCell(CCellID cell)
{
    if (cell == m_idCurrentCell)
        return m_idCurrentCell;

    CCellID idPrev = m_idCurrentCell;

    // EFW - Bug Fix - Force focus to be in a non-fixed cell
    if (cell.row != -1 && cell.row < GetFixedRowCount())
        cell.row = GetFixedRowCount();
    if (cell.col != -1 && cell.col < GetFixedColumnCount())
        cell.col = GetFixedColumnCount();

    m_idCurrentCell = cell;

    if (IsValid(idPrev))
    {
        SetItemState(idPrev.row, idPrev.col,
            GetItemState(idPrev.row, idPrev.col) & ~GVIS_FOCUSED);
        RedrawCell(idPrev); // comment to reduce flicker

        if (GetTrackFocusCell() && idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++)
                RedrawCell(row, idPrev.col);
        if (GetTrackFocusCell() && idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++)
                RedrawCell(idPrev.row, col);
    }

    if (IsValid(m_idCurrentCell))
    {
        SetItemState(m_idCurrentCell.row, m_idCurrentCell.col,
            GetItemState(m_idCurrentCell.row, m_idCurrentCell.col) | GVIS_FOCUSED);

        RedrawCell(m_idCurrentCell); // comment to reduce flicker

        if (GetTrackFocusCell() && idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++)
                RedrawCell(row, m_idCurrentCell.col);
        if (GetTrackFocusCell() && idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++)
                RedrawCell(m_idCurrentCell.row, col);

        // EFW - New addition.  If in list mode, make sure the selected
        // row highlight follows the cursor.
        // Removed by C Maunder 27 May
        //if (m_bListMode)
        //{
        //    m_PrevSelectedCellMap.RemoveAll();
        //    m_MouseMode = MOUSE_SELECT_ROW;
        //    OnSelecting(m_idCurrentCell);

            // Leave this off so that you can still drag the highlight around
            // without selecting rows.
            // m_MouseMode = MOUSE_NOTHING;
        //}

	}

    return idPrev;
}
// Gets the first non-fixed cell ID
CCellID CGridCtrl::GetTopleftNonFixedCell(BOOL bForceRecalculation /*=FALSE*/)
{
    // Used cached value if possible
    if (m_idTopLeftCell.IsValid() && !bForceRecalculation)
        return m_idTopLeftCell;

    int nVertScroll = GetScrollPos(SB_VERT), 
        nHorzScroll = GetScrollPos(SB_HORZ);

    m_idTopLeftCell.col = m_nFixedCols;
    int nRight = 0;
    while (nRight < nHorzScroll && m_idTopLeftCell.col < (GetColumnCount()-1))
        nRight += GetColumnWidth(m_idTopLeftCell.col++);

    m_idTopLeftCell.row = m_nFixedRows;
    int nTop = 0;
    while (nTop < nVertScroll && m_idTopLeftCell.row < (GetRowCount()-1))
        nTop += GetRowHeight(m_idTopLeftCell.row++);

    //TRACE2("TopLeft cell is row %d, col %d\n",m_idTopLeftCell.row, m_idTopLeftCell.col);
    return m_idTopLeftCell;
}
// This gets even partially visible cells
CCellRange CGridCtrl::GetVisibleNonFixedCellRange(LPRECT pRect /*=nullptr*/, 
                                                  BOOL bForceRecalculation /*=FALSE*/)
{
    int i;
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
        {
            bottom = rect.bottom;
            break;
        }
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int right = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
        {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}


CCellRange CGridCtrl::GetVisibleFixedCellRange(LPRECT pRect /*=nullptr*/, 
                                                  BOOL bForceRecalculation /*=FALSE*/)
{
    int i;
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
        {
            bottom = rect.bottom;
            break;
        }
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int right = 0;
    for (i = 0; i < GetFixedColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
        {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, 0, maxVisibleRow, maxVisibleCol);
}
// Resets the selected cell range to the empty set.
void CGridCtrl::ResetSelectedRange()
{
    m_PrevSelectedCellMap.RemoveAll();
//    SetSelectedRange(-1,-1,-1,-1);
    SetFocusCell(-1,-1);
}
////////////////////////////////////////////////////////////////////////////////////
// Row/Column position functions

// returns the top left point of the cell. Returns FALSE if cell not visible.
// consider cell's merge
BOOL CGridCtrl::GetCellOrigin(int nRow, int nCol, LPPOINT p)
{
    int i;

    if (!IsValid(nRow, nCol))
        return FALSE;

    CCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows)
        idTopLeft = GetTopleftNonFixedCell();

	//Merge the selected cells 
	//by Huang Wei

    //if ((nRow >= m_nFixedRows && nRow < idTopLeft.row) ||
    //    (nCol>= m_nFixedCols && nCol < idTopLeft.col))
    //    return FALSE;

    p->x = 0;
    if (nCol < m_nFixedCols)                      // is a fixed column
        for (i = 0; i < nCol; i++)
            p->x += GetColumnWidth(i);
        else 
        {                                        // is a scrollable data column
            for (i = 0; i < m_nFixedCols; i++)
                p->x += GetColumnWidth(i);
			//Merge the selected cells 
			//by Huang Wei
			if(nCol>idTopLeft.col)
			{
				for (i = idTopLeft.col; i < nCol; i++)
					p->x += GetColumnWidth(i);
			}
			else
			{
				for (i = nCol; i <idTopLeft.col ; i++)
					p->x -= GetColumnWidth(i);
			}
        }
        
        p->y = 0;
        if (nRow < m_nFixedRows)                      // is a fixed row
            for (i = 0; i < nRow; i++)
                p->y += GetRowHeight(i);
            else 
            {                                        // is a scrollable data row
                for (i = 0; i < m_nFixedRows; i++)
                    p->y += GetRowHeight(i);
 				//Merge the selected cells 
				//by Huang Wei
				if(nRow>idTopLeft.row)
				{
					for (i = idTopLeft.row; i < nRow; i++)
						p->y += GetRowHeight(i);
				}
				else
				{
					for (i = nRow; i <idTopLeft.row; i++)
						p->y -= GetRowHeight(i);
				}
            }
            
            return TRUE;
}
// Returns the bounding box of the cell
BOOL CGridCtrl::GetCellRect(const CCellID& cell, LPRECT pRect)
{
    return GetCellRect(cell.row, cell.col, pRect);
}

BOOL CGridCtrl::GetCellRect(int nRow, int nCol, LPRECT pRect)
{
    CPoint CellOrigin;
    if (!GetCellOrigin(nRow, nCol, &CellOrigin))
        return FALSE;

    //Merge the selected cells 
    //by Huang Wei
    CGridCellBase *pCell = (CGridCellBase*) GetCell(nRow,nCol);
	
	if(!pCell->IsMerged())
	{
		pRect->left   = CellOrigin.x;
		pRect->top    = CellOrigin.y;
		pRect->right  = CellOrigin.x + GetColumnWidth(nCol)-1;
		pRect->bottom = CellOrigin.y + GetRowHeight(nRow)-1;
	}
	else
	{
		GetCellRangeRect(pCell->m_MergeRange,pRect);
	}
    //TRACE("Row %d, col %d: L %d, T %d, W %d, H %d:  %d,%d - %d,%d\n",
    //      nRow,nCol, CellOrigin.x, CellOrigin.y, GetColumnWidth(nCol), GetRowHeight(nRow),
    //      pRect->left, pRect->top, pRect->right, pRect->bottom);

    return TRUE;
}
// Returns the bounding box of a range of cells
BOOL CGridCtrl::GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect)
{
    CPoint MinOrigin,MaxOrigin;

    if (!GetCellOrigin(cellRange.GetMinRow(), cellRange.GetMinCol(), &MinOrigin))
        return FALSE;
    if (!GetCellOrigin(cellRange.GetMaxRow(), cellRange.GetMaxCol(), &MaxOrigin))
        return FALSE;

    lpRect->left   = MinOrigin.x;
    lpRect->top    = MinOrigin.y;
    lpRect->right  = MaxOrigin.x + GetColumnWidth(cellRange.GetMaxCol()) - 1;
    lpRect->bottom = MaxOrigin.y + GetRowHeight(cellRange.GetMaxRow()) - 1;

    return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////
// Row/Column count functions

BOOL CGridCtrl::SetFixedRowCount(int nFixedRows)
{
    if (m_nFixedRows == nFixedRows)
        return TRUE;

    ASSERT(nFixedRows >= 0);

    ResetSelectedRange();

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nFixedRows > GetRowCount())
        if (!SetRowCount(nFixedRows))
            return FALSE;
        
        if (m_idCurrentCell.row < nFixedRows)
            SetFocusCell(-1, - 1);
        
        {
            if (nFixedRows > m_nFixedRows)
            {
                for (int i = m_nFixedRows; i < nFixedRows; i++)
                    for (int j = 0; j < GetColumnCount(); j++)
                    {
                        SetItemState(i, j, GetItemState(i, j) | GVIS_FIXED | GVIS_FIXEDROW);
                        SetItemBkColour(i, j, CLR_DEFAULT );
                        SetItemFgColour(i, j, CLR_DEFAULT );
                    }
            }
            else
            {
                for (int i = nFixedRows; i < m_nFixedRows; i++)
                {
                    int j;
                    for (j = 0; j < GetFixedColumnCount(); j++)
                        SetItemState(i, j, GetItemState(i, j) & ~GVIS_FIXEDROW );

                    for (j = GetFixedColumnCount(); j < GetColumnCount(); j++)
                    {
                        SetItemState(i, j, GetItemState(i, j) & ~(GVIS_FIXED | GVIS_FIXEDROW) );
                        SetItemBkColour(i, j, CLR_DEFAULT );
                        SetItemFgColour(i, j, CLR_DEFAULT );
                    }
                }
            }
        }

        m_nFixedRows = nFixedRows;
        
        Refresh();
        
        return TRUE;
}

BOOL CGridCtrl::SetFixedColumnCount(int nFixedCols)
{
    if (m_nFixedCols == nFixedCols)
        return TRUE;

    ASSERT(nFixedCols >= 0);

    if (nFixedCols > GetColumnCount())
        if (!SetColumnCount(nFixedCols))
            return FALSE;

    if (m_idCurrentCell.col < nFixedCols)
        SetFocusCell(-1, - 1);

    ResetSelectedRange();

    // Force recalculation
    m_idTopLeftCell.col = -1;

    
    {
        if (nFixedCols > m_nFixedCols)
        {
            for (int i = 0; i < GetRowCount(); i++)
                for (int j = m_nFixedCols; j < nFixedCols; j++)
                {
                    SetItemState(i, j, GetItemState(i, j) | GVIS_FIXED | GVIS_FIXEDCOL);
                    SetItemBkColour(i, j, CLR_DEFAULT );
                    SetItemFgColour(i, j, CLR_DEFAULT );
                }
        }
        else
        {
			{ // Scope limit i,j
	            for (int i = 0; i < GetFixedRowCount(); i++)
		            for (int j = nFixedCols; j < m_nFixedCols; j++)
			            SetItemState(i, j, GetItemState(i, j) & ~GVIS_FIXEDCOL );
			}
			{// Scope limit i,j
	            for (int i = GetFixedRowCount(); i < GetRowCount(); i++)
		            for (int j = nFixedCols; j < m_nFixedCols; j++)
			        {
				        SetItemState(i, j, GetItemState(i, j) & ~(GVIS_FIXED | GVIS_FIXEDCOL) );
					    SetItemBkColour(i, j, CLR_DEFAULT );
						SetItemFgColour(i, j, CLR_DEFAULT );
	                }
			}
        }
    }
        
    m_nFixedCols = nFixedCols;
        
    Refresh();
        
    return TRUE;
}

BOOL CGridCtrl::SetRowCount(int nRows)
{
    BOOL bResult = TRUE;

    ASSERT(nRows >= 0);
    if (nRows == GetRowCount())
        return bResult;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nRows < m_nFixedRows)
        m_nFixedRows = nRows;

    if (m_idCurrentCell.row >= nRows)
        SetFocusCell(-1, - 1);

    int addedRows = nRows - GetRowCount();

    // If we are about to lose rows, then we need to delete the GridCell objects
    // in each column within each row
    if (addedRows < 0)
    {
        {
            for (int row = nRows; row < m_nRows; row++)
            {
                // Delete cells
                for (int col = 0; col < m_nCols; col++)
                    DestroyCell(row, col);
            
                // Delete rows
                GRID_ROW* pRow = m_RowData[row];
                delete pRow;
            }
        }
        m_nRows = nRows;
    }
    
    try
    {
        m_arRowHeights.SetSize(nRows);

        {
            // Change the number of rows.
            m_RowData.SetSize(nRows);

            // If we have just added rows, we need to construct new elements for each cell
            // and set the default row height
            if (addedRows > 0)
            {
                // initialize row heights and data
                int startRow = nRows - addedRows;
                for (int row = startRow; row < nRows; row++)
                {
                    m_arRowHeights[row] = m_cellDefault.GetHeight();

                    m_RowData[row] = new GRID_ROW;
                    m_RowData[row]->SetSize(m_nCols);
                    for (int col = 0; col < m_nCols; col++)
                    {
                        GRID_ROW* pRow = m_RowData[row];
                        if (pRow)
                            pRow->SetAt(col, CreateCell(row, col));
                    }
                    m_nRows++;
                }
            }
        }
    }
    catch(CMemoryException* ex)
    {
        ex->ReportError();
        ex->Delete();
        bResult = FALSE;
    }
	catch(CException* ex)
	{
		ex->Delete();
	}

    SetModified();
    ResetScrollBars();
    Refresh();

    return bResult;
}

BOOL CGridCtrl::SetColumnCount(int nCols)
{
    BOOL bResult = TRUE;

    ASSERT(nCols >= 0);

    if (nCols == GetColumnCount())
        return bResult;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nCols < m_nFixedCols)
        m_nFixedCols = nCols;

    if (m_idCurrentCell.col >= nCols)
        SetFocusCell(-1, - 1);

    int addedCols = nCols - GetColumnCount();

    // If we are about to lose columns, then we need to delete the GridCell objects
    // within each column
    if (addedCols < 0)
    {
        for (int row = 0; row < m_nRows; row++)
            for (int col = nCols; col < GetColumnCount(); col++)
                DestroyCell(row, col);
    }

    try 
    {
        // Change the number of columns.
        m_arColWidths.SetSize(nCols);
    
        // Change the number of columns in each row.
       
            for (int i = 0; i < m_nRows; i++)
                if (m_RowData[i])
                    m_RowData[i]->SetSize(nCols);
        
        // If we have just added columns, we need to construct new elements for each cell
        // and set the default column width
        if (addedCols > 0)
        {
            int row, col;

            // initialized column widths
            int startCol = nCols - addedCols;
            for (col = startCol; col < nCols; col++)
                m_arColWidths[col] = m_cellFixedColDef.GetWidth();
        
            // initialise column data
            {
                for (row = 0; row < m_nRows; row++)
                    for (col = startCol; col < nCols; col++)
                    {
                        GRID_ROW* pRow = m_RowData[row];
                        if (pRow)
                            pRow->SetAt(col, CreateCell(row, col));
                    }
            }
        }
        // else    // check for selected cell ranges
        //    ResetSelectedRange();
    }
    catch(CMemoryException* ex)
    {
        ex->ReportError();
        ex->Delete();
        bResult = FALSE;
    }
	catch(CException* ex)
	{
		ex->Delete();
	}
	m_arColOrder.resize(nCols);  // Reset Column Order
    for (int i = 0; i < nCols; i++)
	{
		m_arColOrder[i] = i;	
	}

    m_nCols = nCols;

    SetModified();
    ResetScrollBars();
    Refresh();

    return bResult;
}
// Creates a new grid cell and performs any necessary initialisation
/*virtual*/ CGridCellBase* CGridCtrl::CreateCell(int nRow, int nCol)
{
    if (!m_pRtcDefault || !m_pRtcDefault->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return nullptr;
    }
    CGridCellBase* pCell = (CGridCellBase*) m_pRtcDefault->CreateObject();
    if (!pCell)
        return nullptr;

    pCell->SetGrid(this);
    pCell->SetCoords(nRow, nCol); 

    if (nCol < m_nFixedCols)
        pCell->SetState(pCell->GetState() | GVIS_FIXED | GVIS_FIXEDCOL);
    if (nRow < m_nFixedRows)
        pCell->SetState(pCell->GetState() | GVIS_FIXED | GVIS_FIXEDROW);
    
    pCell->SetFormat(pCell->GetDefaultCell()->GetFormat());

    return pCell;
}

// Performs any cell cleanup necessary to maintain grid integrity
/*virtual*/ void CGridCtrl::DestroyCell(int nRow, int nCol)
{
    // Set the cells state to 0. If the cell is selected, this
    // will remove the cell from the selected list.
    SetItemState(nRow, nCol, 0);

    delete GetCell(nRow, nCol);
}
BOOL CGridCtrl::SetItemState(int nRow, int nCol, UINT state)
{
    BOOL bSelected = IsCellSelected(nRow, nCol);

    // If the cell is being unselected, remove it from the selected list
    if (bSelected && !(state & GVIS_SELECTED))
    {
        CCellID cell;
        DWORD key = MAKELONG(nRow, nCol);

        if (m_SelectedCellMap.Lookup(key, (CCellID&)cell))
            m_SelectedCellMap.RemoveKey(key);
    }

    // If cell is being selected, add it to the list of selected cells
    else if (!bSelected && (state & GVIS_SELECTED))
    {
        CCellID cell(nRow, nCol);
        m_SelectedCellMap.SetAt(MAKELONG(nRow, nCol), cell);
    }

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    // Set the cell's state
    pCell->SetState(state);

    return TRUE;
}

UINT CGridCtrl::GetItemState(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->GetState();
}
int CGridCtrl::GetRowHeight(int nRow) const
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return -1;

    return m_arRowHeights[nRow];
}

int CGridCtrl::GetColumnWidth(int nCol) const
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return -1;

    return m_arColWidths[m_arColOrder[nCol]];
}
int CGridCtrl::GetFixedRowHeight() const
{
    int nHeight = 0;
    for (int i = 0; i < m_nFixedRows; i++)
        nHeight += GetRowHeight(i);

    return nHeight;
}

int CGridCtrl::GetFixedColumnWidth() const
{
    int nWidth = 0;
    for (int i = 0; i < m_nFixedCols; i++)
        nWidth += GetColumnWidth(i);

    return nWidth;
}
void CGridCtrl::SetModified(BOOL bModified /*=TRUE*/, int nRow /*=-1*/, int nCol /*=-1*/)
{
    if (nRow >= 0 && nCol >= 0)
    {
        if (bModified)
        {
            SetItemState(nRow, nCol, GetItemState(nRow, nCol) | GVIS_MODIFIED);
            m_bModified = TRUE;
        }
        else
            SetItemState(nRow, nCol, GetItemState(nRow, nCol) & ~GVIS_MODIFIED);
    }
    else
        m_bModified = bModified;

    if (!m_bModified)
    {
        for (int row = 0; row < GetRowCount(); row++)
            for (int col = 0; col < GetColumnCount(); col++)
                SetItemState(row, col, GetItemState(row, col) & ~GVIS_MODIFIED);
    }
}

BOOL CGridCtrl::GetModified(int nRow /*=-1*/, int nCol /*=-1*/)
{
    if (nRow >= 0 && nCol >= 0)
        return ( (GetItemState(nRow, nCol) & GVIS_MODIFIED) == GVIS_MODIFIED );
    else
        return m_bModified;
}
BOOL CGridCtrl::IsCellSelected(CCellID &cell) const
{
    return IsCellSelected(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellSelected(int nRow, int nCol) const
{

        return IsSelectable() && ((GetItemState(nRow, nCol) & GVIS_SELECTED) == GVIS_SELECTED);
}
BOOL CGridCtrl::IsCellVisible(CCellID cell) 
{
    return IsCellVisible(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellVisible(int nRow, int nCol)
{
    if (!IsWindow(m_hWnd))
        return FALSE;

    int x, y;

    CCellID TopLeft;
    if (nCol >= GetFixedColumnCount() || nRow >= GetFixedRowCount())
    {
        TopLeft = GetTopleftNonFixedCell();
        if (nCol >= GetFixedColumnCount() && nCol < TopLeft.col)
            return FALSE;
        if (nRow >= GetFixedRowCount() && nRow < TopLeft.row)
            return FALSE;
    }
    
    CRect rect;
    GetClientRect(rect);
    if (nCol < GetFixedColumnCount())
    {
        x = 0;
        for (int i = 0; i <= nCol; i++) 
        {
            if (x >= rect.right)
                return FALSE;
            x += GetColumnWidth(i);    
        }
    } 
    else 
    {
        x = GetFixedColumnWidth();
        for (int i = TopLeft.col; i <= nCol; i++) 
        {
            if (x >= rect.right)
                return FALSE;
            x += GetColumnWidth(i);    
        }
    }
    
    if (nRow < GetFixedRowCount())
    {
        y = 0;
        for (int i = 0; i <= nRow; i++) 
        {
            if (y >= rect.bottom)
                return FALSE;
            y += GetRowHeight(i);    
        }
    } 
    else 
    {
        if (nRow < TopLeft.row)
            return FALSE;
        y = GetFixedRowHeight();
        for (int i = TopLeft.row; i <= nRow; i++) 
        {
            if (y >= rect.bottom)
                return FALSE;
            y += GetRowHeight(i);    
        }
    }
    
    return TRUE;
}
//Merge the selected cells 
//by Huang Wei
CGridCellBase* CGridCtrl::GetCell(CCellID cell)
{
	return GetCell(cell.row,cell.col);
}
