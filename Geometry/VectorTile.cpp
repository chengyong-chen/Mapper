#include "stdafx.h"
#include "VectorTile.h"
#include <list>
#include <direct.h>
#include "../Geometry/Geom.h"

#include "../Public/Function.h"
#include "../Pbf/writer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CVectorTile, CObject, 0)

CVectorTile::CVectorTile()
{
}

CVectorTile::~CVectorTile()
{
	for(auto& it1:m_layergeomlist)
	{
		for(const auto& it2:it1.second)
		{
			CGeom* pGeom = it2;
			delete pGeom;
		}

		it1.second.clear();
	}
	m_layergeomlist.clear();
}

std::list<CGeom*>& CVectorTile::GetGeomList(WORD wId)
{
	for(auto& it:m_layergeomlist)
	{
		if(it.first==wId)
			return it.second;
	}

	m_layergeomlist.push_back(make_pair(wId, std::list<CGeom*>{}));
	return m_layergeomlist.back().second;
}

std::string CVectorTile::Extract(CSize& offset,DWORD dwVersion) const
{
	if(m_layergeomlist.size() == 0)
		return "";

	std::string cdata;
	pbf::writer writer(cdata);
	writer.add_fixed_uint32(dwVersion);

	for(const auto& it1 : m_layergeomlist)
	{
		const WORD wLayer = it1.first;
		const std::list<CGeom*>& geomlist = it1.second;
		if(geomlist.size() == 0)
			continue;

		writer.add_tag((pbf::tag)1,pbf::type::bytes);
		writer.add_variant_uint16(wLayer);
		writer.add_variant_uint32(geomlist.size());
		for(const auto& it2 : geomlist)
		{
			CGeom* pGeom = it2;
			pGeom->ReleaseWeb(writer, offset);
		}
	}	
	return cdata;
}
