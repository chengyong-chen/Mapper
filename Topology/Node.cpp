#include "stdafx.h"
#include "Node.h"
#include "Edge.h"
#include "Topo.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Global.h"

#include "../DataView/viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CNode::CNode()
{
	m_dwId=1;

	m_bModified=false;
}

CNode::~CNode()
{
	m_edgelist.RemoveAll();
}

CNode* CNode::Clone()
{
	CNode* pNode=new CNode();

	pNode->m_dwId=m_dwId;
	pNode->m_Point=m_Point;

	const BYTE nCount=m_edgelist.GetSize();
	for(int i=0; i < nCount; i++)
	{
		const DWORD dwId=m_edgelist.GetAt(i);
		pNode->m_edgelist.Add(dwId);
	}
	return pNode;
}

void CNode::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_dwId;
		ar << m_Point.x;
		ar << m_Point.y;
	}
	else
	{
		ar >> m_dwId;
		ar >> m_Point.x;
		ar >> m_Point.y;
	}

	m_edgelist.Serialize(ar);
}
void CNode::ReleaseWeb(pbf::writer& writer) const
{

}
void CNode::ExportPC1(CFile& file, DWORD& position) const
{
	file.Write(&m_Point, sizeof(CPoint));
	file.Write(&position, sizeof(DWORD));
	const BYTE nCount=m_edgelist.GetSize();
	file.Write(&nCount, sizeof(BYTE));

	BYTE Complement[3];
	Complement[0]=0X00;
	Complement[1]=0X00;
	Complement[2]=0X00;
	file.Write(Complement, sizeof(BYTE)*3);

	position+=nCount*sizeof(DWORD);
}

void CNode::ExportPC2(std::map<DWORD, DWORD>& mapIdtoIndexEdge, CFile& file) const
{
	const BYTE nCount=m_edgelist.GetSize();
	for(int i=0; i < nCount; i++)
	{
		DWORD dwId=m_edgelist.GetAt(i);

		DWORD dwEdgeIndex=mapIdtoIndexEdge.find(dwId) != mapIdtoIndexEdge.end() ? mapIdtoIndexEdge[dwId] : 0XFFFFFFFF;
		if(dwEdgeIndex != 0XFFFFFFFF)
		{
			file.Write(&dwEdgeIndex, sizeof(DWORD));
		}
		else
		{
			AfxMessageBox(_T("�����ļ��п��ܴ������⣬�ڵ�����ıߴ����ڱ������в�����"));
			file.Write(&dwEdgeIndex, sizeof(DWORD));
		}
	}
}

void CNode::ReplaceRelatingEdge(DWORD dwId1, DWORD dwId2)
{
	const long nCount=m_edgelist.GetSize();
	for(int index=0; index < nCount; index++)
	{
		const DWORD dwId=m_edgelist.GetAt(index);
		if(dwId == dwId1)
		{
			m_edgelist.SetAt(index, dwId2);
			m_bModified=true;
			break;
		}
	}
}

void CNode::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, Gdiplus::Brush* brush, CSize& radius, CRect inRect, const CTopology& topology)
{
	const Gdiplus::Point cliPoint=viewinfo.DocToClient<Gdiplus::Point>(m_Point);
	CRect cliRect(cliPoint.X, cliPoint.Y, cliPoint.X, cliPoint.Y);
	cliRect.InflateRect(radius.cx, radius.cy);
	CRect docRect=viewinfo.ClientToDoc(cliRect);
	if(docRect.IntersectRect(inRect, docRect) == FALSE)
		return;

	g.FillEllipse(brush, Gdiplus::Rect(cliRect.left, cliRect.top, cliRect.Width(), cliRect.Height()));

	Gdiplus::StringFormat stringFormat;
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	stringFormat.SetTrimming(Gdiplus::StringTrimmingNone);
	const CString strFont(_T("Arial"));
	const _bstr_t btrFont(strFont);
	const Gdiplus::FontFamily fontFamily(btrFont);
	::SysFreeString(btrFont);
	const Gdiplus::Font* font=::new Gdiplus::Font(&fontFamily, 8*viewinfo.m_sizeDPI.cx/72.f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	const unsigned int nRadius=viewinfo.ClientToDoc(Gdiplus::SizeF(15, 15)).cx;//20 pixels
	const long nCount=m_edgelist.GetSize();
	for(int index=0; index < nCount; index++)
	{
		const DWORD dwEdge=m_edgelist.GetAt(index);
		CEdge* pEdge=topology.GetEdge(dwEdge);
		CPoly* poly=topology.GetEdgePoly(pEdge->m_dwId);
		if(poly != nullptr)
		{
			CPoint sample;
			if(pEdge->m_dwFromNode == m_dwId)
			{
				sample=poly->GetSample(1, m_Point, nRadius);
			}
			else
			{
				sample=poly->GetSample(-1, m_Point, nRadius);
			}
			char str[3];
			itoa(index + 1, str, 10);
			_bstr_t bstr(str);
			const Gdiplus::Point cliPoint=viewinfo.DocToClient<Gdiplus::Point>(sample);
			g.DrawString(bstr, -1, font, Gdiplus::PointF(cliPoint.X, cliPoint.Y - font->GetSize()/2.f), &stringFormat, brush);
		}
	}
	::delete font;
}

void CNode::DrawActive(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CTopology& topology, CSize radius)
{
	const Gdiplus::Brush* brush=::new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0));

	g.FillEllipse(brush, Gdiplus::RectF(m_Point.x - radius.cx, m_Point.y - radius.cy, radius.cx*2, radius.cy*2));

	::delete brush;
	brush=nullptr;
}

void CNode::CenterPoint(const CTopology& topology)
{
	CRect rect;
	rect.SetRectEmpty();
	const long nCount=m_edgelist.GetSize();
	for(int index=0; index < nCount; index++)
	{
		const DWORD dwEdge=m_edgelist.GetAt(index);
		CEdge* pEdge=topology.GetEdge(dwEdge);

		if(pEdge != nullptr)
		{
			CPoint point;

			if(pEdge->m_dwFromNode == m_dwId)
				point=pEdge->GetStartPoint(topology);
			else
				point=pEdge->GetEndPoint(topology);

			rect=UnionRect(rect, CRect(point, CSize(1, 1)));
		}
	}
	m_Point.x=rect.left + rect.Width()/2;
	m_Point.x=rect.top + rect.Height()/2;
}

void CNode::SortEdge(const CTopology& topology, int radius)
{
	std::list<CNode::EdgeInfo*> edgeinfolist;
	const long nCount=m_edgelist.GetSize();
	for(int index=0; index < nCount; index++)
	{
		const DWORD dwEdge=m_edgelist.GetAt(index);
		CEdge* edge=topology.GetEdge(dwEdge);
		CPoly* poly=topology.GetEdgePoly(edge->m_dwId);
		CPoint sample;
		if(edge->m_dwFromNode == m_dwId)
			sample=poly->GetSample(1, m_Point, radius);
		else
			sample=poly->GetSample(-1, m_Point, radius);

		sample=sample - m_Point;
		const float Qi=EvaluatQi(sample);

		CNode::EdgeInfo* edgeinfo=new CNode::EdgeInfo();
		edgeinfo->edge=edge;
		edgeinfo->Qi=Qi;
		auto it=std::find_if(edgeinfolist.begin(), edgeinfolist.end(), [&Qi](const CNode::EdgeInfo* s){ return Qi > s->Qi; });
		if(it != edgeinfolist.end())
			edgeinfolist.insert(it, edgeinfo);
		else
			edgeinfolist.push_back(edgeinfo);
	}

	m_edgelist.RemoveAll();
	for(std::list<EdgeInfo*>::iterator it=edgeinfolist.begin(); it != edgeinfolist.end(); it++)
	{
		EdgeInfo* edgeinfo=*it;
		m_edgelist.Add(edgeinfo->edge->m_dwId);
		delete edgeinfo;
		edgeinfo=nullptr;
	}
	edgeinfolist.clear();
}

BYTE CNode::GetEdgeIndex(const CEdge* pEdge) const
{
	const long nCount=m_edgelist.GetSize();
	for(BYTE index=0; index < nCount; index++)
	{
		const DWORD dwEdge=m_edgelist.GetAt(index);
		if(pEdge->m_dwId == dwEdge)
			return index;
	}

	return -1;
}

BYTE CNode::GetEdgeIndex(const DWORD& dwEdge) const
{
	const long nCount=m_edgelist.GetSize();
	for(BYTE index=0; index < nCount; index++)
	{
		const DWORD edge=m_edgelist.GetAt(index);
		if(edge == dwEdge)
			return index;
	}

	return -1;
}

DWORD CNode::GetNextEdge(const DWORD& dwEdge) const
{
	std::list<DWORD> valid;
	for(int index=0; index < m_edgelist.GetSize(); index++)
	{
		DWORD edge=m_edgelist.GetAt(index);
		std::list<DWORD>::iterator it=std::find(valid.begin(), valid.end(), edge);
		if(it != valid.end())
			valid.erase(it);
		else
			valid.push_back(edge);
	}
	for(std::list<DWORD>::iterator it=valid.begin(); it != valid.end(); it++)
	{
		if(*it == dwEdge)
		{
			const std::list<DWORD>::iterator next=++it;
			if(next == valid.end())
				return valid.front();
			else
				return *next;
		}
	}
	return -1;
}

float CNode::EvaluatQi(const CPoint& point) const
{
	float Qi;
	float dx;
	float dy;

	dx=(float)point.x;
	dy=(float)point.y;

	if(dy > 0)
	{
		if(dx > 0)
		{
			if(dy > dx)
				Qi=dx/dy;
			else
				Qi=2 - dy/dx;
		}
		else
		{
			if(dy > std::abs(dx))
				Qi=8 + dx/dy;
			else
				Qi=6 - dy/dx;
		}
	}
	else
	{
		if(dx > 0)
		{
			if(dx > std::abs(dy))
				Qi=2 - dy/dx;
			else
				Qi=4 + dx/dy;
		}
		else
		{
			if(dy > dx)
				Qi=6 - dy/dx;
			else
				Qi=4 + dx/dy;
		}
	}

	return Qi;
}

void CNode::RemoveRelatingEdge(const DWORD& dwEdge)
{
	const int nCount=m_edgelist.GetSize();
	for(int index=0; index < nCount; index++)
	{
		const DWORD dwId=m_edgelist.GetAt(index);
		if(dwEdge == dwId)
		{
			m_edgelist.RemoveAt(index);
			m_bModified=true;
			break;
		}
	}
}

void CNode::InsertRelatingEdge(const DWORD& dwEdge)
{
	m_edgelist.Add(dwEdge);

	m_bModified=true;
}

bool CNode::Pick(const CViewinfo& viewinfo, const CPoint& cliPoint, int radius) const
{
	const Gdiplus::Point cliNode=viewinfo.DocToClient<Gdiplus::Point>(m_Point);
	const double cx=cliPoint.x - cliNode.X;
	const double cy=cliPoint.y - cliNode.Y;
	if(sqrt(cx*cx + cy*cy) < radius)
		return true;
	else
		return false;
}

void CNode::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const CTopology& topology) const
{
	if(pWnd != nullptr)
	{
		const Gdiplus::Point point=viewinfo.DocToClient<Gdiplus::Point>(m_Point);
		CRect rect(CPoint(point.X, point.Y), CSize(0, 0));
		rect.InflateRect(4 + 1, 4 + 1);
		pWnd->InvalidateRect(rect, TRUE);
	}
}
