#include "stdafx.h"

#include "Geom.h"
#include "Poly.h"
#include "Bezier.h"
#include "LLine.h"
#include "PRect.h"
#include "Text.h"
#include "Global.h"
#include "TextPoly.h"
#include "Tag.h"
#include "Group.h"
#include "TiledPoly.h"
#include "Compound.h"
#include "PropertyDlg.h"

#include "../Style/Line.h"
#include "../Style/LineEmpty.h"
#include "../Style/LineSymbol.h"
#include "../Style/LineRiver.h"
#include "../Style/FillPattern.h"
#include "../Style/Hint.h"

#include "../Mapper/Global.h"

#include "../Public/Global.h"
#include "../Public/BinaryStream.h"
#include "../Rectify/Tin.h"
#include "../Projection/Projection1.h"
#include "../Projection/Clipper.h"
#include "../Projection/Antimeridian.h"
#include "../Projection/Circler.h"

#include "../Action/Modify.h"
#include "../Action/ActionStack.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Poly.h"
#include "../Database/ODBCRecordset.h"
#include "../Utility/Rect.hpp"
#include "../Utility/Simplify.hpp"


#include <vector>
#include <iterator>
#include <map>
#include <tuple>
#include <queue>
#include <string>

#include "clipper2/clipper.h"
#include "../Utility/RectangleWalker.hpp"
#include "../Utility/Clipper.hpp"

using namespace std;
using namespace Undoredo;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

IMPLEMENT_SERIAL(CPoly, CPath, 0)

CPoly::CPoly()
	: CPath()
{
	m_bClosed = false;
	m_bType = 1;
}
CPoly::CPoly(CLine* pLine, CFillCompact* pFill)
	: CPath(pLine, pFill)
{
	m_bClosed = false;
	m_bType = 1;
}

CPoly::CPoly(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: CPath(rect, pLine, pFill)
{
	ASSERT_VALID(this);
	m_bClosed = false;
	m_bType = 1;
}

CPoly::CPoly(const std::vector<CPoint>& points, bool bCLosed)
	: CPoly()
{
	m_pPoints = new CPoint[points.size() + 1];
	for(int index = 0; index < points.size(); index++)
	{
		m_pPoints[index] = points[index];
	}
	if(bCLosed == true && points.front() != points.back())
	{
		m_pPoints[points.size()] = points.front();
		m_nAnchors = m_nAllocs = points.size() + 1;
	}
	else
		m_nAnchors = m_nAllocs = points.size();

	m_bClosed = bCLosed;
	CPoly::RecalRect();
}
CPoly::CPoly(CPoint* pPoints, unsigned int anchors, bool bCLosed)
	: CPoly()
{
	m_pPoints = pPoints;
	m_nAnchors = anchors;

	m_bClosed = bCLosed;
	CPoly::RecalRect();
}
CPoly::~CPoly()
{
	delete[] m_pPoints;
	if(m_geogroid != nullptr)
	{
		delete m_geogroid;
		m_geogroid = nullptr;
	}
	if(m_labeloid != nullptr)
	{
		delete m_labeloid;
		m_labeloid = nullptr;
	}
}
bool CPoly::IsOrphan() const
{
	if(m_nAnchors <= 1)
		return true;
	else if(m_bClosed && m_nAnchors <= 3)
		return true;
	else
		return false;
}
void CPoly::Purify() const
{
	m_southpolin.reset();

	CPath::Purify();
}
void CPoly::Diagnose() const
{
	OutputDebugStringA("\n\r");
	for(unsigned int index = 0; index < m_nAnchors; index++)
	{
		CStringA string;
		string.Format("[%d,%d],", m_pPoints[index].x, m_pPoints[index].y);
		OutputDebugStringA(string);
	}
}
void CPoly::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CPath::Sha1(sha1);

	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		CPoint point = m_pPoints[i];
		sha1.process_bytes(&point.x, sizeof(int));
		sha1.process_bytes(&point.y, sizeof(int));
	}
}
void CPoly::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1, offset);

	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		LONG x = m_pPoints[i].x - offset.cx;
		LONG y = m_pPoints[i].y - offset.cy;
		sha1.process_bytes(&x, sizeof(int));
		sha1.process_bytes(&y, sizeof(int));
	}
}
BOOL CPoly::operator==(const CGeom& geom) const
{
	if(CPath::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
		return FALSE;
	else if(m_nAnchors != ((CPoly&)geom).m_nAnchors)
		return FALSE;
	else if(memcmp(m_pPoints, ((CPoly&)geom).m_pPoints, m_nAnchors * sizeof(CPoint)) != 0)
		return FALSE;
	else
		return TRUE;
}
void CPoly::Attribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	CPropertyDlg dlg(pWnd, m_geoId, m_attId, m_geoCode, this->GetArea() > 0 ? true : false);
	if(dlg.DoModal() == IDOK)
	{
		m_geoCode = dlg.m_strGeoCode;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CPoly::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPath::Serialize(ar, dwVersion);

	DWORD reserved1 = 0B00000000000000000000000000000100;
	if(ar.IsStoring())
	{
		if(m_geogroid != nullptr)
			reserved1 |= 0B00000000000000000000000000000001;
		if(m_labeloid != nullptr)
			reserved1 |= 0B00000000000000000000000000000010;
		ar << reserved1;
		if(m_bClosed == true)
		{
			if(m_nAnchors < 3)
				m_bClosed = false;
			else if(m_pPoints[0] != m_pPoints[m_nAnchors - 1])
			{
				m_nAnchors++;
				m_pPoints[m_nAnchors - 1] = m_pPoints[0];
			}
		}
		ar << m_nAnchors;;
		ar.Write(m_pPoints, m_nAnchors * sizeof(CPoint));
		ar << m_centroid;
		if(m_bClosed && m_geogroid != nullptr)
			ar << *m_geogroid;
		if(m_bClosed && m_labeloid != nullptr)
			ar << *m_labeloid;
	}
	else
	{
		ar >> reserved1;
		ar >> m_nAnchors;

		m_nAllocs = m_nAnchors + 1;
		m_pPoints = new CPoint[m_nAllocs];
		ar.Read(m_pPoints, m_nAnchors * sizeof(CPoint));
		ar >> m_centroid;
		if((reserved1 & 0B00000000000000000000000000000001) == 0B00000000000000000000000000000001)
		{
			m_geogroid = new CPoint();
			ar >> *m_geogroid;
		}
		if((reserved1 & 0B00000000000000000000000000000010) == 0B00000000000000000000000000000010)
		{
			m_labeloid = new CPoint();
			ar >> *m_labeloid;
		}
	}
}

void CPoly::ReleaseCE(CArchive& ar) const
{
	CPath::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar << m_nAnchors;;
		for(unsigned int i = 0; i < m_nAnchors; i++)
		{
			CPoint point = m_pPoints[i];
			point.x /= 10000;
			point.y /= 10000;
			ar << point.x;
			ar << point.y;
		}
	}
}

void CPoly::ReleaseDCOM(CArchive& ar)
{
	CPath::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_nAnchors;
		ar << m_pPoints[0].x;
		ar << m_pPoints[0].y;
		for(unsigned int index = 1; index < m_nAnchors; index++)
		{
			const short deltaX = m_pPoints[index].x - m_pPoints[index - 1].x;
			const short deltaY = m_pPoints[index].y - m_pPoints[index - 1].y;
			ar << deltaX;
			ar << deltaY;
		}
	}
	else
	{
		ar >> m_nAnchors;
		m_pPoints = new CPoint[m_nAnchors];
		ar >> m_pPoints[0].x;
		ar >> m_pPoints[0].y;
		for(unsigned int index = 1; index < m_nAnchors; index++)
		{
			short deltaX;
			short deltaY;
			ar >> deltaX;
			ar >> deltaY;
			m_pPoints[index].x = m_pPoints[index - 1].x + deltaX;
			m_pPoints[index].y = m_pPoints[index - 1].y + deltaY;
		}
	}
}

bool CPoly::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	CPath::GetValues(rs, datainfo, strTag);

	try
	{
		m_bClosed = rs.Field(_T("Closed")).AsBool();
		m_nAnchors = rs.Field(_T("Handles")).AsInt();
		if(m_nAnchors > 0)
		{
			const CLongBinary* pLongBinary = rs.Field(_T("Points")).AsBinary();
			if(pLongBinary->m_dwDataLength > 0)
			{
				const BYTE* pData = (BYTE*)::GlobalLock(pLongBinary->m_hData);
				CPointF* pPoints = new CPointF[m_nAnchors];
				memcpy((char*)pPoints, pData, pLongBinary->m_dwDataLength); ///¸´ÖÆÊý¾Ýµ½»º³åÇø   
				::GlobalUnlock(pLongBinary->m_hData);

				m_pPoints = new CPoint[m_nAnchors];
				for(int index = 0; index < m_nAnchors; index++)
				{
					m_pPoints[index].x = datainfo.MapToDoc(pPoints[index].x);
					m_pPoints[index].y = datainfo.MapToDoc(pPoints[index].y);
				}

				delete[] pPoints;
				pPoints = nullptr;

				m_nAllocs = m_nAnchors;
			}
			else
			{
				CString string;
				string.Format(_T("Error when try to read geometry: %d!"), m_geoId);
				AfxMessageBox(string);
				m_nAnchors = 0;
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

bool CPoly::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
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

		rs.Field(_T("Shape")) = 1;
		rs.Field(_T("Closed")) = m_bClosed;
		rs.Field(_T("Anchors")) = (int)m_nAnchors;

		if(m_pPoints != nullptr && m_nAnchors > 1)
		{
			CPointF* pPoints = new CPointF[m_nAnchors];
			for(int index = 0; index < m_nAnchors; index++)
			{
				pPoints[index].x = datainfo.DocToMap(m_pPoints[index].x);
				pPoints[index].y = datainfo.DocToMap(m_pPoints[index].y);
			}

			SAFEARRAYBOUND rgsabound[1];
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = sizeof(CPointF) * m_nAnchors;

			SAFEARRAY* psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
			if(psa != nullptr)
			{
				LPVOID lpArrayData = nullptr;
				::SafeArrayAccessData(psa, &lpArrayData);
				::memcpy((char*)lpArrayData, pPoints, sizeof(CPointF) * m_nAnchors);
				::SafeArrayUnaccessData(psa);

				VARIANT varPoints;
				varPoints.vt = VT_ARRAY | VT_UI1;
				varPoints.parray = psa;
				rs.Field(_T("Points")) = psa;

				::SafeArrayDestroy(psa);
			}

			delete[] pPoints;
			pPoints = nullptr;
		}
		else
		{
			CString string;
			string.Format(_T("Error when try to write geometry: %d!"), m_geoId);
			AfxMessageBox(string);
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

void CPoly::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
	CClientDC dc(pWnd);

	CPen pen;
	pen.CreatePen(PS_SOLID, (nTimes % 5) * 2, RGB(255, 0, 0));
	CPen* oldpen = dc.SelectObject(&pen);

	const Gdiplus::Point* points1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints, m_nAnchors);
	CPoint* points2 = new CPoint[m_nAnchors];
	::memcpy(points2, points1, sizeof(Gdiplus::Point) * m_nAnchors);
	::delete[] points1;

	int radius = (nTimes % 5) * 2;
	if(nTimes % 5 == 0)
	{
		CPen pen1;
		pen1.CreatePen(PS_SOLID, 8, RGB(255, 255, 255));
		CPen* oldpen1 = dc.SelectObject(&pen1);

		dc.Polyline(points2, m_nAnchors);

		dc.SelectObject(oldpen1);
		pen1.DeleteObject();
	}

	dc.Polyline(points2, m_nAnchors);

	dc.SelectObject(oldpen);
	pen.DeleteObject();

	delete[] points2;
}
void CPoly::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	if(m_nAnchors < 2)
		return;

	const Gdiplus::PointF* points = CPath::Transform(matrix, m_pPoints, m_nAnchors);
	if(m_bClosed == true)
	{
		path.AddPolygon(points, m_nAnchors);
	}
	else
	{
		path.AddLines(points, m_nAnchors);
	}
	::delete[] points;
}

void CPoly::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	if(m_nAnchors < 2)
		return;

	if(this->m_bMeridianCrossing == true){
		std::list<std::pair<unsigned int, Gdiplus::PointF*>> paths;
		viewinfo.DocToClient(m_pPoints, m_nAnchors, m_bClosed, paths, tolerance, this->m_southpolin);
		for(typename std::list<std::pair<unsigned int, Gdiplus::PointF*>>::iterator it = paths.begin(); it != paths.end(); ++it)
		{
			const std::pair<unsigned int, Gdiplus::PointF*> pair = *it;
			if(m_bClosed == true)
				path.AddPolygon(pair.second, pair.first);
			else
				path.AddLines(pair.second, pair.first);

			delete[] pair.second;
		}
		this->m_bMeridianCrossing = paths.size() > 1;
	}
	else
	{
		Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints, m_nAnchors);
		if(m_bClosed == true)
			path.AddPolygon(points, m_nAnchors);
		else
			path.AddLines(points, m_nAnchors);

		::delete[] points;
		points = nullptr;
	}
}
void CPoly::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix, const bool& reverse) const
{
	if(reverse)
	{
		CPoint* pPoints = Reverted();
		const Gdiplus::PointF* points = CPath::Transform(matrix, pPoints, m_nAnchors);
		delete[] pPoints;
		if(m_bClosed == true)
		{
			path.AddPolygon(points, m_nAnchors);
		}
		else
		{
			path.AddLines(points, m_nAnchors);
		}
		::delete[] points;
	}
	else
		CPoly::AddPath(path, matrix);
}

void CPoly::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance, const bool& reverse) const
{
	if(reverse)
	{
		CPoint* pPoints = Reverted();
		std::list<std::pair<unsigned int, Gdiplus::PointF*>> paths;
		viewinfo.DocToClient(pPoints, m_nAnchors, m_bClosed, paths, tolerance, this->m_southpolin);
		delete[] pPoints;

		for(typename std::list<std::pair<unsigned int, Gdiplus::PointF*>>::iterator it = paths.begin(); it != paths.end(); ++it)
		{
			const std::pair<unsigned int, Gdiplus::PointF*> pair = *it;
			if(m_bClosed == true)
				path.AddPolygon(pair.second, pair.first);
			else
				path.AddLines(pair.second, pair.first);

			delete[] pair.second;
		}
	}
	else
		CPoly::AddPath(path, viewinfo, tolerance);
}
void CPoly::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_bClosed == true)
		CGeom::DrawTag(g, viewinfo, pHint);
	else if(m_pTag != nullptr)
		CGeom::DrawTag(g, viewinfo, pHint);
}

void CPoly::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot, const bool& bOblige) const
{
	if(m_strName.IsEmpty() == TRUE)
		return;
	else if(bPivot == true || m_bClosed == true)
		return CPath::DrawTag(g, viewinfo, pHint, bPivot, bOblige);
	const float cliSize = pHint->m_fSize * viewinfo.m_sizeDPI.cx / 72.f;
	const CSize docSize = viewinfo.ClientToDoc(Gdiplus::SizeF(cliSize, cliSize));

	bool bHasEnglish = false;
	Gdiplus::REAL labelLength = 0;
	CObArray labelarray;
	CText::AnalyzeString(m_strName, labelarray);
	const int nWords = labelarray.GetCount();
	for(int index = 0; index < nWords; index++)
	{
		const CText::CWord* lable = (CText::CWord*)labelarray.GetAt(index);
		if(lable->bCJK == true)
		{
			labelLength += docSize.cx * lable->str.GetLength();
		}
		else
		{
			labelLength += (docSize.cx / 2) * lable->str.GetLength();
			bHasEnglish = true;
		}
	}
	const long length = this->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc * 2);
	if(length > labelLength + nWords * docSize.cx)
	{
		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		stringFormat.SetTrimming(Gdiplus::StringTrimmingNone);
		const CPoint& point1 = this->GetAnchor(1);
		const CPoint& point2 = this->GetAnchor(m_nAnchors);
		const long nAngle = (long)(atan2((double)(point2.y - point1.y), (double)(point2.x - point1.x)) * 180.0f / M_PI);
		const bool bForwardCJK = (nAngle >= -135 && nAngle <= 45) ? true : false;
		const bool bForwardENG = (nAngle >= -90 && nAngle <= 90) ? true : false;
		const bool bForward = bHasEnglish == true ? bForwardENG : bForwardCJK;
		const int times = max(length / (labelLength + nWords * docSize.cx * 10), 1);
		const float step = (float)length / times;
		const float gap = (float)(step - labelLength) / (2 + nWords - 1 + 2);
		const float trim = 2 * gap;
		unsigned int stepH = 1;
		double stepT = 0;
		for(int time = 0; time < times; time++)
		{
			unsigned int wordH = stepH;
			double wordT = stepT;
			double distance = trim;
			for(int word = 0; word < nWords; word++)
			{
				const CText::CWord* label = (CText::CWord*)labelarray.GetAt(bForward == true ? word : nWords - 1 - word);
				distance += docSize.cx * (label->bCJK == true ? 1 : 0.5) * label->str.GetLength() / 2;
				this->GetAandT(wordH, wordT, distance, wordH, wordT);
				CPoint docPoint = this->GetPoint(wordH, wordT);
				Gdiplus::PointF cliPoint = viewinfo.DocToClient<Gdiplus::PointF>(docPoint);

				if(label->bCJK == true)
				{
					Gdiplus::RectF tagRect = pHint->DrawString(g, viewinfo, label->str, cliPoint, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
					d_oRectArray.Add(tagRect);
				}
				else
				{
					g.TranslateTransform(cliPoint.X, cliPoint.Y);
					const double angle = this->GetAngle(wordH, wordT);
					if(angle != 0.0f)
					{
						const int nAngle = angle * 180 / M_PI;
						g.RotateTransform(-nAngle);
						Gdiplus::PointF point(0, 0);
						pHint->DrawString(g, viewinfo, label->str, point, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
						g.RotateTransform(+nAngle);
					}
					else
					{
						Gdiplus::PointF point(0, 0);
						pHint->DrawString(g, viewinfo, label->str, point, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
					}

					g.TranslateTransform(-cliPoint.X, -cliPoint.Y);
				}
				distance = docSize.cx * (label->bCJK == true ? 1 : 0.5) * label->str.GetLength() / 2 + gap;
			}
			this->GetAandT(stepH, stepT, step, stepH, stepT);
		}
	}
	for(int index = 0; index < labelarray.GetCount(); index++)
	{
		const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
		delete label;
	}
	labelarray.RemoveAll();
}
void CPoly::DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const
{
	if(tAnchor < fAnchor)
		return;
	if(tAnchor == fAnchor && et <= st)
		return;
	if(pen == nullptr)
		return;

	Gdiplus::PointF* points = new Gdiplus::PointF[tAnchor - fAnchor + 1 + 1];

	//	Gdiplus::PointF fPoint = m_pPoints[fAnchor-1  ];
	//	Gdiplus::PointF tPoint = m_pPoints[tAnchor+1-1];
	if(st != 0.0f)
	{
		const int x = m_pPoints[fAnchor - 1].x + round((m_pPoints[fAnchor + 1 - 1].x - m_pPoints[fAnchor - 1].x) * st);
		const int y = m_pPoints[fAnchor - 1].y + round((m_pPoints[fAnchor + 1 - 1].y - m_pPoints[fAnchor - 1].y) * st);
		points[0] = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(x, y));
	}

	if(et != 1.0f)
	{
		const int x = m_pPoints[tAnchor - 1].x + round((m_pPoints[tAnchor + 1 - 1].x - m_pPoints[tAnchor - 1].x) * et);
		const int y = m_pPoints[tAnchor - 1].y + round((m_pPoints[tAnchor + 1 - 1].y - m_pPoints[tAnchor - 1].y) * et);
		points[tAnchor - fAnchor + 1 + 1 - 1] = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(x, y));
	}

	for(int index = fAnchor; index < tAnchor + 1; index++)
	{
		points[index - fAnchor + 1] = viewinfo.DocToClient<Gdiplus::PointF>(this->GetAnchor(index));
	}

	g.DrawLines(pen, points, tAnchor - fAnchor + 1 + 1);

	::delete[] points;
}

void CPoly::DrawPentip(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor)const
{
	const CPoint& docPoint = this->GetAnchor(nAnchor);
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
	CRect rect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
	rect.InflateRect(3, 3);
	pDC->Rectangle(rect);
}

void CPoly::DrawPath(CDC* pDC, const CViewinfo& viewinfo) const
{
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	const Gdiplus::PointF* points1 = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints, m_nAnchors);
	CPoint* points2 = new CPoint[m_nAnchors];
	memcpy(points2, points1, sizeof(Gdiplus::PointF) * m_nAnchors);
	::delete[] points1;

	pDC->Polyline(points2, m_nAnchors);

	delete[] points2;

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

bool CPoly::CreateName(CWnd* pWnd, const float& fontSize, const float& zoomPointToDoc, CObList& oblist)
{
	if(m_strName.GetLength() < 1)
		return false;

	if(m_pPoints == nullptr)
		return false;

	if(m_bClosed == false)
	{
		CObArray labelarray;
		CText::AnalyzeString(m_strName, labelarray);
		const int nWords = labelarray.GetCount();
		if(nWords == 1)
		{
			const CPoint labeloid = this->GetLabeloid();
			CText* text = new CText(CPoint(labeloid.x, labeloid.y));
			text->m_strName = m_strName;
			text->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);
			oblist.AddTail(text);
			return true;
		}
		else
		{
			const unsigned long length = this->GetLength(zoomPointToDoc);
			if(length < nWords * fontSize * zoomPointToDoc)
				return false;

			long step = 30 * fontSize * zoomPointToDoc;
			unsigned short count = length / (nWords * step);

			if(count == 0)
			{
				count = 1;
				step = length / (nWords + 1);
			}
			else
			{
				const long segm = length / count;
				step = segm / (nWords + 1);
			}

			unsigned int sa = 1;
			double st = 0;
			unsigned int H = 1;
			double T = 0;

			for(unsigned int i = 0; i < count; i++)
			{
				CTextPoly* textpoly = new CTextPoly;
				textpoly->m_dwStyle = 0;
				textpoly->m_dwStyle |= ALIGN_CENTER | ORIENT_VERTICAL | SPACE_ANCHOR;
				textpoly->m_strName = m_strName;

				textpoly->m_nAnchors = labelarray.GetCount();
				textpoly->m_nAllocs = textpoly->m_nAnchors + 1;
				textpoly->m_pPoints = new CPoint[textpoly->m_nAllocs];

				for(unsigned int j = 0; j < labelarray.GetCount(); j++)
				{
					GetAandT(sa, st, step, H, T);
					sa = H;
					st = T;
					textpoly->m_pPoints[j] = GetPoint(H, T);
				}
				GetAandT(sa, st, step, H, T);
				sa = H;
				st = T;

				textpoly->RecalRect();
				textpoly->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);
				oblist.AddTail(textpoly);
			}

			for(int index = 0; index < labelarray.GetCount(); index++)
			{
				const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
				delete label;
			}
			labelarray.RemoveAll();

			return true;
		}
	}
	else
		return CPath::CreateName(pWnd, fontSize, zoomPointToDoc, oblist);
}

CPoly* CPoly::GetByFHTToTHT(unsigned int fAnchor, double st, unsigned int tAnchor, double et) const
{
	if(m_pPoints == nullptr)
		return nullptr;

	if(st == 1.0f)
	{
		fAnchor += 1;
		st = 0.0f;
	}
	if(et == 0.0f)
	{
		tAnchor -= 1;
		et = 1.0f;
	}

	CPoly* poly = new CPoly;
	poly->m_pPoints = new CPoint[tAnchor + 1 - fAnchor + 1 + 1];
	poly->m_nAnchors = tAnchor + 1 - fAnchor + 1;
	poly->m_nAllocs = tAnchor + 1 - fAnchor + 1 + 1;

	poly->m_bLocked = m_bLocked;
	poly->m_bGroup = m_bGroup;
	poly->m_strName = m_strName;

	for(unsigned int i = fAnchor; i <= tAnchor + 1; i++)
	{
		poly->m_pPoints[i - fAnchor] = m_pPoints[i - 1];
	}

	if(st != 0.0f)
	{
		const LLine* line = GetSegment(fAnchor);
		LLine L, R;

		line->Split(L, R, st);
		poly->m_pPoints[0] = R.fpoint;
		delete line;
	}

	if(et != 1.0f)
	{
		const LLine* line = GetSegment(tAnchor);
		LLine L, R;

		line->Split(L, R, et);
		poly->m_pPoints[tAnchor - fAnchor + 1] = L.tpoint;
		delete line;
	}

	poly->RecalRect();
	return poly;
}

void CPoly::DrawBlackAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point)
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(point);
	CRect rect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
	rect.InflateRect(2, 2);
	dc.InvertRect(rect);

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CPoly::DrawSquareAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point)
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(point);
	CRect rect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
	rect.InflateRect(3, 3);
	dc.Rectangle(rect);

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CPoly::DrawSquareAnchor(CDC* pDC, const CViewinfo& viewinfo, const CPoint& point)
{
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(point);
	CRect rect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
	rect.InflateRect(3, 3);
	pDC->Rectangle(rect);

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

void CPoly::DrawLineTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint docPoint1, CPoint docPoint2)
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(docPoint1);
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(docPoint2);

	dc.MoveTo(point1.X, point1.Y);
	dc.LineTo(point2.X, point2.Y);

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

CRect CPoly::GetSegRect(const unsigned int& nAnchor) const
{
	CRect rect;
	rect.SetRectEmpty();

	if(nAnchor < 1 || nAnchor >= m_nAnchors)
		return nullptr;

	if(m_pPoints != nullptr)
	{
		const CPoint& fpoint = GetAnchor(nAnchor);
		const CPoint& tpoint = nAnchor == m_nAnchors ? GetAnchor(1) : GetAnchor(nAnchor + 1);

		rect = CRect(fpoint.x, fpoint.y, tpoint.x, tpoint.y);
		rect.NormalizeRect();
	}

	return rect;
}

void CPoly::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const
{
	if(pWnd == nullptr)
		return;

	if(m_bClosed == true)
	{
		return CPath::Invalidate(pWnd, viewinfo, inflate);
	}
	else if(m_pPoints != nullptr)
	{
		for(unsigned int i = 1; i < m_nAnchors; i++)
		{
			Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(GetAnchor(i));
			Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(GetAnchor(i + 1));

			CRect rect;
			rect.left = min(point1.X, point2.X);
			rect.top = min(point1.Y, point2.Y);
			rect.right = max(point1.X, point2.X);
			rect.bottom = max(point1.Y, point2.Y);

			rect.InflateRect(inflate, inflate);
			pWnd->InvalidateRect(rect, FALSE);
		}
	}
}

void CPoly::InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const
{
	if(pWnd == nullptr)
		return;
	else if(m_pPoints == nullptr)
		return;
	else if(m_nAnchors < 2)
		return;

	CRect docRect1;
	CRect docRect2;
	CRect rect;
	if(nAnchor == 1)
	{
		if(m_bClosed == true)
			docRect1 = GetSegRect(m_nAnchors - 1);
		else
			docRect1.SetRectEmpty();
	}
	else
	{
		docRect1 = GetSegRect(nAnchor - 1);
	}

	if(nAnchor == m_nAnchors)
	{
		if(m_bClosed == true)
			docRect2 = GetSegRect(1);
		else
			docRect2.SetRectEmpty();
	}
	else
	{
		docRect2 = GetSegRect(nAnchor);
	}

	Gdiplus::Rect cliRect1 = viewinfo.DocToClient <Gdiplus::Rect>(docRect1);
	Gdiplus::Rect cliRect2 = viewinfo.DocToClient <Gdiplus::Rect>(docRect2);
	if(m_bClosed == true)
	{
		Gdiplus::Rect cliRect;
		if(cliRect1.IsEmptyArea() == TRUE)
			cliRect = cliRect2;
		else if(cliRect2.IsEmptyArea() == TRUE)
			cliRect = cliRect1;
		else
			cliRect = UnionRect(cliRect1, cliRect2);

		cliRect.Inflate(inflate, inflate);
		pWnd->InvalidateRect(CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom()), FALSE);
	}
	else
	{
		if(cliRect1.IsEmptyArea() == FALSE)
		{
			cliRect1.Inflate(inflate, inflate);
			pWnd->InvalidateRect(CRect(cliRect1.GetLeft(), cliRect1.GetTop(), cliRect1.GetRight(), cliRect1.GetBottom()), FALSE);
		}
		if(cliRect2.IsEmptyArea() == FALSE)
		{
			cliRect2.Inflate(inflate, inflate);
			pWnd->InvalidateRect(CRect(cliRect2.GetLeft(), cliRect2.GetTop(), cliRect2.GetRight(), cliRect2.GetBottom()), FALSE);
		}
	}
}

void CPoly::InvalidateSegment(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const
{
	if(pWnd == nullptr)
		return;
	else if(m_pPoints == nullptr)
		return;
	else if(m_nAnchors < 2)
		return;
	else if(nAnchor < 1)
		return;
	else if(nAnchor > m_nAnchors)
		return;
	else if(nAnchor == m_nAnchors && m_bClosed == false)
		return;

	CRect docRect;
	if(nAnchor == m_nAnchors && m_bClosed == false)
		docRect = GetSegRect(1);
	else
		docRect = GetSegRect(nAnchor);

	Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
	cliRect.Inflate(inflate, inflate);
	pWnd->InvalidateRect(CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom()), FALSE);
}

void CPoly::InvalidateTag(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_bClosed == true)
		CGeom::InvalidateTag(pWnd, viewinfo, pHint);
	else
		CGeom::InvalidateTag(pWnd, viewinfo, pHint);//temporary
}

unsigned int CPoly::GetAnchors() const
{
	return m_nAnchors;
}
CPoint& CPoly::GetAnchor(const unsigned int& nAnchor) const
{
	ASSERT(nAnchor >= 1 && nAnchor <= m_nAnchors);
	ASSERT(m_pPoints != nullptr);

	return m_pPoints[nAnchor - 1];
}

CPoint CPoly::GetPoint(const unsigned int& nAnchor, const double& t) const
{
	ASSERT(m_pPoints != nullptr);

	if(t == 0.f)
		return m_pPoints[nAnchor - 1];
	if(t == 1.f)
		return m_pPoints[nAnchor];
	const CPoint& fpoint = m_pPoints[nAnchor - 1];
	const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];

	CPoint point;
	point.x = fpoint.x + round(((double)tpoint.x - fpoint.x) * t);
	point.y = fpoint.y + round(((double)tpoint.y - fpoint.y) * t);

	return point;
}

double CPoly::GetAngle(const unsigned int& nAnchor, const double& t) const
{
	if(nAnchor >= m_nAnchors)
		return 0.0f;
	const CPoint& fpoint = m_pPoints[nAnchor - 1];
	const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];

	if(t == 0.f)
	{
		const double tdAngle = LineAngle(fpoint, tpoint);
		double fdAngle = 0;
		if(nAnchor == 1)
		{
			if(m_bClosed == true)
			{
				fdAngle = LineAngle(m_pPoints[m_nAnchors - 1 - 1], fpoint);;
				return (fdAngle + tdAngle) / 2;
			}
			else
				return tdAngle;
		}
		else
		{
			fdAngle = LineAngle(m_pPoints[nAnchor - 1 - 1], fpoint);
			return (fdAngle + tdAngle) / 2;
		}
	}
	else if(t == 1.0f)
	{
		const double fdAngle = LineAngle(fpoint, tpoint);
		double tdAngle = 0.0f;

		if(nAnchor == m_nAnchors - 1)
		{
			if(m_bClosed == true)
			{
				tdAngle = LineAngle(tpoint, m_pPoints[1 + 1 - 1]);
				return (fdAngle + tdAngle) / 2;
			}
			else
				return fdAngle;
		}
		else
		{
			tdAngle = LineAngle(tpoint, m_pPoints[nAnchor + 1 + 1 - 1]);
			return (fdAngle + tdAngle) / 2;
		}
	}
	else
	{
		const double dAngle = LineAngle(fpoint, tpoint);
		return dAngle;
	}
}

bool CPoly::IsValid() const
{
	if(m_nAnchors < 2)
		return false;
	else if(m_pPoints == nullptr)
		return false;
	else
		return true;
}

unsigned long long CPoly::Distance2(const CPoint& point) const
{
	unsigned long long dismin = MAXUINT64;
	for(int index = 1; index <= (m_bClosed ? m_nAnchors : m_nAnchors - 1); index++) {
		const CPoint& point1 = CPoly::GetAnchor(index);
		const CPoint& point2 = CPoly::GetAnchor(index == m_nAnchors ? 1 : index + 1);
		const auto vx = point.x - point1.x;
		const auto vy = point.y - point1.y;
		const auto dx = point2.x - point1.x;
		const auto dy = point2.y - point1.y;
		dismin = min(dismin, ::Distance2(point, point1, point2));
	}

	return dismin;
}

CPoint CPoly::GetSample(const short& direction, const CPoint& center, const long& radius) const
{
	CPoint point = direction == 1 ? GetAnchor(1) : GetAnchor(m_nAnchors);

	double distance = sqrt(powf(point.x - center.x, 2) + powf(point.y - center.y, 2));
	if(distance > radius)
		return point;

	unsigned int tAnchor = direction == 1 ? 1 : m_nAnchors;
	do
	{
		tAnchor = tAnchor + direction;
		if(tAnchor < 1 || tAnchor > m_nAnchors)
			return point;

		point = GetAnchor(tAnchor);
		distance = sqrt(pow((double)(point.x - center.x), 2) + pow((double)(point.y - center.y), 2));
	} while(distance < radius);
	const unsigned int fAnchor = tAnchor - direction;

	CPoint fpoint;
	CPoint tpoint;
	double st = 0.0f;
	double et = 1.0f;
	if(direction == 1)
	{
		fpoint = m_pPoints[fAnchor - 1];
		tpoint = m_pPoints[tAnchor - 1];

		st = 0;
		et = 1;
	}
	if(direction == -1)
	{
		fpoint = m_pPoints[tAnchor - 1];
		tpoint = m_pPoints[fAnchor - 1];

		st = 0;
		et = 1;
	}

	fpoint = fpoint - center;
	tpoint = tpoint - center;

	do
	{
		const double t = (st + et) / 2;
		point.x = round(fpoint.x + (tpoint.x - fpoint.x) * t);
		point.y = round(fpoint.y + (tpoint.y - fpoint.y) * t);
		const double distance = sqrt((double)point.x * point.x + (double)point.y * point.y);
		point = point + center;

		if(distance >= radius && distance - radius < (radius / 10000 + 1))
			break;

		if(distance > radius)
		{
			if(direction == 1)
				et = t;
			if(direction == -1)
				st = t;
		}
		else if(distance < radius)
		{
			if(direction == 1)
				st = t;
			if(direction == -1)
				et = t;
		}
	} while(st != et);

	return point;
}

LLine* CPoly::GetSegment(const unsigned int& nAnchor) const
{
	if(nAnchor < 1 || nAnchor >= m_nAnchors)
		return nullptr;

	if(m_pPoints == nullptr)
		return nullptr;

	const CPoint& fpoint = m_pPoints[nAnchor - 1];
	const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];
	return new LLine(fpoint, tpoint);
}

unsigned long CPoly::GetLength(const double& tolerance) const
{
	float length = 0;
	if(m_pPoints != nullptr)
	{
		for(unsigned int i = 1; i < m_nAnchors; i++)
		{
			const CPoint& fpoint = m_pPoints[i - 1];
			const CPoint& tpoint = m_pPoints[i + 1 - 1];

			length += ::LineLength1(fpoint, tpoint);
		}
	}

	return round(length);
}

double CPoly::GetLength(const CDatainfo& datainfo, const double& tolerance) const
{
	if(m_pPoints != nullptr)
		return datainfo.CalLength(m_pPoints, m_nAnchors);
	else
		return 0;
}

double CPoly::GetArea() const
{
	double area = 0.0f;
	if(m_bClosed == true && m_pPoints != nullptr)
	{
		long minY = 0X0FFFFFFF;
		for(unsigned int nAnchor = 1; nAnchor <= m_nAnchors; nAnchor++)
		{
			minY = min(minY, m_pPoints[nAnchor - 1].y);
		}

		for(unsigned int nAnchor = 1; nAnchor < m_nAnchors; nAnchor++)
		{
			const double a = (m_pPoints[nAnchor].x - m_pPoints[nAnchor - 1].x);
			const double b = (m_pPoints[nAnchor].y + m_pPoints[nAnchor - 1].y - 2 * minY);

			area += a * b / 2;
		}
	}

	return area;
}

double CPoly::GetArea(const CDatainfo& datainfo) const
{
	if(m_pPoints != nullptr && m_bClosed == TRUE)
		return datainfo.CalArea(m_pPoints, m_nAnchors);
	else
		return 0;
}

bool CPoly::GetAandT(const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const
{
	if(fAnchor >= m_nAnchors)
	{
		tAnchor = m_nAnchors;
		et = 0;
		return false;
	}
	else if(length == 0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	for(unsigned int i = fAnchor; i < m_nAnchors; i++)
	{
		const CPoint fpoint = m_pPoints[i - 1];
		const CPoint tpoint = m_pPoints[i + 1 - 1];
		const double len = pow((double)(tpoint.x - fpoint.x), 2) + pow((double)(tpoint.y - fpoint.y), 2);
		const double segment = sqrt(len);
		const double leftlen = segment - st * segment;
		if(length < leftlen)
		{
			tAnchor = i;
			et = st + (double)length / segment;

			return true;
		}
		else if(length == leftlen)
		{
			tAnchor = i;
			et = 1;

			return true;
		}
		length -= leftlen;
		st = 0;
	}

	tAnchor = m_nAnchors - 1;
	et = 1;

	return false;
}
void CPoly::Move(const int& dx, const int& dy)
{
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		m_pPoints[i].x += dx;
		m_pPoints[i].y += dy;
	}
	m_centroid.x += dx;
	m_centroid.y += dy;
	if(m_geogroid != nullptr)
	{
		m_geogroid->x += dx;
		m_geogroid->y += dy;
	}
	if(m_labeloid != nullptr)
	{
		m_labeloid->x += dx;
		m_labeloid->y += dy;
	}
	CPath::Move(dx, dy);
}
void CPoly::Move(const CSize& Δ)
{
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		m_pPoints[i].x += Δ.cx;
		m_pPoints[i].y += Δ.cy;
	}
	m_centroid.x += Δ.cx;
	m_centroid.y += Δ.cy;
	if(m_geogroid != nullptr)
	{
		m_geogroid->x += Δ.cx;
		m_geogroid->y += Δ.cy;
	}
	if(m_labeloid != nullptr)
	{
		m_labeloid->x += Δ.cx;
		m_labeloid->y += Δ.cy;
	}
	CPath::Move(Δ);
}

void CPoly::ChangeAnchor(const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch, Undoredo::CActionStack* pActionstack)
{
	std::map<unsigned int, CPoint> oldpoints;
	std::map<unsigned int, CPoint> newpoints;

	CSize delta(Δ);
	if(bMatch == true && m_nAnchors > 1)
	{
		CPoint fPoint;
		CPoint local = GetAnchor(nAnchor) + CPoint(Δ.cx, Δ.cy);

		if(nAnchor == 1)
		{
			if(m_bClosed == true)
				fPoint = GetAnchor(m_nAnchors - 1);
			else
				fPoint = GetAnchor(nAnchor + 1);
		}
		else
		{
			fPoint = GetAnchor(nAnchor - 1);
		}
		local = RegulizePoint(fPoint, local, 8);

		delta.cx = local.x - GetAnchor(nAnchor).x;
		delta.cy = local.y - GetAnchor(nAnchor).y;
	}

	oldpoints[nAnchor - 1] = m_pPoints[nAnchor - 1];
	m_pPoints[nAnchor - 1].x += delta.cx;
	m_pPoints[nAnchor - 1].y += delta.cy;
	newpoints[nAnchor - 1] = m_pPoints[nAnchor - 1];
	if(m_bClosed == true && nAnchor == 1)
	{
		oldpoints[m_nAnchors - 1] = m_pPoints[m_nAnchors - 1];
		m_pPoints[m_nAnchors - 1].x += delta.cx;
		m_pPoints[m_nAnchors - 1].y += delta.cy;
		newpoints[m_nAnchors - 1] = m_pPoints[m_nAnchors - 1];
	}

	RecalRect();
	m_bModified = true;

	if(pActionstack != nullptr)
	{
		Undoredo::CModify<CWnd*, CPoly*, std::map<unsigned int, CPoint>>* pModify = new Undoredo::CModify<CWnd*, CPoly*, std::map<unsigned int, CPoint>>(nullptr, this, oldpoints, newpoints);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Anchors;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Anchors;
		pActionstack->Record(pModify);
	}
}

void CPoly::MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch)
{
	ASSERT(nAnchor >= 1 && nAnchor <= m_nAnchors);

	CSize delta(Δ);
	if(bMatch == true && m_nAnchors > 1)
	{
		CPoint fPoint;
		CPoint local = GetAnchor(nAnchor);
		local.x += Δ.cx;
		local.y += Δ.cy;

		if(nAnchor != 1)
			fPoint = GetAnchor(nAnchor - 1);
		else if(m_bClosed == true)
			fPoint = GetAnchor(m_nAnchors - 1);
		else
			fPoint = GetAnchor(nAnchor + 1);

		local = RegulizePoint(fPoint, local, 8);
		delta.cx = local.x - GetAnchor(nAnchor).x;
		delta.cy = local.y - GetAnchor(nAnchor).y;
	}

	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	if(nAnchor != 1 && delta != CSize(0, 0))
	{
		const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor - 2]);
		const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor - 1] + CPoint(delta.cx, delta.cy));
		pDC->MoveTo(point1.X, point1.Y);
		pDC->LineTo(point2.X, point2.Y);
	}
	else if(m_bClosed == true && delta != CSize(0, 0))
	{
		const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[m_nAnchors - 1 - 1]);
		const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[0] + CPoint(delta.cx, delta.cy));
		pDC->MoveTo(point1.X, point1.Y);
		pDC->LineTo(point2.X, point2.Y);
	}

	if(nAnchor != m_nAnchors && delta != CSize(0, 0))
	{
		const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor - 1] + CPoint(delta.cx, delta.cy));
		const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor]);
		pDC->MoveTo(point1.X, point1.Y);
		pDC->LineTo(point2.X, point2.Y);
	}
	else if(m_bClosed == true && delta != CSize(0, 0))
	{
		const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor - 1 - 1] + CPoint(delta.cx, delta.cy));
		const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[0]);
		pDC->MoveTo(point1.X, point1.Y);
		pDC->LineTo(point2.X, point2.Y);
	}

	CPoint point = GetAnchor(nAnchor);
	point.x += delta.cx;
	point.y += delta.cy;
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(point);
	CRect rect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
	rect.InflateRect(3, 3);
	pDC->Rectangle(rect);

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

bool CPoly::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CPath::PickOn(point, gap) == false)
		return false;

	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[i - 1];
		const CPoint& tpoint = m_pPoints[i + 1 - 1];

		if(PointOnLine(point, fpoint, tpoint, gap) == true)
			return true;
	}

	return false;
}

unsigned int CPoly::PickOn(const CPoint& point, const unsigned long& gap, double& t) const
{
	if(CPath::PickOn(point, gap) == false)
		return 0;

	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[i - 1];
		const CPoint& tpoint = m_pPoints[i + 1 - 1];
		if(PointOnLine(point, fpoint, tpoint, gap, t) == true)
			return i;
	}

	return 0;
}

bool CPoly::PickIn(const CPoint& point) const
{
	if(m_bClosed == false)
		return false;

	if(CPath::PickIn(point) == false)
		return false;
	else
		return CPoly::PointInPolygon(point, m_pPoints, m_nAnchors);
}

bool CPoly::PickIn(const CRect& rect) const
{
	if(CPath::PickIn(rect) == false)
		return false;

	for(unsigned int i = 1; i <= m_nAnchors; i++)
	{
		const CPoint point = GetAnchor(i);
		if(rect.PtInRect(point) == TRUE)
			return true;
	}
	if(m_bClosed == true)
	{
		const CPoint point1 = CPoint(rect.left, rect.top);
		const CPoint point2 = CPoint(rect.left, rect.bottom);
		const CPoint point3 = CPoint(rect.right, rect.top);
		const CPoint point4 = CPoint(rect.right, rect.bottom);
		if(this->PickIn(point1) == true || this->PickIn(point2) == true || this->PickIn(point3) == true || this->PickIn(point4) == true)
			return true;
	}
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const long x1 = m_pPoints[i - 1].x;
		const long y1 = m_pPoints[i - 1].y;
		const long x2 = m_pPoints[i].x;
		const long y2 = m_pPoints[i].y;

		for(unsigned int j = 1; j < 5; j++)
		{
			const long u1 = j == 1 || j == 4 ? rect.left : rect.right;
			const long v1 = j == 1 || j == 2 ? rect.top : rect.bottom;
			const long u2 = j == 1 || j == 2 ? rect.right : rect.left;
			const long v2 = j == 1 || j == 4 ? rect.top : rect.bottom;

			if((u1 == u2) && (v1 == v2))
				continue;
			const double a1 = (double)(y2 - v1) * (x2 - x1) > (double)(y2 - y1) * (x2 - u1) ? 1 : (double(y2 - v1) * (x2 - x1) == (double)(y2 - y1) * (x2 - u1) ? 0 : -1);
			const double a2 = (double)(y2 - v2) * (x2 - x1) > (double)(y2 - y1) * (x2 - u2) ? 1 : (double(y2 - v2) * (x2 - x1) == (double)(y2 - y1) * (x2 - u2) ? 0 : -1);
			const double a3 = (double)(v2 - y1) * (u2 - u1) > (double)(v2 - v1) * (u2 - x1) ? 1 : (double(v2 - y1) * (u2 - u1) == (double)(v2 - v1) * (u2 - x1) ? 0 : -1);
			const double a4 = (double)(v2 - y2) * (u2 - u1) > (double)(v2 - v1) * (u2 - x2) ? 1 : (double(v2 - y2) * (u2 - u1) == (double)(v2 - v1) * (u2 - x2) ? 0 : -1);
			if(a1 * a2 < 0 && a3 * a4 < 0)
				return true;
		}
	}
	return false;
}

bool CPoly::PickIn(const CPoint& center, const unsigned long& radius) const
{
	if(CPath::PickIn(center, radius) == false)
		return false;

	if(m_bClosed == true)
	{
		if(this->PickIn(center) == true)
			return true;
	}
	int sqradius = radius * radius;
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& point1 = GetAnchor(i);
		const CPoint& point2 = GetAnchor(i + 1);
		if(::Distance2(center, point1, point2) <= sqradius)
		{
			return true;
		}
	}
	return false;
}

bool CPoly::PickIn(const CPoly& polygon) const
{
	if(CPath::PickIn(polygon.m_Rect) == false)
		return false;

	for(unsigned int i = 1; i <= m_nAnchors; i++)
	{
		const CPoint& point = GetAnchor(i);
		if(polygon.PickIn(point) == true)
			return true;
	}
	if(m_bClosed == true)
	{
		for(unsigned int j = 1; j < polygon.m_nAnchors; j++)
		{
			const CPoint& point = polygon.GetAnchor(j);
			if(this->PickIn(point) == true)
				return true;
		}
	}
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const long x1 = m_pPoints[i - 1].x;
		const long y1 = m_pPoints[i - 1].y;
		const long x2 = m_pPoints[i].x;
		const long y2 = m_pPoints[i].y;

		for(unsigned int j = 1; j < polygon.m_nAnchors; j++)
		{
			const long u1 = polygon.m_pPoints[j - 1].x;
			const long v1 = polygon.m_pPoints[j - 1].y;
			const long u2 = polygon.m_pPoints[j].x;
			const long v2 = polygon.m_pPoints[j].y;

			if((u1 == u2) && (v1 == v2))
				continue;
			const double a1 = double(y2 - v1) * (x2 - x1) > double(y2 - y1) * (x2 - u1) ? 1 : (double(y2 - v1) * (x2 - x1) == double(y2 - y1) * (x2 - u1) ? 0 : -1);
			const double a2 = double(y2 - v2) * (x2 - x1) > double(y2 - y1) * (x2 - u2) ? 1 : (double(y2 - v2) * (x2 - x1) == double(y2 - y1) * (x2 - u2) ? 0 : -1);
			const double a3 = double(v2 - y1) * (u2 - u1) > double(v2 - v1) * (u2 - x1) ? 1 : (double(v2 - y1) * (u2 - u1) == double(v2 - v1) * (u2 - x1) ? 0 : -1);
			const double a4 = double(v2 - y2) * (u2 - u1) > double(v2 - v1) * (u2 - x2) ? 1 : (double(v2 - y2) * (u2 - u1) == double(v2 - v1) * (u2 - x2) ? 0 : -1);
			if(a1 * a2 < 0 && a3 * a4 < 0)
				return true;
		}
	}
	return false;
}

void CPoly::Reverse()
{
	for(unsigned int i = 0; i < m_nAnchors / 2; i++)
	{
		const CPoint buffer = m_pPoints[i];
		m_pPoints[i] = m_pPoints[m_nAnchors - 1 - i];
		m_pPoints[m_nAnchors - 1 - i] = buffer;
	}
	m_bModified = true;
}
CPoint* CPoly::Reverted() const
{
	CPoint* pPoints = new CPoint[m_nAnchors];
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		pPoints[i] = m_pPoints[m_nAnchors - 1 - i];
	}
	return pPoints;
}

void CPoly::CopyTo(CGeom* pGeom, bool ignore) const
{
	CPath::CopyTo(pGeom, ignore);

	if(m_geogroid != nullptr)
	{
		pGeom->SetGeogroid(*m_geogroid);
	}
	if(m_labeloid != nullptr)
	{
		pGeom->SetLabeloid(*m_labeloid);
	}
	if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
	{
		CPoly* pPoly = (CPoly*)pGeom;
		if(pPoly->m_pPoints == nullptr)
		{
			delete[] pPoly->m_pPoints;
			pPoly->m_pPoints = new CPoint[m_nAnchors];
			pPoly->m_nAllocs = m_nAnchors;
		}
		else if(pPoly->m_nAllocs < m_nAnchors)
		{
			delete[] pPoly->m_pPoints;
			pPoly->m_pPoints = new CPoint[m_nAnchors];
			pPoly->m_nAllocs = m_nAnchors;
		}
		if(m_pPoints != nullptr && m_nAnchors > 0)
		{
			memcpy(pPoly->m_pPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
			pPoly->m_nAnchors = m_nAnchors;
		}
	}
}

void CPoly::Swap(CGeom* pGeom)
{
	CPath::Swap(pGeom);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
	{
		CBezier* pBezier = (CBezier*)pGeom;

		Swapclass<CPoint*>(m_pPoints, pBezier->m_pPoints);
		Swapdata<unsigned int>(m_nAnchors, pBezier->m_nAnchors);
		Swapdata<unsigned int>(m_nAllocs, pBezier->m_nAllocs);
		Swapclass<CPoint*>(m_geogroid, pBezier->m_geogroid);
		Swapclass<CPoint*>(m_labeloid, pBezier->m_labeloid);
	}
}

void CPoly::CopyPoints(CPoint* points, bool direction, unsigned int& current)
{
	if(direction == true)
	{
		memcpy(points, m_pPoints, sizeof(CPoint) * m_nAnchors);
	}
	else
	{
		CPoint* pointer = points;
		for(int anchor = m_nAnchors - 1; anchor >= 0; anchor--)
		{
			*pointer = m_pPoints[anchor];
			pointer++;
		}
	}
	current += m_nAnchors;
}
CPoint* CPoly::ClonePoints(bool direction)
{
	CPoint* pPoints = new CPoint[m_nAnchors];

	memcpy(pPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
	if(direction == false)
	{
		for(int index = 0; index < m_nAnchors / 2; index++)
		{
			CPoint buf = pPoints[index];
			pPoints[index] = pPoints[m_nAnchors - index - 1];
			pPoints[m_nAnchors - index - 1] = buf;
		}
	}
	return pPoints;
}

bool CPoly::Close(bool pForce, const unsigned int& tolerance, Undoredo::CActionStack* pActionstack, bool ignore)
{
	if(m_bClosed == true)
		return false;

	if(m_nAnchors < 3)
		return false;

	if(m_pPoints == nullptr)
		return false;

	CPoint distance = GetAnchor(1) - GetAnchor(m_nAnchors);
	distance.x = abs(distance.x);
	distance.y = abs(distance.y);
	if(distance.x == 0 && distance.y == 0)
	{
		if(pActionstack != nullptr)
		{
			Undoredo::CModify<CWnd*, CPoly*, CPoint>* pModify = new Undoredo::CModify<CWnd*, CPoly*, CPoint>(nullptr, this, m_pPoints[m_nAnchors - 1], m_pPoints[0]);
			pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Close;
			pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Close;
			pActionstack->Record(pModify);
		}
		m_bClosed = true;
		m_bModified = true;
	}
	else if(distance.x <= tolerance && distance.y <= tolerance)
	{
		if(pActionstack != nullptr)
		{
			Undoredo::CModify<CWnd*, CPoly*, CPoint>* pModify = new Undoredo::CModify<CWnd*, CPoly*, CPoint>(nullptr, this, m_pPoints[m_nAnchors - 1], m_pPoints[0]);
			pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Close;
			pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Close;
			pActionstack->Record(pModify);
		}
		m_pPoints[m_nAnchors - 1] = m_pPoints[0];
		m_bClosed = true;
		m_bModified = true;
	}
	else if(pForce == true)
	{
		if(m_nAnchors >= m_nAllocs)
		{
			CPoint* newPoints = new CPoint[m_nAnchors + 10];
			if(m_pPoints != nullptr)
			{
				memcpy(newPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
				delete[] m_pPoints;
				m_pPoints = nullptr;
			}
			m_pPoints = newPoints;
			m_nAllocs = m_nAnchors + 10;
		}
		if(pActionstack != nullptr)
		{
			Undoredo::CModify<CWnd*, CPoly*, CPoint>* pModify = new Undoredo::CModify<CWnd*, CPoly*, CPoint>(nullptr, this, m_pPoints[m_nAnchors - 1], m_pPoints[0]);
			pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_ForceClose;
			pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_ForceClose;
			pActionstack->Record(pModify);
		}
		m_nAnchors++;
		m_pPoints[m_nAnchors - 1] = m_pPoints[0];
		m_bClosed = true;
		m_bModified = true;
	}
	if(ignore == false)
	{
		RecalCentroid();
	}
	return m_bClosed;
}
unsigned short CPoly::Joinable(const CPoly* pPoly1, const CPoly* pPoly2, const unsigned int& tolerance)
{
	if(pPoly1->m_bClosed == true || pPoly1->m_bClosed == true)
		return 0;
	if(pPoly2->m_bClosed == true || pPoly2->m_bClosed == true)
		return 0;
	if(pPoly1->Gettype() != pPoly2->Gettype())
		return false;

	const CPoint& point11 = pPoly1->GetAnchor(1);
	const CPoint& point12 = pPoly1->GetAnchor(pPoly1->m_nAnchors);
	const CPoint& point21 = pPoly2->GetAnchor(1);;
	const CPoint& point22 = pPoly2->GetAnchor(pPoly2->m_nAnchors);;
	const double distance1 = pow((double)point11.x - (double)point21.x, 2) + pow((double)point11.y - (double)point21.y, 2);//head to head
	const double distance2 = pow((double)point11.x - (double)point22.x, 2) + pow((double)point11.y - (double)point22.y, 2);//head to tail
	const double distance3 = pow((double)point12.x - (double)point21.x, 2) + pow((double)point12.y - (double)point21.y, 2);//tail to head
	const double distance4 = pow((double)point12.x - (double)point22.x, 2) + pow((double)point12.y - (double)point22.y, 2);//tail to tail
	const double distance = min(min(distance1, distance2), min(distance3, distance4));
	if(sqrt(distance) > tolerance)
		return false;
	if(distance == distance3)
		return 3;
	else if(distance == distance4)
		return 4;
	else if(distance == distance2)
		return 2;
	else if(distance == distance1)
		return 1;
	else
	{
		return 0;
	}
}
CPoly* CPoly::Join(const CPoly* pPoly, const BYTE& mode) const
{
	if(mode == 0)
		return nullptr;
	if(mode > 4)
		return nullptr;

	ASSERT(pPoly != this);

	CPoint buffer;
	unsigned int nPoints = 0;
	CPoint* pPoints = nullptr;
	const bool bClosed = false;
	switch(mode)
	{
		case 4:
			{
				nPoints = m_nAnchors + pPoly->m_nAnchors - 1;
				pPoints = new CPoint[nPoints];

				memcpy(pPoints + m_nAnchors - 1, pPoly->m_pPoints, sizeof(CPoint) * pPoly->m_nAnchors);
				ReverseOrder(pPoints + m_nAnchors - 1, pPoly->m_nAnchors);
				memcpy(pPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
			}
			break;
		case 3:
			{
				nPoints = m_nAnchors + pPoly->m_nAnchors - 1;
				pPoints = new CPoint[nPoints];

				memcpy(pPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
				memcpy(pPoints + m_nAnchors, pPoly->m_pPoints + 1, sizeof(CPoint) * (pPoly->m_nAnchors - 1));
			}
			break;
		case 2:
			{
				nPoints = m_nAnchors - 1 + pPoly->m_nAnchors;
				pPoints = new CPoint[nPoints];

				memcpy(pPoints, pPoly->m_pPoints, sizeof(CPoint) * pPoly->m_nAnchors);
				memcpy(pPoints + pPoly->m_nAnchors, m_pPoints + 1, sizeof(CPoint) * (m_nAnchors - 1));
			}
			break;
		case 1:
			{
				nPoints = m_nAnchors - 1 + pPoly->m_nAnchors;
				pPoints = new CPoint[nPoints];

				memcpy(pPoints, pPoly->m_pPoints, sizeof(CPoint) * pPoly->m_nAnchors);
				ReverseOrder(pPoints, pPoly->m_nAnchors);
				memcpy(pPoints + pPoly->m_nAnchors, m_pPoints + 1, sizeof(CPoint) * (m_nAnchors - 1));
			}
			break;
		default:
			return nullptr;
	}

	if(nPoints >= 2 && pPoints != nullptr)
	{
		CPoly* pPoly = new CPoly();
		CGeom::CopyTo(pPoly);

		pPoly->m_bClosed = bClosed;
		pPoly->m_pPoints = pPoints;
		pPoly->m_nAnchors = nPoints;
		pPoly->m_nAllocs = m_nAnchors;
		pPoly->RecalRect();
		return pPoly;
	}
	else if(pPoints != nullptr)
	{
		delete[] pPoints;
		pPoints = nullptr;
		return nullptr;
	}
	else
		return nullptr;
}

std::list<CGeom*> CPoly::Split(const CPoint& point, const unsigned long& gap) const
{
	std::list<CGeom*> polys;
	if(m_bLocked == true)
		return polys;

	if(m_pPoints == nullptr)
		return polys;

	CRect rect = m_Rect;
	rect.InflateRect(gap, gap);
	if(!rect.PtInRect(point))
		return polys;

	if(m_bClosed == false && ((point.x == m_pPoints[0].x && point.y == m_pPoints[0].y) || (point.x == m_pPoints[m_nAnchors - 1].x && point.y == m_pPoints[m_nAnchors - 1].y)))
		return polys;

	int nAnchor = -1;
	CPoint wound;

	double minDis = gap;
	for(unsigned int index = 1; index < m_nAnchors; index++)
	{
		const CPoint& fpoint = m_pPoints[index - 1];
		const CPoint& tpoint = m_pPoints[index + 1 - 1];

		CPoint knife = point;
		const double distance = Vertical(knife, fpoint, tpoint);
		if(minDis > distance)
		{
			minDis = distance;
			nAnchor = index;
			wound = knife;
		}
	}

	if(nAnchor != -1)
	{
		const CPoint& fpoint = m_pPoints[nAnchor - 1];
		const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];
		if(m_bClosed == true)
		{
			CPoly* poly = new CPoly();
			this->CopyTo(poly);
			poly->m_bClosed = false;

			if(wound.x == fpoint.x && wound.y == fpoint.y)
			{
				poly->m_nAnchors = m_nAnchors;
				poly->m_nAllocs = m_nAnchors;
				poly->m_pPoints = new CPoint[m_nAnchors];
				memcpy(poly->m_pPoints, m_pPoints + nAnchor - 1, sizeof(CPoint) * (m_nAnchors - nAnchor));
				memcpy(poly->m_pPoints + m_nAnchors - nAnchor, m_pPoints, sizeof(CPoint) * nAnchor);
			}
			else if(wound.x == tpoint.x && wound.y == tpoint.y)
			{
				poly->m_nAnchors = m_nAnchors;
				poly->m_nAllocs = m_nAnchors;
				poly->m_pPoints = new CPoint[m_nAnchors];
				memcpy(poly->m_pPoints, m_pPoints + nAnchor, sizeof(CPoint) * (m_nAnchors - nAnchor - 1));
				memcpy(poly->m_pPoints + m_nAnchors - nAnchor - 1, m_pPoints, sizeof(CPoint) * (nAnchor + 1));
			}
			else
			{
				poly->m_nAnchors = m_nAnchors + 1;
				poly->m_nAllocs = m_nAnchors + 1;
				poly->m_pPoints = new CPoint[m_nAnchors + 1];
				poly->m_pPoints[0] = wound;
				memcpy(poly->m_pPoints + 1, m_pPoints + nAnchor, sizeof(CPoint) * (m_nAnchors - nAnchor - 1));
				memcpy(poly->m_pPoints + m_nAnchors - nAnchor, m_pPoints, sizeof(CPoint) * nAnchor);
				poly->m_pPoints[m_nAnchors + 1 - 1] = wound;
			}
			poly->RecalRect();
			polys.push_back(poly);
		}
		else
		{
			CPoly* poly1 = new CPoly;
			CPoly* poly2 = new CPoly;
			CGeom::CopyTo(poly1);
			CGeom::CopyTo(poly2);
			poly1->m_bClosed = false;
			poly2->m_bClosed = false;
			if(wound.x == fpoint.x && wound.y == fpoint.y)
			{
				poly1->m_nAnchors = nAnchor;
				poly1->m_nAllocs = nAnchor;
				poly1->m_pPoints = new CPoint[nAnchor];
				memcpy(poly1->m_pPoints, m_pPoints, sizeof(CPoint) * nAnchor);

				poly2->m_nAnchors = m_nAnchors - nAnchor + 1;
				poly2->m_nAllocs = m_nAnchors - nAnchor + 1;
				poly2->m_pPoints = new CPoint[m_nAnchors - nAnchor + 1];
				memcpy(poly2->m_pPoints, m_pPoints + nAnchor - 1, sizeof(CPoint) * (m_nAnchors - nAnchor + 1));
			}
			else if(wound.x == tpoint.x && wound.y == tpoint.y)
			{
				poly1->m_nAnchors = nAnchor + 1;
				poly1->m_nAllocs = nAnchor + 1;
				poly1->m_pPoints = new CPoint[nAnchor + 1];
				memcpy(poly1->m_pPoints, m_pPoints, sizeof(CPoint) * (nAnchor + 1));

				poly2->m_nAnchors = m_nAnchors - nAnchor;
				poly2->m_nAllocs = m_nAnchors - nAnchor;
				poly2->m_pPoints = new CPoint[m_nAnchors - nAnchor];
				memcpy(poly2->m_pPoints, m_pPoints + nAnchor, sizeof(CPoint) * (m_nAnchors - nAnchor));
			}
			else
			{
				poly1->m_nAnchors = nAnchor + 1;
				poly1->m_nAllocs = nAnchor + 1;
				poly1->m_pPoints = new CPoint[nAnchor + 1];
				memcpy(poly1->m_pPoints, m_pPoints, sizeof(CPoint) * (nAnchor + 1));
				poly1->m_pPoints[nAnchor] = wound;

				poly2->m_nAnchors = m_nAnchors - nAnchor + 1;
				poly2->m_nAllocs = m_nAnchors - nAnchor + 1;
				poly2->m_pPoints = new CPoint[m_nAnchors - nAnchor + 1];
				poly2->m_pPoints[0] = wound;
				memcpy(poly2->m_pPoints + 1, m_pPoints + nAnchor, sizeof(CPoint) * (m_nAnchors - nAnchor));
			}
			poly1->RecalRect();
			poly2->RecalRect();
			polys.push_back(poly1);
			polys.push_back(poly2);
		}
	}
	return polys;
}

void CPoly::GetIntersectinon(const CPoly* poly1, const CPoly* poly2, CArray<CPoint, CPoint>& pointarray)
{
	const CRect rect1 = poly1->m_Rect;
	const CRect rect2 = poly2->m_Rect;
	if(Util::Rect::Intersect(rect1, rect2) == false)
		return;

	for(int index1 = 1; index1 < poly1->m_nAnchors; index1++)
	{
		const CPoint& fpoint1 = poly1->GetAnchor(index1);
		const CPoint& tpoint1 = poly1->GetAnchor(index1 + 1);

		for(int index2 = 1; index2 < poly2->m_nAnchors; index2++)
		{
			const CPoint& fpoint2 = poly2->GetAnchor(index2);
			const CPoint& tpoint2 = poly2->GetAnchor(index2 + 1);

			if(fpoint1 == fpoint2)
				pointarray.Add(fpoint1);
			if(fpoint1 == tpoint2)
				pointarray.Add(fpoint1);
			if(tpoint1 == fpoint2)
				pointarray.Add(tpoint1);
			if(tpoint1 == tpoint2)
				pointarray.Add(tpoint1);

			double t1 = 0.0f;
			double t2 = 0.0f;
			if(::LineInter(fpoint1, tpoint1, fpoint2, tpoint2, t1, t2) == true)
			{
				if(index1 > 1 && t1 == 0.0f)
					continue;
				if(index2 > 1 && t2 == 0.0f)
					continue;

				CPoint point;
				point.x = fpoint1.x + (tpoint1.x - fpoint1.x) * t1;
				point.y = fpoint1.y + (tpoint1.y - fpoint1.y) * t1;
				pointarray.Add(point);
			}
		}
	}

	int nCount = pointarray.GetCount();
	for(int index1 = 0; index1 < nCount; index1++)
	{
		const CPoint point1 = pointarray.ElementAt(index1);

		for(int index2 = index1 + 1; index2 < nCount; index2++)
		{
			CPoint point2 = pointarray.ElementAt(index2);
			if(point2 == point1)
			{
				pointarray.RemoveAt(index2);
				index2--;
				nCount--;
			}
		}
	}
}

bool CPoly::Add(const CPoly* poly)
{
	if(m_bClosed == true || poly->m_bClosed == true)
		return false;

	if(poly->IsKindOf(RUNTIME_CLASS(CBezier)) == TRUE)
		return false;

	if(m_pPoints == nullptr)
	{
		m_pPoints = new CPoint[poly->m_nAnchors];
		memcpy(m_pPoints, poly->m_pPoints, sizeof(CPoint) * poly->m_nAnchors);
		m_nAnchors = poly->m_nAnchors;
		m_nAllocs = m_nAnchors;
		m_Rect = poly->m_Rect;
		return true;
	}

	const double distance1 = pow((double)m_pPoints[0].x - (double)poly->m_pPoints[0].x, 2)
		+ pow((double)m_pPoints[0].y - (double)poly->m_pPoints[0].y, 2);
	const double distance2 = pow((double)m_pPoints[0].x - (double)poly->m_pPoints[poly->m_nAnchors - 1].x, 2)
		+ pow((double)m_pPoints[0].y - (double)poly->m_pPoints[poly->m_nAnchors - 1].y, 2);
	const double distance3 = pow((double)m_pPoints[m_nAnchors - 1].x - (double)poly->m_pPoints[0].x, 2)
		+ pow((double)m_pPoints[m_nAnchors - 1].y - (double)poly->m_pPoints[0].y, 2);
	const double distance4 = pow((double)m_pPoints[m_nAnchors - 1].x - (double)poly->m_pPoints[poly->m_nAnchors - 1].x, 2)
		+ pow((double)m_pPoints[m_nAnchors - 1].y - (double)poly->m_pPoints[poly->m_nAnchors - 1].y, 2);

	double distance = min(distance1, distance2);
	distance = min(distance, distance3);
	distance = min(distance, distance4);

	CPoint buffer;
	unsigned int nAnchor;
	CPoint* newPoints;
	if(distance == distance3)
	{
		nAnchor = m_nAnchors + poly->m_nAnchors - 1;
		newPoints = new CPoint[nAnchor];

		memcpy(newPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
		memcpy(newPoints + m_nAnchors, poly->m_pPoints + 1, sizeof(CPoint) * (poly->m_nAnchors - 1));
		delete[] m_pPoints;
	}
	else if(distance == distance4)
	{
		CPoint* reverted = poly->Reverted();
		nAnchor = m_nAnchors + poly->m_nAnchors - 1;
		newPoints = new CPoint[nAnchor];

		memcpy(newPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
		memcpy(newPoints + m_nAnchors, reverted + 1, sizeof(CPoint) * (poly->m_nAnchors - 1));
		delete[] m_pPoints;
		delete[] reverted;
	}
	else if(distance == distance2)
	{
		nAnchor = m_nAnchors - 1 + poly->m_nAnchors;
		newPoints = new CPoint[nAnchor];

		memcpy(newPoints, poly->m_pPoints, sizeof(CPoint) * poly->m_nAnchors);
		memcpy(newPoints + poly->m_nAnchors, m_pPoints + 1, sizeof(CPoint) * (m_nAnchors - 1));
		delete[] m_pPoints;
	}
	else if(distance == distance1)
	{
		CPoint* reverted = poly->Reverted();
		nAnchor = m_nAnchors - 1 + poly->m_nAnchors;
		newPoints = new CPoint[nAnchor];

		memcpy(newPoints, reverted, sizeof(CPoint) * poly->m_nAnchors);
		memcpy(newPoints + poly->m_nAnchors, m_pPoints + 1, sizeof(CPoint) * (m_nAnchors - 1));
		delete[] m_pPoints;
		delete[] reverted;
	}
	else
		return false;

	m_pPoints = newPoints;
	m_nAnchors = nAnchor;
	m_nAllocs = m_nAnchors;
	RecalRect();

	m_bModified = true;
	return true;
}

bool CPoly::AddAnchor(const CPoint& point, bool pTail, bool ignore)
{
	if(m_nAnchors >= m_nAllocs)
	{
		CPoint* newPoints = new CPoint[m_nAnchors + 10];
		if(m_pPoints != nullptr)
		{
			memcpy(newPoints, m_pPoints, sizeof(CPoint) * m_nAnchors);
			delete[] m_pPoints;
			m_pPoints = nullptr;
		}
		m_pPoints = newPoints;
		m_nAllocs = m_nAnchors + 10;
	}

	if(pTail)
	{
		if(m_nAnchors >= 1 && point.x == m_pPoints[m_nAnchors - 1].x && point.y == m_pPoints[m_nAnchors - 1].y)
			return false;

		m_pPoints[m_nAnchors] = point;
	}
	else
	{
		if(m_nAnchors >= 1 && point.x == m_pPoints[0].x && point.y == m_pPoints[0].y)
			return false;

		for(unsigned int i = m_nAnchors; i >= 1; i--)
		{
			m_pPoints[i + 1 - 1] = m_pPoints[i - 1];
		}
		m_pPoints[0] = point;
	}
	if(m_nAnchors == 0)
	{
		m_Rect.left = point.x;
		m_Rect.right = point.x;
	}
	else if(point.x < m_Rect.left)
		m_Rect.left = point.x;
	else if(point.x > m_Rect.right)
		m_Rect.right = point.x;

	if(m_nAnchors == 0)
	{
		m_Rect.top = point.y;
		m_Rect.bottom = point.y;
	}
	else if(point.y < m_Rect.top)
		m_Rect.top = point.y;
	else if(point.y > m_Rect.bottom)
		m_Rect.bottom = point.y;

	m_nAnchors++;
	m_bModified = true;
	if(ignore == false)
	{
		RecalCentroid();
	}
	return true;
}

void CPoly::InsertAnchor(const unsigned nAnchor, const CPoint& point)
{
	if(m_nAnchors >= m_nAllocs)
	{
		CPoint* newPoints = new CPoint[m_nAllocs + 10];
		if(m_pPoints != nullptr)
		{
			memcpy(newPoints, m_pPoints, sizeof(CPoint) * m_nAllocs);
			delete[] m_pPoints;
		}
		m_pPoints = newPoints;
		m_nAllocs += 10;
	}

	for(unsigned int i = m_nAnchors; i >= nAnchor; i--)
	{
		m_pPoints[i + 1 - 1] = m_pPoints[i - 1];
	}

	m_pPoints[nAnchor - 1].x = point.x;
	m_pPoints[nAnchor - 1].y = point.y;

	m_nAnchors++;
	m_bModified = true;
	RecalRect();
}

void CPoly::RemoveAnchor(const unsigned int& nAnchor)
{
	if(nAnchor < 1 || nAnchor > m_nAnchors)
		return;

	for(unsigned int i = nAnchor; i < m_nAnchors; i++)
	{
		m_pPoints[i - 1] = m_pPoints[i];
	}

	m_nAnchors--;

	if(nAnchor == 1 && m_bClosed == true)
	{
		m_pPoints[m_nAnchors - 1] = m_pPoints[0];
	}

	if(m_nAnchors == 3 && m_bClosed == true)
	{
		m_nAnchors = 2;
		m_bClosed = false;
	}

	m_bModified = true;
	RecalRect();
}

void CPoly::RecalRect()
{
	CPath::RecalRect();

	if(m_nAnchors == 0)
		m_Rect.SetRectEmpty();
	else if(m_pPoints == nullptr)
		m_Rect.SetRectEmpty();
	else
	{
		CRect rect(m_pPoints[0], CSize(0, 0));
		for(unsigned int i = 1; i <= m_nAnchors; i++)
		{
			if(m_pPoints[i - 1].x < rect.left)
				rect.left = m_pPoints[i - 1].x;
			if(m_pPoints[i - 1].x > rect.right)
				rect.right = m_pPoints[i - 1].x;
			if(m_pPoints[i - 1].y < rect.top)
				rect.top = m_pPoints[i - 1].y;
			if(m_pPoints[i - 1].y > rect.bottom)
				rect.bottom = m_pPoints[i - 1].y;
		}
		rect.NormalizeRect();
		m_Rect = rect;
	}
}

CPoint CPoly::FindCentroid() const
{
	if(m_bClosed == true)
	{
		double area = 0;
		double x = 0;
		double y = 0;
		for(unsigned int index = 1; index < m_nAnchors; index++)
		{
			CPoint point1 = this->GetAnchor(index);
			CPoint point2 = this->GetAnchor(index + 1);
			point1.Offset(-m_Rect.TopLeft());
			point2.Offset(-m_Rect.TopLeft());
			const double factor = (double)point1.x * point2.y - (double)point2.x * point1.y;
			x += point1.x * factor + point2.x * factor;
			y += point1.y * factor + point2.y * factor;
			area += factor / 2;
		}
		CPoint centroid;
		centroid.x = m_Rect.left + (area == 0 ? (m_Rect.right - m_Rect.left) / 2 : x / (6 * area));
		centroid.y = m_Rect.top + (area == 0 ? (m_Rect.bottom - m_Rect.top) / 2 : y / (6 * area));
		if(this->PickIn(centroid) == FALSE)
		{
			int maxlen = 0;
			int x = centroid.x;
			int y = centroid.y;

			std::vector<int> vertices1 = XRadialPoly(Direction::DI_Horizontal, centroid, this->m_pPoints, this->m_nAnchors);
			std::sort(vertices1.begin(), vertices1.end());
			for(long k = 0; k < vertices1.size() / 2; k++)
			{
				const long x1 = vertices1[k * 2];
				const long x2 = vertices1[k * 2 + 1];
				if(abs(x1 - x2) > maxlen)
				{
					maxlen = abs(x2 - x1);
					x = (x1 + x2) / 2;
					y = centroid.y;
				}
			}

			std::vector<int> vertices2 = YRadialPoly(Direction::DI_Vertical, centroid, this->m_pPoints, this->m_nAnchors);
			std::sort(vertices2.begin(), vertices2.end());
			for(long k = 0; k < vertices2.size() / 2; k++)
			{
				const long y1 = vertices2[k * 2];
				const long y2 = vertices2[k * 2 + 1];
				if(abs(y2 - y1) > maxlen)
				{
					maxlen = abs(y2 - y1);
					y = (y1 + y2) / 2;
					x = centroid.x;
				}
			}
			return CPoint(x, y);
		}
		else
			return centroid;
	}
	else
	{
		unsigned long length = GetLength(0);
		length = length / 2;
		unsigned int nAnchor;
		double t;

		GetAandT(1, 0, length, nAnchor, t);

		return GetPoint(nAnchor, t);
	}
}
void CPoly::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_pPoints == nullptr)
		return;

	CPoint* points = new CPoint[m_nAnchors];
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		const long x = m_pPoints[i].x * m11 + m_pPoints[i].y * m21 + m31;
		const long y = m_pPoints[i].x * m12 + m_pPoints[i].y * m22 + m32;
		points[i].x = x;
		points[i].y = y;
	}
	CPoint* replace = m_pPoints;
	m_pPoints = points;
	DrawPath(pDC, viewinfo);
	m_pPoints = replace;

	delete[] points;
}

void CPoly::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_pPoints == nullptr)
		return;

	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		const long x = round(m_pPoints[i].x * m11 + m_pPoints[i].y * m21 + m31);
		const long y = round(m_pPoints[i].x * m12 + m_pPoints[i].y * m22 + m32);
		m_pPoints[i].x = x;
		m_pPoints[i].y = y;
	}
	{
		const long x = round(m_centroid.x * m11 + m_centroid.y * m21 + m31);
		const long y = round(m_centroid.x * m12 + m_centroid.y * m22 + m32);
		m_centroid.x = x;
		m_centroid.y = y;
	}
	if(m_geogroid != nullptr)
	{
		const long x = round(m_geogroid->x * m11 + m_geogroid->y * m21 + m31);
		const long y = round(m_geogroid->x * m12 + m_geogroid->y * m22 + m32);
		m_geogroid->x = x;
		m_geogroid->y = y;
	}
	if(m_labeloid != nullptr)
	{
		const long x = round(m_labeloid->x * m11 + m_labeloid->y * m21 + m31);
		const long y = round(m_labeloid->x * m12 + m_labeloid->y * m22 + m32);
		m_labeloid->x = x;
		m_labeloid->y = y;
	}
	CPath::Transform(m11, m21, m31, m12, m22, m32);
}

void CPoly::Transform(const CViewinfo& viewinfo)
{
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[i]);
		m_pPoints[i].x = point.X;
		m_pPoints[i].y = point.Y;
	}

	CPath::Transform(viewinfo);
}

void CPoly::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	for(unsigned int i = 1; i <= m_nAnchors; i++)
	{
		m_pPoints[i - 1] = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, m_pPoints[i - 1]);
	}

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
	RecalRect();
	m_bModified = true;
}

CGeom* CPoly::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	if(fMapinfo.m_pProjection == nullptr)
		return this;
	bool differnetGcs = fMapinfo.m_pProjection->GCS.m_Key && tMapinfo.m_pProjection->GCS.m_Key && strcmp(fMapinfo.m_pProjection->GCS.m_Key, tMapinfo.m_pProjection->GCS.m_Key) != 0;
	for(int index = 0; index < m_nAnchors; index++)
	{
		CPointF geoPoint = fMapinfo.DocToGeo(m_pPoints[index]);
		if(differnetGcs)
		{
			geoPoint = fMapinfo.m_pProjection->GCS.ThistoWGS84(geoPoint);
			geoPoint = tMapinfo.m_pProjection->GCS.WGS84toThis(geoPoint);
		}
		CPointF mapPoint = tMapinfo.m_pProjection->GeoToMap(geoPoint);
		m_pPoints[index] = tMapinfo.MapToDoc(mapPoint);

	}
	m_bModified = true;
	RecalRect();
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
	return this;
}

void CPoly::Rectify(CTin& tin)
{
	for(unsigned int i = 1; i <= m_nAnchors; i++)
	{
		const CPoint point = tin.Rectify(m_pPoints[i - 1]);
		m_pPoints[i - 1] = point;
	}
	RecalRect();
	m_bModified = true;
}

void CPoly::ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const
{
	if(this->IsValid() == false)
		return;

	Gdiplus::PointF* points = new Gdiplus::PointF[m_nAnchors];
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		points[i].X = m_pPoints[i].x;
		points[i].Y = m_pPoints[i].y;
	}
	matrix.TransformPoints(points, m_nAnchors);

	_ftprintf(file, _T("%g %g m\n"), points[0].X, points[0].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		_ftprintf(file, _T("%g %g L\n"), points[i].X, points[i].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CPoly::ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const
{
	if(this->IsValid() == false)
		return;

	const Gdiplus::PointF* points = viewinfo.DocToClient <Gdiplus::PointF>(m_pPoints, m_nAnchors);

	_ftprintf(file, _T("%g %g m\n"), points[0].X, points[0].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		_ftprintf(file, _T("%g %g L\n"), points[i].X, points[i].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CPoly::ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(this->IsValid() == false)
		return;

	if(GetAnchors() < 2)
		return;

	const Gdiplus::PointF* points = viewinfo.DocToClient <Gdiplus::PointF>(m_pPoints, m_nAnchors);
	HPDF_Page_MoveTo(page, points[0].X, points[0].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		HPDF_Page_LineTo(page, points[i].X, points[i].Y);
	}

	delete[] points;
}

void CPoly::ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid() == false)
		return;

	Gdiplus::PointF* points = new Gdiplus::PointF[m_nAnchors];
	for(unsigned int i = 0; i < m_nAnchors; i++)
	{
		points[i].X = m_pPoints[i].x;
		points[i].Y = m_pPoints[i].y;
	}
	matrix.TransformPoints(points, m_nAnchors);

	HPDF_Page_MoveTo(page, points[0].X, points[0].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		HPDF_Page_LineTo(page, points[i].X, points[i].Y);
	}
	delete[] points;
}

CPoint CPoly::BezierCtrlPoint(const CPoint& prev, const CPoint& point, const CPoint& next, const float& scale)
{
	const int x = (point.x + ((double)point.x - prev.x) * scale + ((double)point.x + next.x) / 2.0) / 2.0;
	const int y = (point.y + ((double)point.y - prev.y) * scale + ((double)point.y + next.y) / 2.0) / 2.0;
	return CPoint(x, y);
}

CBezier* CPoly::Bezierize(const float scale) const
{
	if(m_nAnchors < 2)
		return nullptr;
	else if(m_pPoints != nullptr)
	{
		CPoint* pPoints = new CPoint[m_nAnchors * 3 + 3];
		pPoints[0] = m_pPoints[0];
		pPoints[1] = m_pPoints[0];
		pPoints[2] = m_pPoints[0];
		for(int anchor = 1; anchor < m_nAnchors - 1; anchor++)
		{
			pPoints[anchor * 3 + 0] = BezierCtrlPoint(m_pPoints[anchor + 1], m_pPoints[anchor], m_pPoints[anchor - 1], scale);
			pPoints[anchor * 3 + 1] = m_pPoints[anchor];
			pPoints[anchor * 3 + 2] = BezierCtrlPoint(m_pPoints[anchor - 1], m_pPoints[anchor], m_pPoints[anchor + 1], scale);
		}
		pPoints[(m_nAnchors - 1) * 3 + 0] = m_pPoints[m_nAnchors - 1];
		pPoints[(m_nAnchors - 1) * 3 + 1] = m_pPoints[m_nAnchors - 1];
		pPoints[(m_nAnchors - 1) * 3 + 2] = m_pPoints[m_nAnchors - 1];

		CBezier* pBezier = new CBezier();
		CGeom::CopyTo(pBezier);
		pBezier->m_nAnchors = this->m_nAnchors;
		pBezier->m_nAllocs = this->m_nAnchors * 3 + 3;
		pBezier->m_pPoints = pPoints;
		pBezier->m_bActive = this->m_bActive;
		pBezier->RecalRect();
		return pBezier;
	}
	else
		return nullptr;
}

void CPoly::Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_pPoints == nullptr)
		return;
	else if(m_nAnchors < 2)
		return;

	std::list<CPoint> points;
	for(unsigned int index1 = 1; index1 < m_nAnchors; index1++)
	{
		const CPoint& fpoint = m_pPoints[index1 - 1];
		const CPoint& tpoint = m_pPoints[index1 + 1 - 1];
		points.push_back(fpoint);
		const double distance = ::LineLength1(fpoint, tpoint);
		const int count = int(distance / tolerance);
		if(count >= 2)
		{
			const double dx = ((double)tpoint.x - fpoint.x) / count;
			const double dy = ((double)tpoint.y - fpoint.y) / count;
			for(int index2 = 1; index2 < count; index2++)
			{
				const int x = round(fpoint.x + index2 * dx);
				const int y = round(fpoint.y + index2 * dy);
				points.push_back(CPoint(x, y));
			}
		}
	}
	points.push_back(m_pPoints[m_nAnchors - 1]);

	if(points.size() > m_nAnchors)
	{
		const std::pair<unsigned int, CPoint*> original(m_nAnchors, m_pPoints);
		m_nAnchors = points.size();
		m_nAllocs = m_nAnchors;
		m_pPoints = new CPoint[m_nAnchors];
		for(int index = 0; index < m_nAnchors; index++)
		{
			m_pPoints[index] = *points.begin();
			points.pop_front();
		}
		const std::pair<unsigned int, CPoint*> modified(m_nAnchors, m_pPoints);
		m_bModified = true;

		Undoredo::CModify<CWnd*, CPoly*, std::pair<unsigned int, CPoint*>>* pModify = new Undoredo::CModify<CWnd*, CPoly*, std::pair<unsigned int, CPoint*>>(nullptr, this, original, modified);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Points;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Points;
		pModify->free = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Free_Modify_Points;
		actionstack.Record(pModify);
	}
}
void CPoly::Replace(std::vector<int>& coords, Undoredo::CActionStack& actionstack)
{
	const std::pair<unsigned int, CPoint*> original(m_nAnchors, m_pPoints);

	m_nAnchors = 0;
	m_pPoints = new CPoint[coords.size() / 2];
	for(std::vector<int>::iterator it = coords.begin(); it < coords.end(); it++)
	{
		m_pPoints[m_nAnchors].x = *it;
		it++;
		m_pPoints[m_nAnchors].y = *it;
		m_nAnchors++;
	}
	m_nAllocs = m_nAnchors;
	m_bModified = true;
	RecalRect();

	const std::pair<unsigned int, CPoint*> modified(m_nAnchors, m_pPoints);
	Undoredo::CModify<CWnd*, CPoly*, std::pair<unsigned int, CPoint*>>* pModify = new Undoredo::CModify<CWnd*, CPoly*, std::pair<unsigned int, CPoint*>>(nullptr, this, original, modified);
	pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Points;
	pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Points;
	pModify->free = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Free_Modify_Points;
	actionstack.Record(pModify);
}
void CPoly::Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	std::vector<int> result;
	if(m_bClosed && m_nAnchors > 2)
	{
		bool clockwise = this->GetArea() >= 0;
		Simplify::Simplify4(m_pPoints, m_nAnchors, tolerance, result);
	}
	else
	{
		Simplify::Simplify1(m_pPoints, m_nAnchors, tolerance, result);
	}

	CPoly::Replace(result, actionstack);
}
void CPoly::Simplify(const unsigned int& tolerance)
{
	if(m_nAnchors <= 2)
		return;
	else if(m_bClosed && m_nAnchors < 4)
		return;

	std::vector<int> result;
	if(m_bClosed)
	{
		Simplify::Simplify4(m_pPoints, m_nAnchors, tolerance, result);
	}
	else
	{
		Simplify::Simplify1(m_pPoints, m_nAnchors, tolerance, result);
	}
	delete[] m_pPoints;

	CPoint* pPoints = new CPoint[result.size() / 2 + 1];
	m_nAnchors = 0;
	for(auto it = result.begin(); it != result.end(); ++it)
	{
		pPoints[m_nAnchors].x = *it++;
		pPoints[m_nAnchors].y = *it;
		m_nAnchors++;
	}
	if(m_bClosed && pPoints[0] != pPoints[m_nAnchors])
	{
		pPoints[m_nAnchors++] = pPoints[0];
	}
	m_pPoints = pPoints;
}
void CPoly::Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms)
{
	if(m_nAnchors <= 2)
		return;
	else if(m_bClosed && m_nAnchors < 4)
		return;

	if(m_bClosed)
	{
		bool clockwise = this->GetArea() >= 0;
		std::list<std::vector<int>> result;
		Simplify::Correct(m_pPoints, m_nAnchors, clockwise, tolerance, result);
		for(auto it1 = result.begin(); it1 != result.end(); ++it1)
		{
			auto xys = *it1;
			CPoint* pPoints = new CPoint[xys.size() / 2];
			int nAnchors = 0;
			for(auto it2 = xys.begin(); it2 != xys.end(); it2++)
			{
				pPoints[nAnchors].x = *it2++;
				pPoints[nAnchors].y = *it2;
				nAnchors++;
			}
			CPoly* pPoly = new CPoly(pPoints, nAnchors, true);
			geoms.push_back(pPoly);
		}
	}
}
void CPoly::Normalize()
{
	if(GetArea() < 0)
	{
		this->Reverse();
	}
}
void CPoly::Normalize(bool bHole)
{
	if(GetArea() > 0 ^ bHole)
	{
	}
	else
	{
		this->Reverse();
	}
}
CGeom* CPoly::Buffer(const unsigned int& distance, const unsigned int& tolerance, const Clipper2Lib::JoinType& jointype, const Clipper2Lib::EndType& endtype) const
{
	Clipper2Lib::Paths64 paths;
	this->GetPaths64(paths, distance);

	Clipper2Lib::Paths64 solution = Clipper2Lib::InflatePaths(paths, distance, jointype, endtype, 2.0, tolerance);
	return Convert(solution, true);
}

CGeom* CPoly::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(CGeom::Clip(bound, clips, in, tolerance, bStroke) == nullptr)
		return nullptr;

	Clipper2Lib::Path64 path(m_nAnchors);
	for(unsigned int index = 0; index < m_nAnchors; index++)
	{
		path[index].x = m_pPoints[index].x;
		path[index].y = m_pPoints[index].y;
	}
	Clipper2Lib::Paths64 subjects;
	subjects.push_back(path);

	Clipper64 clipper;
	clipper.AddClip(clips);
	if(m_bClosed)
		clipper.AddSubject(subjects);
	else
		clipper.AddOpenSubject(subjects);

	auto cliptype = in ? ClipType::Intersection : ClipType::Difference;
	Paths64 closedPaths;
	Paths64 openPaths;
	if(m_bClosed && bStroke)
		clipper.Execute(cliptype, FillRule::NonZero, closedPaths, openPaths);
	else if(m_bClosed)
		clipper.Execute(cliptype, FillRule::NonZero, closedPaths);
	else
		clipper.Execute(cliptype, FillRule::NonZero, closedPaths, openPaths);

	CGeom* pCliped = nullptr;
	if(closedPaths.size() > 0 && openPaths.size() > 0)
	{
		auto pCliped1 = Convert(closedPaths, true);
		auto pCliped2 = Convert(openPaths, true);
		CGroup* pGroup = new CGroup();
		pGroup->AddMember(pCliped1);
		pGroup->AddMember(pCliped2);
		pCliped = pGroup;
	}
	else if(closedPaths.size() > 0)
		pCliped = Convert(closedPaths, true);
	else if(openPaths.size() > 0)
		pCliped = Convert(openPaths, false);

	if(pCliped != nullptr)
	{
		CGeom::CopyTo(pCliped);
	}
	return pCliped;
}

void CPoly::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
	if(this->IsValid() == false)
		return;

	if(m_bClosed == true)
	{
		_ftprintf(fileMif, _T("Region 1\n"));
		_ftprintf(fileMif, _T("%d\n"), m_nAnchors);
	}
	else
	{
		_ftprintf(fileMif, _T("PLine %d\n"), m_nAnchors);
	}

	for(unsigned int nAnchor = 1; nAnchor <= m_nAnchors; nAnchor++)
	{
		if(bTransGeo == true)
		{
			const CPointF geoPoint = datainfo.DocToGeo(m_pPoints[nAnchor - 1]);
			_ftprintf(fileMif, _T("%g %g\n"), geoPoint.x, geoPoint.y);
		}
		else
		{
			_ftprintf(fileMif, _T("%d %d\n"), m_pPoints[nAnchor - 1].x, m_pPoints[nAnchor - 1].y);
		}
	}
}

DWORD CPoly::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CPath::PackPC(pFile, bytes);

	if(pFile != nullptr)
	{
		const double area = GetArea();
		if(area < 0)
		{
			Reverse();
		}
		const unsigned short nCode = (((m_geoId << 7) + 2 * m_geoId) ^ 0X5643);
		const unsigned int nAnchors = (m_nAnchors ^ nCode);
		pFile->Write(&nAnchors, sizeof(unsigned int));

		for(unsigned int i = 0; i < m_nAnchors; i++)
		{
			CPoint point = m_pPoints[i];
			const DWORD nCode1 = (((m_geoId << 8) + i * m_geoId) ^ 0X54354354);
			const DWORD nCode2 = (((m_geoId << 6) + i * m_geoId) ^ 0X54353534);
			point.x = point.x ^ nCode1;
			point.y = point.y ^ nCode2;

			pFile->Write(&point, sizeof(CPoint));
		}

		size += sizeof(unsigned int) + m_nAnchors * sizeof(CPoint);

		return size;
	}
	else
	{
		const unsigned short nCode = (((m_geoId << 7) + 2 * m_geoId) ^ 0X5643);
		const unsigned int nAnchors = (unsigned int)*(unsigned int*)bytes;
		m_nAnchors = (nAnchors ^ nCode);
		bytes += sizeof(unsigned int);

		/*	m_nAnchors = 100000000000000;
		DWORD tick1 = GetTickCount();
		m_pPoints = new CPoint[m_nAnchors];
		DWORD tick2 = GetTickCount();
		memcpy(m_pPoints,bytes,m_nAnchors*sizeof(CPoint));
		DWORD tick3 = GetTickCount();
		*/
		m_pPoints = new CPoint[m_nAnchors];
		memcpy(m_pPoints, bytes, m_nAnchors * sizeof(CPoint));

		bytes += m_nAnchors * sizeof(CPoint);

		for(unsigned int i = 0; i < m_nAnchors; i++)
		{
			const DWORD nCode1 = (((m_geoId << 8) + i * m_geoId) ^ 0X54354354);
			const DWORD nCode2 = (((m_geoId << 6) + i * m_geoId) ^ 0X54353534);
			m_pPoints[i].x = m_pPoints[i].x ^ nCode1;
			m_pPoints[i].y = m_pPoints[i].y ^ nCode2;
		}
		return 0;
	}
}

DWORD CPoly::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CPath::ReleaseCE(file, type);
	const double area = GetArea();
	if(area < 0)
	{
		//needed	Reverse();
	}
	file.Write(&m_nAnchors, sizeof(unsigned int));
	for(unsigned int nAnchor = 1; nAnchor <= m_nAnchors; nAnchor++)
	{
		CPoint point = m_pPoints[nAnchor - 1];
		point.x /= 10000;
		point.y /= 10000;

		file.Write(&point.x, sizeof(int));
		file.Write(&point.y, sizeof(int));
	}

	size += sizeof(unsigned int) + m_nAnchors * (sizeof(int) + sizeof(int));

	return size;
}

void CPoly::ReleaseWeb(CFile& file, CSize offset) const
{
	CPath::ReleaseWeb(file, offset);;

	if(m_bClosed == false)
	{
		CObArray labelarray;
		CText::AnalyzeString(m_strName, labelarray);
		const BYTE bWords = min(255, labelarray.GetCount());
		file.Write(&bWords, 1);
		for(int index = 0; index < bWords; index++)
		{
			const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
			SaveAsUTF8(file, label->str);
			delete label;
		}
		labelarray.RemoveAll();
	}

	unsigned int nAnchors = m_nAnchors;
	ReverseOrder(nAnchors);
	file.Write(&nAnchors, sizeof(unsigned int));

	for(unsigned int index = 0; index < m_nAnchors; index++)
	{
		short x = m_pPoints[index].x - offset.cx;
		short y = m_pPoints[index].y - offset.cy;
		ReverseOrder(x);
		file.Write(&x, sizeof(short));
		ReverseOrder(y);
		file.Write(&y, sizeof(short));
	}

	if(m_bClosed == true)
	{
		{
			short x = m_centroid.x - offset.cx;
			short y = m_centroid.y - offset.cy;
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
		if(m_geogroid != nullptr)
		{
			short x = m_geogroid->x - offset.cx;
			short y = m_geogroid->y - offset.cy;
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
		if(m_labeloid != nullptr)
		{
			short x = m_labeloid->x - offset.cx;
			short y = m_labeloid->y - offset.cy;
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
	}
}

void CPoly::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CPath::ReleaseWeb(stream, offset);;

	if(m_bClosed == false)
	{
		CObArray labelarray;
		CText::AnalyzeString(m_strName, labelarray);
		const BYTE bWords = min(255, labelarray.GetCount());
		stream << bWords;
		for(int index = 0; index < bWords; index++)
		{
			const CText::CWord* label = (CText::CWord*)labelarray.GetAt(index);
			SaveAsUTF8(stream, label->str);
			delete label;
		}
		labelarray.RemoveAll();
	}

	stream << m_nAnchors;
	for(unsigned int index = 0; index < m_nAnchors; index++)
	{
		const short x = m_pPoints[index].x - offset.cx;
		const short y = m_pPoints[index].y - offset.cy;
		stream << x;
		stream << y;
	}

	if(m_bClosed == true)
	{
		{
			const short x = m_centroid.x - offset.cx;
			const short y = m_centroid.y - offset.cy;
			stream << x;
			stream << y;
		}
		if(m_geogroid != nullptr)
		{
			const short x = m_geogroid->x - offset.cx;
			const short y = m_geogroid->y - offset.cy;
			stream << x;
			stream << y;
		}
		if(m_labeloid != nullptr)
		{
			const short x = m_labeloid->x - offset.cx;
			const short y = m_labeloid->y - offset.cy;
			stream << x;
			stream << y;
		}
	}
}
void CPoly::ReleaseWeb(boost::json::object& json) const
{
	CPath::ReleaseWeb(json);

	if(m_bClosed == false)
	{
		if(m_strName.IsEmpty() == FALSE)
		{
			CObArray labelarray;
			CText::AnalyzeString(m_strName, labelarray);
			const BYTE bWords = min(255, labelarray.GetCount());

			boost::json::object child1;
			for(int index = 0; index < bWords; index++)
			{
				const CText::CWord* word = (CText::CWord*)labelarray.GetAt(index);
				SaveAsEscapedASCII(child1, nullptr, word->str);
				delete word;
			}
			json["Words"] = child1;
			labelarray.RemoveAll();
		}
	}
	json["Anchors"] = m_nAnchors;
	std::vector<int> child2;

	child2.push_back(m_pPoints[0].x);
	child2.push_back(m_pPoints[0].y);
	CPoint prev = m_pPoints[0];
	for(unsigned int index = 1; index < m_nAnchors; index++)
	{
		child2.push_back(m_pPoints[index].x - prev.x);
		child2.push_back(m_pPoints[index].y - prev.y);
		prev = m_pPoints[index];
	}
	json["Points"] = boost::json::value_from(child2);
	if(m_bClosed == true)
	{
		const CPoint geogroid = this->GetGeogroid();
		if(geogroid.x != 0 && geogroid.y != 0)
		{
			std::vector<int> child3;
			child3.push_back(geogroid.x);
			child3.push_back(geogroid.y);
			json["Geogroid"] = boost::json::value_from(child3);
		}
		const CPoint labeloid = this->GetLabeloid();
		if(labeloid.x != 0 && labeloid.y != 0)
		{
			std::vector<int> child4;
			child4.push_back(labeloid.x);
			child4.push_back(labeloid.y);
			json["Labeloid"] = boost::json::value_from(child4);
		}
	}
}
void CPoly::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPath::ReleaseWeb(writer, offset);

	if(m_bClosed == false && m_strName.IsEmpty() == FALSE)
	{
		CObArray labelarray;
		CText::AnalyzeString(m_strName, labelarray);
		const BYTE bWords = min(255, labelarray.GetCount());
		writer.add_variant_uint16(bWords);
		for(int index = 0; index < bWords; index++)
		{
			const CText::CWord* word = (CText::CWord*)labelarray.GetAt(index);
			writer.add_string(EscapedASCII(word->str));
			delete word;
		}
		labelarray.RemoveAll();
	}
	writer.add_variant_uint32(m_nAnchors);

	for(unsigned int index = 0; index < m_nAnchors; index++)
	{
		writer.add_variant_sint32(m_pPoints[index].x - offset.cx);
		writer.add_variant_sint32(m_pPoints[index].y - offset.cy);
	}

	if(m_bClosed == true)
	{
		const CPoint geogroid = this->GetGeogroid();
		writer.add_variant_sint32(geogroid.x - offset.cx);
		writer.add_variant_sint32(geogroid.y - offset.cy);

		const CPoint labeloid = this->GetLabeloid();
		writer.add_variant_sint32(labeloid.x - offset.cx);
		writer.add_variant_sint32(labeloid.y - offset.cy);
	}
}
void CPoly::ReleasePoints(pbf::writer& writer, const CSize offset)
{
	writer.add_variant_uint32(m_nAnchors);

	for(unsigned int index = 0; index < m_nAnchors; index++)
	{
		writer.add_variant_sint32(m_pPoints[index].x - offset.cx);
		writer.add_variant_sint32(m_pPoints[index].y - offset.cy);
	}
}
double CPoly::Shortcut(const CPoint& point, CPoint& trend) const
{
	if(m_bClosed == true && PickIn(point) == true)
	{
		return 0;
	}
	else if(m_pPoints != nullptr)
	{
		double miniDis = 100000000000;
		for(unsigned int nAnchor = 1; nAnchor < m_nAnchors; nAnchor++)
		{
			const CPoint& fpoint = m_pPoints[nAnchor - 1];
			const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];

			CPoint knife = point;
			const double distance = Vertical(knife, fpoint, tpoint);
			if(distance < miniDis)
			{
				miniDis = distance;
				trend.x = knife.x;
				trend.y = knife.y;
			}
		}
		return miniDis;
	}
	else
		return 0X00FFFFFF;
}

bool CPoly::GetOrthocentre(const CPoint& point, unsigned int& nAnchor, double& t) const
{
	if(m_bClosed == true && PickIn(point) == true)
	{
		return false;
	}
	else if(m_pPoints != nullptr)
	{
		const CPoint point0 = point;
		double miniDis = 100000000000;
		for(unsigned int index = 1; index < m_nAnchors; index++)
		{
			const CPoint& fpoint = m_pPoints[index - 1];
			const CPoint& tpoint = m_pPoints[index + 1 - 1];

			double distance = 0.0f;
			const double D1 = pow((double)(fpoint.x - point0.x), 2) + pow((double)(fpoint.y - point0.y), 2);
			const double D2 = pow((double)(tpoint.x - point0.x), 2) + pow((double)(tpoint.y - point0.y), 2);
			const double D0 = pow((double)(tpoint.x - fpoint.x), 2) + pow((double)(tpoint.y - fpoint.y), 2);

			if((D1 >= D2 && D1 < D2 + D0) || (D2 >= D1 && D2 < D1 + D0))
			{
				const double A = fpoint.y - tpoint.y;
				const double B = tpoint.x - fpoint.x;
				const double C = B * fpoint.y + A * fpoint.x;

				distance = (A * point0.x + B * point0.y - C) / sqrt(A * A + B * B);
				distance = abs(distance);
			}
			else if(D1 < D2)
			{
				distance = sqrt(D1);
			}
			else
			{
				distance = sqrt(D2);
			}

			if(distance < miniDis)
			{
				const double A = fpoint.y - tpoint.y;
				const double B = tpoint.x - fpoint.x;
				const double C = B * fpoint.y + A * fpoint.x;
				const double X = (B * B * point0.x - A * B * point0.y + A * C) / (A * A + B * B);
				const double Y = (A * A * point0.y - A * B * point0.x + B * C) / (A * A + B * B);
				const double d1 = sqrt(pow(X - fpoint.x, 2) + pow(Y - fpoint.y, 2));
				const double d2 = sqrt(pow(X - tpoint.x, 2) + pow(Y - tpoint.y, 2));

				t = d1 / (d1 + d2);

				nAnchor = index;
				miniDis = distance;
			}
		}

		return true;
	}
	else
		return false;
}

long CPoly::Length(Gdiplus::Point* pPoints, int nAnchors)
{
	double length = 0;
	if(pPoints != nullptr)
	{
		for(unsigned int index = 1; index < nAnchors; index++)
		{
			Gdiplus::Point fpoint = pPoints[index - 1];
			Gdiplus::Point tpoint = pPoints[index + 1 - 1];

			length += ::LineLength1(fpoint, tpoint);
		}
	}

	return round(length);
}

Gdiplus::Point CPoly::GetPoint(const Gdiplus::Point* pPoints, int nAnchors, const unsigned int& nAnchor, const double& t)
{
	ASSERT(pPoints != nullptr);

	if(t == 0.f)
		return pPoints[nAnchor - 1];
	if(t == 1.f)
		return pPoints[nAnchor];
	const Gdiplus::Point fpoint = pPoints[nAnchor - 1];
	const Gdiplus::Point tpoint = pPoints[nAnchor + 1 - 1];
	const int X = fpoint.X + round((tpoint.X - fpoint.X) * t);
	const int Y = fpoint.Y + round((tpoint.Y - fpoint.Y) * t);

	return Gdiplus::Point(X, Y);
}

bool CPoly::GetAandT(const Gdiplus::Point* pPoints, int nAnchors, const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et)
{
	if(length == 0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	for(unsigned int i = fAnchor; i < nAnchors; i++)
	{
		const Gdiplus::Point fpoint = pPoints[i - 1];
		const Gdiplus::Point tpoint = pPoints[i + 1 - 1];
		const double len = pow((double)(tpoint.X - fpoint.X), 2) + pow((double)(tpoint.Y - fpoint.Y), 2);
		const double segment = sqrt(len);
		const double leftlen = segment - st * segment;
		if(length < leftlen)
		{
			tAnchor = i;
			et = st + (double)length / segment;

			return true;
		}

		if(length == leftlen)
		{
			tAnchor = i;
			et = 1;

			return true;
		}
		length -= leftlen;
		st = 0;
	}

	tAnchor = nAnchors - 1;
	et = 1;

	return false;
}

std::vector<std::pair<unsigned int, CPoint*>> CPoly::Clipin(CRect clipper, const CPoint* pPoints, unsigned int nAnchors)
{
	std::vector<std::pair<unsigned int, CPoint*>> polylist;

	unsigned int fAnchor = 1;
	CPoint fPoint = pPoints[1 - 1];

	BOOL bPrevInside = clipper.PtInRect(pPoints[1 - 1]);
	for(unsigned int I = 2; I <= nAnchors; I++)
	{
		CPoint point1 = pPoints[I - 1 - 1];
		CPoint point2 = pPoints[I - 1];
		const BOOL bCurtInside = clipper.PtInRect(point2);
		if(bPrevInside == false && bCurtInside == true)
		{
			::LineClip(clipper, point1, point2);
			fPoint = point1;
			fAnchor = I - 1;

			if(I == nAnchors)
			{
				CPoint* points = new CPoint[2];
				points[0] = point1;
				points[1] = point2;
				std::pair<unsigned int, CPoint*> poly = make_pair(2, points);
				polylist.push_back(poly);
			}
		}
		else if(bPrevInside == false && bCurtInside == false)
		{
			const bool ret = LineClip(clipper, point1, point2);
			if(ret == true)
			{
				CPoint* points = new CPoint[2];
				points[0] = point1;
				points[1] = point2;
				std::pair<unsigned int, CPoint*> poly = make_pair(2, points);
				polylist.push_back(poly);
			}
		}
		else if(bPrevInside == true && bCurtInside == false)
		{
			const bool ret = LineClip(clipper, point1, point2);
			if(ret == true)
			{
				CPoint* points = new CPoint[I - fAnchor + 1];
				memcpy(points, pPoints + fAnchor - 1, (I - fAnchor + 1) * sizeof(CPoint));
				points[0] = fPoint;
				points[I - fAnchor] = point2;
				std::pair<unsigned int, CPoint*> poly = make_pair(I - fAnchor + 1, points);
				polylist.push_back(poly);
			}
			else
			{
				CPoint* points = new CPoint[I - fAnchor];
				memcpy(points, pPoints + fAnchor - 1, (I - fAnchor) * sizeof(CPoint));
				points[0] = fPoint;
				points[I - fAnchor - 1] = point1;
				std::pair<unsigned int, CPoint*> poly = make_pair(I - fAnchor, points);
				polylist.push_back(poly);
			}
		}
		else if(I == nAnchors)
		{
			CPoint* points = new CPoint[I - fAnchor + 1];
			memcpy(points, pPoints + fAnchor - 1, (I - fAnchor + 1) * sizeof(CPoint));
			points[0] = fPoint;
			std::pair<unsigned int, CPoint*> poly = make_pair(I - fAnchor + 1, points);
			polylist.push_back(poly);
		}

		bPrevInside = bCurtInside;
	}

	return polylist;
}

int compare(const void* p1, const void* p2)
{
	const struct TileIntersect* sector1 = (struct TileIntersect*)p1;
	const struct TileIntersect* sector2 = (struct TileIntersect*)p2;

	if(sector1->distance < sector2->distance)
		return -1;
	else if(sector1->distance > sector2->distance)
		return 1;
	else
		return 0;
}

bool InsideSameCell(int prevCol, int prevRow, const CPoint& point, int cxTile, int cyTile)
{
	if(point.x % cxTile == 0)
		return false;
	else if(point.y % cyTile == 0)
		return false;
	else if(point.x / cxTile != prevCol)
		return false;
	else if(point.y / cyTile != prevRow)
		return false;
	else
		return true;
}
std::list<std::tuple<int, int, CGeom*>> CPoly::Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const
{
	if(m_pPoints == nullptr)
		return std::list<std::tuple<int, int, CGeom*>>{};
	if(m_nAnchors <= 1)
		return std::list<std::tuple<int, int, CGeom*>>{};

	int minX = m_Rect.left / cxTile;
	int maxX = m_Rect.right / cxTile;
	int minY = m_Rect.top / cyTile;
	int maxY = m_Rect.bottom / cyTile;
	maxX = m_Rect.right % cxTile == 0 ? maxX - 1 : maxX;
	maxY = m_Rect.bottom % cyTile == 0 ? maxY - 1 : maxY;
	if(maxX <= minX && maxY <= minY)
		return std::list<std::tuple<int, int, CGeom*>>{make_tuple(minX, minY, this->Clone())};

	auto getSubvector = [](std::vector<CPoint> points, int start, int end)
	{
		std::vector<CPoint> result;
		for(int index = start; index <= min(points.size() - 1, end); index++)
		{
			result.push_back(points[index]);
		}
		return result;
	};
	std::vector<CPoint> points;
	points.push_back(m_pPoints[0]);
	CPoint point1 = m_pPoints[0];
	for(unsigned int anchor = 1; anchor < m_nAnchors; anchor++)
	{
		const CPoint point2 = m_pPoints[anchor];
		int col1 = point1.x / cxTile;
		int row1 = point1.y / cyTile;
		int col2 = point2.x / cxTile;
		int row2 = point2.y / cyTile;
		std::vector<struct TileIntersect> intersects;
		for(int col = min(col1, col2); col <= max(col1, col2); col++)
		{
			if(col * 256 <= min(point1.x, point2.x))
				continue;
			if(col * 256 >= max(point1.x, point2.x))
				continue;

			struct TileIntersect inter;
			inter.x = col * cxTile;
			inter.y = point1.y + round((double)(point2.y - point1.y) * (inter.x - point1.x) / (point2.x - point1.x));
			inter.distance = sqrt((inter.x - point1.x) * (inter.x - point1.x) + (inter.y - point1.y) * (inter.y - point1.y));
			intersects.push_back(inter);
		}
		for(int row = min(row1, row2); row <= max(row1, row2); row++)
		{
			if(row * 256 <= min(point1.y, point2.y))
				continue;
			if(row * 256 >= max(point1.y, point2.y))
				continue;

			struct TileIntersect inter;
			inter.y = row * cyTile;
			inter.x = point1.x + round((double)(point2.x - point1.x) * (inter.y - point1.y) / (point2.y - point1.y));
			inter.distance = sqrt((inter.x - point1.x) * (inter.x - point1.x) + (inter.y - point1.y) * (inter.y - point1.y));
			intersects.push_back(inter);
		}

		if(intersects.size() > 1)
		{
			std::qsort((void*)&intersects[0], intersects.size(), sizeof(struct TileIntersect), compare);
		}
		if(intersects.size() > 0)
		{
			for(std::vector<struct TileIntersect>::iterator it = intersects.begin(); it != intersects.end(); ++it)
			{
				points.push_back(CPoint(it->x, it->y));
			}
		}
		points.push_back(point2);
		point1 = point2;
	}
	int prevCol = points[0].x / cxTile;
	int prevRow = points[0].y / cyTile;
	bool prevOnFenceCol = points[0].x % cxTile == 0;
	bool prevOnFenceRow = points[0].y % cyTile == 0;
	unsigned int current = 0;
	unsigned int prevAnchor = 0;
	int sequence = 0;
	std::list<TiledSegment> segments;
	while(current < (points.size() - 1))
	{
		if(prevOnFenceCol == false && prevOnFenceRow == false)
		{
			while(current + 1 < points.size() && InsideSameCell(prevCol, prevRow, points[current + 1], cxTile, cyTile))
				current++;

			TiledSegment segment = {prevRow, prevCol, 0x00, getSubvector(points, prevAnchor, current + 1), sequence++};
			segments.push_back(segment);

			prevAnchor = ++current;
		}
		else if(prevOnFenceCol == true && prevOnFenceRow == true)
		{
			int col = points[current + 1].x / cxTile;
			int row = points[current + 1].y / cyTile;
			int onFenceCol = points[current + 1].x % cxTile == 0;
			int onFenceRow = points[current + 1].y % cyTile == 0;
			if(onFenceCol == true && onFenceRow == true)
			{
				TiledSegment segment = {min(prevRow, row), min(prevCol, col), 0x00, getSubvector(points, prevAnchor, current + 1), sequence++};
				segments.push_back(segment);

				prevAnchor = ++current;
			}
			else//let next loop to process the current point
			{
				prevCol = col;
				prevRow = row;
				prevOnFenceCol = onFenceCol;
				prevOnFenceRow = onFenceRow;
				continue;
			}
		}
		else if(prevOnFenceCol == true)
		{
			while(current + 1 < points.size() && points[current + 1].x % cxTile == 0 && points[current + 1].x / cxTile == prevCol && abs(points[current].y / cyTile - prevRow) <= 1)
				current++;

			if((current - prevAnchor) >= 1)
			{
				if(prevCol != 0)
				{
					TiledSegment segment = {prevRow, prevCol - 1, 0x00, getSubvector(points, prevAnchor, current), sequence};
					segments.push_back(segment);
				}
				if(prevCol != maxCol)
				{
					TiledSegment segment = {prevRow, prevCol, 0x00, getSubvector(points, prevAnchor, current), sequence};
					segments.push_back(segment);
				}
				sequence++;
				prevAnchor = current;
			}
			else if(points[current + 1].x % cxTile == 0 || points[current + 1].y % cyTile == 0)
			{
				int col = points[current + 1].x / cxTile;
				int row = points[current + 1].y / cyTile;
				TiledSegment segment = {min(prevRow, row), min(prevCol, col), 0x00, getSubvector(points, prevAnchor, current + 1), sequence++};
				segments.push_back(segment);
				prevAnchor = ++current;
			}
			else
			{
				prevCol = points[current + 1].x / cxTile;
				prevRow = points[current + 1].y / cyTile;
				prevOnFenceCol = false;
				prevOnFenceRow = false;
				continue;
			}
		}
		else if(prevOnFenceRow == true)
		{
			while(current + 1 < points.size() && points[current + 1].y % cyTile == 0 && points[current + 1].y / cyTile == prevRow && abs(points[current].x / cxTile - prevCol) <= 1)
				current++;

			if((current - prevAnchor) >= 1)
			{
				if(prevRow != 0)
				{
					TiledSegment segment = {prevRow - 1, prevCol, 0x00, getSubvector(points, prevAnchor, current), sequence};
					segments.push_back(segment);
				}
				if(prevRow != maxRow)
				{
					TiledSegment segment = {prevRow, prevCol, 0x00, getSubvector(points, prevAnchor, current), sequence};
					segments.push_back(segment);
				}
				sequence++;
				prevAnchor = current;
			}
			else if(points[current + 1].x % cxTile == 0 || points[current + 1].y % cyTile == 0)
			{
				int col = points[current + 1].x / cxTile;
				int row = points[current + 1].y / cyTile;
				TiledSegment segment = {min(prevRow, row), min(prevCol, col), 0x00, getSubvector(points, prevAnchor, current + 1), sequence++};
				segments.push_back(segment);
				prevAnchor = ++current;
			}
			else
			{
				prevCol = points[current + 1].x / cxTile;
				prevRow = points[current + 1].y / cyTile;
				prevOnFenceCol = false;
				prevOnFenceRow = false;
				continue;
			}
		}
		if(prevAnchor < points.size())
		{
			prevCol = points[prevAnchor].x / cxTile;
			prevRow = points[prevAnchor].y / cyTile;
			prevOnFenceCol = points[prevAnchor].x % cxTile == 0;
			prevOnFenceRow = points[prevAnchor].y % cyTile == 0;
		}
	}

	{
		BYTE prevCol = 0X00;
		BYTE prevRow = 0X00;
		WORD prevSeq = 0XFFFF;
		for(auto it = segments.begin(); it != segments.end(); it++)//calculate the prev and next for each segment
		{
			if(it != segments.begin())
			{
				if(it->col == prevCol + 1)
					it->prevnext |= 0X10;
				else if(it->col == prevCol - 1)
					it->prevnext |= 0X20;

				if(it->row == prevRow + 1)
					it->prevnext |= 0X40;
				else if(it->row == prevRow - 1)
					it->prevnext |= 0X80;
			}
			if(it->sequence != prevSeq)
			{
				prevCol = it->col;
				prevRow = it->row;
				prevSeq = it->sequence;
			}
		}
	}
	{
		BYTE nextCol = 0X00;
		BYTE nextRow = 0X00;
		WORD nextSeq = 0XFFFF;
		for(auto it = segments.rbegin(); it != segments.rend(); it++)//calculate the prev and next for each segment
		{
			if(it != segments.rbegin())
			{
				if(it->col == nextCol + 1)
					it->prevnext |= 0X01;
				else if(it->col == nextCol - 1)
					it->prevnext |= 0X02;

				if(it->row == nextRow + 1)
					it->prevnext |= 0X04;
				else if(it->row == nextRow - 1)
					it->prevnext |= 0X08;
			}
			if(it->sequence != nextSeq)
			{
				nextCol = it->col;
				nextRow = it->row;
				nextSeq = it->sequence;
			}
		}
	}

	std::map<std::pair<int, int>, std::list<std::vector<CPoint>*>> cellpolylines;
	for(auto& it : segments)
	{
		std::pair<int, int> key = make_pair(it.col, it.row);
		if(cellpolylines.count(key) == 0)
		{
			cellpolylines[key] = std::list<std::vector<CPoint>*>();
			cellpolylines[key].push_back(&it.points);
		}
		else if(cellpolylines[key].back()->back() == it.points.front())
			cellpolylines[key].back()->insert(cellpolylines[key].back()->end(), ++it.points.begin(), it.points.end());
		else
			cellpolylines[key].push_back(&it.points);
	}
	for(auto& it : cellpolylines)
	{
		auto& polys = it.second;
		if(polys.size() >= 2)
		{
			auto& first = polys.front();
			auto& last = polys.back();
			if(first->front() == last->back())
			{
				last->insert(last->end(), ++first->begin(), first->end());
				polys.pop_front();
			}
		}
	}
	std::list<std::tuple<int, int, CGeom*>> cellgeoms;
	auto area = this->GetArea();
	if(m_bClosed == true && abs(area) > 0)
	{
		bool clockwise = area > 0;
		CPoint centroid = this->GetLabeloid();
		int colCenter = centroid.x / cxTile;
		int rowCenter = centroid.y / cxTile;

		std::map<std::pair<int, int>, std::list<std::vector<CPoint>>> cellpolygons;
		for(auto& it : cellpolylines)
		{
			CRect rect(CPoint(it.first.first * cxTile, it.first.second * cyTile), CSize(cxTile, cyTile));
			std::list<std::vector<CPoint>> polygons = CloseSegment(rect, it.second, clockwise);
			if(cellpolygons.count(it.first) == 0)
				cellpolygons[it.first] = {polygons};
			else
				cellpolygons[it.first].insert(cellpolygons[it.first].end(), polygons.begin(), polygons.end());
		}
		for(auto& it1 : cellpolygons)
		{
			for(auto& it2 : it1.second)
			{
				CPoly* polygon = new CPoly(it2, true);
				CGeom::CopyTo(polygon);
				delete polygon->m_pLine;
				polygon->m_Rect.SetRectEmpty();

				polygon->m_pLine = new CLineEmpty();
				cellgeoms.push_back(make_tuple(it1.first.first, it1.first.second, polygon));
				if(it1.first.first != colCenter && it1.first.second != rowCenter)
				{
					polygon->m_strName = _T("");
					polygon->m_geoCode = _T("");
				}
			}
		}
		for(int col = minX; col <= maxX; col++)
		{
			for(int row = minY; row <= maxY; row++)
			{
				std::pair<int, int> key = make_pair(col, row);
				if(cellpolygons.count(key) == 0)
				{
					CPoint center = CPoint(col * cxTile + cxTile / 2, row * cyTile + cyTile / 2);
					if(this->PickIn(center) == true)
					{
						CPRect* pPRect = new CPRect();
						CGeom::CopyTo(pPRect, true);
						pPRect->m_Rect = CRect(col * cxTile, row * cyTile, col * cxTile + cxTile, row * cyTile + cyTile);
						delete pPRect->m_pLine;
						pPRect->m_pLine = new CLineEmpty();
						cellgeoms.push_back(make_tuple(col, row, pPRect));
						if(col != colCenter && row != rowCenter)
						{
							pPRect->m_strName = _T("");
							pPRect->m_geoCode = _T("");
						}
					}
				}
			}
		}
	}

	bStroke |= (m_pLine != nullptr);
	if(bStroke)
	{
		for(const auto& it : segments)
		{
			CTiledPoly* polyline = new CTiledPoly(it);
			CGeom::CopyTo(polyline);
			polyline->m_bClosed = false;
			if(CGeom::m_bClosed == true)
				polyline->m_strName = "";
			cellgeoms.push_back(make_tuple(it.col, it.row, polyline));
		}
	}

	cellpolylines.clear();
	segments.clear();
	return cellgeoms;
}
std::list<std::vector<CPoint>> CPoly::CloseSegment(const CRect& clipper, std::list<std::vector<CPoint>*> polylist, bool clockwise)
{
	RectangleWalker<long> walker(clipper.left, clipper.top, clipper.right, clipper.bottom);
	std::list<Crossing<std::vector<CPoint>>*> crosses;
	for(std::list<std::vector<CPoint>*>::iterator it = polylist.begin(); it != polylist.end(); it++)
	{
		std::vector<CPoint>* polyline = *it;

		auto cross1 = new Crossing<std::vector<CPoint>>();
		cross1->vertex = polyline;
		cross1->isStart = true;
		crosses.emplace_back(cross1);

		auto cross2 = new Crossing<std::vector<CPoint>>();
		cross2->vertex = polyline;
		cross2->isStart = false;
		crosses.emplace_back(cross2);

		cross1->other = cross2;
		cross2->other = cross1;
	}
	crosses.sort([&](const Crossing<std::vector<CPoint>>* a, const Crossing<std::vector<CPoint>>* b)->bool {
		CPoint ap = a->isStart ? a->vertex->front() : a->vertex->back();
		CPoint bp = b->isStart ? b->vertex->front() : b->vertex->back();
		double advalue = walker.CalculateD(ap.x, ap.y);
		double bdvalue = walker.CalculateD(bp.x, bp.y);
		return advalue < bdvalue;
	});
	std::list<std::vector<CPoint>> polygonlist;
	do
	{
		auto it = std::find_if(crosses.begin(), crosses.end(), [](Crossing<std::vector<CPoint>>* cross)->bool {
			return cross->visited == false && cross->isStart == true;
		});
		if(it == crosses.end())
			break;

		auto cross = *it;
		std::vector<CPoint> pointlist;
		auto callback = [&](long x, long y) {
			pointlist.push_back(CPoint(x, y));
		};
		do
		{
			cross->visited = true;
			std::vector<CPoint>* polyline = cross->vertex;
			pointlist.insert(pointlist.end(), polyline->begin(), polyline->end());

			cross = cross->other;
			cross->visited = true;
			CPoint p1 = cross->vertex->back();
			float d1 = walker.CalculateD(p1.x, p1.y);
			cross = walker.getNext(crosses, cross, clockwise);
			while(cross->isStart == false) {
				cross = walker.getNext(crosses, cross, clockwise);
			}
			CPoint p2 = cross->vertex->front();
			float d2 = walker.CalculateD(p2.x, p2.y);
			walker.interplate(d1, d2, clockwise, callback);
		} while(cross->visited == false);

		polygonlist.push_back(pointlist);
	} while(true);

	for(auto cross : crosses)
	{
		delete cross;
	}
	crosses.clear();
	return polygonlist;
}
void CPoly::GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const
{
	Clipper2Lib::Path64 path(m_nAnchors);
	for(int index = 0; index < m_nAnchors; index++)
	{
		path[index].x = m_pPoints[index].x;
		path[index].y = m_pPoints[index].y;
	}
	paths64.push_back(path);
}