#include "stdafx.h"

#include "Datainfo.h"
#include "ViewMonitor.h"

#include "../Projection/Projection1.h"


#include "../Public/Function.h"
#include "../Public/Global.h"
#include "../Public/BinaryStream.h"

#include "../Projection/Geocentric.h"
#include "../Pbf/writer.hpp"

#include <boost/json.hpp>
using namespace boost;


class CGeocentric;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDatainfo::CDatainfo()
{
	m_strName = _T("");
	m_TimeLine = CTime::GetCurrentTime();
	m_dwMapKey = 0;

	m_scaleMinimum = 128; // ¶ÔÓÚÓÐÍ¶Ó°µÄÎÄ¼þ
	m_scaleSource = 1;
	m_scaleMaximum = 0.01;

	m_pProjection = nullptr;
	m_zoomPointToDoc = 10000.f;
}

CDatainfo::CDatainfo(const CDatainfo& datainfo)
{
	m_mapOrigin = datainfo.m_mapOrigin;
	m_mapCanvas = datainfo.m_mapCanvas;
	if(datainfo.m_pProjection!=nullptr)
	{
		m_pProjection = datainfo.m_pProjection->Clone();
	}
	m_strType = datainfo.m_strType;
	m_strName = datainfo.m_strName;
	m_TimeLine = datainfo.m_TimeLine;
	m_dwMapKey = datainfo.m_dwMapKey;
	m_scaleMinimum = datainfo.m_scaleMinimum;
	m_scaleSource = datainfo.m_scaleSource;
	m_scaleMaximum = datainfo.m_scaleMaximum;
}

CDatainfo::~CDatainfo()
{
	delete m_pProjection;
	m_pProjection = nullptr;
}

CDatainfo& CDatainfo::operator=(const CDatainfo& datainfo)
{
	m_mapOrigin = datainfo.m_mapOrigin;
	m_mapCanvas = datainfo.m_mapCanvas;
	delete m_pProjection;
	m_pProjection = nullptr;

	m_strType = datainfo.m_strType;
	m_strName = datainfo.m_strName;
	m_TimeLine = datainfo.m_TimeLine;
	m_dwMapKey = datainfo.m_dwMapKey;
	m_scaleMinimum = datainfo.m_scaleMinimum;
	m_scaleSource = datainfo.m_scaleSource;
	m_scaleMaximum = datainfo.m_scaleMaximum;
	m_zoomPointToDoc = datainfo.m_zoomPointToDoc;
	CProjection* pProjection = datainfo.m_pProjection!=nullptr ? datainfo.m_pProjection->Clone() : nullptr;
	CMapinfo::ResetProjection(pProjection, false);
	return *this;
}

CRect CDatainfo::GetDocRect() const
{
	const CRectF mapRect = this->GetMapRect();
	return CRect(mapRect.left*m_dilationMapToDoc, mapRect.top*m_dilationMapToDoc, mapRect.right*m_dilationMapToDoc, mapRect.bottom*m_dilationMapToDoc);
}

CSize CDatainfo::GetDocCanvas() const
{
	return CSize(m_mapCanvas.cx*m_dilationMapToDoc, m_mapCanvas.cy*m_dilationMapToDoc);
}

CPoint CDatainfo::GetDocOrigin() const
{
	return CPoint(m_mapOrigin.x*m_dilationMapToDoc, m_mapOrigin.y*m_dilationMapToDoc);
}

double CDatainfo::GetMapToViewFactorFromScale(const CProjection* pProjection, const float& fScale, const CPointF& mapPoint, const CSize& dpi)
{
	if(pProjection==nullptr)
		return dpi.cx/(fScale*72.f);
	else if(pProjection->m_strUnit=="Meter")
		return dpi.cx/fScale/0.0254;
	else if(fScale<=0.0)
		return -INFINITE;
	else
	{
		const CPointF mapPoint1 = mapPoint;
		const CPointF mapPoint2 = CPointF(mapPoint1.x+0.1, mapPoint1.y);
		const CPointF geoPoint1 = pProjection->MapToGeo(mapPoint1);
		const CPointF geoPoint2 = pProjection->MapToGeo(mapPoint2);
		float distance = pProjection->Distance(geoPoint1.x, geoPoint1.y, geoPoint2.x, geoPoint2.y); //0.1map³¤¶È¶ÔÓ¦µÄÊµ¼Ê¾àÀë
		distance /= 0.1;//1map¶ÔÓ¦µÄ¾àÀë
		distance /= fScale;//ditance ±ÈÀý±ä»»ºóµÄ¾àÀë
		return distance/0.0254*dpi.cx;//1map¶ÔÓ¦µÄÆÁÄ»ÉÏµÄÏñËØÊý,Ò²¾ÍÊÇfZoom
	}
}

float CDatainfo::GetScaleFromMapToViewRatio(const CProjection* pProjection, const float& factorMapToView, const CPointF& mapPoint, const CSize& dpi)
{
	if(pProjection==nullptr)
		return dpi.cx/(factorMapToView*72.f);
	else if(pProjection->m_strUnit=="Meter")
		return dpi.cx/(factorMapToView*0.0254);
	else
	{
		const CPointF mapPoint1 = mapPoint;
		const CPointF mapPoint2 = CPointF(mapPoint1.x+0.001, mapPoint1.y);
		const CPointF geoPoint1 = pProjection->MapToGeo(mapPoint1);
		const CPointF geoPoint2 = pProjection->MapToGeo(mapPoint2);
		const float distance = pProjection->Distance(geoPoint1.x, geoPoint1.y, geoPoint2.x, geoPoint2.y);
		const double pixels = 0.001*factorMapToView;
		const double length = 0.0254*pixels/72.f;
		return distance/length;
		float Δ = 72.f/factorMapToView;//1 inch(2.54cm)¶ÔÓ¦µÄµØÍ¼×ø±êµÄ´óÐ¡
		return distance/0.0254;//1Ã×ÆÁÄ»¶ÔÓ¦¶ÔÓ¦µÄµØÇòÉÏµÄ¾àÀë,²¢ÇÒ·Å´ó100±¶
	}
}

CRect CDatainfo::WanToDoc(CRect rect) const
{
	rect.left = round(rect.left*m_zoomPointToDoc/10000.f);
	rect.top = round(rect.top*m_zoomPointToDoc/10000.f);
	rect.right = round(rect.right*m_zoomPointToDoc/10000.f);
	rect.bottom = round(rect.bottom*m_zoomPointToDoc/10000.f);

	return rect;
}

void CDatainfo::ResetProjection(CProjection* pProjection, bool action)
{
	if(m_pProjection==nullptr&&pProjection!=nullptr)
		m_zoomPointToDoc = CalPointToDocZoom(pProjection);
	else if(m_pProjection!=nullptr&&pProjection==nullptr)
		m_zoomPointToDoc = 10000.f;
	else if(m_pProjection==nullptr&&pProjection==nullptr)
		m_zoomPointToDoc = CalPointToDocZoom(pProjection);
	else if(*m_pProjection!=*pProjection)
	{
		m_zoomPointToDoc = CalPointToDocZoom(pProjection);
		if(action)
		{
			double minx = 100000000;
			double miny = 100000000;
			double maxx = -100000000;
			double maxy = -100000000;

			CRectF  rect = GetMapRect();
			CPointF mapPoint1 = CPointF(rect.left, rect.top);
			CPointF mapPoint2 = CPointF(rect.right, rect.top);
			CPointF mapPoint3 = CPointF(rect.right, rect.bottom);
			CPointF mapPoint4 = CPointF(rect.left, rect.bottom);

			Detail(mapPoint1, mapPoint2, 100, minx, miny, maxx, maxy, m_pProjection, pProjection);
			Detail(mapPoint2, mapPoint3, 100, minx, miny, maxx, maxy, m_pProjection, pProjection);
			Detail(mapPoint3, mapPoint4, 100, minx, miny, maxx, maxy, m_pProjection, pProjection);
			Detail(mapPoint4, mapPoint1, 100, minx, miny, maxx, maxy, m_pProjection, pProjection);
			Detail(mapPoint1, mapPoint3, 100, minx, miny, maxx, maxy, m_pProjection, pProjection);
			Detail(mapPoint4, mapPoint2, 100, minx, miny, maxx, maxy, m_pProjection, pProjection);

			m_mapOrigin.x = minx;
			m_mapOrigin.y = miny;
			m_mapCanvas.cx = maxx-minx;
			m_mapCanvas.cy = maxy-miny;
		}
	}
	else
		m_zoomPointToDoc = CalPointToDocZoom(pProjection);

	CMapinfo::ResetProjection(pProjection, false);
}

void CDatainfo::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CStringA strMark = "Diwatu";
	if(ar.IsStoring())
	{
		ar<<strMark;
		ar<<AfxGetSoftVersion();
		ar<<m_strName;
		ar<<m_TimeLine;
		ar<<m_dwMapKey;

		ar<<(m_pProjection!=nullptr ? m_pProjection->MapToUser(m_mapOrigin.x) : m_mapOrigin.x);
		ar<<(m_pProjection!=nullptr ? m_pProjection->MapToUser(m_mapOrigin.y) : m_mapOrigin.y);
		ar<<(m_pProjection!=nullptr ? m_pProjection->MapToUser(m_mapCanvas.cx) : m_mapCanvas.cx);
		ar<<(m_pProjection!=nullptr ? m_pProjection->MapToUser(m_mapCanvas.cy) : m_mapCanvas.cy);

		ar<<m_scaleMinimum;
		ar<<m_scaleSource;
		ar<<m_scaleMaximum;
		const BYTE type = m_pProjection==nullptr ? 0XFF : m_pProjection->Gettype();
		ar<<type;
		if(m_pProjection!=nullptr)
		{
			m_pProjection->Serialize(ar, dwVersion);
		}
	}
	else
	{
		DWORD dwSoftVersion;
		ar>>strMark;
		ar>>dwSoftVersion;
		ar>>m_strName;
		ar>>m_TimeLine;
		ar>>m_dwMapKey;

		CPointF userOrigin;
		CSizeF userCanvas;
		ar>>userOrigin.x;
		ar>>userOrigin.y;
		ar>>userCanvas.cx;
		ar>>userCanvas.cy;

		ar>>m_scaleMinimum;
		ar>>m_scaleSource;
		ar>>m_scaleMaximum;

		BYTE type;
		ar>>type;
		delete m_pProjection;
		m_pProjection = nullptr;
		CProjection* pProjection = CProjection::Apply(type);
		if(pProjection!=nullptr)
		{
			pProjection->Serialize(ar, dwVersion);
			if(type==3)
			{
				::free(((CGeocentric*)pProjection)->m_keyProj4);
				((CGeocentric*)pProjection)->m_keyProj4 = _strdup("tmerc");
			}
			pProjection->Reconfig();
			m_mapOrigin.x = pProjection->UserToMap(userOrigin.x);
			m_mapOrigin.y = pProjection->UserToMap(userOrigin.y);
			m_mapCanvas.cx = pProjection->UserToMap(userCanvas.cx);
			m_mapCanvas.cy = pProjection->UserToMap(userCanvas.cy);
			ResetProjection(pProjection, false);
		}
		else
		{
			m_mapOrigin = userOrigin;
			m_mapCanvas = userCanvas;
		}
		m_zoomPointToDoc = CalPointToDocZoom(m_pProjection);
	}

	m_BackInfo.Serialize(ar, dwVersion);
}

void CDatainfo::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		SerializeStrCE(ar, m_strName);
		ar<<AfxGetSoftVersion();
		ar<<m_dwMapKey;
		const BYTE type = m_pProjection==nullptr ? 0XFF : m_pProjection->Gettype();
		ar<<type;
	}

	if(m_pProjection!=nullptr)
	{
		m_pProjection->ReleaseCE(ar);
	}
}

void CDatainfo::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CDatainfo::ReleaseWeb(CFile& file) const
{
	SaveAsUTF8(file, _T("Diwatu"));
	{
		DWORD dwVersion = AfxGetSoftVersion();
		ReverseOrder(dwVersion);
		file.Write(&dwVersion, sizeof(float));
	}
	SaveAsUTF8(file, m_strName);

	const BYTE type = m_pProjection==nullptr ? 0XFF : m_pProjection->Gettype();
	file.Write(&type, sizeof(BYTE));
	if(m_pProjection!=nullptr)
	{
		m_pProjection->ReleaseWeb(file);
	}

	{
		const double data = ReverseOrder(m_mapOrigin.x);
		file.Write(&data, sizeof(double));
	}
	{
		const double data = ReverseOrder(m_mapOrigin.y);
		file.Write(&data, sizeof(double));
	}
	{
		const double data = ReverseOrder(m_mapCanvas.cx);
		file.Write(&data, sizeof(double));
	}
	{
		const double data = ReverseOrder(m_mapCanvas.cy);
		file.Write(&data, sizeof(double));
	}
	const CRectF mapRect = this->GetMapRect();
	const double mapWidth = mapRect.Width();
	const double mapHeight = mapRect.Height();
	const double mapMax = max(mapWidth, mapHeight);
	const double factorLevel0 = 256.f/mapMax;
	const double ratioMaximum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMaximum, mapRect.CenterPoint(), CSize(72, 72));
	const double ratioMinimum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMinimum, mapRect.CenterPoint(), CSize(72, 72));
	const BYTE levelMinimum = ratioMinimum<factorLevel0 ? 0 : floor(log(ratioMinimum/factorLevel0)/log(2.0));
	const BYTE levelMaximum = ratioMaximum<factorLevel0 ? 0 : ceil(log(ratioMaximum/factorLevel0)/log(2.0));
	double levelSource = GetLevelFromScale(this->m_scaleSource);

	file.Write(&levelMinimum, sizeof(BYTE));
	file.Write(&levelMaximum, sizeof(BYTE));
	levelSource = ReverseOrder(levelSource);
	file.Write(&levelSource, sizeof(double));
}

void CDatainfo::ReleaseWeb(BinaryStream& stream) const
{
	SaveAsUTF8(stream, _T("Diwatu"));
	stream<<AfxGetSoftVersion();
	SaveAsUTF8(stream, m_strName);
	const BYTE type = m_pProjection==nullptr ? 0XFF : m_pProjection->Gettype();
	stream<<type;
	if(m_pProjection!=nullptr)
	{
		m_pProjection->ReleaseWeb(stream);
	}

	stream<<m_mapOrigin.x;
	stream<<m_mapOrigin.y;

	stream<<m_mapCanvas.cx;
	stream<<m_mapCanvas.cy;
	const CRectF mapRect = this->GetMapRect();
	const double mapWidth = mapRect.Width();
	const double mapHeight = mapRect.Height();
	const double mapMax = max(mapWidth, mapHeight);
	const double factorLevel0 = 256.f/mapMax;
	const double ratioMaximum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMaximum, mapRect.CenterPoint(), CSize(72, 72));
	const double ratioMinimum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMinimum, mapRect.CenterPoint(), CSize(72, 72));
	const BYTE levelMinimum = ratioMinimum<factorLevel0 ? 0 : floor(log(ratioMinimum/factorLevel0)/log(2.0));
	const BYTE levelMaximum = ratioMaximum<factorLevel0 ? 0 : ceil(log(ratioMaximum/factorLevel0)/log(2.0));
	const double levelSource = GetLevelFromScale(this->m_scaleSource);

	stream<<levelMinimum;
	stream<<levelMaximum;
	stream<<levelSource;
}

double CDatainfo::GetLevelFromScale(const float& scale) const
{
	if(scale<=0.f)
		return 25;
	else
	{
		const CRectF mapRect = this->GetMapRect();
		const double mapWidth = mapRect.Width();
		const double mapHeight = mapRect.Height();
		const double mapMax = max(mapWidth, mapHeight);
		const double factorLevel0 = 256.f/mapMax;
		const double factor = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, scale, mapRect.CenterPoint(), CSize(72, 72));
		return log(factor/factorLevel0)/log(2.0);
	}
}

void CDatainfo::ReleaseWeb(boost::json::object& json, BYTE& minMapLevel, BYTE& maxMapLevel, float& sourceLevel) const
{
	json["Marker"] = "Diwatu";
	json["Version"] = AfxGetSoftVersion();
	json["Name"] = CW2A(m_strName.GetString());

	if(m_pProjection!=nullptr)
	{
		boost::json::object child;
		const BYTE type = m_pProjection->Gettype();
		child["Type"] = type;
		m_pProjection->ReleaseWeb(child);
		json["Project"] = child;
	}

	json["x"] = m_mapOrigin.x;
	json["y"] = m_mapOrigin.y;
	json["cx"] = m_mapCanvas.cx;
	json["cy"] = m_mapCanvas.cy;

	const CRectF mapRect = this->GetMapRect();
	const double mapWidth = mapRect.Width();
	const double mapHeight = mapRect.Height();
	const double mapMax = max(mapWidth, mapHeight);
	const double factorLevel0 = 256.f/mapMax;
	const double ratioMaximum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMaximum, mapRect.CenterPoint(), CSize(72, 72));
	const double ratioMinimum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMinimum, mapRect.CenterPoint(), CSize(72, 72));
	const double factorSource = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleSource, mapRect.CenterPoint(), CSize(72, 72));
	const BYTE levelMinimum = ratioMinimum<factorLevel0 ? 0 : floor(log(ratioMinimum/factorLevel0)/log(2.0));
	const BYTE levelMaximum = ratioMaximum<factorLevel0 ? 0 : ceil(log(ratioMaximum/factorLevel0)/log(2.0));
	const float levelSource = log(factorSource/factorLevel0)/log(2.0);

	json["MinLevel"] = levelMinimum;
	json["MaxLevel"] = levelMaximum;
	json["DftLevel"] = levelSource;

	minMapLevel = levelMinimum;
	maxMapLevel = levelMaximum;
	sourceLevel = levelSource;
}
void CDatainfo::ReleaseWeb(pbf::writer& writer, pbf::tag tag, BYTE& minMapLevel, BYTE& maxMapLevel, float& sourceLevel) const
{
	writer.add_tag(tag, pbf::type::bytes);

	writer.add_string("Diwatu");
	writer.add_fixed_uint32(AfxGetSoftVersion());
	writer.add_string(CW2A(m_strName.GetString(), CP_UTF8));

	writer.add_double(m_mapOrigin.x);
	writer.add_double(m_mapOrigin.y);
	writer.add_double(m_mapCanvas.cx);
	writer.add_double(m_mapCanvas.cy);

	const CRectF mapRect = this->GetMapRect();
	const double mapWidth = mapRect.Width();
	const double mapHeight = mapRect.Height();
	const double mapMax = max(mapWidth, mapHeight);
	const double factorLevel0 = 256.f/mapMax;
	const double ratioMaximum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMaximum, mapRect.CenterPoint(), CSize(72, 72));
	const double ratioMinimum = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleMinimum, mapRect.CenterPoint(), CSize(72, 72));
	const double factorSource = CDatainfo::GetMapToViewFactorFromScale(this->m_pProjection, this->m_scaleSource, mapRect.CenterPoint(), CSize(72, 72));
	const BYTE levelMinimum = ratioMinimum<factorLevel0 ? 0 : floor(log(ratioMinimum/factorLevel0)/log(2.0));
	const BYTE levelMaximum = ratioMaximum<factorLevel0 ? 0 : ceil(log(ratioMaximum/factorLevel0)/log(2.0));
	const float levelSource = log(factorSource/factorLevel0)/log(2.0);

	writer.add_byte(levelMinimum);
	writer.add_byte(levelMaximum);
	writer.add_float(levelSource);
	writer.add_float(levelSource);

	minMapLevel = levelMinimum;
	maxMapLevel = levelMaximum;
	sourceLevel = levelSource;

	writer.add_bool(false);//this is the room for central

	if(m_pProjection!=nullptr)
	{
		writer.add_bool(true);

		const BYTE type = m_pProjection->Gettype();
		writer.add_byte(type);
		m_pProjection->ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);
}

double CDatainfo::CalPointToDocZoom(const CProjection* pProjection) const
{
	if(pProjection==nullptr)
	{
		return 10000.0f;//when no project the m_dilationMapToDoc is always 10
	}
	else if(pProjection->m_strUnit=="Meter")
	{
		const double distance = m_scaleSource*0.0254/72.f;//1Point¶ÔÓ¦µÄÖ½ÉÏµÄ¾àÀë(Ã×) 1Point ¶ÔÓ¦µÄÊµ¼Ê¾àÀë(Ã×)
		return distance*pProjection->GetDilationMapToDoc();//1 Gdiplus::Point ¶ÔÓ¦µÄdoc
	}
	else
	{
		const CPointF mapPoint1 = CPointF(m_mapOrigin.x+m_mapCanvas.cx/2, m_mapOrigin.y+m_mapCanvas.cy/2);
		const CPointF mapPoint2 = CPointF(mapPoint1.x+0.1, mapPoint1.y);
		const CPointF geoPoint1 = pProjection->MapToGeo(mapPoint1);
		const CPointF geoPoint2 = pProjection->MapToGeo(mapPoint2);
		double distance1 = pProjection->Distance(geoPoint1.x, geoPoint1.y, geoPoint2.x, geoPoint2.y); //0.1map³¤¶È¶ÔÓ¦µÄÊµ¼Ê¾àÀë(Ã×)
		distance1 *= 10;//1map¶ÔÓ¦µÄµØÇòÉÏµÄ¾àÀë(Ã×)
		distance1 *= pProjection->GetDilationDocToMap();//1doc ¶ÔÓ¦µÄÊµ¼Ê¾àÀë(Ã×)
		const double distance2 = m_scaleSource*0.0254/72.f;//1Point¶ÔÓ¦µÄÖ½ÉÏµÄ¾àÀë(Ã×)//1Point ¶ÔÓ¦µÄÊµ¼Ê¾àÀë(Ã×)
		return distance2/distance1;//1Point ¶ÔÓ¦µÄDoc
	}
}

CPoint CDatainfo::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const CPoint& point)
{
	CPointF mapPoint = fMapinfo.DocToMap(point);
	if(fMapinfo.m_pProjection!=nullptr&&tMapinfo.m_pProjection!=nullptr)
	{
		CPointF geoPoint = fMapinfo.m_pProjection->MapToGeo(mapPoint);
		if(fMapinfo.m_pProjection->GCS.m_Key&&tMapinfo.m_pProjection->GCS.m_Key&&strcmp(fMapinfo.m_pProjection->GCS.m_Key, tMapinfo.m_pProjection->GCS.m_Key)!=0)
		{
			geoPoint = fMapinfo.m_pProjection->GCS.ThistoWGS84(geoPoint);
			geoPoint = tMapinfo.m_pProjection->GCS.WGS84toThis(geoPoint);
		}
		mapPoint = tMapinfo.m_pProjection->GeoToMap(geoPoint);
	}
	return tMapinfo.MapToDoc(mapPoint);
}