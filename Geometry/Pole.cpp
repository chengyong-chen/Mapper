#include "stdafx.h"
#include <array>
#include "Global.h"

#include "Geom.h"
#include "Pole.h"

#include "../Public/Global.h"
#include "../Public/BinaryStream.h"
#include "../DataView/viewinfo.h"
#include "../Style/Line.h"
#include "../Style/FillCompact.h"
#include "../Pbf/writer.hpp"
using namespace boost;
#include <boost/json.hpp>
#include <boost/json/array.hpp>

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPole, CGeom, 0)

CPole::CPole()
{
	m_bDirection = false;
	m_bType = 15;
}

CPole::CPole(const CRect& rect) :
	CGeom(rect, nullptr, nullptr)
{
	m_bDirection = false;
}

CPole::~CPole()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		const CGeom* geom = m_geomlist.GetNext(pos);
		delete geom;
	}

	m_geomlist.RemoveAll();
}

BOOL CPole::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CPole)) == FALSE)
		return FALSE;
	else if(m_geomlist.GetSize() != ((CPole&)geom).m_geomlist.GetSize())
		return FALSE;
	else
	{
		for(int index = 0; index < min(m_geomlist.GetSize(), ((CPole&)geom).m_geomlist.GetSize()); index++)
		{
			const CGeom* pGeom1 = m_geomlist.GetAt(m_geomlist.FindIndex(index));
			const CGeom* pGeom2 = ((CPole&)geom).m_geomlist.GetAt(((CPole&)geom).m_geomlist.FindIndex(index));
			if(*pGeom1 != *pGeom2)
				return FALSE;
		}
		return TRUE;
	}
}

void CPole::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_Center.x;
		ar << m_Center.y;
		ar << m_bDirection;
	}
	else
	{
		ar >> m_Center.x;
		ar >> m_Center.y;
		ar >> m_bDirection;
	}

	CGeom::Serializelist(ar, dwVersion, m_geomlist);

	if(ar.IsStoring())
	{
	}
	else
	{
		m_Rect.SetRectEmpty();
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			m_Rect = UnionRect(m_Rect, pGeom->m_Rect);
		}
		m_Rect.OffsetRect(m_Center.x, m_Center.y);
	}
}

void CPole::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar << m_Center.x;
		ar << m_Center.y;
		ar << m_bDirection;
		const int nCount = m_geomlist.GetCount();
		ar << nCount;

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);

			const BYTE geomtype = pGeom->Gettype();
			const CLine::LINETYPE linetype = pGeom->m_pLine == nullptr ? CLine::LINETYPE::Inherit : pGeom->m_pLine->Gettype();
			const CFill::FILLTYPE filltype = pGeom->m_pFill == nullptr ? CFill::FILLTYPE::Inherit : pGeom->m_pFill->Gettype();
			ar << (BYTE)geomtype;
			ar << (BYTE)linetype;
			ar << (BYTE)filltype;

			pGeom->ReleaseCE(ar);
			if(pGeom->m_pLine != nullptr)
				pGeom->m_pLine->ReleaseCE(ar);
			if(pGeom->m_pFill != nullptr)
				pGeom->m_pFill->ReleaseCE(ar);
		}
	}
	else
	{
	}
}

void CPole::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_Center.x;
		ar << m_Center.y;
		ar << m_bDirection;
		const unsigned short nCount = m_geomlist.GetCount();
		ar << nCount;

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);

			const BYTE geomtype = pGeom->Gettype();
			const CLine::LINETYPE linetype = pGeom->m_pLine == nullptr ? CLine::LINETYPE::Inherit : pGeom->m_pLine->Gettype();
			const CFill::FILLTYPE filltype = pGeom->m_pFill == nullptr ? CFill::FILLTYPE::Inherit : pGeom->m_pFill->Gettype();
			ar << (BYTE)geomtype;
			ar << (BYTE)linetype;
			ar << (BYTE)filltype;

			pGeom->ReleaseDCOM(ar);
			if(pGeom->m_pLine != nullptr)
				pGeom->m_pLine->ReleaseDCOM(ar);
			if(pGeom->m_pFill != nullptr)
				pGeom->m_pFill->ReleaseDCOM(ar);
		}
	}
	else
	{
		ar >> m_Center.x;
		ar >> m_Center.y;
		ar >> m_bDirection;

		unsigned short nCount;
		ar >> nCount;
		for(unsigned short index = 0; index < nCount; index++)
		{
			BYTE geomtype;
			BYTE linetype;
			BYTE filltype;
			ar >> geomtype;
			ar >> linetype;
			ar >> filltype;

			CGeom* pGeom = CGeom::Apply(geomtype);
			pGeom->ReleaseDCOM(ar);

			pGeom->m_pLine = CLine::Apply(linetype);
			if(pGeom->m_pLine != nullptr)
			{
				pGeom->m_pLine->ReleaseDCOM(ar);
			}

			pGeom->m_pFill = CFillCompact::Apply(filltype);
			if(pGeom->m_pFill != nullptr)
			{
				pGeom->m_pFill->ReleaseDCOM(ar);
			}
		}
	}
}

void CPole::ReleaseWeb(CFile& file) const
{
	int x = m_Center.x;
	ReverseOrder(x);
	file.Write(&x, sizeof(int));

	int y = m_Center.y;
	ReverseOrder(y);
	file.Write(&y, sizeof(int));

	const BYTE byte = m_bDirection == true ? 0X01 : 0X00;
	file.Write(&byte, sizeof(BYTE));

	unsigned short nCount = m_geomlist.GetCount();
	ReverseOrder(nCount);
	file.Write(&nCount, sizeof(unsigned short));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(file);
	}
}

void CPole::ReleaseWeb(BinaryStream& stream) const
{
	stream << m_Center.x;
	stream << m_Center.y;
	const BYTE byte = m_bDirection == true ? 0X01 : 0X00;
	stream << byte;
	const unsigned short nCount = m_geomlist.GetCount();
	stream << nCount;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(stream);
	}
}
void CPole::ReleaseWeb(boost::json::object& json) const
{
	std::vector<int> child1;
	child1.push_back(m_Center.x);
	child1.push_back(m_Center.x);
	json["Center"] = boost::json::value_from(child1);

	const BYTE byte = m_bDirection == true ? 0X01 : 0X00;
	json["Direction"] = byte;
	const unsigned short nCount = m_geomlist.GetCount();
	json["Count"] = nCount;

	boost::json::array child2;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		boost::json::object child3;
		pGeom->ReleaseWeb(child3);
		child2.push_back(child3);
	}
	json["Geoms"] = child2;
}
void CPole::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	writer.add_variant_sint32(m_Center.x - offset.cx);
	writer.add_variant_sint32(m_Center.y - offset.cy);

	writer.add_bool(m_bDirection);

	writer.add_variant_uint32(m_geomlist.GetSize());
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(writer, offset);
	}
}
void CPole::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
	if(this->IsValid() == false)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPs3(file, matrix, aiKey);
	}
}

void CPole::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid() == false)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPdf(pdf, page, matrix);
	}
}

void CPole::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	Gdiplus::PointF center = Gdiplus::PointF(m_Center.x, m_Center.y);
	matrix.TransformPoints(&center);

	g.TranslateTransform(0, center.Y);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Draw(g, matrix, context, dpi);
	}

	g.TranslateTransform(-0, -center.Y);
}
void CPole::Preoccupy(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	Gdiplus::PointF center = Gdiplus::PointF(m_Center.x, m_Center.y);
	matrix.TransformPoints(&center);

	g.TranslateTransform(0, center.Y);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Preoccupy(g, matrix, context, dpi);
	}

	g.TranslateTransform(-0, -center.Y);
}
void CPole::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	const Gdiplus::PointF center = viewinfo.DocToClient<Gdiplus::PointF>(m_Center);
	const Gdiplus::PointF origin = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(0, 0));
	g.TranslateTransform(center.X - origin.X, center.Y - origin.Y);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Draw(g, viewinfo, ratio);
	}
	g.TranslateTransform(origin.X - center.X, origin.Y - center.Y);
}
void CPole::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	const Gdiplus::PointF center = viewinfo.DocToClient<Gdiplus::PointF>(m_Center);
	const Gdiplus::PointF origin = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(0, 0));
	g.TranslateTransform(center.X-origin.X, center.Y-origin.Y);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Preoccupy(g, viewinfo, ratio);
	}
	g.TranslateTransform(origin.X-center.X, origin.Y-center.Y);
}
void CPole::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	const Gdiplus::PointF center = viewinfo.DocToClient<Gdiplus::PointF>(m_Center);
	const Gdiplus::PointF origin = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(0, 0));
	g.TranslateTransform(center.X - origin.X, center.Y - origin.Y);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Draw(g, viewinfo, library, context);
	}
	g.TranslateTransform(origin.X - center.X, origin.Y - center.Y);
}

void CPole::CopyTo(CGeom* pGeom, bool ignore) const
{
	CGeom::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CPole)) == TRUE)
	{
		CPole* pPole = (CPole*)pGeom;

		pPole->m_Center = m_Center;
		pPole->m_bDirection = m_bDirection;

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* geom = m_geomlist.GetNext(pos);
			CGeom* dObj = geom->Clone();
			pPole->m_geomlist.AddTail(dObj);
		}
	}
}

void CPole::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CPole)) == TRUE)
	{
		CPole* pPole = (CPole*)pGeom;

		Swapdata<CPoint>(pPole->m_Center, m_Center);
		Swapdata<bool>(pPole->m_bDirection, m_bDirection);
		Swaplist<CObList>(pPole->m_geomlist, m_geomlist);
	}

	CGeom::Swap(pGeom);
}
void CPole::Move(const int& dx, const int& dy)
{
	m_Center.x -= dx;
	m_Center.y -= dy;

	CGeom::Move(dx, dy);
}
void CPole::Move(const CSize& Δ)
{
	m_Center.x -= Δ.cx;
	m_Center.y -= Δ.cy;

	CGeom::Move(Δ);
}

void CPole::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	const long x = round(m_Center.x*m11 + m_Center.y*m21 + m31);
	const long y = round(m_Center.x*m12 + m_Center.y*m22 + m32);
	m_Center.x = x;
	m_Center.y = y;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);

		CLine* oldLine = pGeom->m_pLine;
		CFillCompact* oldFill = pGeom->m_pFill;
		CType* oldType = pGeom->m_pType;
		pGeom->m_pLine = nullptr;
		pGeom->m_pFill = nullptr;
		pGeom->m_pType = nullptr;
		pGeom->Transform(m11, m21, m31, m12, m22, m32);
		pGeom->m_pLine = oldLine;
		pGeom->m_pFill = oldFill;
		pGeom->m_pType = oldType;
	}

	m_bModified = true;
}
