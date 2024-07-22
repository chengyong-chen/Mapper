//----------------------------------------------------------------------
//  File    : CColorPicker.cpp 
//  Project :
//  Author  : Patrick PRACHE (pprache@kalisto.com)
//  Created : september 10 1999
//----------------------------------------------------------------------
//  Purpose	: this class defines a new type of color picker MFC control.   
//  Notes	: use this code as you want, it's free
//----------------------------------------------------------------------

#include "stdafx.h"
#include "ColorPicker.h"

//----------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_LUMINANCE				0.50f

#define MOVEMODE_LUMINANCE		1
#define MOVEMODE_PICKER			2
#define	MOVEMODE_NONE			3


//----------------------------------------------------------------------
// Conversion between the HSL (Hue, Saturation, and Luminosity) 
// and RBG color model.
//----------------------------------------------------------------------
// The conversion algorithms presented here come from the book by 
// Fundamentals of Interactive Computer Gdiplus::Graphics by Foley and van Dam. 
// In the example code, HSL values are represented as floating point 
// number in the range 0 to 1. RGB tridrants use the Windows convention 
// of 0 to 255 of each element. 
//----------------------------------------------------------------------

static double HuetoRGB(double m1, double m2, double h)
{
	if(h < 0) h += 1.0;
	if(h > 1) h -= 1.0;
	if(6.0*h < 1)
		return (m1+(m2-m1)*h*6.0);
	if(2.0*h < 1)
		return m2;
	if(3.0*h < 2.0)
		return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
	return m1;
}


COLORREF HLStoRGB(double H, double L, double S)
{
	double r,g,b;
	double m1, m2;

	if(S==0) {
		r=g=b=L;
	} else {
		if(L <=0.5)
			m2 = L*(1.0+S);
		else
			m2 = L+S-L*S;
		m1 = 2.0*L-m2;
		r = HuetoRGB(m1,m2,H+1.0/3.0);
		g = HuetoRGB(m1,m2,H);
		b = HuetoRGB(m1,m2,H-1.0/3.0);
	}
	return RGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255));
}

void RGBtoHSL(COLORREF rgb, double *H, double *S, double *L)
{   
	double ?;
	double r = (double)GetRValue(rgb)/255;
	double g = (double)GetGValue(rgb)/255;
	double b = (double)GetBValue(rgb)/255;   
	double cmax = max(r,max(g,b));
	double cmin = min(r,min(g,b));   
	*L=(cmax+cmin)/2.0;   

	if(cmax==cmin) 
	{
		*S = 0;      
		*H = 0; // it's really undefined   
	} else 
	{
		if(*L < 0.5) 
			*S = (cmax-cmin)/(cmax+cmin);      
		else
			*S = (cmax-cmin)/(2.0-cmax-cmin);      

		? = cmax - cmin;
		if(r==cmax) 
			*H = (g-b)/?;      
		else if(g==cmax)
			*H = 2.0 +(b-r)/?;
		else          
			*H=4.0+(r-g)/?;
		*H /= 6.0; 
		if(*H < 0.0)
			*H += 1;  
	}
}




//----------------------------------------------------------------------
// CColorPicker class
//----------------------------------------------------------------------
CColorPicker::CColorPicker()
{
	luminanceBarState	= 0;
	pickerBmpState		= 0;
	pOnMove				= nullptr;
	pOnLDown			= nullptr;
	szTotal.cx = 100;
	szTotal.cy = 100;
	szColorPicker.cx	= 10;
	szColorPicker.cy	= 10;
	createPickerDC		= TRUE;
	createLuminanceDC	= TRUE;
	currentColor		= 0;
	RGBtoHSL(currentColor, &currentHue, &currentSaturation, &currentLuminance);
	width_of_luminance_bar  = 20;

	pickerDC			= nullptr;
	luminanceBarDC		= nullptr;

}

CColorPicker::~CColorPicker()
{
	if(luminanceBarDC != nullptr)
	{
		luminanceBarDC->DeleteDC();
		delete luminanceBarDC;
	}

	if(pickerDC != nullptr)
	{
		pickerDC->DeleteDC();
		delete pickerDC;
	}
}


BEGIN_MESSAGE_MAP(CColorPicker, CWnd)
	//{{AFX_MSG_MAP(CColorPicker)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//----------------------------------------------------------------------
// CColorPicker message handlers
//----------------------------------------------------------------------

void CColorPicker::GeneratePicker()
{
	static int start =1;
	if(createPickerDC)
	{
		//
		// create bitmap
		//
		CBitmap *pPickerBmp = new CBitmap();
		pPickerBmp->CreateCompatibleBitmap(GetDC(),	szColorPicker.cx,szColorPicker.cy);
		if(pickerDC != nullptr)
		{
			pickerDC->DeleteDC();
			delete pickerDC;
		}

		pickerDC = new CDC();
		pickerDC->CreateCompatibleDC(GetDC());
		pickerDC->SelectObject(pPickerBmp);


		delete pPickerBmp; // we don't need this object anymore
		pPickerBmp = nullptr;
		createPickerDC = FALSE;
	}	

	//
	// fill color picker bitmap
	//
	COLORREF ref;
	for(int i= 0;i<szColorPicker.cx;i++)
	{
		for(int j=0;j<szColorPicker.cy;j++)
		{
			ref = 	HLStoRGB((double)j/(double)szColorPicker.cy,DEFAULT_LUMINANCE,(double)i/(double)szColorPicker.cx);
			//ref = 	HLStoRGB((double)j/(double)szColorPicker.cy,currentLuminance,(double)i/(double)szColorPicker.cx);
			pickerDC->SetPixelV(i,j,ref);
		}
	}
	pickerBmpState = 1;
}

void CColorPicker :: GenerateLuminanceBar()
{
	static int first = 1;
	if(createLuminanceDC)
	{
		//
		// create bitmap
		//
		CBitmap *pLuminanceBmp = new CBitmap();
		pLuminanceBmp->CreateCompatibleBitmap(	GetDC(),szLuminanceBar.cx,szLuminanceBar.cy);
		//
		// create picker DC 
		//
		if(luminanceBarDC != nullptr)
		{
			luminanceBarDC->DeleteDC();
			delete luminanceBarDC;
		}
		luminanceBarDC = new CDC();
		luminanceBarDC->CreateCompatibleDC(GetDC());
		luminanceBarDC->SelectObject(pLuminanceBmp);

		delete pLuminanceBmp; // we don't need this object anymore
		pLuminanceBmp = nullptr;
		createLuminanceDC = FALSE;
	}

	//
	// fill color picker bitmap
	//
	COLORREF col;
	for(int j= 0;j<szLuminanceBar.cy;j++)
	{
		col = 	HLStoRGB(currentHue,(double)j/(double)szLuminanceBar.cy,currentSaturation);
		luminanceBarDC->FillSolidRect(0,j,szLuminanceBar.cx,j+1,col);
	}
	luminanceBarState = 1;
}

void CColorPicker::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(!pickerBmpState)		GeneratePicker();
	if(!luminanceBarState)	GenerateLuminanceBar();

	dc.BitBlt(0,0,szTotal.cx,szTotal.cy,pickerDC,0,0,SRCCOPY);
	dc.BitBlt(szColorPicker.cx,0,szLuminanceBar.cx,szTotal.cy,luminanceBarDC,0,0,SRCCOPY);


	SIZE sz={6,6};
	DrawCrossAt(mousePos,dc,sz);// draw a rectangle on picker

	sz.cx = szLuminanceBar.cx;
	sz.cy = 6;
	CPoint pt(szColorPicker.cx+szLuminanceBar.cx/2,(int)(currentLuminance*szLuminanceBar.cy));
	DrawCrossAt(pt,dc,sz);//draw rectangle on luminance bar

	// Do not call CWnd::OnPaint() for painting messages
}

void CColorPicker::DrawCrossAt(CPoint &point,CPaintDC &dc, SIZE &sz)
{
	CPoint	localPoint;
	localPoint = point;
	localPoint.x -= sz.cx/2;
	localPoint.y -= sz.cy/2;
	CRect	localRect(localPoint,sz);	

	dc.DrawEdge(localRect,EDGE_BUMP   ,BF_TOPLEFT |BF_BOTTOMRIGHT    );
	//dc.SetPixelV(point,0xffffff);
}

// compute conversion to HSL
// place the rectangle on the nearest color
void CColorPicker::SetRGB(COLORREF ref)
{
	RGBtoHSL(ref,&currentHue,&currentSaturation,&currentLuminance);
	currentColor = ref;

	mousePos.x =(long)((double)szColorPicker.cx*currentSaturation);	
	mousePos.y =(long)((double)szColorPicker.cy*currentHue);	

	//	if(pOnLDown)pOnLDown(currentColor,onLDownCD);

	luminanceBarState = 0;// regenerate luminance bar
	Invalidate();
}

COLORREF CColorPicker::GetRGB()
{
	return currentColor;
}


void CColorPicker::SetHLS(double hue,double luminance, double saturation)
{
	currentHue			= hue;
	currentSaturation	= saturation;
	currentLuminance	= luminance;

	currentColor = HLStoRGB(currentHue,currentSaturation,currentLuminance);

	mousePos.x =(long)((double)szColorPicker.cx*currentSaturation);	
	mousePos.y =(long)((double)szColorPicker.cy*currentHue);	

	if(pOnLDown)pOnLDown(currentColor,onLDownCD);

	luminanceBarState = 0;// regenerate luminance bar
	Invalidate();
}

void CColorPicker::GetHLS(double *hue,double *luminance, double *saturation)
{
	*hue		= currentHue;
	*luminance	= currentLuminance;
	*saturation = currentSaturation;
}


BOOL CColorPicker::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	HCURSOR curs = LoadCursor(nullptr,IDC_CROSS);
	SetCursor(curs);
	return FALSE;
	//return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CColorPicker::OnLButtonDown(UINT nFlags, CPoint point) 
{
	COLORREF ref = pickerDC->GetPixel(point);

	if(pickerBmpState)
	{
		if(point.y<szColorPicker.cy)
		{
			if(point.x>szColorPicker.cx)
			{
				//
				// change luminance
				//
				moveMode = MOVEMODE_LUMINANCE;
				if(point.y>szColorPicker.cy) point.y=szColorPicker.cy;
				currentLuminance = ((double)point.y)/((double)szLuminanceBar.cy);
				currentColor = HLStoRGB(currentHue,currentLuminance,currentSaturation);
			}
			else
			{
				//
				// change color
				//
				moveMode = MOVEMODE_PICKER;

				if(nFlags & MK_CONTROL)	point.x = mousePos.x;
				if(nFlags & MK_SHIFT)		point.y = mousePos.y;

				currentSaturation = (double)point.x/(double)szColorPicker.cx;
				currentHue = (double)point.y/(double)szColorPicker.cy;
				currentColor = HLStoRGB(currentHue,currentLuminance,currentSaturation);

				CWnd::OnLButtonDown(nFlags, point);
				mousePos	= point;
				luminanceBarState = 0;// regenerate luminance bar
			}
		}
		//
		// call callback function
		//
		if(pOnLDown)pOnLDown(currentColor,onLDownCD);
		Invalidate();
	}
}


void CColorPicker::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(pickerBmpState && (nFlags & MK_LBUTTON))
	{
		if(moveMode == MOVEMODE_LUMINANCE)
		{
			//
			// change luminance
			//
			if(point.y>szColorPicker.cy) point.y=szColorPicker.cy;
			currentLuminance = ((double)point.y)/((double)szLuminanceBar.cy);
			currentColor = HLStoRGB(currentHue,currentLuminance,currentSaturation);
		}
		else if(moveMode == MOVEMODE_PICKER)
		{

			if(point.x>szColorPicker.cx) point.x=szColorPicker.cx;
			if(point.y>szColorPicker.cy) point.y=szColorPicker.cy;

			// stay on saturation
			if(nFlags & MK_CONTROL) point.x = mousePos.x;	   
			if(nFlags & MK_SHIFT)	point.y = mousePos.y;

			currentSaturation = (double)point.x/(double)szColorPicker.cx;
			currentHue = (double)point.y/(double)szColorPicker.cy;
			currentColor = HLStoRGB(currentHue,currentLuminance,currentSaturation);

			mousePos	= point;
			luminanceBarState = 0;// regenerate luminance bar
		}
		//
		// call callback function
		//
		if(pOnMove)pOnMove(currentColor,onMoveCD);
		Invalidate();
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CColorPicker::OnLButtonUp(UINT nFlags, CPoint point) 
{
	moveMode = MOVEMODE_NONE;

	CWnd::OnLButtonUp(nFlags, point);
}

// this function must be call before the first paint message
// on the picker control
void CColorPicker::SetLuminanceBarWidth(int w)
{
	width_of_luminance_bar = w;
	//	if(createLuminanceDC == TRUE) 
	//		return;

	RECT rect;
	GetClientRect(&rect);
	//
	// compute size of the entire window
	//

	int maxi = rect.right-rect.left;
	int maxj = rect.bottom-rect.top;

	szColorPicker.cx = maxi - width_of_luminance_bar;
	szColorPicker.cy = maxj;

	szLuminanceBar.cx = width_of_luminance_bar;
	szLuminanceBar.cy = maxj;

	szTotal.cx = maxi;
	szTotal.cy = maxj;

	createPickerDC = TRUE;
	createLuminanceDC = TRUE;
	pickerBmpState = FALSE;
	luminanceBarState = FALSE;

	Invalidate();

}
/*
void CColorPicker::OnSize(UINT nType, int cx, int cy) 
{
CWnd::OnSize(nType, cx, cy);

RECT rect;

//
// compute size of the entire window
//
GetClientRect(&rect);
int maxi = rect.right-rect.left;
int maxj = rect.bottom-rect.top;

szColorPicker.cx = maxi - width_of_luminance_bar;
szColorPicker.cy = maxj;

szLuminanceBar.cx = width_of_luminance_bar;
szLuminanceBar.cy = maxj;

szTotal.cx = maxi;
szTotal.cy = maxj;

createPickerDC = TRUE;
createLuminanceDC = TRUE;
pickerBmpState = FALSE;
luminanceBarState = FALSE;
}
*/