#include "stdafx.h"

#include "ImportGrf.h"
#include "../Geometry/Poly.h"
#include "../Projection/Projection1.h"
#include "../Layer/Layer.h"
#include "../DataView/DataInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImportGrf::CImportGrf(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId)
	: CImport(datainfo, layertree, dwMaxGeomId)
{
}

BOOL CImportGrf::Import(LPCTSTR lpszFile, CLayer* pLayer)
{
	CFile Route;
	if(Route.Open(lpszFile, CFile::modeRead|CFile::shareDenyWrite|CFile::typeBinary)==FALSE)
	{
		return FALSE;
	}

	DWORD dwId;
	Route.Read(&dwId, 4);

	const long length = Route.GetLength();
	const int Count = (length-4)/(8*2+4*3);
	CPoint fPoint = CPoint(-100000, -100000);
	CPoly* pPoly = nullptr;
	for(int i = 0; i<Count; i++)
	{
		double lat;
		double lng;
		double altitude;
		float nBearing;
		float fSpeed;
		long nTime;

		Route.Read(&lng, 8);
		Route.Read(&lat, 8);
		Route.Read(&fSpeed, 4);
		Route.Read(&nBearing, 4);
		Route.Read(&nTime, 4);

		CPointF geoPoint(lng, lat);
		geoPoint = m_datainfo.m_pProjection->GCS.WGS84toThis(geoPoint);
		const CPointF mapPoint = m_datainfo.m_pProjection->GeoToMap(geoPoint);
		CPoint docPoint = m_datainfo.MapToDoc(mapPoint);
		const CSize Δ = docPoint-fPoint;
		if(abs(Δ.cx)>500||abs(Δ.cy)>500)
		{
			if(pPoly!=nullptr)
			{
				pPoly->RecalRect();
				pLayer->m_geomlist.AddTail(pPoly);
				pPoly = nullptr;
			}

			pPoly = new CPoly;
			pPoly->m_geoId = ++m_dwMaxGeomId;
		}

		if(pPoly!=nullptr)
		{
			pPoly->AddAnchor(CPoint(docPoint.x, docPoint.y), true, true);
		}

		fPoint = docPoint;
	}

	if(pPoly!=nullptr)
	{
		pLayer->m_geomlist.AddTail(pPoly);
	}

	return TRUE;
}
