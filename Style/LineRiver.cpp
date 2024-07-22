#include "stdafx.h"

#include "Dash.h"
#include "Line.h"
#include "LineRiver.h"
#include "Library.h"
#include "ColorSpot.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Dataview/viewinfo.h"
#include "../Public/BinaryStream.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLineRiver, CLine, 0)

CLineRiver::CLineRiver()
	: CLine()
{
	m_nFWidth = 3;
	m_nTWidth = 10;
	m_nTrend = 1;
	m_nWidth = m_nFWidth;
}

CLineRiver::~CLineRiver()
{
}
void CLineRiver::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CLine::Sha1(sha1);

	sha1.process_bytes(&m_nFWidth,sizeof(unsigned short));
	sha1.process_bytes(&m_nTWidth, sizeof(unsigned short));
	sha1.process_bytes(&m_nTrend, sizeof(unsigned short));
	sha1.process_bytes(&m_nWidth, sizeof(unsigned short));
}
BOOL CLineRiver::operator==(const CLine& line) const
{
	if(CLine::operator==(line)==FALSE)
		return FALSE;
	else if(line.IsKindOf(RUNTIME_CLASS(CLineRiver))==FALSE)
		return FALSE;
	else if(m_nFWidth!=((CLineRiver*)&line)->m_nFWidth)
		return FALSE;
	else if(m_nTWidth!=((CLineRiver*)&line)->m_nTWidth)
		return FALSE;
	else if(m_nTrend!=((CLineRiver*)&line)->m_nTrend)
		return FALSE;
	else
		return TRUE;
}

void CLineRiver::CopyTo(CLine* line) const
{
	CLine::CopyTo(line);

	if(line->IsKindOf(RUNTIME_CLASS(CLineRiver))==TRUE)
	{
		((CLineRiver*)line)->m_nFWidth = m_nFWidth;
		((CLineRiver*)line)->m_nTWidth = m_nTWidth;
		((CLineRiver*)line)->m_nTrend = m_nTrend;
	}
}
void CLineRiver::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fScale) const
{
	const long length = pPath->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc/2);
	const long n = (m_nTWidth-m_nFWidth+1)/m_nTrend;
	const float f = (float)length*2/(n*(n+1));

	unsigned int fAnchor = 1;
	unsigned int tAnchor = 1;
	double st = 0;
	double et = 0;

	for(unsigned int i = 0; i<n; i++)
	{
		m_nWidth = m_nFWidth+i*m_nTrend;

		Gdiplus::Pen* pen = this->GetPen(1.0f, viewinfo.m_sizeDPI);
		if(pen!=nullptr)
		{
			const float seglen = (i+1)*f;
			if(pPath->GetAandT(fAnchor, st, seglen, tAnchor, et)==true)
			{
				pPath->DrawFATToTAT(g, viewinfo, pen, fAnchor, st, tAnchor, et);
			}
			fAnchor = tAnchor;
			st = et;

			::delete pen;
			pen = nullptr;
		}
	}

	m_nWidth = 0;
}
void CLineRiver::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fScale) const
{
	auto pOldColor = this->m_pColor;	
	const_cast<CColor*&>(this->m_pColor) = CColorSpot::White();
	
	CLineRiver::Draw(g, viewinfo, library, pPath, fScale);

	const_cast<CColor*&>(this->m_pColor) = pOldColor;
}
void CLineRiver::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CLine::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_nFWidth;
		ar<<m_nTWidth;
		ar<<m_nTrend;
	}
	else
	{
		ar>>m_nFWidth;
		ar>>m_nTWidth;
		ar>>m_nTrend;
	}
}

void CLineRiver::ReleaseCE(CArchive& ar) const
{
	CLine::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_nFWidth;
		ar<<m_nTWidth;
		ar<<m_nTrend;
	}
}

void CLineRiver::ReleaseDCOM(CArchive& ar)
{
	CLine::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_nFWidth;
		ar<<m_nTWidth;
		ar<<m_nTrend;
	}
	else
	{
		ar>>m_nFWidth;
		ar>>m_nTWidth;
		ar>>m_nTrend;
	}
}

DWORD CLineRiver::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CLine::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_wColorId, sizeof(WORD));
		pFile->Write(&m_wDashId, sizeof(WORD));

		pFile->Write(&m_nFWidth, sizeof(unsigned short));
		pFile->Write(&m_nTWidth, sizeof(unsigned short));
		pFile->Write(&m_nTrend, sizeof(unsigned short));

		size += sizeof(WORD);
		size += sizeof(WORD);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);

		return size;
	}
	else
	{
		m_wColorId = *(WORD*)bytes;
		bytes += sizeof(WORD);

		m_wDashId = *(WORD*)bytes;
		bytes += sizeof(WORD);

		m_nFWidth = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_nTWidth = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_nTrend = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CLineRiver::ReleaseWeb(CFile& file) const
{
	CLine::ReleaseWeb(file);

	unsigned short nFWidth = m_nFWidth;
	ReverseOrder(nFWidth);
	file.Write(&nFWidth, sizeof(unsigned short));

	unsigned short nTWidth = m_nTWidth;
	ReverseOrder(nTWidth);
	file.Write(&nTWidth, sizeof(unsigned short));

	unsigned short nStep = m_nTrend;
	ReverseOrder(nStep);
	file.Write(&nStep, sizeof(unsigned short));
}

void CLineRiver::ReleaseWeb(BinaryStream& stream) const
{
	CLine::ReleaseWeb(stream);

	stream<<m_nFWidth;
	stream<<m_nTWidth;
	stream<<m_nTrend;
}
void CLineRiver::ReleaseWeb(boost::json::object& json) const
{
	CLine::ReleaseWeb(json);

	json["TWidth"] = m_nTWidth;
	json["FWidth"] = m_nFWidth;
	json["Trend"] = m_nTrend;
}
void CLineRiver::ReleaseWeb(pbf::writer& writer) const
{
	CLine::ReleaseWeb(writer);
	
	writer.add_variant_uint16(m_nTWidth);
	writer.add_variant_uint16(m_nFWidth);
	writer.add_variant_uint16(m_nTrend);
}
void CLineRiver::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const
{
	if(pPath ==nullptr)
		return;

	CPoly* pPoly = pPath->GetPoly();
	if(pPoly==nullptr)
		return;
	const long length = pPoly->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc/2);
	const long n = (m_nTWidth-m_nFWidth+1)/m_nTrend;
	const float f = (float)length*2/(n*(n+1));

	unsigned int fAnchor = 1;
	unsigned int tAnchor = 1;
	double st = 0;
	double et = 0;

	for(unsigned int i = 0; i<n; i++)
	{
		m_nWidth = m_nFWidth+i*m_nTrend;
		_ftprintf(file, _T("%g w\n"), m_nWidth/10.f);

		if(i==n-1)
		{
			tAnchor = pPoly->m_bClosed==true ? pPoly->GetAnchors() : pPoly->GetAnchors()-1;
			et = 1;
		}
		else
		{
			const float seglen = (i+1)*f;

			if(pPoly->GetAandT(fAnchor, st, seglen, tAnchor, et)==false)
				break;
		}

		CPoly* seg = pPoly->GetByFHTToTHT(fAnchor, st, tAnchor, et);
		if(seg!=nullptr)
		{
			seg->ExportPs3(file, this, viewinfo, aiKey);
			delete seg;
		}

		fAnchor = tAnchor;
		st = et;
	}

	delete pPoly;
}
