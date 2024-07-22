#include "stdafx.h"
#include "Resource.h"
#include "Global.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include "ImportArcView.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Doughnut.h"

#include "../Projection/Projection1.h"
#include "../Projection/Geographic.h"
#include "../Projection/Geocentric.h"

#include "../Dataview/Datainfo.h"
#include "../DataView/DatainfoDlg.h"

#include "../../Thirdparty\GDAL\3.3.3\gcore\gdal.h"
#include "../../Thirdparty\GDAL\3.3.3\ogr\ogr_api.h"
#include "../../Thirdparty\GDAL\3.3.3\ogr\ogr_srs_api.h"
#include "../../Thirdparty\GDAL\3.3.3\ogr\ogr_feature.h"
#include "../../Thirdparty\GDAL\3.3.3\ogr\ogrsf_frmts\ogrsf_frmts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CImportArcView::CImportArcView(CDatainfo& datainfo)
	:CImport(datainfo)
{
	m_fScale = 100;
}

void CImportArcView::ReadLayer(CLayer* layer, LPCTSTR lpszShape, CDaoDatabase* pDataBase)
{
	CT2CA caShape(lpszShape);
	SHPHandle shphandle = SHPOpen(caShape.m_psz, "rb");
	if(shphandle == nullptr)
		return;

	int shapetype;
	int entitycount;
	SHPGetInfo(shphandle, &entitycount, &shapetype, nullptr, nullptr);
	for(int entity = 0; entity < entitycount; entity++)
	{
		SHPObject* pSHPObject = SHPReadObject(shphandle, entity);

		CGeom* pGeom = nullptr;
		switch(pSHPObject->nSHPType)
		{
			case SHPT_NULL:
				break;
			case SHPT_POINT:
			case SHPT_POINTZ:
			case SHPT_POINTM:
				{
					double x = pSHPObject->padfX[0];
					double y = pSHPObject->padfY[0];

					CPoint center = Change(x, y);
					CMark* mark = new CMark(center);
					mark->m_geoId = ++m_maxId;
					mark->CalCorner(layer->GetSpot(), m_datainfo, &library);
					pGeom = mark;
				}
				break;
			case SHPT_MULTIPOINT:
			case SHPT_MULTIPOINTZ:
			case SHPT_MULTIPOINTM:
				break;
			case SHPT_ARC:
			case SHPT_ARCZ:
			case SHPT_ARCM:
				{
					CGroup* pGroup = nullptr;
					if(pSHPObject->nParts > 1)
					{
						pGroup = new CGroup;
						pGroup->m_geoId = ++m_maxId;
						pGroup->m_bClosed = false;
						pGeom = pGroup;
					}
					for(int part = 0; part < pSHPObject->nParts; ++part)
					{
						CPoly* poly = new CPoly;
						poly->m_geoId = ++m_maxId;
						poly->m_bClosed = false;

						int idxStart = pSHPObject->panPartStart[part];
						int idxStop = part == pSHPObject->nParts - 1 ? pSHPObject->nVertices : pSHPObject->panPartStart[part + 1];
						for(int anchor = idxStart + 1; anchor < idxStop; ++anchor)
						{
							double x = pSHPObject->padfX[anchor];
							double y = pSHPObject->padfY[anchor];
							CPoint point = Change(x, y);
							poly->AddAnchor(point, true, true);
						}
						poly->RecalRect();
						if(pGroup != nullptr)
							pGroup->m_geomlist.AddTail(poly);
						else
							pGeom = poly;
					}
				}
				break;
			case SHPT_POLYGON:
			case SHPT_POLYGONZ:
			case SHPT_POLYGONM:
				{
					CDoughnut* pDonut = nullptr;
					if(pSHPObject->nParts > 1)
					{
						pDonut = new CDoughnut;
						pDonut->m_geoId = ++m_maxId;
						pDonut->m_bClosed = true;
						pGeom = pDonut;
					}
					for(int part = 0; part < pSHPObject->nParts; ++part)
					{
						CPoly* poly = new CPoly;
						poly->m_geoId = ++m_maxId;
						poly->m_bClosed = true;

						int idxStart = pSHPObject->panPartStart[part];
						int idxStop = part == pSHPObject->nParts - 1 ? pSHPObject->nVertices : pSHPObject->panPartStart[part + 1];
						for(int anchor = idxStart + 1; anchor < idxStop; ++anchor)
						{
							double x = pSHPObject->padfX[anchor];
							double y = pSHPObject->padfY[anchor];
							CPoint point = Change(x, y);
							poly->AddAnchor(point, true, true);
						}
						poly->RecalRect();
						if(pDonut != nullptr)
							pDonut->m_geomlist.AddTail(poly);
						else
							pGeom = poly;
					}
				}
				break;
			case SHPT_MULTIPATCH:
				break;
		}

		if(pGeom == nullptr)
			continue;
		try
		{
			if(rs.IsOpen() == TRUE)
			{
				rs.AddNew();
				int index = 0;
				int	fields = DBFGetFieldCount(dbfhandle);
				for(int fieldno = 0; fieldno < fields; fieldno++)//Dump the feature attributes...
				{
					char pszName[255];
					DBFFieldType type = DBFGetFieldInfo(dbfhandle, fieldno, pszName, nullptr, nullptr);
					switch(type)
					{
						case FTInvalid:
							break;
						case FTString:
							{
								CString value(DBFReadStringAttribute(dbfhandle, entity, fieldno));
							}
							break;
						case FTInteger:
							{
								int value = DBFReadIntegerAttribute(dbfhandle, entity, fieldno);
								rs.SetFieldValue(index++, COleVariant((long)value, VT_I4));
							}
							break;
						case FTDouble:
							{
								double value = DBFReadDoubleAttribute(dbfhandle, entity, fieldno);
								rs.SetFieldValue(index++, COleVariant(value));
							}
							break;
						case FTLogical:
							break;
						default:
							break;
					}
				}
				rs.Update();
			}
		}
		catch(CDBException* ex)
		{
			OutputDebugString(ex->m_strError + "\r\n");
			e->ReportError();
			ex->Delete();
		}

		layer->m_geomlist.AddTail(pGeom);
	}

	SHPClose(shphandle);
	DBFClose(dbfhandle);
}

