#include "stdafx.h"

#include "Line.h"
#include "LineEmpty.h"
#include "LineNormal.h"
#include "LineSymbol.h"
#include "LineRiver.h"
#include "Psboard.h"
#include "Color.h"
#include "../Dataview/viewinfo.h"
#include "../Pbf/writer.hpp"
#include "../Public/BinaryStream.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLine, CObject, 0)

CLine::CLine()
{
	m_bCapindex = 0;
	m_bJoinindex = 1;
	m_bMiterlimit = 30;
	m_nWidth = 3;
	m_bOverprint = false;
	m_pColor = nullptr;
}

CLine::CLine(CColor* pColor, unsigned short nWidth)
	:m_pColor(pColor), m_nWidth(nWidth)
{
}

CLine::~CLine()
{
	delete m_pColor;
	m_pColor=nullptr;
}
void CLine::Sha1(boost::uuids::detail::sha1::digest_type& hash) const
{
	boost::uuids::detail::sha1 sha1;
	this->Sha1(sha1);
	sha1.get_digest(hash);
}
void CLine::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_byte(Gettype());
	
	if(m_pColor!=nullptr)
	{
		m_pColor->Sha1(sha1);
	}
	sha1.process_bytes(&m_nWidth,sizeof(unsigned short));
	m_dash.Sha1(sha1);
	sha1.process_byte(m_bCapindex);
	sha1.process_byte(m_bJoinindex);
	sha1.process_byte(m_bMiterlimit);
	sha1.process_byte(m_bOverprint);
}
void CLine::Scale(float ratio)
{
	m_nWidth *= ratio;
	m_dash.Scale(ratio);
}

BOOL CLine::operator==(const CLine& line) const
{
	CRuntimeClass* pRuntimeClass1 = this->GetRuntimeClass();
	CRuntimeClass* pRuntimeClass2 = line.GetRuntimeClass();
	if(pRuntimeClass1!=pRuntimeClass2)
		return FALSE;
	else if(m_pColor!=nullptr&&line.m_pColor!=nullptr&&*m_pColor!=*line.m_pColor)
		return FALSE;
	else if(m_dash!=line.m_dash)
		return FALSE;
	else if(m_nWidth!=line.m_nWidth)
		return FALSE;
	else if(m_bCapindex!=line.m_bCapindex)
		return FALSE;
	else if(m_bJoinindex!=line.m_bJoinindex)
		return FALSE;
	else if(m_bMiterlimit!=line.m_bMiterlimit)
		return FALSE;
	else if(m_bOverprint!=line.m_bOverprint)
		return FALSE;
	else
		return TRUE;
}

BOOL CLine::operator !=(const CLine& line) const
{
	return !(*this==line);
}

CLine* CLine::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CLine* pClone = (CLine*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	return pClone;
}

void CLine::CopyTo(CLine* line) const
{
	delete line->m_pColor;
	line->m_pColor = nullptr;
	if(m_pColor!=nullptr)
	{
		line->m_pColor = m_pColor->Clone();
	}

	line->m_dash = m_dash;
	line->m_nWidth = m_nWidth;

	line->m_bCapindex = m_bCapindex;
	line->m_bJoinindex = m_bJoinindex;
	line->m_bMiterlimit = m_bMiterlimit;
	line->m_bOverprint = m_bOverprint;
}

Gdiplus::Pen* CLine::GetPen(const float& fScale, const CSize& dpi) const
{
	if(m_nWidth==0)
	{
		return nullptr;
	}
	else
	{
		const Gdiplus::SizeF pixels = CViewinfo::PointsToPixels(m_nWidth*fScale/10.0f, dpi);
		const Gdiplus::Color color = m_pColor==nullptr ? Gdiplus::Color(255, 0, 0, 0) : m_pColor->GetColor();
		Gdiplus::Pen* pen = ::new Gdiplus::Pen(color, pixels.Width);
		pen->SetAlignment(Gdiplus::PenAlignmentCenter);

		switch(m_bCapindex)
		{
		case 0:
			pen->SetStartCap(Gdiplus::LineCapFlat);
			pen->SetEndCap(Gdiplus::LineCapFlat);
			pen->SetDashCap(Gdiplus::DashCap::DashCapFlat);
			break;
		case 1:
			pen->SetStartCap(Gdiplus::LineCapRound);
			pen->SetEndCap(Gdiplus::LineCapRound);
			pen->SetDashCap(Gdiplus::DashCap::DashCapRound);
			break;
		case 2:
			pen->SetStartCap(Gdiplus::LineCapSquare);
			pen->SetEndCap(Gdiplus::LineCapSquare);
			break;
		}

		switch(m_bJoinindex)
		{
		case 0:
			pen->SetLineJoin(Gdiplus::LineJoinBevel);
			break;
		case 1:
			pen->SetLineJoin(Gdiplus::LineJoinRound);
			break;
		case 2:
			pen->SetLineJoin(Gdiplus::LineJoinMiter);
			break;
		case 3:
			pen->SetLineJoin(Gdiplus::LineJoinMiterClipped);
			break;
		}
		const int nDashCount = m_dash.GetCount();
		if(nDashCount>0)
		{
			Gdiplus::REAL segment[8];
			for(int index = 0; index<nDashCount; index++)
			{
				segment[index] = CViewinfo::PointsToPixels(m_dash.m_segment[index]*fScale/10.f/pixels.Width, dpi).Width;
				if(segment[index]<0.00001)
				{
					segment[index] = 0.01;
				}
			}
			pen->SetDashStyle(Gdiplus::DashStyleCustom);
			pen->SetDashPattern(segment, nDashCount);
		}
		else
		{
			pen->SetDashStyle(Gdiplus::DashStyleSolid);
		}

		return pen;
	}
}

bool CLine::IsSolid() const
{
	return 	m_dash.GetCount()==0;
}

void CLine::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	m_dash.Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_nWidth;
		ar<<m_bCapindex;
		ar<<m_bJoinindex;
		ar<<m_bMiterlimit;
		const BYTE colortype = m_pColor==nullptr ? 0XFF : m_pColor->Gettype();
		ar<<colortype;
		ar<<m_bOverprint;
	}
	else
	{
		ar>>m_nWidth;

		ar>>m_bCapindex;
		ar>>m_bJoinindex;
		ar>>m_bMiterlimit;

		char colorindex = -1;
		ar>>colorindex;
		delete m_pColor;
		m_pColor = CColor::Apply(colorindex);
		ar>>m_bOverprint;
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->Serialize(ar, dwVersion);
	}
}

void CLine::ReleaseCE(CArchive& ar) const
{
	m_dash.ReleaseCE(ar);
	if(ar.IsStoring())
	{
		unsigned short nWidth = m_nWidth/10;
		if(nWidth==0&&m_nWidth!=0)
			nWidth = 1;

		ar<<nWidth;
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseCE(ar);
	}
}

void CLine::ReleaseDCOM(CArchive& ar)
{
	m_dash.ReleaseDCOM(ar);
	if(ar.IsStoring())
	{
		ar<<m_nWidth;

		ar<<m_bCapindex;
		ar<<m_bJoinindex;
		ar<<m_bMiterlimit;
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}
	else
	{
		ar>>m_nWidth;

		ar>>m_bCapindex;
		ar>>m_bJoinindex;
		ar>>m_bMiterlimit;

		char colorindex = -1;
		ar>>colorindex;
		delete m_pColor;
		m_pColor = CColor::Apply(colorindex);
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseDCOM(ar);
	}
}

DWORD CLine::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile!=nullptr)
	{
		DWORD size = 0;

		pFile->Write(&m_dash, sizeof(CDash));
		pFile->Write(&m_nWidth, sizeof(unsigned short));
		pFile->Write(&m_bCapindex, sizeof(unsigned short));
		pFile->Write(&m_bJoinindex, sizeof(unsigned short));
		pFile->Write(&m_bMiterlimit, sizeof(unsigned short));
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		pFile->Write(&colortype, sizeof(BYTE));

		size += sizeof(CDash);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);
		size += sizeof(BYTE);

		if(m_pColor!=nullptr)
		{
			size += m_pColor->PackPC(pFile, bytes);
		}

		return size;
	}
	else
	{
		m_dash = *(CDash*)bytes;
		bytes += sizeof(CDash);

		m_nWidth = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_bCapindex = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_bJoinindex = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_bMiterlimit = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		const BYTE colorindex = *(char*)bytes;
		bytes += sizeof(BYTE);

		delete m_pColor;
		m_pColor = CColor::Apply(colorindex);

		if(m_pColor!=nullptr)
		{
			m_pColor->PackPC(nullptr, bytes);
		}
		return 0;
	}
}

void CLine::ReleaseWeb(CFile& file) const
{
	m_dash.ReleaseWeb(file);

	unsigned short nWidth = m_nWidth;
	ReverseOrder(nWidth);
	file.Write(&nWidth, sizeof(unsigned short));

	file.Write(&m_bCapindex, sizeof(BYTE));
	file.Write(&m_bJoinindex, sizeof(BYTE));
	file.Write(&m_bMiterlimit, sizeof(BYTE));

	if(m_pColor!=nullptr)
		m_pColor->ReleaseWeb(file);
	else
	{
		const BYTE alpha = 255;
		file.Write(&alpha, sizeof(BYTE));

		const DWORD color = 0B00000000000000000000000000000000;
		file.Write(&color, sizeof(DWORD));
	}
}

void CLine::ReleaseWeb(BinaryStream& stream) const
{
	m_dash.ReleaseWeb(stream);

	stream<<m_nWidth;
	stream<<m_bCapindex;
	stream<<m_bJoinindex;
	stream<<m_bMiterlimit;

	if(m_pColor!=nullptr)
		m_pColor->ReleaseWeb(stream);
	else
	{
		const BYTE alpha = 255;
		stream<<alpha;
		const DWORD color = 0B00000000000000000000000000000000;
		stream<<color;
	}
}
void CLine::ReleaseWeb(boost::json::object& json) const
{
	m_dash.ReleaseWeb(json);

	json["Width"] = m_nWidth;
		
	json["Capindex"] = m_bCapindex;
	json["Joinindex"] = m_bJoinindex;
	json["Miterlimit"] = m_bMiterlimit;

	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(json);
	else
		json["Color"] = "rgba(0,0,0,1)";
}
void CLine::ReleaseWeb(pbf::writer& writer) const
{
	m_dash.ReleaseWeb(writer);

	writer.add_variant_uint16(m_nWidth);
	writer.add_byte(m_bCapindex);
	writer.add_byte(m_bJoinindex);
	writer.add_byte(m_bMiterlimit);

	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(writer);
	else
		writer.add_string("rgba(0,0,0,1)");
}
void CLine::ExportPs3(FILE* file, CPsboard& aiKey) const
{
	if(m_bOverprint!=aiKey.R)
	{
		_ftprintf(file, _T("%d R\n"), m_bOverprint);
		aiKey.R = m_bOverprint;
	}
	if(m_pColor!=nullptr)
	{
		m_pColor->ExportAIStroke(file, aiKey);
	}
	else if(aiKey.K!=100)
	{
		_ftprintf(file, _T("0 0 0 1 K\n"));
		aiKey.K = 100;
	}

	if(m_bCapindex!=aiKey.J)
	{
		_ftprintf(file, _T("%d J "), m_bCapindex);
		aiKey.J = m_bCapindex;
	}
	if(m_bJoinindex!=aiKey.j)
	{
		_ftprintf(file, _T("%d j "), m_bJoinindex);
		aiKey.j = m_bJoinindex;
	}
	if(m_nWidth!=aiKey.w)
	{
		_ftprintf(file, _T("%g w "), m_nWidth/10.f);
		aiKey.w = m_nWidth;
	}
	if(m_bMiterlimit!=aiKey.M)
	{
		_ftprintf(file, _T("%d M "), m_bMiterlimit);
		aiKey.M = m_bMiterlimit;
	}

	m_dash.ExportPs3(file, aiKey);

	_ftprintf(file, _T("\n"));
}

void CLine::ExportPdf(HPDF_Page page) const
{
	if(m_nWidth==0)
	{
		HPDF_Page_SetLineWidth(page, 0);
	}
	else
	{
		HPDF_Page_SetLineWidth(page, m_nWidth/10.0f);
		const Gdiplus::Color color = m_pColor==nullptr ? Gdiplus::Color(255, 0, 0, 0) : m_pColor->GetColor();
		HPDF_Page_SetRGBStroke(page, color.GetR()/255.f, color.GetG()/255.f, color.GetB()/255.f);

		//		pen->SetAlignment(PenAlignmentCenter);

		switch(m_bCapindex)
		{
		case 0:
			HPDF_Page_SetLineCap(page, HPDF_BUTT_END);
			break;
		case 1:
			HPDF_Page_SetLineCap(page, HPDF_ROUND_END);
			break;
		case 2:
			HPDF_Page_SetLineCap(page, HPDF_PROJECTING_SQUARE_END);
			break;
		}

		switch(m_bJoinindex)
		{
		case 0:
			HPDF_Page_SetLineJoin(page, HPDF_BEVEL_JOIN);
			break;
		case 1:
			HPDF_Page_SetLineJoin(page, HPDF_ROUND_JOIN);
			break;
		case 2:
			HPDF_Page_SetLineJoin(page, HPDF_MITER_JOIN);
			break;
		case 3:
			HPDF_Page_SetLineJoin(page, HPDF_LINEJOIN_EOF);
			break;
		}

		m_dash.ExportPdf(page);
	}
}

CLine* CLine::Apply(BYTE type)
{
	CLine* line = nullptr;

	switch(type)
	{
	case LINETYPE::Blank:
		line = new CLineEmpty();
		break;
	case LINETYPE::Alone:
		line = new CLine;
		break;
	case LINETYPE::Normal:
		line = new CLineNormal;
		break;
	case LINETYPE::Symbol:
		line = new CLineSymbol;
		break;
	case LINETYPE::River:
		line = new CLineRiver;
		break;
	}

	return line;
}
