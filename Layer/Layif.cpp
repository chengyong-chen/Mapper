#include "stdafx.h"
#include "Layif.h"
#include "LayerTree.h"

#include "../Style/Spot.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Tag.h"

#include "../Public/ODBCDatabase.h"
#include "../Dataview/viewinfo.h"
#include "../Database/ODBCRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CLayif::CLayif(CTree<CLayer>& tree)
	:CLayer(tree)
{

}
CLayif::CLayif(CTree<CLayer>& tree, CLaylg* parent, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: CLayer(tree, (CLayer*)parent, minLevelObj, maxLevelObj, minLevelTag, maxLevelTag)
{
}

CLayif::~CLayif()
{
	m_geomlist.RemoveAll();
}
void CLayif::Wipeout()
{
	m_geomlist.RemoveAll();

	CLayer::Wipeout();	
}
void CLayif::Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms)
{
	CLayer::Serialize(ar, dwVersion, true);

	if(ar.IsStoring())
	{
		ar<<m_strIf;
	}
	else
	{
		ar>>m_strIf;
	}
}