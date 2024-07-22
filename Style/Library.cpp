#include "stdafx.h"
#include <cstdio>

#include "Library.h"
#include "MidTable.hpp"
#include "PatternMid.h"
#include "SymbolMid.hpp"
#include "SymbolFill.h"
#include "SymbolLine.h"
#include "SymbolSpot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLibrary, CObject, 0)

CLibrary::CLibrary()
{
}

CLibrary::~CLibrary()
{
}

void CLibrary::Initialize()
{
}

void CLibrary::Serialize(CArchive& ar, const DWORD& dwVersion)
{	
	//	m_Icmm.Serialize.Serialize(ar);
	m_libLineSymbol.Serialize(ar, dwVersion);
	m_libFillSymbol.Serialize(ar, dwVersion);
	m_libFillPattern.Serialize(ar, dwVersion);
	m_libFillFlash.Serialize(ar, dwVersion);
	m_libSpotSymbol.Serialize(ar, dwVersion);
	m_libSpotPattern.Serialize(ar, dwVersion);
	m_libSpotFlash.Serialize(ar, dwVersion);
}

void CLibrary::ReleaseCE(CArchive& ar) const
{
	m_libLineSymbol.ReleaseCE(ar);
	m_libFillSymbol.ReleaseCE(ar);
	m_libFillPattern.ReleaseCE(ar);
	m_libFillFlash.ReleaseCE(ar);
	m_libSpotSymbol.ReleaseCE(ar);
	m_libSpotPattern.ReleaseCE(ar);
	m_libSpotFlash.ReleaseCE(ar);
}

void CLibrary::ReleaseDCOM(CArchive& ar)
{
	m_libLineSymbol.ReleaseDCOM(ar);
	m_libFillSymbol.ReleaseDCOM(ar);
	m_libFillPattern.ReleaseDCOM(ar);
	m_libFillFlash.ReleaseDCOM(ar);
	m_libSpotSymbol.ReleaseDCOM(ar);
	m_libSpotPattern.ReleaseDCOM(ar);
	m_libSpotFlash.ReleaseDCOM(ar);
}

void CLibrary::ReleaseWeb(CFile& file) const
{
	m_libLineSymbol.ReleaseWeb(file);
	m_libFillSymbol.ReleaseWeb(file);
	m_libFillPattern.ReleaseWeb(file);
	m_libFillFlash.ReleaseWeb(file);
	m_libSpotSymbol.ReleaseWeb(file);
	m_libSpotPattern.ReleaseWeb(file);
	m_libSpotFlash.ReleaseWeb(file);
}

void CLibrary::ReleaseWeb(BinaryStream& stream) const
{
	m_libLineSymbol.ReleaseWeb(stream);
	m_libFillSymbol.ReleaseWeb(stream);
	m_libFillPattern.ReleaseWeb(stream);
	m_libFillFlash.ReleaseWeb(stream);
	m_libSpotSymbol.ReleaseWeb(stream);
	m_libSpotPattern.ReleaseWeb(stream);
	m_libSpotFlash.ReleaseWeb(stream);
}
void CLibrary::ReleaseWeb(boost::json::object& json) const
{
	boost::json::object child;	
	m_libLineSymbol.ReleaseWeb(child, "LineSymbol");
	m_libFillSymbol.ReleaseWeb(child, "FillSymbol");
	m_libFillPattern.ReleaseWeb(child, "FillPattern");
	//	m_libFillFlash.  ReleaseWeb(child1,);
	m_libSpotSymbol.ReleaseWeb(child, "SpotSymbol");
	m_libSpotPattern.ReleaseWeb(child, "SpotPattern");
	//	m_libSpotFlash.  ReleaseWeb(child1,);

	json["Library"] = child;
}

void CLibrary::ReleaseWeb(pbf::writer& writer) const
{
	m_libLineSymbol.ReleaseWeb(writer);
	m_libFillSymbol.ReleaseWeb(writer);
	m_libFillPattern.ReleaseWeb(writer);
//	m_libFillFlash.ReleaseWeb(writer);
	m_libSpotSymbol.ReleaseWeb(writer);
	m_libSpotPattern.ReleaseWeb(writer);
//	m_libSpotFlash.ReleaseWeb(writer);
}
