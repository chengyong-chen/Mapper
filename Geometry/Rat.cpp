#include "stdafx.h"
#include "Global.h"

#include <math.h>
#include "Tag.h"
#include "Geom.h"
#include "Rat.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CRat, CGeom, 0)

CRat::CRat()
{
	m_bType = 100;
	m_bClosed = false;
}

CRat::CRat(const CRect& rect)  
	:CGeom(rect)
{
	m_bType = 100;
	m_bClosed = false;
}

CRat::~CRat()
{	
	POSITION pos = m_polylist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPoly* poly = m_polylist.GetNext(pos);
		if(poly != nullptr)
		{
			delete poly;
			poly = nullptr;
		}
	}
	m_polylist.RemoveAll();
}

void CRat::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar);
	
	if(ar.IsStoring())
	{
	}
	else
	{
	}
		
	m_polylist.Serialize(ar);	
}

DWORD CRat::PackPC(CFile* pFile,BYTE*& bytes)
{
	DWORD size = CGeom::PackPC(pFile,bytes);
	
	if(pFile != nullptr)
	{
		int nCount = m_polylist.GetCount();
		pFile->Write(&nCount,sizeof(int));
		size += sizeof(int);
		
		POSITION pos = m_polylist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPoly* poly = m_polylist.GetNext(pos);
			if(poly != nullptr)
			{
				size += poly->PackPC(pFile,bytes);
			}
		}

		return size;
	}
	else
	{	
		POSITION pos = m_polylist.GetHeadPosition();
		while(pos != nullptr)
		{
			CPoly* poly = m_polylist.GetNext(pos);
			if(poly != nullptr)
			{
				poly->m_pPoints = nullptr;
				delete poly;
				poly = nullptr;
			}
		}
		m_polylist.RemoveAll();

		int nCount = *(int*)bytes;
		bytes += sizeof(int);

		for(int i=0;i<nCount;i++)
		{
			CPoly* poly = new CPoly;
			m_polylist.AddTail(poly);
			
			poly->PackPC(nullptr,bytes);
		}
	}
}

DWORD CRat::ReleaseCE(CFile& file,BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file,type);
	
	int nCount = m_polylist.GetCount();
	file.Write(&nCount,sizeof(int));
	size += sizeof(int);
	
	POSITION pos2 = m_polylist.GetHeadPosition();
	while(pos2 != nullptr)
	{
		CPoly* poly = m_polylist.GetNext(pos2);
		BYTE del;
		size += poly->ReleaseCE(file,del);
	}

	return size;
}

void CRat::ReleaseWeb(CFile& file)
{
	CGeom::ReleaseWeb(file);
}
void CRat::ReleaseWeb(boost::json::object& json)
{
	CGeom::ReleaseWeb(json);
}
void CRat::ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const
{
	CGeom::ReleaseWeb(json, offset);
}
void CRat::Draw(Gdiplus::Graphics& g,CViewinfo& viewinfo,CLibrary& library,Context& context) 
{		
	if(pen != nullptr)
	{
		POSITION pos = m_polylist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPoly* poly = (CPoly*)m_polylist.GetNext(pos);
			poly->Draw(g,library,fZoom);
		}
	}
}
