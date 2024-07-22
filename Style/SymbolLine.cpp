#include "stdafx.h"

#include "Symbol.h"
#include "SymbolLine.h"
#include "Library.h"

#include "../DataView/Viewinfo.h"
#include "../Public/BinaryStream.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Pole.h"
#include "../Pbf/writer.hpp"
#include <Math.h>

#include "Line.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSymbolLine, CSymbol, 0)

CSymbolLine::CSymbolLine()
{
	m_wId = 0;
	m_nStep = 100;
	m_strName = _T("Mapper");
}

CSymbolLine::~CSymbolLine()
{
}

void CSymbolLine::CopyTo(CSymbol* symbol) const
{
	CSymbol::CopyTo(symbol);

	if(symbol->IsKindOf(RUNTIME_CLASS(CSymbolLine))==TRUE)
	{
		((CSymbolLine*)symbol)->m_strName = m_strName;
		((CSymbolLine*)symbol)->m_wId = m_wId;
		((CSymbolLine*)symbol)->m_Rect = m_Rect;
	}
}

void CSymbolLine::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CPath* pPath, Context& context, float fZoom) const
{
	if(m_wId<0)
		return;
	if(pPath ==nullptr)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_geomlist.GetNext(pos);
		if(geom->IsKindOf(RUNTIME_CLASS(CPoly))==TRUE)
		{
			CLine* line = geom->m_pLine;
			if(line==nullptr)
				continue;

			CLine* pOldLine = pPath->m_pLine;			
			pPath->Draw(g, geom->m_pLine, viewinfo, context.ratioLine);
		}
		else if(geom->IsKindOf(RUNTIME_CLASS(CPole))==TRUE)
		{
			const double m = viewinfo.m_datainfo.DocToMap((1/10000.f)*fZoom*viewinfo.m_datainfo.m_zoomPointToDoc)*viewinfo.m_xFactorMapToView;
			Gdiplus::Matrix matrix(m, 0, 0, -m, 0, 0);

			CPole* pole = (CPole*)geom;
			const int step = (this->m_nStep/10.f)*fZoom*viewinfo.m_datainfo.m_zoomPointToDoc;
			const int offset = (pole->m_Center.x/10000.f)*fZoom*viewinfo.m_datainfo.m_zoomPointToDoc;

			unsigned int fAnchor = 1;
			double st = 0;
			do
			{
				unsigned int tAnchor = 1;
				double et = 0;

				if(pPath->GetAandT(fAnchor, st, offset, tAnchor, et)==false)
					break;

				CPoint docPoint = pPath->GetPoint(tAnchor, et);
				const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
				g.TranslateTransform(cliPoint.X, cliPoint.Y);

				if(pole->m_bDirection==true)
				{
					const double fAngle = pPath->GetAngle(tAnchor, et);
					const int nAngle = (int)(fAngle*180/M_PI);
					g.RotateTransform(-nAngle);
					pole->Draw(g, matrix, context, viewinfo.m_sizeDPI);
					g.RotateTransform(nAngle);
				}
				else
				{
					pole->Draw(g, viewinfo, context.ratioLine);
				}

				g.TranslateTransform(-cliPoint.X, -cliPoint.Y);

				if(pPath->GetAandT(fAnchor, st, step, tAnchor, et)==false)
					break;
				else if(fAnchor==tAnchor&&st==et)
					break;
				else if(et==1.0f)
				{
					fAnchor = tAnchor+1;
					st = 0;
				}
				else
				{
					fAnchor = tAnchor;
					st = et;
				}
			} while(TRUE);
		}
	}
}
void CSymbolLine::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CPath* pPath, Context& context, float fZoom) const
{
	if(m_wId<0)
		return;
	if(pPath==nullptr)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_geomlist.GetNext(pos);
		if(geom->IsKindOf(RUNTIME_CLASS(CPoly))==TRUE)
		{
			CLine* line = geom->m_pLine;
			if(line==nullptr)
				continue;

			CLine* pOldLine = pPath->m_pLine;
			pPath->Preoccupy(g, geom->m_pLine, viewinfo, context.ratioLine);
		}
		else if(geom->IsKindOf(RUNTIME_CLASS(CPole))==TRUE)
		{
			const double m = viewinfo.m_datainfo.DocToMap((1/10000.f)*fZoom*viewinfo.m_datainfo.m_zoomPointToDoc)*viewinfo.m_xFactorMapToView;
			Gdiplus::Matrix matrix(m, 0, 0, -m, 0, 0);

			CPole* pole = (CPole*)geom;
			const int step = (this->m_nStep/10.f)*fZoom*viewinfo.m_datainfo.m_zoomPointToDoc;
			const int offset = (pole->m_Center.x/10000.f)*fZoom*viewinfo.m_datainfo.m_zoomPointToDoc;

			unsigned int fAnchor = 1;
			double st = 0;
			do
			{
				unsigned int tAnchor = 1;
				double et = 0;

				if(pPath->GetAandT(fAnchor, st, offset, tAnchor, et)==false)
					break;

				CPoint docPoint = pPath->GetPoint(tAnchor, et);
				const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
				g.TranslateTransform(cliPoint.X, cliPoint.Y);

				if(pole->m_bDirection==true)
				{
					const double fAngle = pPath->GetAngle(tAnchor, et);
					const int nAngle = (int)(fAngle*180/M_PI);
					g.RotateTransform(-nAngle);
					pole->Preoccupy(g, matrix, context, viewinfo.m_sizeDPI);
					g.RotateTransform(nAngle);
				}
				else
				{
					pole->Preoccupy(g, viewinfo, context.ratioLine);
				}

				g.TranslateTransform(-cliPoint.X, -cliPoint.Y);

				if(pPath->GetAandT(fAnchor, st, step, tAnchor, et)==false)
					break;
				else if(fAnchor==tAnchor&&st==et)
					break;
				else if(et==1.0f)
				{
					fAnchor = tAnchor+1;
					st = 0;
				}
				else
				{
					fAnchor = tAnchor;
					st = et;
				}
			} while(TRUE);
		}
	}
}

void CSymbolLine::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const
{
	if(pPath==nullptr)
		return;
	CPoly* pPoly = pPath->GetPoly();
	if(pPoly==nullptr)
		return;

	_ftprintf(file, _T("q\n"));
	_ftprintf(file, _T("u\n"));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_geomlist.GetNext(pos);
		if(geom->IsKindOf(RUNTIME_CLASS(CPoly)))
		{
			const CPoly* poly = (CPoly*)geom;

			CLine* old = pPoly->m_pLine;
			pPoly->m_pLine = poly->m_pLine;
			pPoly->ExportPs3(file, viewinfo, aiKey);
			pPoly->m_pLine = old;
		}
		else if(geom->IsKindOf(RUNTIME_CLASS(CPole))==TRUE)
		{
			CPole* pole = (CPole*)geom;
			const int step = (this->m_nStep/10.f)*1*viewinfo.m_datainfo.m_zoomPointToDoc;
			const int offset = (pole->m_Center.x/10000.f)*1*viewinfo.m_datainfo.m_zoomPointToDoc;

			unsigned int fAnchor = 1;
			double st = 0;
			do
			{
				unsigned int tAnchor = 1;
				double et = 0;

				if(pPoly->GetAandT(fAnchor, st, offset, tAnchor, et)==false)
					break;
				const Gdiplus::PointF point = viewinfo.DocToClient <Gdiplus::PointF>(pPoly->GetPoint(tAnchor, et));
				double m11 = 1/10000.f;
				double m12 = 0/10000.f;
				double m21 = 0/10000.f;
				double m22 = 1/10000.f;
				const double m31 = point.X;
				const double m32 = point.Y+pole->m_Center.y/10000.f;
				if(pole->m_bDirection==true)
				{
					const double angle = pPoly->GetAngle(tAnchor, et);
					const double fCos = cos(angle);
					const double fSin = sin(angle);

					m11 = fCos/10000.f;
					m12 = fSin/10000.f;
					m21 = -fSin/10000.f;
					m22 = fCos/10000.f;
				}
				Gdiplus::Matrix matrix(m11, m12, m21, m22, m31, m32);
				pole->ExportPs3(file, matrix, aiKey);

				if(pPoly->GetAandT(fAnchor, st, step, tAnchor, et)==false)
					break;
				if(pPath->GetAandT(fAnchor, st, step, tAnchor, et)==false)
					break;
				else if(fAnchor==tAnchor&&st==et)
					break;
				else if(et==1.0f)
				{
					fAnchor = tAnchor+1;
					st = 0;
				}
				else
				{
					fAnchor = tAnchor;
					st = et;
				}
			} while(true);
		}
	}

	_ftprintf(file, _T("U\n"));
	_ftprintf(file, _T("Q\n"));

	delete pPoly;
}

void CSymbolLine::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CPath* pPath) const
{
	if(pPath==nullptr)
		return;
	CPoly* pPoly = pPath->GetPoly();
	if(pPoly==nullptr)
		return;

	Context context;
	context.tolerance = viewinfo.GetPrecision(10);
	CLibrary library;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_geomlist.GetNext(pos);
		if(geom->IsKindOf(RUNTIME_CLASS(CPoly)))
		{
			const CPoly* poly = (CPoly*)geom;

			CLine* old = pPoly->m_pLine;
			pPoly->m_pLine = poly->m_pLine;
			pPoly->ExportPdf(pdf, page, viewinfo, library, context);
			pPoly->m_pLine = old;
		}
		else if(geom->IsKindOf(RUNTIME_CLASS(CPole))==TRUE)
		{
			CPole* pole = (CPole*)geom;
			const int step = (this->m_nStep/10.f)*1*viewinfo.m_datainfo.m_zoomPointToDoc;
			const int offset = (pole->m_Center.x/10000.f)*1*viewinfo.m_datainfo.m_zoomPointToDoc;

			unsigned int fAnchor = 1;
			double st = 0;
			do
			{
				unsigned int tAnchor = 1;
				double et = 0;

				if(pPoly->GetAandT(fAnchor, st, offset, tAnchor, et)==false)
					break;
				const Gdiplus::PointF point = viewinfo.DocToClient <Gdiplus::PointF>(pPoly->GetPoint(tAnchor, et));
				double m11 = 1/10000.f;
				double m12 = 0/10000.f;
				double m21 = 0/10000.f;
				double m22 = 1/10000.f;
				const double m31 = point.X;
				const double m32 = point.Y+pole->m_Center.y/10000.f;
				if(pole->m_bDirection==true)
				{
					const double angle = pPoly->GetAngle(tAnchor, et);
					const double fCos = cos(angle);
					const double fSin = sin(angle);

					m11 = fCos/10000.f;
					m12 = fSin/10000.f;
					m21 = -fSin/10000.f;
					m22 = fCos/10000.f;
				}
				Gdiplus::Matrix matrix(m11, m12, m21, m22, m31, m32);
				pole->ExportPdf(pdf, page, matrix);

				if(pPoly->GetAandT(fAnchor, st, step, tAnchor, et)==false)
					break;
				if(pPath->GetAandT(fAnchor, st, step, tAnchor, et)==false)
					break;
				else if(fAnchor==tAnchor&&st==et)
					break;
				else if(et==1.0f)
				{
					fAnchor = tAnchor+1;
					st = 0;
				}
				else
				{
					fAnchor = tAnchor;
					st = et;
				}
			} while(true);
		}
	}

	delete pPoly;
}

void CSymbolLine::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSymbol::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		RecalRect();
		ar<<m_nWidth;
		ar<<m_nStep;
	}
	else
	{
		ar>>m_nWidth;
		ar>>m_nStep;
	}
}

void CSymbolLine::ReleaseCE(CArchive& ar) const
{
	CSymbol::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_nWidth;
		ar<<m_nStep;
	}
}

void CSymbolLine::ReleaseDCOM(CArchive& ar)
{
	CSymbol::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_nWidth;
		ar<<m_nStep;
	}
	else
	{
		ar>>m_nWidth;
		ar>>m_nStep;
	}
}

void CSymbolLine::ReleaseWeb(CFile& file) const
{
	CSymbol::ReleaseWeb(file);

	unsigned short nWidth = m_nWidth;
	ReverseOrder(nWidth);
	file.Write(&nWidth, sizeof(unsigned short));

	unsigned short nStep = m_nStep;
	ReverseOrder(nStep);
	file.Write(&nStep, sizeof(unsigned short));
}

void CSymbolLine::ReleaseWeb(BinaryStream& stream) const
{
	CSymbol::ReleaseWeb(stream);

	stream<<m_nWidth;
	stream<<m_nStep;
}
void CSymbolLine::ReleaseWeb(boost::json::object& json) const
{
	CSymbol::ReleaseWeb(json);

	json["Width"] = m_nWidth;
	json["Step"] = m_nStep;
}
void CSymbolLine::ReleaseWeb(pbf::writer& writer) const
{
	CSymbol::ReleaseWeb(writer);

	writer.add_variant_uint16(m_nWidth);
	writer.add_variant_uint16(m_nStep);
}
void CSymbolLine::RecalRect()
{
	CSymbol::RecalRect();
	m_nWidth = max(abs(m_Rect.top*10.f/10000), abs(m_Rect.bottom*10.f/10000));
	m_nWidth = m_nWidth*2;
}
