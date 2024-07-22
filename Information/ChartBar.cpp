//Chart.cpp - Version 3.0 (Brian Convery, May, 2001)

#include "stdafx.h"
#include "ChartBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChartBar

CChartBar::CChartBar()
	: CChart()
{
}

CChartBar::~CChartBar()
{
}

BEGIN_MESSAGE_MAP(CChartBar, CStatic)
	//{{AFX_MSG_MAP(CChartBar)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChartBar message handlers

void CChartBar::DrawSeries(CDC* pDC, CRecordset& rs, long nPos)
{
	if(nCount<1)
		return;

	pDC->IntersectClipRect(&m_Axis);

	COLORREF oldColor = pDC->SetTextColor(BLACK);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	int oldBKMode = pDC->SetBkMode(TRANSPARENT);

	CFont font;
	font.CreateFont(-12, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));
	CFont* pOldFont = (CFont*)pDC->SelectObject(&font);

	CBrush brush(0XFF0000);
	CBrush* oldBrush = pDC->SelectObject(&brush);

	int seriesSpace = m_Axis.Width()/nCount;
	if(seriesSpace<70)
		seriesSpace = 70;
	minTick = 0;
	rs.MoveFirst();
	int tickXLocation = m_Apex.x+seriesSpace/2;
	while(rs.IsEOF()==FALSE)
	{
		CString strValue;
		rs.GetFieldValue(m_strValue, strValue);

		if(tickXLocation>=nPos)
		{
			const int nValue = _ttoi(strValue);
			const double dataScale = (m_Axis.Height()*1.0f)/(maxTick-minTick);
			int barHeight = (int)((nValue-minTick)*dataScale);
			if((nValue-minTick)>0&&barHeight<3)
				barHeight = 2;

			int barWidth = (int)(seriesSpace*.6);
			if(barWidth>20)
				barWidth = 20;
			CRect rect(tickXLocation-barWidth/2-nPos, m_Axis.bottom-barHeight, tickXLocation+barWidth/2-nPos, m_Axis.bottom);
			pDC->Rectangle(rect);
			pDC->TextOut(rect.CenterPoint().x, rect.top, strValue);
		}
		if(tickXLocation>=nPos+m_Axis.Width())
			break;

		tickXLocation = tickXLocation+seriesSpace;
		rs.MoveNext();
	}

	pDC->SelectObject(oldBrush);
}
