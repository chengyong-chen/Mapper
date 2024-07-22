#include "stdafx.h"

#include "Resource.h"
#include "Segment.h"
#include "Route.h"
#include "RoadTopo.h"
#include "Probe.h"
#include "Global.h"

#include "../Geometry/Global.h"
#include "../Geometry/Poly.h"

//#include "../Projection/GCS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImageList CRoute::m_imagelist;
CPointF CRoute::m_geoCurrent;

IMPLEMENT_SERIAL(CRoute, CObject, 0)

CRoute::CRoute()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_imagelist.m_hImageList==nullptr)
	{
		m_imagelist.Create(IDB_ROUTE16, 16, 0, RGB(0, 128, 128));
	}

	CString str;

	str = AfxGetApp()->GetProfileString(_T("Route"), _T("StartX"), _T("0.0"));
	m_ptStart.x = _tcstod(str, nullptr);
	str = AfxGetApp()->GetProfileString(_T("Route"), _T("StartY"), _T("0.0"));
	m_ptStart.y = _tcstod(str, nullptr);

	str = AfxGetApp()->GetProfileString(_T("Route"), _T("EndX"), _T("0.0"));
	m_ptEnd.x = _tcstod(str, nullptr);
	str = AfxGetApp()->GetProfileString(_T("Route"), _T("EndY"), _T("0.0"));
	m_ptEnd.y = _tcstod(str, nullptr);

	if(m_Pole.GetSize()==0)
	{
		const short nPole = AfxGetApp()->GetProfileInt(_T("Route"), _T("Pole"), 0);
		for(int index = 0; index<nPole; index++)
		{
			CString strXItem;
			CString strYItem;
			strXItem.Format(_T("PoleX%d"), index);
			strYItem.Format(_T("PoleY%d"), index);

			CPointF point;
			CString string;

			string = AfxGetApp()->GetProfileString(_T("Route"), strXItem, _T("0.0"));
			point.x = _tcstod(string, nullptr);
			string = AfxGetApp()->GetProfileString(_T("Route"), strYItem, _T("0.0"));
			point.y = _tcstod(string, nullptr);

			m_Pole.Add(point);
		}
	}
}

CRoute::~CRoute()
{
	if(m_imagelist.m_hImageList!=nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	CString str;
	str.Format(_T("%g"), m_ptStart.x);
	AfxGetApp()->WriteProfileString(_T("Route"), _T("StartX"), str);
	str.Format(_T("%g"), m_ptStart.y);
	AfxGetApp()->WriteProfileString(_T("Route"), _T("StartY"), str);

	str.Format(_T("%g"), m_ptEnd.x);
	AfxGetApp()->WriteProfileString(_T("Route"), _T("EndX"), str);
	str.Format(_T("%g"), m_ptEnd.y);
	AfxGetApp()->WriteProfileString(_T("Route"), _T("EndY"), str);
	const short nPole = m_Pole.GetSize();
	AfxGetApp()->WriteProfileInt(_T("Route"), _T("Pole"), nPole);
	for(int index = 0; index<nPole; index++)
	{
		CString strXItem;
		CString strYItem;
		strXItem.Format(_T("PoleX%d"), index);
		strYItem.Format(_T("PoleY%d"), index);
		const CPointF point = m_Pole.GetAt(index);
		CString string;
		string.Format(_T("%g"), point.x);
		AfxGetApp()->WriteProfileString(_T("Route"), strXItem, string);
		string.Format(_T("%g"), point.y);
		AfxGetApp()->WriteProfileString(_T("Route"), strYItem, string);
	}

	POSITION pos = m_segmentlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CSegment* segment = m_segmentlist.GetNext(pos);
		delete segment;
	}
	m_segmentlist.RemoveAll();
}

void CRoute::SetStart(CWnd* pWnd, const CViewinfo& viewinfo, const CPointF& geoPoint)
{
	if(pWnd!=nullptr)
	{
		Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
		cliPoint.X -= 8;
		cliPoint.Y -= 8;

		CClientDC dc(pWnd);
		m_imagelist.Draw(&dc, 0, CPoint(cliPoint.X, cliPoint.X), ILD_NORMAL|ILD_TRANSPARENT);
	}

	if(m_segmentlist.GetCount()>0)
	{
		POSITION pos = m_segmentlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			const CSegment* segment = m_segmentlist.GetNext(pos);
			delete segment;
		}
		m_segmentlist.RemoveAll();

		pWnd->Invalidate();
	}

	m_ptStart = geoPoint;
}

void CRoute::SetPass(CWnd* pWnd, const CViewinfo& viewinfo, const CPointF& geoPoint)
{
	Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
	cliPoint.X -= 8;
	cliPoint.Y -= 8;

	CClientDC dc(pWnd);

	if(m_Pole.GetSize()>6)
	{
		AfxMessageBox(_T("经过点不能超过6个!"));
		return;
	}

	m_Pole.Add(geoPoint);
	m_imagelist.Draw(&dc, 2+m_Pole.GetSize()-1, CPoint(cliPoint.X, cliPoint.X), ILD_NORMAL|ILD_TRANSPARENT);
}

void CRoute::SetEnd(CWnd* pWnd, const CViewinfo& viewinfo, const CPointF& geoPoint)
{
	if(pWnd!=nullptr)
	{
		Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
		cliPoint.X -= 8;
		cliPoint.Y -= 8;

		CClientDC dc(pWnd);
		m_imagelist.Draw(&dc, 1, CPoint(cliPoint.X, cliPoint.X), ILD_NORMAL|ILD_TRANSPARENT);
	}

	if(m_segmentlist.GetCount()>0)
	{
		POSITION pos = m_segmentlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			const CSegment* segment = m_segmentlist.GetNext(pos);
			delete segment;
		}
		m_segmentlist.RemoveAll();

		pWnd->Invalidate();
	}

	m_ptEnd = geoPoint;
}

void CRoute::Stop(CWnd* pWnd) const
{
	if(m_Pole.GetSize()>0&&pWnd!=nullptr)
	{
		pWnd->Invalidate();
	}

	m_geoCurrent = CPointF(0.0f, 0.0f);
}

void CRoute::Over(CWnd* pWnd)
{
	m_ptStart = CPointF(0.0f, 0.0f);
	m_ptEnd = CPointF(0.0f, 0.0f);
	m_Pole.RemoveAll();

	if(pWnd!=nullptr)
	{
		pWnd->Invalidate();
	}
}

void CRoute::DrawMark(CWnd* pWnd, const CViewinfo& viewinfo)
{
	CPointF geoPoint;
	CPointF mapPoint;
	CPoint docPoint;
	CPoint cliPoint;

	if(m_ptStart!=CPointF(0.0f, 0.0f))
	{
		Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(m_ptStart);
		cliPoint.X -= 8;
		cliPoint.Y -= 8;

		CClientDC dc(pWnd);
		m_imagelist.Draw(&dc, 0, CPoint(cliPoint.X, cliPoint.X), ILD_NORMAL|ILD_TRANSPARENT);
	}

	if(m_ptEnd!=CPointF(0.0f, 0.0f))
	{
		Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(m_ptEnd);
		cliPoint.X -= 8;
		cliPoint.Y -= 8;

		CClientDC dc(pWnd);
		m_imagelist.Draw(&dc, 1, CPoint(cliPoint.X, cliPoint.X), ILD_NORMAL|ILD_TRANSPARENT);
	}

	for(int index = 0; index<m_Pole.GetSize(); index++)
	{
		const CPointF geoPoint = m_Pole.GetAt(index);
		Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
		cliPoint.X -= 8;
		cliPoint.Y -= 8;

		CClientDC dc(pWnd);
		m_imagelist.Draw(&dc, 2+index, CPoint(cliPoint.X, cliPoint.X), ILD_NORMAL|ILD_TRANSPARENT);
	}
}

void CRoute::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRectF& geoRect)
{
	Gdiplus::Pen* pen = ::new Gdiplus::Pen(Gdiplus::Color(0, 0, 0), 30);
	pen->SetAlignment(Gdiplus::PenAlignmentCenter);

	POSITION pos = m_segmentlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CSegment* segment = m_segmentlist.GetNext(pos);
		segment->Draw(pView, viewinfo, g, pen, geoRect);
	}

	::delete pen;
	pen = nullptr;
}

void CRoute::Move(CWnd* pWnd, const CViewinfo& viewinfo, const double& lng, const double& lat, const double& altitude)
{
	if(pWnd==nullptr)
		return;

	if(m_ptStart==CPointF(0.0f, 0.0f))
		return;

	if(m_ptEnd==CPointF(0.0f, 0.0f))
		return;

	if(lng==0||lat==0)
		return;
	const int nextPole = GetNextPole(lng, lat, altitude);

	CRect cliRect;
	pWnd->GetClientRect(cliRect);

	CClientDC dc(pWnd);
	dc.SetROP2(R2_NOTXORPEN);
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	CPen* oldPen = (CPen*)dc.SelectObject(&pen);

	CPointF geoPoint = CPointF(lng, lat);
	const Gdiplus::PointF cliMoveTo = viewinfo.WGS84ToClient(geoPoint);

	geoPoint = (m_Pole.GetSize()==0||nextPole>m_Pole.GetSize()) ? m_ptEnd : m_Pole.GetAt(nextPole-1);
	const Gdiplus::PointF cliLineTo = viewinfo.WGS84ToClient(geoPoint);
	dc.MoveTo(cliLineTo.X, cliLineTo.Y);

	CRect iconRect = CRect(cliMoveTo.X, cliMoveTo.Y, cliMoveTo.X, cliMoveTo.Y);
	iconRect.InflateRect(20, 20);
	dc.LineTo(cliLineTo.X, cliLineTo.Y);

	iconRect.InflateRect(2, 2);
	dc.Ellipse(iconRect);

	dc.SelectObject(oldPen);
	pen.DeleteObject();

	m_geoCurrent.x = lng;
	m_geoCurrent.y = lat;
}

void CRoute::ViewDrawOver(CWnd* pWnd, const CViewinfo& viewinfo)
{
	ViewWillDraw(pWnd, viewinfo);
}

void CRoute::ViewWillDraw(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(pWnd==nullptr)
		return;

	if(m_ptStart==CPointF(0.0f, 0.0f))
		return;

	if(m_ptEnd==CPointF(0.0f, 0.0f))
		return;

	if(m_geoCurrent==CPointF(0.0f, 0.0f))
		return;

	if(pWnd!=nullptr&&m_geoCurrent!=CPointF(0.0f, 0.0f))
	{
		CRect cliRect;
		pWnd->GetClientRect(cliRect);
		const int nextPole = GetNextPole(m_geoCurrent.x, m_geoCurrent.y, 0);
		CPointF geoPoint = (m_Pole.GetSize()==0||nextPole>m_Pole.GetSize()) ? m_ptEnd : m_Pole.GetAt(nextPole-1);
		const Gdiplus::PointF cliMoveTo = viewinfo.WGS84ToClient(geoPoint);

		geoPoint = m_geoCurrent;
		const Gdiplus::PointF cliLineTo = viewinfo.WGS84ToClient(geoPoint);

		CClientDC dc(pWnd);
		dc.SetROP2(R2_NOTXORPEN);
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		CPen* oldPen = (CPen*)dc.SelectObject(&pen);

		CRect iconRect = CRect(cliLineTo.X, cliLineTo.Y, cliLineTo.X, cliLineTo.Y);
		iconRect.InflateRect(20, 20);
		dc.MoveTo(cliMoveTo.X, cliMoveTo.Y);
		dc.LineTo(cliMoveTo.X, cliMoveTo.Y);

		iconRect.InflateRect(2, 2);
		dc.Ellipse(iconRect);

		dc.SelectObject(oldPen);
		pen.DeleteObject();
	}
}

float CRoute::GetShortcutLength()
{
	if(m_segmentlist.GetCount()>0)
	{
		float fLength = 0.0f;

		POSITION pos = m_segmentlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CSegment* pSegment = m_segmentlist.GetNext(pos);
			fLength += pSegment->m_fLength;
		}

		return fLength;
	}
	else
		return -1.0f;
}

float CRoute::GetBeelineLength(CPointF point1, CPointF point2)
{
	//float length = GCSWGS84::Distance(point1.x,point1.y,point2.x,point2.y);
	//return length;
	return 0;
}

float CRoute::GetBeelineLength()
{
	const float fLength = 0.0f;

	CPointF point1 = m_ptStart;
	for(long i = 0; i<m_Pole.GetSize(); i++)
	{
		const CPointF point2 = m_Pole.GetAt(i);

		//float length = GCSWGS84::Distance(point1.x,point1.y,point2.x,point2.y);//need recover
		//fLength += length;

		point1 = point2;
	}
	CPointF point2 = m_ptEnd;
	//float length = GCSWGS84::Distance(point1.x,point1.y,point2.x,point2.y);//need recover
	//fLength += length;

	return fLength;
}

float CRoute::GetLeftLength()
{
	const float fLength = 0;
	const int nextPole = GetNextPole(m_geoCurrent.x, m_geoCurrent.y, 0);
	CPointF point1 = m_geoCurrent;
	for(long i = nextPole; i<m_Pole.GetSize()+2; i++)
	{
		const CPointF point2 = i==m_Pole.GetSize()+1 ? m_ptEnd : m_Pole.GetAt(i-1);

		//float length = GCSWGS84::Distance(point1.x,point1.y,point2.x,point2.y); //need recover
		//fLength += length;

		point1 = point2;
	}
	return fLength;
}

int CRoute::GetNextPole(const double& lng, const double& lat, const double& altitude)
{
	int nextPole = 1;
	float minimun = 1000000.f;
	for(long index = 0; index<=m_Pole.GetSize(); index++)
	{
		CPointF point1 = index==0 ? m_ptStart : m_Pole.GetAt(index-1);
		CPointF point2 = index==m_Pole.GetSize() ? m_ptEnd : m_Pole.GetAt(index);
		const float length = ::Distance1(CPointF(lng, lat), point1, point2);
		if(length<minimun)
		{
			minimun = length;
			nextPole = index+1;
		}
	}

	return nextPole;
}

bool CRoute::Search(CRoadTopo* pTopology)
{
	return Search(pTopology, m_ptStart, m_ptEnd);
}

bool CRoute::Search(CRoadTopo* pTopology, CPointF point1, CPointF point2)
{
	POSITION pos = m_segmentlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CSegment* segment = m_segmentlist.GetNext(pos);
		delete segment;
	}
	m_segmentlist.RemoveAll();

	if(point1==CPointF(0.0f, 0.0f))
		return false;

	if(point2==CPointF(0.0f, 0.0f))
		return false;

	if(pTopology!=nullptr)
	{
		const DWORD tick1 = GetTickCount();
		const DWORD dwHeadNode = pTopology->CreatStartTousy(point1);
		if(dwHeadNode==-1)
		{
			pTopology->Release();
			return false;
		}
		const DWORD tick2 = GetTickCount();

		CString str;
		str.Format(_T("查找起点到最近距离边花费的时间： %d 秒"), (tick2-tick1)/1000);
		OutputDebugString(str);

		DWORD dwTailNode = pTopology->CreatStopTousy(point2);
		if(dwTailNode==-1)
		{
			pTopology->Release();
			return false;
		}
		const DWORD tick3 = GetTickCount();

		str.Format(_T("查找终点点到最近距离边花费的时间： %d 秒"), (tick3-tick2)/1000);
		OutputDebugString(str);

		//	if()
		Probe* pProbe = pTopology->Dijkstra(dwHeadNode, dwTailNode);
		if(pProbe!=nullptr)
		{
			const DWORD tick4 = GetTickCount();
			str.Format(_T("Dijkstra算法花费的时间： %d 秒"), (tick4-tick3)/1000);
			OutputDebugString(str);

			while(pProbe[dwTailNode].m_dwPreShortestEdge!=-1)
			{
				const DWORD dwEdge = pProbe[dwTailNode].m_dwPreShortestEdge;

				CSegment* segment = new CSegment;
				segment->m_fLength = pTopology->m_pEdges[dwEdge].m_fLength;
				CPoly* poly = nullptr;
				if(dwEdge==pTopology->m_nEdgeCount-1+4)
				{
					poly = pTopology->e_pNewlyPoly;
					pTopology->e_pNewlyPoly = nullptr;
				}
				else if(dwEdge==pTopology->m_nEdgeCount-1+5)
				{
					poly = pTopology->e_pCutedPoly1;
					pTopology->e_pCutedPoly1 = nullptr;
				}
				else if(dwEdge==pTopology->m_nEdgeCount-1+6)
				{
					poly = pTopology->e_pCutedPoly2;
					pTopology->e_pCutedPoly2 = nullptr;
				}
				else if(dwEdge==pTopology->m_nEdgeCount-1+1)
				{
					poly = pTopology->s_pNewlyPoly;
					pTopology->s_pNewlyPoly = nullptr;
				}
				else if(dwEdge==pTopology->m_nEdgeCount-1+2)
				{
					poly = pTopology->s_pCutedPoly1;
					pTopology->s_pCutedPoly1 = nullptr;
				}
				else if(dwEdge==pTopology->m_nEdgeCount-1+3)
				{
					poly = pTopology->s_pCutedPoly2;
					pTopology->s_pCutedPoly2 = nullptr;
				}
				else
				{
					poly = pTopology->GetEdgePoly(dwEdge);
				}

				segment->m_pPoly = poly;

				if(poly!=nullptr)
				{
					if(segment->m_fLength>1.0f)
					{
						long length = poly->GetLength(1000/2.f);

						length -= (0.5*length)/segment->m_fLength;
						unsigned int tAnchor;
						double et;
						poly->GetAandT(1, 0, length, tAnchor, et);
						segment->m_Prompt = poly->GetPoint(tAnchor, et);
					}
					else if(segment->m_fLength>0.4)
					{
						long length = poly->GetLength(1000/2.f);

						length -= (0.2*length)/segment->m_fLength;
						unsigned int tAnchor;
						double et;
						poly->GetAandT(1, 0, length, tAnchor, et);
						segment->m_Prompt = poly->GetPoint(tAnchor, et);
					}
					else if(segment->m_fLength>0.2)
					{
						long length = poly->GetLength(1000/2.f);

						length -= (0.1*length)/segment->m_fLength;
						unsigned int tAnchor;
						double et;
						poly->GetAandT(1, 0, length, tAnchor, et);
						segment->m_Prompt = poly->GetPoint(tAnchor, et);
					}
					else
					{
						segment->m_bPromted = true;
					}
					/*
										if(m_segmentlist.GetCount() > 0)
										{
											CSegment* seg = m_segmentlist.GetHead();

											CPoly* nextPoly = seg->pPoly;
											if(nextPoly != nullptr)
											{
												CPoly* poly = pTopology->GetPoly(layerlist,dwEdge);
												CPoint corner = poly->GetAnchor(poly->m_nAnchors);

												CPoint b = nextPoly->GetSample(+1,corner,500);
												CPoint a = poly->GetSample(-1,corner,500);

												double angle = GetInclination(b,corner,corner,a);
												if(std::abs(angle)>0.05)
												{	//0为有路口直行,1为左拐,2为右拐,3为左前方,4为右前方,5为终点,6为途经地点
													if(angle>0.79)
														segment->m_nTurnType = 1;//左拐
													else if(angle<=0.79 && angle>0)
														segment->m_nTurnType = 3;//左前方
													else if(angle<-0.79)
														segment->m_nTurnType = 2;//右拐
													else if(angle>=-0.79 && angle<0)
														segment->m_nTurnType = 4;//右前方
												}
												else
													segment->m_nTurnType = 0;//前方直行
											}
										}
										else
										{
											segment->m_nTurnType = 5;
										}*/
				}

				m_segmentlist.AddHead(segment);
				dwTailNode = pTopology->m_pEdges[dwEdge].m_indexFromNode==dwTailNode ? pTopology->m_pEdges[dwEdge].m_indexToNode : pTopology->m_pEdges[dwEdge].m_indexFromNode;
			}
			const DWORD tick5 = GetTickCount();
			str.Format(_T("得到结果花费的时间： %d 秒"), (tick5-tick4)/1000);
			OutputDebugString(str);

			delete[] pProbe;

			pTopology->Release();
			return true;
		}
		else
		{
			pTopology->Release();
			AfxMessageBox(_T("没有找到路径"));
			return false;
		}
	}
	else
	{
		return false;
	}
}
