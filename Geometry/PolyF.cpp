#include "stdafx.h"
#include "PolyF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPolyF, CObject, 0)

CPolyF::CPolyF()
{
	m_pPoints = nullptr;
	m_nAnchors = 0;
	m_nAllocs = 0;
	m_rect = CRectF(32768, 32768, -32768, -32768);
}

CPolyF::~CPolyF()
{
	delete[] m_pPoints;
	m_nAnchors = 0;
	m_nAllocs = 0;
}

void CPolyF::RecalRect()
{
	if(m_nAnchors==0)
	{
		m_rect.SetRectEmpty();
		return;
	}
	if(m_pPoints==nullptr)
	{
		m_rect.SetRectEmpty();
		return;
	}

	m_rect = CRectF(m_pPoints[0].x, m_pPoints[0].y, m_pPoints[0].x, m_pPoints[0].y);

	for(unsigned int i = 1; i<=m_nAnchors; i++)
	{
		if(m_pPoints[i-1].x<m_rect.left)
			m_rect.left = m_pPoints[i-1].x;
		if(m_pPoints[i-1].x>m_rect.right)
			m_rect.right = m_pPoints[i-1].x;
		if(m_pPoints[i-1].y<m_rect.top)
			m_rect.top = m_pPoints[i-1].y;
		if(m_pPoints[i-1].y>m_rect.bottom)
			m_rect.bottom = m_pPoints[i-1].y;
	}
}

CPointF& CPolyF::GetAnchor(const unsigned int& nAnchor) const
{
	ASSERT(nAnchor>=1&&nAnchor<=m_nAnchors);
	return m_pPoints[nAnchor-1];
}

void CPolyF::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	m_rect.Serialize(ar);

	if(ar.IsStoring())
	{
		ar << m_nAnchors;
		ar.Write(m_pPoints, m_nAnchors*sizeof(CPointF));
	}
	else
	{
		ar >> m_nAnchors;
		m_pPoints = new CPointF[m_nAnchors];
		ar.Read(m_pPoints, m_nAnchors*sizeof(CPointF));
		m_nAllocs = m_nAnchors;
	}
}


bool CPolyF::PickOn(const CPointF& point, const double& gap) const
{
	CRectF rect = m_rect;
	rect.InflateRect(gap, gap);
	if(rect.PtInRect(point)==false)
		return false;

	for(unsigned int i = 1; i<m_nAnchors; i++)
	{
		const CPointF& fpoint = m_pPoints[i-1];
		const CPointF& tpoint = m_pPoints[i+1-1];

		return true;
	}

	return false;
}

bool CPolyF::PickIn(const CPointF& point) const
{
	if(m_rect.PtInRect(point)==false)
		return false;

	long odd = 0;
	for(unsigned int I = 0; I<m_nAnchors-1; I++)
	{
		if(point.x>m_pPoints[I].x&& point.x>m_pPoints[I+1].x)
			continue;

		if(point==m_pPoints[I])
			return true;
		const long a1 = point.y>m_pPoints[I].y ? 1 : (point.y==m_pPoints[I].y ? 0 : -1);
		const long a2 = point.y>m_pPoints[I+1].y ? 1 : (point.y==m_pPoints[I+1].y ? 0 : -1);
		const long product = a1*a2;

		if(product>0)
			continue;

		if(product==0)
		{
			if((m_pPoints[I].y-point.y)+(m_pPoints[I+1].y-point.y)>=0)
			{
				if(m_pPoints[I].y==point.y&&point.x<m_pPoints[I].x)
					odd++;
				if(m_pPoints[I+1].y==point.y&&point.x<m_pPoints[I+1].x)
					odd++;
			}
		}
		else
		{
			const double xtemp = m_pPoints[I].x+(point.y-m_pPoints[I].y)*(m_pPoints[I+1].x-m_pPoints[I].x)/(float)(m_pPoints[I+1].y-m_pPoints[I].y);

			if(xtemp==point.x)
				return true;

			if(xtemp>point.x)
				odd++;
		}
	}

	odd = odd%2;
	if(odd==1)
		return true;
	else
		return false;
}

void CPolyF::AddAnchor(const CPointF& point)
{
	if(m_nAnchors>=m_nAllocs)
	{
		CPointF* newPoints = new CPointF[m_nAllocs+10];
		if(m_pPoints!=nullptr)
		{
			memcpy(newPoints, m_pPoints, sizeof(CPointF)*m_nAllocs);
			delete[] m_pPoints;
		}
		m_pPoints = newPoints;
		m_nAllocs += 10;
	}

	if(m_nAnchors>=1&&point==m_pPoints[m_nAnchors-1])
		return;

	m_pPoints[m_nAnchors] = point;
	m_nAnchors++;

	if(point.x<m_rect.left)
		m_rect.left = point.x;
	else if(point.x<m_rect.right)
		m_rect.right = point.x;

	if(point.y<m_rect.top)
		m_rect.top = point.y;
	else if(point.y<m_rect.bottom)
		m_rect.bottom = point.y;
}

void CPolyF::SerializeList(CArchive& ar, CObList& oblist)
{
	oblist.Serialize(ar);
}
