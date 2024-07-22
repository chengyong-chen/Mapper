#pragma once

//color definitions
#define RED				RGB(255,0,0)
#define GREEN			RGB(0,255,0)
#define BLUE			RGB(0,0,255)
#define YELLOW			RGB(255,255,0)
#define ORANGE			RGB(255,153,51)
#define HOT_PINK		RGB(255,51,153)
#define PURPLE			RGB(153,0,204)
#define CYAN			RGB(0,255,255)
#define BLACK			RGB(0,0,0)
#define WHITE			RGB(255,255,255)
#define LAVENDER		RGB(199,177,255)
#define PEACH			RGB(255,226,177)
#define SKY_BLUE		RGB(142,255,255)
#define FOREST_GREEN	RGB(0,192,0)
#define BROWN			RGB(80,50,0)
#define TURQUOISE		RGB(0,192,192)
#define ROYAL_BLUE		RGB(0,0,192)
#define GREY			RGB(192,192,192)
#define DARK_GREY		RGB(128,128,128)
#define TAN				RGB(255,198,107)
#define DARK_BLUE		RGB(0,0,128)
#define MAROON			RGB(128,0,0)
#define DUSK			RGB(255,143,107)
#define LIGHT_GREY		RGB(225,225,225)	//only for 3D chart lines

class __declspec(dllexport) CChart : public CStatic
{
public:
	CChart();

public:
	CObList colorList;

	CDatabase m_database;
	CString m_strTable;
	CString m_strWhere;

	CString m_strClass;
	CString m_strValue;

	CRect m_Rect;
	CRect m_Axis;
	CPoint m_Apex;
public:
	BOOL m_bDrawAxis;
	int TickFontSize;
	int minTick;
	int maxTick;
	int StepTick;
	int nCount;

public:
	virtual BOOL Open(CString strConnect, CString strTable, CString strWhere, CString strClass, CString strValue);
	virtual void Close();

	void ChangeSize(int cx, int cy);
public:
	virtual void Draw(CDC* pDC, long nPos);
	virtual void DrawAxis(CDC* pDC, CRecordset& rs, long nPos);
	virtual void DrawSeries(CDC* pDC, CRecordset& rs, long nPos);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChart)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CChart() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CChart)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
