#pragma once
#include "Geom.h"

namespace Arcinfo
{
	struct Arc
	{
	public:
		DWORD m_dwId;
		CRect m_Rect;
		unsigned int m_nAnchors;
		CPoint* m_pPoints;
		Arc()
		{
			m_dwId=0;
			m_nAnchors=0;
			m_pPoints=nullptr;
		}
		~Arc()
		{
			if(m_pPoints != nullptr)
			{
				delete[] m_pPoints;
			}
		}
	};
}
class AFX_EXT_CLASS Polyline : public CGeom
{
public:
	DWORD m_dwId;
	CRect m_Rect;
	unsigned int m_nArcs;
	Arc* m_pArcs;
	Polyline()
	{
		m_dwId=0;
		m_nArcs=0;
		m_pArcs=nullptr;
	}
	~Polyline()
	{
		if(m_pArcs != nullptr)
		{
			delete[] m_pArcs;
		}
		m_pArcs=nullptr;
	}
};

class AFX_EXT_CLASS Polygon : public Polyline
{
};