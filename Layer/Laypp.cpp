#include "stdafx.h"
#include <io.h>
#include <cstdio>
#include <stdlib.h>

#include "Laypp.h"
#include "LayerTree.h"

#include "../Style/Hint.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Tag.h"
#include "../Geometry/Global.h"
#include "../Geometry/DynamicTag.h"

#include "../Public/Function.h"
#include "../Public/ODBCDatabase.h"
#include "../Database/ODBCRecordset.h"
#include "../Dataview/viewinfo.h"

extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLaypp::CMarker::CMarker()
	: CMark()
{
	//	memcpy(m_strType, _T("0000"),4);
	m_strType = _T("0000");
}

CLaypp::CMarker::CMarker(const CPoint& origin)
	: CMark(origin)
{
	//	memcpy(m_strType, _T("0000"),4);
	m_strType = _T("0000");
}

CLaypp::CMarker::~CMarker()
{
}

bool CLaypp::CMarker::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	CMark::GetValues(rs, datainfo, strTag);

	try
	{
		m_strType = rs(_T("Type")).AsString();
		return true;
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return false;
}

CLaypp::CLaypp(CTree<CLayer>& layertree)
	: CLaypt(layertree)
{
	m_nIconCount = 0;
	m_nIconHeight = 0;
	m_Bitmap = nullptr;

	m_strIconFile = _T("c:\\1.bmp");
	m_tableType = _T("POI_Type3");
}

CLaypp::CLaypp(CTree<CLayer>& layertree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: CLaypt(layertree, minLevelObj, maxLevelObj, minLevelTag, maxLevelTag)
{
	m_nIconCount = 0;
	m_nIconHeight = 0;
	m_Bitmap = nullptr;

	m_strIconFile = _T("c:\\1.bmp");
	m_tableType = _T("POI_Type3");
}

CLaypp::~CLaypp()
{
	if(m_Iconlist.m_hImageList!=nullptr)
	{
		m_Iconlist.DeleteImageList();
		m_Iconlist.Detach();
	}

	delete m_Bitmap;
	m_Bitmap = nullptr;

	m_mapTypeIcon.empty();
}

void CLaypp::Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms)
{
	if(ar.IsStoring())
	{
		CString strIconFile = m_strIconFile;
		AfxToRelatedPath(ar.m_strFileName, strIconFile);

		ar<<m_tableType;
		ar<<strIconFile;
	}
	else
	{
		ar>>m_tableType;
		ar>>m_strIconFile;
		AfxToFullPath(ar.m_strFileName, m_strIconFile);
		LoadIcons(m_strIconFile);
	}

	CLaypt::Serialize(ar, dwVersion, bIgnoreGeoms);
}

BOOL CLaypp::LoadIcons(CString strIconFile)
{
	if(_taccess(strIconFile, 00)==-1)
		return FALSE;

	HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), strIconFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(hBitmap==nullptr)
		return FALSE;
	const _bstr_t bstrIconFile(m_strIconFile);
	m_Bitmap = Gdiplus::Bitmap::FromFile(bstrIconFile);

	BITMAP bm;
	if(::GetObject(hBitmap, sizeof(BITMAP), &bm)==0)
	{
		DeleteObject(hBitmap);
		return FALSE;
	}

	if(bm.bmHeight>bm.bmWidth)
	{
		AfxMessageBox(_T("The width must be bigger than height for Patterns!"));
		DeleteObject(hBitmap);
		return FALSE;
	}

	if(m_Iconlist.m_hImageList!=nullptr)
	{
		m_Iconlist.DeleteImageList();
		m_Iconlist.Detach();
	}

	CBitmap bitmap;
	if(bitmap.Attach(hBitmap)==TRUE)
	{
		m_nIconHeight = bm.bmHeight;
		m_nIconCount = bm.bmWidth/bm.bmHeight;

		if(m_Iconlist.Create(m_nIconHeight, m_nIconHeight, ILC_COLORDDB|ILC_MASK, m_nIconCount+1, 0)==TRUE)
		{
			m_Iconlist.Add(&bitmap, RGB(192, 192, 192));

			m_strIconFile = strIconFile;

			bitmap.Detach();
			if(hBitmap!=nullptr)
			{
				DeleteObject(hBitmap);
				hBitmap = nullptr;
			}
			return TRUE;
		}
		else
		{
			AfxMessageBox(_T("Failed to create the pattern check the file's format please!"));
		}

		bitmap.Detach();
	}
	if(hBitmap!=nullptr)
	{
		DeleteObject(hBitmap);
		hBitmap = nullptr;
	}
	return FALSE;
}

void CLaypp::Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect)
{
	CLaydb::Reload(pWnd, datainfo, viewinfo, mapRect);

	if(m_bVisible==false)
		return;

	if(mapRect==m_lastRect)
		return;

	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase==nullptr)
		return;
	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent< m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	const bool bDrawTag = (m_bShowTag == false || viewinfo.m_levelCurrent<m_minLevelTag || viewinfo.m_levelCurrent >= m_maxLevelTag) ? false : true;
	if(bDrawGeom==false&&bDrawTag==false)
		return;

	CString strRange;
	strRange.Format(_T("A.X<=%g And A.X>=%g And A.Y<=%g And A.Y>=%g"), mapRect.right, mapRect.left, mapRect.bottom, mapRect.top);
	CString strSQL = m_geoDatasource.GetHeaderProfile().MakeSelectStatment()+strRange;
	try
	{
		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CLaypp::CMarker* pMarker = new CLaypp::CMarker();
			pMarker->GetValues(rs, datainfo, m_geoDatasource.GetHeaderProfile().m_strAnnoField);
			if(pMarker->m_strName.IsEmpty()==FALSE)
			{
				pMarker->m_pTag = new CTag(pMarker->m_strName, ANCHOR_6);
			}

			m_geomlist.AddTail(pMarker);

			rs.MoveNext();
		}
		rs.Close();

		m_lastRect = mapRect;
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	if(m_mapTypeIcon.size()==0&&m_tableType.IsEmpty()==FALSE)
	{
		try
		{
			strSQL.Format(_T("Select * From %s order by Type"), m_tableType);
			CRecordset rs(pDatabase);
			rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				CStringA strType;
				rs.GetFieldValue(_T("Type"), strType);
				CStringA strValue;
				rs.GetFieldValue(_T("Icon"), strValue);
				const int iIcon = atoi(strValue);
				m_mapTypeIcon[strType] = iIcon;

				rs.MoveNext();
			}

			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}
}

void CLaypp::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const
{
	if(m_bVisible==false)
		return;
	const bool bDrawGeom = (m_bShowGeom==false||viewinfo.m_levelCurrent < m_minLevelObj||viewinfo.m_levelCurrent>=m_maxLevelObj) ? false : true;
	const bool bDrawTag = (m_bShowTag==false||viewinfo.m_levelCurrent   < m_minLevelTag||viewinfo.m_levelCurrent>=m_maxLevelTag) ? false : true;
	if(bDrawGeom==false&&bDrawTag==false)
		return;
	const float scaleMark = (m_bDynamic&SpotDynamic)==SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	const Gdiplus::SizeF size = CViewinfo::PointsToPixels(m_nIconHeight, viewinfo.m_sizeDPI);
	const CRect iconRect = CRect(-size.Width/2, -size.Height/2, size.Width/2, size.Height/2);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		CMarker* pMarker = (CMarker*)pGeom;
		const int indexIcon = m_mapTypeIcon.find(pMarker->m_strType) != m_mapTypeIcon.end() ? m_mapTypeIcon.at(pMarker->m_strType) : 5;
		if(indexIcon==-1)
			continue;

		pMarker->m_Rect = iconRect;
		pMarker->m_Rect.OffsetRect(pMarker->m_Origin.x, pMarker->m_Origin.y);

		CRect rect = pMarker->m_Rect;
		rect.InflateRect(1, 1);
		if(rect.IntersectRect(rect, invalidRect)==FALSE)
			continue;

		if(bDrawGeom==true)
		{
			if(m_Bitmap!=nullptr)
			{
				const Gdiplus::GraphicsState state = g.Save();
				g.TranslateTransform(pMarker->m_Origin.x, pMarker->m_Origin.y);
				g.ScaleTransform(10000, -10000);
				g.ScaleTransform(scaleMark, scaleMark);

				Gdiplus::ImageAttributes imaatt;
				imaatt.SetColorKey(Gdiplus::Color(192, 192, 192), Gdiplus::Color(192, 192, 192), Gdiplus::ColorAdjustTypeDefault);

				Gdiplus::RectF destRect = Gdiplus::RectF(-m_nIconHeight/2, -m_nIconHeight/2, m_nIconHeight, m_nIconHeight);

				g.DrawImage(m_Bitmap, destRect, m_nIconHeight*indexIcon, 0, m_nIconHeight, m_nIconHeight, Gdiplus::UnitPixel, &imaatt);

				g.ScaleTransform(1/scaleMark, 1/scaleMark);
				g.ScaleTransform(0.0001, -0.0001);
				g.TranslateTransform(-pMarker->m_Origin.x, -pMarker->m_Origin.y);
				g.Restore(state);
			}

			if(viewinfo.m_bViewer==true)
			{
				Gdiplus::RectF markRect = Gdiplus::RectF(iconRect.left, iconRect.top, iconRect.Width(), iconRect.Height());
				markRect.Offset(pMarker->m_Origin.x, pMarker->m_Origin.y);
				d_oRectArray.Add(markRect);
			}
		}

		if(bDrawTag==true&&pGeom->m_strName.IsEmpty()==FALSE)
		{
			if(viewinfo.m_bViewer==true)
			{
				Gdiplus::RectF markRect = Gdiplus::RectF(iconRect.left, iconRect.top, iconRect.Width(), iconRect.Height());
				markRect.Offset(pMarker->m_Origin.x, pMarker->m_Origin.y);

				CRectTag* pTag = new CRectTag;
				pTag->m_rect = markRect;
				pTag->m_string = pMarker->m_strName;
				if(pGeom->m_pTag!=nullptr)
				{
					pTag->defAnchor = pGeom->m_pTag->GetAnchor();
				}
				m_rectTaglist.AddTail(pTag);
			}
		}
	}
}

void CLaypp::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect, const bool showCreated, const bool showDynamic) const
{
	if(m_bVisible==false)
		return;
	if(m_bShowTag==false)
		return;
	if(viewinfo.m_levelCurrent<m_minLevelTag)
		return;
	if(viewinfo.m_levelCurrent>=m_maxLevelTag)
		return;

	float scaleMark = (m_bDynamic&SpotDynamic)==SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	float scaleHint = (m_bDynamic&HintDynamic)==HintDynamic ? viewinfo.CurrentRatio() : 1.0f;
	CHint* pHint = this->GetHint()!=nullptr ? this->GetHint() : CHint::Default();

	POSITION Pos4 = m_rectTaglist.GetHeadPosition();
	while(Pos4!=nullptr)
	{
		CRectTag* pTag = m_rectTaglist.GetNext(Pos4);
		if(pTag!=nullptr)
		{
			pTag->Draw(g, viewinfo, pHint);
			delete pTag;
			pTag = nullptr;
		}
	}
	m_rectTaglist.RemoveAll();

	if(viewinfo.m_bViewer==false)
	{
		const HALIGN hAlign = HALIGN::HA_LEFT;
		const VALIGN vAlign = VALIGN::VA_CENTER;

		ANCHOR anchor = ANCHOR::ANCHOR_3;
		Gdiplus::RectF bounds;
		g.GetClipBounds(&bounds);

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			if(pGeom->HasTag()==false)
				continue;

			CRect rect = pGeom->m_Rect;
			rect.InflateRect(12*10/viewinfo.m_xFactorMapToView, 12*10/viewinfo.m_yFactorMapToView);//��������10��СΪ12P�����ֵľ��뻹������Ұ�ڵĻ����ų����
			if(rect.IntersectRect(rect, invalidRect)==FALSE)
				continue;

			Gdiplus::RectF markRect = viewinfo.DocToClient <Gdiplus::RectF>(pGeom->m_Rect);
			Gdiplus::PointF point(markRect.GetRight(), markRect.GetTop());
			Gdiplus::SizeF textSize = pHint->MeasureText<Gdiplus::SizeF>(g, viewinfo, pGeom->m_pTag->m_string, 0);
			Gdiplus::RectF textRect = ::GetTagRect(markRect, textSize, anchor, hAlign, vAlign);
			if(textRect.IntersectsWith(bounds)==FALSE)
				continue;

			pGeom->m_pTag->Draw(g, viewinfo, pHint, point);
		}
	}
}