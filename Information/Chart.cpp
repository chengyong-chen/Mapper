#include "stdafx.h"
#include "Chart.h"

#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChart
CChart::CChart()
{
	minTick = 0;
	maxTick = 100;
	StepTick = 10;

	TickFontSize = 12;
	m_bDrawAxis = TRUE;
}

CChart::~CChart()
{
	m_database.Close();
}

BEGIN_MESSAGE_MAP(CChart, CStatic)
	//{{AFX_MSG_MAP(CChart)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChart message handlers

BOOL CChart::Open(CString strConnect, CString strTable, CString strWhere, CString strClass, CString strValue)
{
	m_database.Close();

	try
	{
		m_database.OpenEx(strConnect, CDatabase::noOdbcDialog);

		CString strSQL;
		strSQL.Format(_T("Select COUNT(*) As myCount,Max([%s]) As myMax,Min([%s]) As myMin,AVG([%s]) As myAvg,SUM([%s]) As mySum FROM %s WHERE %s"), strValue, strValue, strValue, strValue, strTable, strWhere);
		CODBCDatabase::ParseSQL(&m_database, strSQL);

		CRecordset rs(&m_database);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);

		CString strCount;
		CString strMax;
		CString strMin;
		CString strAvg;
		CString strSum;
		if(rs.IsEOF()==FALSE)
		{
			rs.GetFieldValue(_T("myCount"), strCount);
			rs.GetFieldValue(_T("myMax"), strMax);
			rs.GetFieldValue(_T("myMin"), strMin);
			rs.GetFieldValue(_T("myAvg"), strAvg);
			rs.GetFieldValue(_T("mySum"), strSum);
		}
		rs.Close();

		minTick = _ttoi(strMin);
		maxTick = _ttoi(strMax);;
		StepTick = maxTick-minTick;

		int Heght = m_Axis.Height()/10+1;
		StepTick = StepTick/Heght;
		StepTick /= 10;
		StepTick *= 10;

		if(abs(StepTick)<10)
			StepTick = 10;

		nCount = _ttoi(strCount);
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
	m_strTable = strTable;
	m_strWhere = strWhere;
	m_strClass = strClass;
	m_strValue = strValue;

	return FALSE;
}

void CChart::Draw(CDC* pDC, long nPos)
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(rect);

	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(rect);

	if(m_database.IsOpen()==FALSE)
		return;

	CRecordset rs(&m_database);
	try
	{
		CString strSQL;
		if(m_strWhere.IsEmpty()==FALSE)
			strSQL.Format(_T("SELECT [%s],[%s] FROM [%s] WHERE %s"), m_strClass, m_strValue, m_strTable, m_strWhere);
		else
			strSQL.Format(_T("SELECT [%s],[%s] FROM [%s]"), m_strClass, m_strValue, m_strTable);
		CODBCDatabase::ParseSQL(&m_database, strSQL);

		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
	DrawAxis(&dc, rs, nPos);
	DrawSeries(&dc, rs, nPos);

	rs.Close();
}

void CChart::DrawAxis(CDC* pDC, CRecordset& rs, long nPos)
{
	if(rs.IsOpen()==FALSE)
		return;

	CPen axisPen;
	axisPen.CreatePen(PS_SOLID, 2, (COLORREF)0X0000);
	CPen* pOldAxisPen = pDC->SelectObject(&axisPen);

	pDC->MoveTo(m_Apex.x, m_Axis.top);
	pDC->LineTo(m_Apex.x, m_Axis.bottom);
	pDC->MoveTo(m_Axis.left, m_Apex.y);
	pDC->LineTo(m_Axis.right, m_Apex.y);

	pDC->SelectObject(pOldAxisPen);
	axisPen.DeleteObject();

	int numTicks = (maxTick-minTick)/StepTick;
	if((numTicks*StepTick)<(maxTick-minTick))
		numTicks++;
	const double tickScale = (double)m_Axis.Height()/numTicks;

	COLORREF oldColor = pDC->SetTextColor(BLACK);
	UINT oldAlign = pDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
	const int oldBKMode = pDC->SetBkMode(TRANSPARENT);

	axisPen.CreatePen(PS_SOLID, 1, (COLORREF)0X0000);
	pOldAxisPen = pDC->SelectObject(&axisPen);

	CFont font;
	font.CreateFont(-12, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));
	CFont* pOldFont = (CFont*)pDC->SelectObject(&font);

	int tickYLocation = m_Apex.y;
	for(int y = 1; y<=numTicks; y++)
	{
		tickYLocation -= tickScale;

		//draw tick mark
		pDC->MoveTo(m_Apex.x-5, tickYLocation);
		pDC->LineTo(m_Apex.x, tickYLocation);

		//draw tick label
		CString tickLabel;
		tickLabel.Format(_T("%d"), minTick+(y*StepTick));

		pDC->TextOut(m_Apex.x-7, tickYLocation+6, tickLabel);
	}
	pDC->TextOut(m_Apex.x-5, m_Apex.y+5, _T("0"));

	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldColor);
	pDC->SetTextAlign(oldAlign);

	//draw X axis tick marks
	if(nCount<1)
	{
		pDC->SelectObject(pOldAxisPen);
		axisPen.DeleteObject();

		pDC->SelectObject(&pOldFont);
		font.DeleteObject();
		return;
	}

	int seriesSpace = m_Axis.Width()/nCount;
	if(seriesSpace<70)
		seriesSpace = 70;

	oldColor = pDC->SetTextColor(RGB(255, 0, 0));
	oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	int tickXLocation = m_Apex.x+seriesSpace/2;
	while(rs.IsEOF()==FALSE)
	{
		CString strName;
		rs.GetFieldValue(m_strClass, strName);

		if(tickXLocation>=nPos+m_Apex.x)
		{
			pDC->MoveTo(tickXLocation-nPos, m_Apex.y);
			pDC->LineTo(tickXLocation-nPos, m_Apex.y+5);

			pDC->TextOut(tickXLocation-nPos, m_Apex.y+18, strName);
		}
		if(tickXLocation>=nPos+m_Axis.Width()+m_Apex.x)
			break;

		tickXLocation += seriesSpace;
		rs.MoveNext();
	}
	pDC->SelectObject(pOldAxisPen);
	axisPen.DeleteObject();

	pDC->SetBkMode(oldBKMode);
	pDC->SetTextColor(oldColor);
	pDC->SetTextAlign(oldAlign);

	pDC->SelectObject(&pOldFont);
	font.DeleteObject();
}

void CChart::Close()
{
	m_database.Close();
	m_strTable.Empty();
	m_strWhere.Empty();

	m_strClass.Empty();
	m_strValue.Empty();
}

void CChart::DrawSeries(CDC* pDC, CRecordset& rs, long nPos)
{
	return;
}

void CChart::ChangeSize(int cx, int cy)
{
	m_Rect = CRect(0, 0, cx, cy);;
	m_Axis = m_Rect;
	m_Axis.DeflateRect(40, 20, 20, 20);
	m_Apex = CPoint(m_Axis.left, m_Axis.bottom);
}
