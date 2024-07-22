#include "stdafx.h"

#include "Global.h"
#include "Tag.h"
#include "Geom.h"
#include "Poly.h"
#include "Cluster.h"
#include "PRect.h"
#include "../Database/ODBCRecordset.h"
#include "../Style/Line.h"
#include "../Style/Hint.h"
#include "../Style/FillCompact.h"
#include "../Pbf/writer.hpp"
#include "../Public/BinaryStream.h"

#include <vector>
#include <queue>
#include <string>
#include <boost/json.hpp>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCluster::CCluster()
	: CCollection<CPath>()
{
	m_geogroid = nullptr;
	m_labeloid = nullptr;

	m_bType = 19;
	m_bClosed = true;
}

CCluster::~CCluster()
{
	if(m_geogroid!=nullptr)
	{
		delete m_geogroid;
		m_geogroid = nullptr;
	}
	if(m_labeloid!=nullptr)
	{
		delete m_labeloid;
		m_labeloid = nullptr;
	}
}

CLine* CCluster::GetLine() const
{
	if(m_geomlist.GetCount()==0)
		return nullptr;

	CPath* pWrapper = m_geomlist.GetHead();
	if(pWrapper==nullptr)
		return nullptr;

	return pWrapper->m_pLine!=nullptr ? pWrapper->m_pLine : m_pLine;
}

CFillCompact* CCluster::GetFill() const
{
	if(m_geomlist.GetCount()==0)
		return nullptr;

	CPath* pWrapper = m_geomlist.GetHead();
	if(pWrapper==nullptr)
		return nullptr;

	return pWrapper->m_pFill!=nullptr ? pWrapper->m_pFill : m_pFill;
}
CPoint CCluster::GetGeogroid() const
{
	return m_geogroid!=nullptr ? *m_geogroid : m_centroid;
}
CPoint CCluster::GetLabeloid() const
{
	return m_labeloid!=nullptr ? *m_labeloid : m_centroid;
}
void CCluster::GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPath* pPath = m_geomlist.GetNext(pos);
		pPath->GetPaths64(paths64, tolerance);
	}
}
void CCluster::SetGeogroid(const CPoint& point)
{
	m_geogroid = m_geogroid == nullptr ? new CPoint() : m_geogroid;
	*m_geogroid = point;
};
void CCluster::SetLabeloid(const CPoint& point)
{
	m_labeloid = m_labeloid == nullptr ? new CPoint(point) : m_labeloid;
	*m_labeloid = point;
};
Gdiplus::PointF CCluster::GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const
{
	if(m_bClosed==true)
	{
		hAlign = HALIGN::HA_CENTER;
		vAlign = VALIGN::VA_CENTER;
		const CPoint docPoint = this->GetLabeloid();
		return viewinfo.DocToClient<Gdiplus::PointF>(docPoint);
	}
	else
		return CGeom::GetTagAnchor(viewinfo, hAlign, vAlign);
}

void CCluster::CopyTo(CGeom* pGeom, bool ignore) const
{
	CCollection<CPath>::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CCluster)) == TRUE)
	{
		((CCluster*)pGeom)->m_centroid = m_centroid;
	}
}
void CCluster::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	DWORD reserved1 = 0B00000000000000000000000000000000;
	if(ar.IsStoring())
	{
		if(m_geogroid!=nullptr)
			reserved1 |= 0B00000000000000000000000000000001;
		if(m_labeloid!=nullptr)
			reserved1 |= 0B00000000000000000000000000000010;
		ar<<reserved1;
		ar<<m_centroid;
		if(m_geogroid!=nullptr)
			ar<<*m_geogroid;
		if(m_labeloid!=nullptr)
			ar<<*m_labeloid;
	}
	else
	{
		ar>>reserved1;
		ar>>m_centroid;
		if(reserved1&0B00000000000000000000000000000001==(DWORD)0B00000000000000000000000000000001)
		{
			m_geogroid = new CPoint;
			ar>>*m_geogroid;
		}
		if(reserved1&0B00000000000000000000000000000010==(DWORD)0B00000000000000000000000000000010)
		{
			m_labeloid = new CPoint;
			ar>>*m_labeloid;
		}
	}
	CGeom::Serializelist(ar, dwVersion, m_geomlist);
}
void CCluster::ReleaseCE(CArchive& ar) const
{
	CCollection<CPath>::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<(int)m_geomlist.GetCount();
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPath* geom = m_geomlist.GetNext(pos);
			if(geom!=nullptr)
			{
				const BYTE type = geom->Gettype();
				ar<<type;
				geom->ReleaseCE(ar);
			}
		}
	}
}

void CCluster::ReleaseDCOM(CArchive& ar)
{
	CCollection<CPath>::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		const unsigned short nCount = m_geomlist.GetCount();
		ar<<nCount;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPath* geom = m_geomlist.GetNext(pos);
			const BYTE type = geom->Gettype();
			ar<<type;
			geom->ReleaseDCOM(ar);
		}
	}
	else
	{
		unsigned short nCount;
		ar>>nCount;
		for(int index = 0; index<nCount; index++)
		{
			BYTE type;
			ar>>type;
			CPath* pPath = (CPath*)CGeom::Apply(type);
			if(pPath!=nullptr)
			{
				pPath->ReleaseDCOM(ar);
				m_geomlist.AddTail(pPath);
			}
		}
	}
}

bool CCluster::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	return CCollection<CPath>::GetValues(rs, datainfo, strTag);
}

bool CCluster::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	try
	{
		double minX = datainfo.DocToMap(m_Rect.left);
		double minY = datainfo.DocToMap(m_Rect.top);
		double maxX = datainfo.DocToMap(m_Rect.right);
		double maxY = datainfo.DocToMap(m_Rect.bottom);
		rs.Field(_T("minX")) = minX;
		rs.Field(_T("minY")) = minY;
		rs.Field(_T("maxX")) = maxX;
		rs.Field(_T("maxY")) = maxY;
		rs.Field(_T("Shape")) = 4;
		rs.Field(_T("Closed")) = m_bClosed;
		rs.Field(_T("Anchors")) = m_geomlist.GetCount();

		int nAnchors = 0;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPoly* pPath = (CPoly*)m_geomlist.GetNext(pos);
			nAnchors += pPath->m_nAnchors;
		}
		if(nAnchors>0)
		{
			SAFEARRAYBOUND rgsabound[1];
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = sizeof(CPointF)*nAnchors+m_geomlist.GetCount()*sizeof(unsigned short);

			SAFEARRAY* psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
			if(psa!=nullptr)
			{
				LPVOID lpArrayData = nullptr;
				::SafeArrayAccessData(psa, &lpArrayData);

				POSITION pos = m_geomlist.GetHeadPosition();
				while(pos!=nullptr)
				{
					CPoly* pPath = (CPoly*)m_geomlist.GetNext(pos);
					CPointF* pPoints = new CPointF[pPath->m_nAnchors];
					for(unsigned int index = 0; index<pPath->m_nAnchors; index++)
					{
						pPoints[index].x = datainfo.DocToMap(pPath->m_pPoints[index].x);
						pPoints[index].y = datainfo.DocToMap(pPath->m_pPoints[index].y);
					}

					::memcpy((char*)lpArrayData, &(pPath->m_nAnchors), sizeof(unsigned short));
					lpArrayData = (char*)lpArrayData+sizeof(unsigned short);
					::memcpy((char*)lpArrayData, pPoints, sizeof(CPointF)*pPath->m_nAnchors);
					lpArrayData = (char*)lpArrayData+sizeof(CPointF)*pPath->m_nAnchors;

					delete[] pPoints;
					pPoints = nullptr;
				}

				::SafeArrayUnaccessData(psa);

				VARIANT varPoints;
				varPoints.vt = VT_ARRAY|VT_UI1;
				varPoints.parray = psa;
				rs.Field(_T("Points")) = psa;

				::SafeArrayDestroy(psa);
			}
		}
		return true;
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return false;
}

DWORD CCluster::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CCollection<CPath>::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		POSITION pos1 = m_geomlist.GetHeadPosition();
		POSITION pos2 = pos1;
		while((pos2 = pos1)!=nullptr)
		{
			const CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==false)
				continue;

			if(pPath->GetArea()==0.f)
			{
				m_geomlist.RemoveAt(pos2);
				delete pPath;
				pPath = nullptr;
			}
		}

		const int nCount = m_geomlist.GetCount();
		pFile->Write(&nCount, sizeof(int));
		size += sizeof(int);

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos);

			BYTE bType = pPath->Gettype();

			pFile->Write(&bType, sizeof(BYTE));
			pFile->Write(&pPath->m_geoId, sizeof(DWORD));
			pFile->Write(&pPath->m_Rect, sizeof(CRect));
			size += sizeof(BYTE);
			size += sizeof(DWORD);
			size += sizeof(CRect);
		}

		POSITION Pos3 = m_geomlist.GetHeadPosition();
		while(Pos3!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(Pos3);
			size += pPath->PackPC(pFile, bytes);
		}

		return size;
	}
	else
	{
		const int nCount = *(int*)bytes;
		bytes += sizeof(int);
		for(int i = 0; i<nCount; i++)
		{
			const BYTE bType = *(BYTE*)bytes;
			bytes += sizeof(BYTE);
			const DWORD dwId = *(DWORD*)bytes;
			bytes += sizeof(DWORD);
			const CRect rect = *(CRect*)bytes;
			bytes += sizeof(CRect);

			CPath* pPath = (CPath*)CGeom::Apply(bType);
			if(pPath!=nullptr)
			{
				pPath->m_geoId = dwId;
				pPath->m_Rect = rect;
				m_geomlist.AddTail(pPath);
			}
		}

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos);
			pPath->PackPC(nullptr, bytes);
		}

		return 0;
	}
}

DWORD CCluster::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CCollection<CPath>::ReleaseCE(file, type);

	const int nCount = m_geomlist.GetCount();
	file.Write(&nCount, sizeof(int));

	size += sizeof(int);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPath* geom = m_geomlist.GetNext(pos);
		BYTE bType = geom->Gettype();
		file.Write(&bType, sizeof(BYTE));
		file.Write(&geom->m_geoId, sizeof(DWORD));
		CRect rect = geom->m_Rect;
		int left = geom->m_Rect.left/10000;
		int top = geom->m_Rect.top/10000;
		int width = geom->m_Rect.Width()/10000;
		int height = geom->m_Rect.Height()/10000;
		file.Write(&left, sizeof(int));
		file.Write(&top, sizeof(int));
		file.Write(&width, sizeof(int));
		file.Write(&height, sizeof(int));
		size += sizeof(BYTE);
		size += sizeof(DWORD);
		size += sizeof(int);
		size += sizeof(int);
		size += sizeof(int);
		size += sizeof(int);
	}

	POSITION Pos3 = m_geomlist.GetHeadPosition();
	while(Pos3!=nullptr)
	{
		CPath* geom = m_geomlist.GetNext(Pos3);
		BYTE del;
		size += geom->ReleaseCE(file, del);
	}

	return size;
}
void CCluster::DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const
{
	if(labeloid&&m_geogroid!=nullptr)
	{
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(*m_geogroid);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);
		pDC->FillSolidRect(cliRect, 0XFF0000);
	}
	if(geogroid&&m_labeloid!=nullptr)
	{
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(*m_labeloid);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);

		pDC->FillSolidRect(cliRect, 0X0000FF);
	}
	if(m_geogroid==nullptr&&m_labeloid==nullptr&&m_centroid!=CPoint(0, 0))
	{
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(m_centroid);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);
		pDC->FillSolidRect(cliRect, 0X000000);
	}
}

void CCluster::AddMember(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CPath)))
	{
		CCluster::AddMember((CPath*)pGeom);
	}
}
void CCluster::AddMember(CPath* pPath)
{
	m_geomlist.AddTail(pPath);

	if(m_Rect.IsRectNull())
		m_Rect = pPath->m_Rect;
	else
	{
		m_Rect.left = min(m_Rect.left, pPath->m_Rect.left);
		m_Rect.top = min(m_Rect.top, pPath->m_Rect.top);
		m_Rect.right = max(m_Rect.right, pPath->m_Rect.right);
		m_Rect.bottom = max(m_Rect.bottom, pPath->m_Rect.bottom);
	}
}
void CCluster::AddMembers(std::list<CGeom*>& geoms)
{
	for(auto it = geoms.begin(); it!=geoms.end(); ++it)
	{
		CCluster::AddMember(*it);
	}
}
void CCluster::AddMembers(std::list<CPoly*>& polys)
{
	for(auto it = polys.begin(); it!=polys.end(); ++it)
	{
		CCluster::AddMember(*it);
	}
}
void CCluster::AddMembers(std::list<CPath*>& pathes)
{
	for(auto it = pathes.begin(); it!=pathes.end(); ++it)
	{
		CCluster::AddMember(*it);
	}
}
bool CCluster::CreateName(CWnd* pWnd, const float& fontSize, const  float& zoomPointToDoc, CObList& geomlist)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPath* pPath = m_geomlist.GetNext(pos);
		const CString oldStr = pPath->m_strName;
		pPath->m_strName = m_strName;

		pPath->CreateName(pWnd, fontSize, zoomPointToDoc, geomlist);

		pPath->m_strName = oldStr;

		break;
	}

	return true;
}


void CCluster::ReleaseWeb(CFile& file, CSize offset) const
{
	CCollection<CPath>::ReleaseWeb(file, offset);;

	{
		short x = m_centroid.x-offset.cx;
		short y = m_centroid.y-offset.cy;
		ReverseOrder(x);
		file.Write(&x, sizeof(short));
		ReverseOrder(y);
		file.Write(&y, sizeof(short));
	}
	if(m_geogroid!=nullptr)
	{
		short x = m_geogroid->x-offset.cx;
		short y = m_geogroid->y-offset.cy;
		ReverseOrder(x);
		file.Write(&x, sizeof(short));
		ReverseOrder(y);
		file.Write(&y, sizeof(short));
	}
	if(m_labeloid!=nullptr)
	{
		short x = m_labeloid->x-offset.cx;
		short y = m_labeloid->y-offset.cy;
		ReverseOrder(x);
		file.Write(&x, sizeof(short));
		ReverseOrder(y);
		file.Write(&y, sizeof(short));
	}
}
void CCluster::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CCollection<CPath>::ReleaseWeb(stream, offset);;
	{
		const short x = m_centroid.x-offset.cx;
		const short y = m_centroid.y-offset.cy;
		stream<<x;
		stream<<y;
	}
	if(m_geogroid!=nullptr)
	{
		const short x = m_geogroid->x-offset.cx;
		const short y = m_geogroid->y-offset.cy;
		stream<<x;
		stream<<y;
	}
	if(m_labeloid!=nullptr)
	{
		const short x = m_labeloid->x-offset.cx;
		const short y = m_labeloid->y-offset.cy;
		stream<<x;
		stream<<y;
	}
}
void CCluster::ReleaseWeb(boost::json::object& json) const
{
	if(m_bClosed == true)
	{
		const CPoint geogroid = this->GetGeogroid();
		if(geogroid.x != 0 && geogroid.y != 0)
		{
			std::vector<long> child3;
			child3.push_back((long)(geogroid.x));
			child3.push_back((long)(geogroid.y));
			json["Geogroid"] = boost::json::value_from(child3);
		}
		const CPoint labeloid = this->GetLabeloid();
		if(labeloid.x != 0 && labeloid.y != 0)
		{
			std::vector<long> child4;
			child4.push_back((long)(labeloid.x));
			child4.push_back((long)(labeloid.y));
			json["Labeloid"] = boost::json::value_from(child4);
		}
	}

	CCollection<CPath>::ReleaseWeb(json);;
}
void CCluster::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CCollection::ReleaseWeb(writer, offset);

	if(m_bClosed == true)
	{
		const CPoint geogroid = this->GetGeogroid();
		writer.add_variant_sint32((long)(geogroid.x - offset.cx));
		writer.add_variant_sint32((long)(geogroid.y - offset.cy));

		const CPoint labeloid = this->GetLabeloid();
		writer.add_variant_sint32((long)(labeloid.x - offset.cx));
		writer.add_variant_sint32((long)(labeloid.y - offset.cy));
	}
}

void CCluster::Move(const int& dx, const int& dy)
{
	CCollection<CPath>::Move(dx, dy);

	{
		m_centroid.x += dx;
		m_centroid.y += dy;
	}
	if(m_geogroid!=nullptr)
	{
		m_geogroid->x += dx;
		m_geogroid->y += dy;
	}
	if(m_labeloid!=nullptr)
	{
		m_labeloid->x += dx;
		m_labeloid->y += dy;
	}
}
void CCluster::Move(const CSize& Δ)
{
	CCollection<CPath>::Move(Δ);

	{
		m_centroid.x += Δ.cx;
		m_centroid.y += Δ.cy;
	}
	if(m_geogroid!=nullptr)
	{
		m_geogroid->x += Δ.cx;
		m_geogroid->y += Δ.cy;
	}
	if(m_labeloid!=nullptr)
	{
		m_labeloid->x += Δ.cx;
		m_labeloid->y += Δ.cy;
	}
}
void CCluster::Transform(const CViewinfo& viewinfo)
{
	CCollection<CPath>::Transform(viewinfo);

	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_centroid);
		m_centroid.x = point.X;
		m_centroid.y = point.Y;
	}
	if(m_geogroid != nullptr)
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(*m_geogroid);
		m_geogroid->x = point.X;
		m_geogroid->y = point.Y;
	}
	if(m_labeloid != nullptr)
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(*m_labeloid);
		m_labeloid->x = point.X;
		m_labeloid->y = point.Y;
	}
}
void CCluster::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	CCollection<CPath>::Transform(m11, m21, m31, m12, m22, m32);

	{
		const long x = round(m_centroid.x*m11+m_centroid.y*m21+m31);
		const long y = round(m_centroid.x*m12+m_centroid.y*m22+m32);
		m_centroid.x = x;
		m_centroid.y = y;
	}
	if(m_geogroid!=nullptr)
	{
		const long x = round(m_geogroid->x*m11+m_geogroid->y*m21+m31);
		const long y = round(m_geogroid->x*m12+m_geogroid->y*m22+m32);
		m_geogroid->x = x;
		m_geogroid->y = y;
	}
	if(m_labeloid!=nullptr)
	{
		const long x = round(m_labeloid->x*m11+m_labeloid->y*m21+m31);
		const long y = round(m_labeloid->x*m12+m_labeloid->y*m22+m32);
		m_labeloid->x = x;
		m_labeloid->y = y;
	}
}
void CCluster::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	CCollection<CPath>::Rotate(mapinfo, deltaLng, deltaLat, facing);

	m_centroid = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, m_centroid);
	if(m_geogroid != nullptr)
	{
		const CPoint centroid = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, *m_geogroid);
		m_geogroid->x = centroid.x;
		m_geogroid->y = centroid.y;
	}
	if(m_labeloid != nullptr)
	{
		const CPoint centroid = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, *m_labeloid);
		m_labeloid->x = centroid.x;
		m_labeloid->y = centroid.y;
	}
}
CGeom* CCluster::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	auto ret = CCollection<CPath>::Project(fMapinfo, tMapinfo, interpolation, tolerance);
	if(ret != nullptr)
	{
		m_centroid = CDatainfo::Project(fMapinfo, tMapinfo, m_centroid);
		if(m_geogroid != nullptr)
		{
			const CPoint centroid = CDatainfo::Project(fMapinfo, tMapinfo, *m_geogroid);
			m_geogroid->x = centroid.x;
			m_geogroid->y = centroid.y;
		}
		if(m_labeloid != nullptr)
		{
			const CPoint centroid = CDatainfo::Project(fMapinfo, tMapinfo, *m_labeloid);
			m_labeloid->x = centroid.x;
			m_labeloid->y = centroid.y;
		}
	}
	return ret;
}
void CCluster::ResetGeogroid()
{
	if(m_geogroid!=nullptr)
	{
		delete m_geogroid;
		m_geogroid = nullptr;
	}
	RecalCentroid();
}
void CCluster::ResetLabeloid()
{
	if(m_labeloid!=nullptr)
	{
		delete m_labeloid;
		m_labeloid = nullptr;
	}
	RecalCentroid();
}
