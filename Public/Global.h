#pragma once

#define _OPEN_SYS_ITOA_EXT
#include <stdlib.h>


class CDatabase;

#define WM_GETSYSICONLIST    (WM_USER + 901)
#define WM_DESTROYMENU       (WM_USER + 902)
#define WM_CROSSTHREAD       (WM_USER + 903)

typedef struct tagUNIT
{
	TCHAR unitname[15];
	float pointpro;
	float inchepro;
} UNIT;

class CSizeF
{
public:
	double cx;
	double cy;

	CSizeF()
	{
		cx = 0;
		cy = 0;
	}

	CSizeF(double fX, double fY)
	{
		cx = fX;
		cy = fY;
	}

	~CSizeF()
	{
	}

	CSizeF operator =(const CSizeF& size)
	{
		cx = size.cx;
		cy = size.cy;

		return *this;
	}
};

class CPointF
{
public:
	double x;
	double y;
	//	double z;	

	CPointF()
	{
		x = 0;
		y = 0;
	}

	CPointF(double fX, double fY)
	{
		x = fX;
		y = fY;
	}

	CPointF(const CPointF& dPoint)
	{
		x = dPoint.x;
		y = dPoint.y;
	}

	~CPointF()
	{
	}

	void operator =(const CPointF& point)
	{
		x = point.x;
		y = point.y;
	};

	void operator+=(const CSizeF& size)
	{
		x += size.cx;
		y += size.cy;
	}

	CPointF operator+(const CSizeF& size) const
	{
		return CPointF(x+size.cx, y+size.cy);
	}

	CPointF operator-=(const CSizeF& size) const
	{
		return CPointF(x-size.cx, y-size.cy);
	}

	bool operator ==(const CPointF& point) const
	{
		if(x!=point.x)
			return false;
		if(y!=point.y)
			return false;

		return true;
	}

	bool operator !=(const CPointF& point) const
	{
		if(x!=point.x)
			return true;
		if(y!=point.y)
			return true;

		return false;
	}
};

class CRectD : CRect
{
	BOOL UnionRect(CRect& rect1, CRect& rect2)
	{
		//	allRect.UnionRect(allRect,oneRect); this dose not work
		if(rect1.IsRectEmpty()==TRUE)
		{
			*(CRect)this = rect2;
		}
		else if(rect2.IsRectEmpty()==TRUE)
		{
			*(CRect)this = rect1;
		}
		else
		{
			this->left = min(rect1.left, rect2.left);
			this->top = min(rect1.top, rect2.top);
			this->right = max(rect1.right, rect2.right);
			this->bottom = max(rect1.bottom, rect2.bottom);
		}
	}
};

class CRectF
{
public:
	double left;
	double top;
	double right;
	double bottom;
	// CRectF

	CRectF()
	{
		left = 0.f;
		top = 0.f;
		right = 0.f;
		bottom = 0.f;
	}

	CRectF(double l, double t, double r, double b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	CRectF(CPointF point, CSizeF size)
	{
		left = point.x;
		top = point.y;
		right = point.x+size.cx;
		bottom = point.y+size.cy;

		NormalizeRect();
	}

	CRectF(CRect rect)
	{
		left = rect.left;
		top = rect.top;
		right = rect.right;
		bottom = rect.bottom;
	}

	double Width() const
	{
		return right-left;
	}

	double Height() const
	{
		return bottom-top;
	}

	CSizeF CSize() const
	{
		return CSizeF(right-left, bottom-top);
	}

	CPointF& TopLeft()
	{
		return *((CPointF*)this);
	}

	CPointF& BottomRight()
	{
		return *((CPointF*)this+1);
	}

	const CPointF& TopLeft() const
	{
		return *((CPointF*)this);
	}

	const CPointF& BottomRight() const
	{
		return *((CPointF*)this+1);
	}

	CPointF CenterPoint() const
	{
		return CPointF((left+right)/2, (top+bottom)/2);
	}

	bool IsRectEmpty() const
	{
		return (left-right==0.0f&&top-bottom==0.0f);
	}

	void EmptyRect()
	{
		left = 0.f;
		right = 0.f;
		top = 0.f;
		bottom = 0.f;
	}

	bool IsRectNull() const
	{
		return (left==0&&right==0&&top==0&&bottom==0);
	}

	bool PtInRect(CPointF point) const
	{
		if(point.x<left)
			return false;
		if(point.x>right)
			return false;

		if(point.y<top)
			return false;
		if(point.y>bottom)
			return false;

		return true;
	}

	bool PtInRect(double x, double y) const
	{
		if(x<left)
			return false;
		if(x>right)
			return false;

		if(y<top)
			return false;
		if(y>bottom)
			return false;

		return true;
	}

	bool Contains(CRectF& rect) const
	{
		if(rect.left>right)
			return false;
		if(rect.left<left)
			return false;

		if(rect.right<left)
			return false;
		if(rect.right>right)
			return false;

		if(rect.top>bottom)
			return false;
		if(rect.top<top)
			return false;

		if(rect.bottom<top)
			return false;
		if(rect.bottom>bottom)
			return false;

		return true;
	}

	void SetRect(double x1, double y1, double x2, double y2)
	{
		left = x1;
		top = y1;
		right = x2;
		bottom = y2;
	}

	void SetRectEmpty()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	bool IntersectWith(const CRectF& rect) const
	{
		if(right-left==0.f||bottom-top==0.f)
			return false;
		else if(rect.right-rect.left==0.f||rect.bottom-rect.top==0.f)
			return false;
		else
		{
			const double Left = max(left, rect.left);
			const double Top = max(top, rect.top);
			const double Right = min(right, rect.right);
			const double Bottom = min(bottom, rect.bottom);
			if(Left>=Right)
				return false;
			else if(Top>=Bottom)
				return false;
			else
				return true;
		}
	}

	void InflateRect(double x, double y)
	{
		left -= x;
		top -= y;
		right += x;
		bottom += y;
	}

	void InflateRect(CSizeF size)
	{
		left -= size.cx;
		top -= size.cy;
		right += size.cx;
		bottom += size.cy;
	}

	void DeflateRect(double x, double y)
	{
		left += x;
		top += y;
		right -= x;
		bottom -= y;
		NormalizeRect();
	}

	void DeflateRect(CSizeF size)
	{
		left += size.cx;
		top += size.cy;
		right -= size.cx;
		bottom -= size.cy;

		NormalizeRect();
	}

	void OffsetRect(double x, double y)
	{
		left += x;
		top += y;
		right += x;
		bottom += y;
	}

	void OffsetRect(const CPoint& point)
	{
		left += point.x;
		top += point.y;
		right += point.x;
		bottom += point.y;
	}

	void OffsetRect(CSizeF size)
	{
		left += size.cx;
		top += size.cy;
		right += size.cx;
		bottom += size.cy;
	}

	bool operator ==(const CRectF& rect) const
	{
		if(left!=rect.left)
			return false;
		if(top!=rect.top)
			return false;
		if(right!=rect.right)
			return false;
		if(bottom!=rect.bottom)
			return false;

		return true;
	}

	bool operator !=(const CRectF& rect) const
	{
		if(left!=rect.left)
			return true;
		if(top!=rect.top)
			return true;
		if(right!=rect.right)
			return true;
		if(bottom!=rect.bottom)
			return true;

		return false;
	}

	void operator+=(CPointF point)
	{
		OffsetRect(point.x, point.y);
	}

	void operator+=(CSizeF size)
	{
		OffsetRect(size.cx, size.cy);
	}

	void operator-=(CPointF point)
	{
		OffsetRect(-point.x, -point.y);
	}

	void operator-=(CSizeF size)
	{
		OffsetRect(-size.cx, -size.cy);
	}

	CRectF operator+(CPointF pt) const
	{
		CRectF rect(*this);
		rect.OffsetRect(pt.x, pt.y);
		return rect;
	}

	CRectF operator-(CPointF pt) const
	{
		CRectF rect(*this);
		rect.OffsetRect(-pt.x, -pt.y);
		return rect;
	}

	CRectF operator+(CSizeF size) const
	{
		CRectF rect(*this);
		rect.OffsetRect(size.cx, size.cy);
		return rect;
	}

	CRectF operator-(CSizeF size) const
	{
		CRectF rect(*this);
		rect.OffsetRect(-size.cx, -size.cy);
		return rect;
	}

	void NormalizeRect()
	{
		double nTemp;
		if(left>right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}
		if(top>bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

	void InflateRect(double l, double t, double r, double b)
	{
		left -= l;
		top -= t;
		right += r;
		bottom += b;
	}

	void DeflateRect(double l, double t, double r, double b)
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	void UnionRect(CRectF rect)
	{
		if(right-left==0.f||bottom-top==0.f)
		{
			left = rect.left;
			top = rect.top;
			right = rect.right;
			bottom = rect.bottom;
		}
		else
		{
			if(rect.left<left)
				left = rect.left;
			if(rect.top<top)
				top = rect.top;
			if(rect.right>right)
				right = rect.right;
			if(rect.bottom>bottom)
				bottom = rect.bottom;
		}

		NormalizeRect();
	}

	void IntersectRect(CRectF rect)
	{
		if(right-left==0.f||bottom-top==0.f)
		{
			return;
		}
		if(rect.right-rect.left==0.f||rect.bottom-rect.top==0.f)
		{
			left = 0.f;
			top = 0.f;
			right = 0.f;
			bottom = 0.f;
			return;
		}

		left = max(left, rect.left);
		top = max(top, rect.top);
		right = min(right, rect.right);
		bottom = min(bottom, rect.bottom);

		if(left>right||top>bottom)
			EmptyRect();
		else
			NormalizeRect();
	}

	void Serialize(CArchive& ar)
	{
		if(ar.IsStoring())
		{
			ar<<left;
			ar<<top;
			ar<<right;
			ar<<bottom;
		}
		else
		{
			ar>>left;
			ar>>top;
			ar>>right;
			ar>>bottom;
		}
	}
};

typedef struct tagLOCATION
{
	CString Province;
	CString County;

	CString RoadEast;
	long miniRoadEast;
	CString RoadWest;
	long miniRoadWest;
	CString RoadSouth;
	long miniRoadSouth;
	CString RoadNorth;
	long miniRoadNorth;
	CString SpotEast;
	long miniSpotEast;
	CString SpotWest;
	long miniSpotWest;
	CString SpotSouth;
	long miniSpotSouth;
	CString SpotNorth;
	long miniSpotNorth;

	tagLOCATION()
	{
		Province.Empty();
		County.Empty();

		RoadEast.Empty();
		miniRoadEast = 3000;
		RoadWest.Empty();
		miniRoadWest = 3000;
		RoadSouth.Empty();
		miniRoadSouth = 3000;
		RoadNorth.Empty();
		miniRoadNorth = 3000;
		SpotEast.Empty();
		miniSpotEast = 3000;
		SpotWest.Empty();
		miniSpotWest = 3000;
		SpotSouth.Empty();
		miniSpotSouth = 3000;
		SpotNorth.Empty();
		miniSpotNorth = 3000;
	};
} LOCATION;

typedef enum
{
	X86,
	X64,
	Unknown
} Platform;

CArchive& AFXAPI operator <<(CArchive& ar, CPointF& point);
CArchive& AFXAPI operator >>(CArchive& ar, CPointF& point);
CArchive& AFXAPI operator <<(CArchive& ar, CSizeF& size);
CArchive& AFXAPI operator >>(CArchive& ar, CSizeF& size);

__declspec(dllexport) HWND GetWndFromThread(DWORD threadID, DWORD dwProcessID);
__declspec(dllexport) BOOL IsOCXRegistered(CStringA strOCX, BSTR bstrLicKey = nullptr);
__declspec(dllexport) BOOL RegisterOCX(CStringA strOCX, Platform platform, CStringA extra);
__declspec(dllexport) CStringA GetExeDirectory();
__declspec(dllexport) void DeleteFiles(CString strWildcards);
__declspec(dllexport) bool CreateDeepDirectory(LPCSTR lpszPath);
__declspec(dllexport) BOOL IsWow64();
__declspec(dllexport) bool IsNumeric(const char* string);
__declspec(dllexport) bool IsDecimal(const char* string);
__declspec(dllexport) bool IsInteger(const char* string);
__declspec(dllexport) void Replace(CStringA& string, CStringA original, int data);

template<typename T>
void Swapdata(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
};

template<typename T>
void Swapclass(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
};

template<typename T>
void Swaplist(T& a, T& b)
{
	T c;
	POSITION pos1 = a.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CObject* pObject = a.GetNext(pos1);
		c.AddTail(pObject);
	}
	a.RemoveAll();

	POSITION pos2 = b.GetHeadPosition();
	while(pos2!=nullptr)
	{
		CObject* pObject = b.GetNext(pos2);
		a.AddTail(pObject);
	}
	b.RemoveAll();

	POSITION Pos3 = c.GetHeadPosition();
	while(Pos3!=nullptr)
	{
		CObject* pObject = c.GetNext(Pos3);
		b.AddTail(pObject);
	}
};
