
#pragma once

#include "..\GridCtrl\CellRange.h"
#include "../Geometry/Widget.h"
#include "..\GridCtrl\GridCellBase.h"
#include "..\GridCtrl\GridCell.h"

#include <afxtempl.h>
#include <vector>

// storage typedef for each row in the grid
typedef CTypedPtrArray<CObArray, CGridCellBase*> GRID_ROW;
// Autosizing option
#define GVS_DEFAULT             0
#define GVS_HEADER              1       // Size using column fixed cells data only
#define GVS_DATA                2       // Size using column non-fixed cells data only
#define GVS_BOTH                3       // Size using column fixed and non-fixed

class CGridCtrl : public CWidget
{
// Construction
public:
    CGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);

    BOOL Create();

///////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    int  GetRowCount() const                    { return m_nRows; }
    int  GetColumnCount() const                 { return m_nCols; }
    int  GetFixedRowCount() const               { return m_nFixedRows; }
    int  GetFixedColumnCount() const            { return m_nFixedCols; }
    BOOL SetRowCount(int nRows = 10);
    BOOL SetColumnCount(int nCols = 10);
    BOOL SetFixedRowCount(int nFixedRows = 1);
    BOOL SetFixedColumnCount(int nFixedCols = 1);
    int  GetRowHeight(int nRow) const;
    BOOL SetRowHeight(int row, int height);
    int  GetColumnWidth(int nCol) const;
    BOOL SetColumnWidth(int col, int width);

    BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p);
    BOOL GetCellRect(int nRow, int nCol, LPRECT pRect);
    BOOL GetCellRect(const CCellID& cell, LPRECT pRect);
    int  GetFixedRowHeight() const;
    int  GetFixedColumnWidth() const;
    void     SetGridBkColor(COLORREF clr)         { m_crGridBkColour = clr;           }
    COLORREF GetGridBkColor() const               { return m_crGridBkColour;          }
    void     SetGridLineColor(COLORREF clr)       { m_crGridLineColour = clr;         }
    COLORREF GetGridLineColor() const             { return m_crGridLineColour;        }
	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT) { m_crTTipBackClr = clr;  }
	COLORREF GetTitleTipBackClr()				            { return m_crTTipBackClr; }
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT) { m_crTTipTextClr = clr;  }
	COLORREF GetTitleTipTextClr()				            { return m_crTTipTextClr; }
    CCellID SetFocusCell(CCellID cell);
    CCellID SetFocusCell(int nRow, int nCol);
    CCellID GetFocusCell() const                  { return m_idCurrentCell;           }
    void SetImageList(CImageList* pList)          { m_pImageList = pList;             }
    void SetSingleRowSelection(BOOL bSing = TRUE) { m_bSingleRowSelection = bSing;    }
    BOOL GetSingleRowSelection()                  { return m_bSingleRowSelection; }
    void SetSingleColSelection(BOOL bSing = TRUE) { m_bSingleColSelection = bSing;    }
    BOOL GetSingleColSelection()                  { return m_bSingleColSelection;     }
    BOOL IsSelectable() const                     { return m_bEnableSelection;        }
    void EnableTitleTips(BOOL bEnable = TRUE)     { m_bTitleTips = bEnable;           }
    BOOL GetTitleTips()                           { return m_bTitleTips;              }
    BOOL GetTrackFocusCell()                      { return m_bTrackFocusCell;         }
    void SetAutoSizeStyle(int nStyle = GVS_BOTH)  { m_nAutoSizeColumnStyle = nStyle;  }
    int  GetAutoSizeStyle()                       { return m_nAutoSizeColumnStyle; }
///////////////////////////////////////////////////////////////////////////////////
// Grid cell Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    CGridCellBase* GetCell(int nRow, int nCol) const;   // Get the actual cell!
    void SetModified(BOOL bModified = TRUE, int nRow = -1, int nCol = -1);
    BOOL GetModified(int nRow = -1, int nCol = -1);
    BOOL   SetItemState(int nRow, int nCol, UINT state);
    UINT   GetItemState(int nRow, int nCol) const;
    BOOL   SetItemBkColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    BOOL   SetItemFgColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    void Refresh();
    BOOL IsCellVisible(int nRow, int nCol);
    BOOL IsCellVisible(CCellID cell);
    BOOL IsCellSelected(int nRow, int nCol) const;
    BOOL IsCellSelected(CCellID &cell) const;
    BOOL RedrawCell(int nRow, int nCol, CDC* pDC = nullptr);
    BOOL RedrawCell(const CCellID& cell, CDC* pDC = nullptr);
    BOOL IsValid(int nRow, int nCol) const;
    BOOL IsValid(const CCellID& cell) const;
    BOOL IsValid(const CCellRange& range) const;
// Implementation
public:
	//Merge the selected cells 
	//by Huang Wei
	CGridCellBase* GetCell(CCellID cell);
    virtual ~CGridCtrl();
    void SetupDefaultCells();
    BOOL GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect);
    int  SetMouseMode(int nMode) { int nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode; }
    int  GetMouseMode() const    { return m_MouseMode; }
    CCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE);
    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = nullptr, BOOL bForceRecalculation = FALSE);
    CCellRange GetVisibleFixedCellRange(LPRECT pRect = nullptr, BOOL bForceRecalculation = FALSE);
    void ResetSelectedRange();
    virtual void  OnDraw(CDC* pDC);
    // CGridCellBase Creation and Cleanup
    virtual CGridCellBase* CreateCell(int nRow, override;
    virtual void DestroyCell(int nRow, int nCol) override;

// Attributes
protected:
    // General attributes
    COLORREF    m_crFixedTextColour, m_crFixedBkColour;
    COLORREF    m_crGridBkColour, m_crGridLineColour;
    COLORREF    m_crWindowText, m_crWindowColour, m_cr3DFace,     // System colours
                m_crShadow;
    COLORREF    m_crTTipBackClr, m_crTTipTextClr;                 // Titletip colours - FNA
    int         m_nGridLines;
    BOOL        m_bModified;
    BOOL        m_bSingleRowSelection;
    BOOL        m_bSingleColSelection;
    BOOL        m_bAllowDraw;
    BOOL        m_bEnableSelection;
    BOOL        m_bFixedRowSelection, m_bFixedColumnSelection;
    BOOL        m_bTitleTips;
    BOOL        m_bTrackFocusCell;
    BOOL        m_bFrameFocus;
    UINT        m_nAutoSizeColumnStyle;
    // Cell size details
    int         m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    CUIntArray  m_arRowHeights, m_arColWidths;
    // Fonts and images
    CRuntimeClass*   m_pRtcDefault; // determines kind of Grid Cell created by default
    CGridDefaultCell m_cellDefault;  // "default" cell. Contains default colours, font etc.
    CGridDefaultCell m_cellFixedColDef, m_cellFixedRowDef, m_cellFixedRowColDef;
    CImageList* m_pImageList;
    // Cell data
    CTypedPtrArray<CObArray, GRID_ROW*> m_RowData;
    // Mouse operations such as cell selection
    int         m_MouseMode;
    BOOL        m_bLMouseButtonDown, m_bRMouseButtonDown;
    CPoint      m_LeftClickDownPoint, m_LastMousePoint;
    CCellID     m_LeftClickDownCell, m_SelectionStartCell;
    CCellID     m_idCurrentCell, m_idTopLeftCell;
    CMap<DWORD,DWORD, CCellID, CCellID&> m_SelectedCellMap, m_PrevSelectedCellMap;
    enum eMouseModes { MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                       MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                       MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
                       MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                       MOUSE_PREPARE_EDIT,
#ifndef GRIDCONTROL_NO_DRAGDROP
                       MOUSE_PREPARE_DRAG, MOUSE_DRAGGING
#endif
    };
//      for sort in virtual mode, and column order, save and load layer
public:
	typedef std::vector<int> intlist;
	intlist m_arRowOrder, m_arColOrder;
};
