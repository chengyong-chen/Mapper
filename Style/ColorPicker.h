//----------------------------------------------------------------------
//  File    : CColorPicker.h 
//  Project :
//  Author  : Patrick PRACHE (pprache@kalisto.com)
//  Created : september 10 1999
//----------------------------------------------------------------------
//  Purpose	: this class defines a new type of color picker MFC control.   
//  Notes	: use this code as you want, it's free
//----------------------------------------------------------------------




#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CColorPicker.h : header file

typedef void(*CallbackFunc)(COLORREF, void *ClientData);

/////////////////////////////////////////////////////////////////////////////
// CColorPicker window

class CColorPicker : public CWnd
{
// Construction
public:
	CColorPicker();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPicker)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorPicker();
	virtual void RegisterCallbackOnMove(CallbackFunc ptr,void* ClientData) override{pOnMove = ptr;onMoveCD=ClientData;};
	virtual void RegisterCallbackOnLButtonDown(CallbackFunc ptr,void* ClientData) override{pOnLDown=ptr;onLDownCD=ClientData;};
	virtual void DrawCrossAt(CPoint &point,CPaintDC &dc,SIZE &sz) override;
	
	void		SetRGB(COLORREF ref);
	COLORREF	GetRGB();

	void SetHLS(double hue,double luminance, double saturation);
	void GetHLS(double *hue,double *luminance, double *saturation);

	void SetLuminanceBarWidth	(int w);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPicker)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
    CDC *pickerDC;
    CDC *luminanceBarDC;

	BOOL pickerBmpState;
	BOOL luminanceBarState;

	CallbackFunc pOnMove;	// callback
	CallbackFunc pOnLDown;	// callback
	void*onMoveCD;		// client data
	void*onLDownCD;		// client data

	CPoint		mousePos;	// position of the picker
	int			moveMode;	// moving on picker or on luminance

	double		currentLuminance, currentSaturation, currentHue;
	COLORREF	currentColor;

	SIZE szColorPicker;
	SIZE szLuminanceBar;
	SIZE szTotal;

	BOOL createPickerDC;
	BOOL createLuminanceDC;
	
	int  width_of_luminance_bar;

	void GeneratePicker();
	void GenerateLuminanceBar();
	

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


