#include "stdafx.h"
#include <array>
#include "Symbol.h"
#include "SymbolLine.h"
#include "SymbolFill.h"
#include "SymbolSpot.h"
#include "Line.h"

#include "../DataView/Viewinfo.h"
#include "../Public/BinaryStream.h"
#include "../Geometry/Pole.h"
#include "../Geometry/Global.h"
#include "../Pbf/writer.hpp"
#include "../Utility/array.hpp"
#include "../Utility/monochrome.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSymbol, CObject)

CSymbol::CSymbol()
{
	m_wId = 0;
	m_strName = _T("Mapper");
	std::get<3>(m_cache1) = nullptr;
	std::get<3>(m_cache2) = nullptr;
}

CSymbol::~CSymbol()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CGeom* geom = m_geomlist.GetNext(pos);
		delete geom;
	}
	m_geomlist.RemoveAll();

	Gdiplus::Bitmap* gBitmap = std::get<3>(m_cache1);
	if(gBitmap!=nullptr)
	{
		delete gBitmap;
	}
	HBITMAP hBitmap = std::get<3>(m_cache2);
	if(hBitmap!=nullptr)
	{
		::DeleteObject(hBitmap);
	}
}

CSymbol* CSymbol::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CSymbol* pClone = (CSymbol*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	return pClone;
}

void CSymbol::CopyTo(CSymbol* symbol) const
{
	symbol->m_strName = m_strName;
	symbol->m_wId = m_wId;
	symbol->m_Rect = m_Rect;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom = pGeom->Clone();
		symbol->m_geomlist.AddTail(pGeom);
	}
}

CSymbol* CSymbol::Apply(BYTE index)
{
	CSymbol* symbol = nullptr;

	switch(index)
	{
		case 0:
			break;
		case 1:
			symbol = new CSymbolLine;
			break;
		case 2:
			symbol = new CSymbolFill;
			break;
		case 3:
			symbol = new CSymbolSpot;
			break;
	}

	return symbol;
}
std::tuple<int, int, const Gdiplus::Bitmap*> CSymbol::GetBitmap1(float ratio, const CSize& dpi) const
{
	boost::uuids::detail::sha1 sha1;
	sha1.process_bytes(&ratio, sizeof(float));
	sha1.process_bytes(&dpi.cx, sizeof(LONG));
	sha1.process_bytes(&dpi.cy, sizeof(LONG));
	unsigned int hash[5];
	sha1.get_digest(hash);

	Gdiplus::Bitmap* pOld = std::get<3>(m_cache1);
	if(arr::equal(hash, std::get<0>(m_cache1))==true&&pOld!=nullptr)
		return {std::get<1>(m_cache1),std::get<2>(m_cache1),pOld};
	else
	{
		delete pOld;

		CRect rect = this->GetRect();
		rect.left = CViewinfo::PointsToPixels(rect.left*(float)ratio/10000.f, dpi).Width;
		rect.top = CViewinfo::PointsToPixels(rect.top*(float)ratio/10000.f, dpi).Height;
		rect.right = CViewinfo::PointsToPixels(rect.right*(float)ratio/10000.f, dpi).Width;
		rect.bottom = CViewinfo::PointsToPixels(rect.bottom*(float)ratio/10000.f, dpi).Height;
		rect.InflateRect(10, 10);//in order to make all pixels inside

		Gdiplus::Bitmap* gBitmap = ::new Gdiplus::Bitmap(rect.Width(), rect.Height());
		Gdiplus::Graphics g(gBitmap);
		g.SetPageUnit(Gdiplus::UnitPixel);
		g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
		g.TranslateTransform(-rect.left, rect.bottom);
		this->Draw(g, 0, 1, ratio, dpi);
		
		memcpy(std::get<0>(m_cache1), hash, sizeof(unsigned int)*5);
		int left, top, right, bottom;
		Gdiplus::Bitmap* gTrimmed = monochrome::trim(gBitmap, left, top, right, bottom);		
		if(gTrimmed!=nullptr)
		{
			::delete gBitmap;
			std::get<1>(m_cache1) = -rect.left-left;
			std::get<2>(m_cache1) = rect.bottom-top;
			std::get<3>(m_cache1) = gTrimmed;
			return {-rect.left-left, rect.bottom-top, gTrimmed};
		}
		else
		{
			std::get<1>(m_cache1) = -rect.left;
			std::get<2>(m_cache1) = rect.bottom;
			std::get<3>(m_cache1) = gBitmap;
			return {-rect.left, rect.bottom, gBitmap};
		}
	}
}
std::tuple<int, int, HBITMAP> CSymbol::GetBitmap2(float ratio, const CSize& dpi) const
{
	boost::uuids::detail::sha1 sha1;
	sha1.process_bytes(&ratio, sizeof(float));
	sha1.process_bytes(&dpi.cx, sizeof(LONG));
	sha1.process_bytes(&dpi.cy, sizeof(LONG));
	unsigned int hash[5];
	sha1.get_digest(hash);

	HBITMAP& pOld = std::get<3>(m_cache2);
	if(arr::equal(hash, std::get<0>(m_cache2))==true&&pOld!=nullptr)
		return {std::get<1>(m_cache2), std::get<2>(m_cache2), pOld};
	else
	{
		DeleteObject(pOld);
		std::tuple<int, int, const Gdiplus::Bitmap*> tuple = CSymbol::GetBitmap1(ratio, dpi);
		//	monochrome::SaveHBITMAPToFile(const_cast<Gdiplus::Bitmap*&>(std::get<2>(tuple)), _T("c:\\hbitmap-b.png"));
		HBITMAP hBitmap = monochrome::convert(const_cast<Gdiplus::Bitmap*&>(std::get<2>(tuple)));
		if(hBitmap!=nullptr)
		{
			//	monochrome::SaveHBITMAPToFile(hBitmap, _T("c:\\hbitmap-a.bmp"));
			std::get<1>(m_cache2) = std::get<0>(tuple);
			std::get<2>(m_cache2) = std::get<1>(tuple);
			std::get<3>(m_cache2) = hBitmap;
		}
		return {std::get<1>(m_cache2),std::get<2>(m_cache2), std::get<3>(m_cache2)};
	}
}

void CSymbol::Draw(Gdiplus::Graphics& g, float fSin, float fCos, float fZoom, const CSize& dpi) const
{
	const double m11 = CViewinfo::PointsToPixels(fZoom*fCos/10000.f, dpi).Width;
	const double m21 = -CViewinfo::PointsToPixels(fZoom*fSin*fCos/10000.f, dpi).Height;
	const double m31 = 0;
	const double m12 = CViewinfo::PointsToPixels(fZoom*fSin*fCos/10000.f, dpi).Width;
	const double m22 = -CViewinfo::PointsToPixels(fZoom*fCos*fCos/10000.f, dpi).Height;
	const double m32 = 0;
	Gdiplus::Matrix matrix(m11, m12, m21, m22, m31, m32);

	Context context;
	context.ratioLine = fZoom;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_geomlist.GetNext(pos);
		geom->Draw(g, matrix, context, dpi);
	}
}
void CSymbol::Mono(Gdiplus::Graphics& g, float fSin, float fCos, float fZoom, const CSize& dpi) const
{
	const double m11 = CViewinfo::PointsToPixels(fZoom*fCos/10000.f, dpi).Width;
	const double m21 = -CViewinfo::PointsToPixels(fZoom*fSin*fCos/10000.f, dpi).Height;
	const double m31 = 0;
	const double m12 = CViewinfo::PointsToPixels(fZoom*fSin*fCos/10000.f, dpi).Width;
	const double m22 = -CViewinfo::PointsToPixels(fZoom*fCos*fCos/10000.f, dpi).Height;
	const double m32 = 0;
	Gdiplus::Matrix matrix(m11, m12, m21, m22, m31, m32);

	Context context;
	context.ratioLine = fZoom;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_geomlist.GetNext(pos);
		geom->Mono(g, matrix, context, dpi);
	}
}
void CSymbol::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_wId;
		ar<<m_strName;
	}
	else
	{
		ar>>m_wId;
		ar>>m_strName;
	}

	CGeom::Serializelist(ar, dwVersion, m_geomlist);
}

void CSymbol::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar<<m_wId;
		SerializeStrCE(ar, m_strName);
		const int nCount = m_geomlist.GetCount();
		ar<<nCount;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			if(pGeom->IsKindOf(RUNTIME_CLASS(CPole))==FALSE)
			{
				ar<<(BYTE)20;
				const BYTE type = pGeom->Gettype();
				ar<<type;

				pGeom->ReleaseCE(ar);
			}
			else
			{
				ar<<(BYTE)21;
				CPole* pole = (CPole*)pGeom;
				pole->ReleaseCE(ar);
			}
		}
	}
	else
	{
	}
}

void CSymbol::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar<<m_wId;
		const unsigned short nCount = m_geomlist.GetCount();
		ar<<nCount;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);

			if(pGeom->IsKindOf(RUNTIME_CLASS(CPole))==FALSE)
			{
				ar<<(BYTE)20;
				const BYTE type = pGeom->Gettype();
				ar<<type;

				pGeom->ReleaseDCOM(ar);
			}
			else
			{
				ar<<(BYTE)21;
				CPole* pole = (CPole*)pGeom;
				pole->ReleaseDCOM(ar);
			}
		}
	}
	else
	{
		ar>>m_wId;

		unsigned short nCount;
		ar>>nCount;
		for(unsigned short index = 0; index<nCount; index++)
		{
			BYTE type;
			ar>>type;
			if(type==20)
			{
				BYTE type;
				ar>>type;
				CGeom* pGeom = CGeom::Apply(type);
				if(pGeom!=nullptr)
				{
					pGeom->ReleaseDCOM(ar);
					m_geomlist.AddTail(pGeom);
				}
			}
			else if(type==21)
			{
				CPole* pole = new CPole();
				pole->ReleaseDCOM(ar);
			}
		}
	}
}

void CSymbol::ReleaseWeb(CFile& file) const
{
	unsigned short wId = m_wId;
	ReverseOrder(wId);
	file.Write(&wId, sizeof(unsigned short));

	unsigned short nCount = m_geomlist.GetCount();
	ReverseOrder(nCount);
	file.Write(&nCount, sizeof(unsigned short));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPole))==FALSE)
		{
			BYTE marker = 20;
			file.Write(&marker, sizeof(BYTE));

			pGeom->ReleaseWeb(file);
		}
		else
		{
			BYTE marker = 21;
			file.Write(&marker, sizeof(BYTE));

			CPole* pole = (CPole*)pGeom;
			pole->ReleaseWeb(file);
		}
	}
}

void CSymbol::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_wId;
	const unsigned short nCount = m_geomlist.GetCount();
	stream<<nCount;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPole))==FALSE)
		{
			const BYTE marker = 20;
			stream<<marker;

			pGeom->ReleaseWeb(stream);
		}
		else
		{
			const BYTE marker = 21;
			stream<<marker;

			CPole* pole = (CPole*)pGeom;
			pole->ReleaseWeb(stream);
		}
	}
}
void CSymbol::ReleaseWeb(boost::json::object& json) const
{
	json["Id"] = m_wId;
	const unsigned short nCount = m_geomlist.GetCount();
	json["Count"] = nCount;

	boost::json::array child1;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);

		boost::json::object child2;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPole))==FALSE)
		{
			child2["Mark"] = 20;
			pGeom->ReleaseWeb(child2);
		}
		else
		{
			child2["Mark"] = 21;
			CPole* pole = (CPole*)pGeom;
			pole->ReleaseWeb(child2);
		}
		child1.push_back(child2);
	}
	json["Geoms"] = child1;
}
void CSymbol::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_wId);

	writer.add_variant_uint32(m_geomlist.GetCount());
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPole))==FALSE)
		{
			writer.add_byte(20);
			pGeom->ReleaseWeb(writer);
		}
		else
		{
			writer.add_byte(21);
			CPole* pole = (CPole*)pGeom;
			pole->ReleaseWeb(writer);
		}
	}
}
void CSymbol::RecalRect()
{
	m_Rect.SetRectEmpty();

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		unsigned short inflate = 0;
		if(pGeom->m_pLine!=nullptr)
			inflate = pGeom->m_pLine->m_nWidth/2+1;
		CRect rect = pGeom->m_Rect;
		rect.InflateRect(inflate, inflate);
		m_Rect = UnionRect(m_Rect, rect);
	}
}
