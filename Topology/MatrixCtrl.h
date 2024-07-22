#pragma once

// CMatrixCtrl.h : header file

class CMatrix;

/////////////////////////////////////////////////////////////////////////////
// CMatrixCtrl window

class CMatrixCtrl : public CWnd
{
public:
	CMatrixCtrl();
	DECLARE_DYNCREATE(CMatrixCtrl)

	// Attributes
protected:
	CMatrix* m_pMatrix; // Array of CByteArrays used as 2D grid data

public:
	int m_nActiveRow;
	CImageList m_imagelist;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatrixCtrl)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CMatrixCtrl() override;

	void Initialize(CMatrix* pMatrix);
	

	// Generated message map functions
protected:
	//{{AFX_MSG(CMatrixCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
