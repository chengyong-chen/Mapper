#include "stdafx.h"
#include <stdio.h>

#include "POU.h"
#include "POUList.h"
#include "WhereDlg.h"
#include "FieldsPicker.h"
#include "Poupin.h"

#include "../Public/Function.h"
#include "../Public/ODBCDatabase.h"

#include "../Geometry/Global.h"
#include "../Geometry/Text.h"
#include "../Geometry/Tag.h"
#include "../Style/Spot.h"
#include "../Style/SpotCtrl.h"
#include "../Style/Hint.h"
#include "../Style/HintDlg.h"
#include "../Dataview/viewinfo.h"
#include "../Pbf/writer.hpp"
#include "../Utility/Constants.hpp"

#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

CPOU::CPOU(CPOUList& poulist)
	: m_poulist(poulist)
{
	m_dwId = 0;
	m_strName = _T("Pou");
	m_bVisible = TRUE;
	m_bShowTag = TRUE;

	m_pSpot = nullptr;
	m_pHint = nullptr;

	m_minLevelTag = 0;
	m_maxLevelTag = 25;
}

CPOU::~CPOU()
{
	delete m_pHint;
	delete m_pSpot;

	Clear();
}

void CPOU::Clear()
{
	POSITION pos = m_Pinlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CPoupin* pPin = m_Pinlist.GetNext(pos);
		delete pPin;
		pPin = nullptr;
	}
	m_Pinlist.RemoveAll();

	m_ValidRect.EmptyRect();
}

void CPOU::LoadPinlist(const CRectF& geoRect)
{
	CODBCDatabase* pDatabase = m_poulist.GetDatabase(m_datasource.m_strDatabase);
	if(pDatabase==nullptr)
		return;

	const CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	CString strSelect;
	if(headerprofile.m_strAnnoField!=_T(""))
		strSelect.Format(_T("SELECT %s,X,Y,[%s] FROM [%s]"), headerprofile.m_strIdField, headerprofile.m_strAnnoField, headerprofile.m_strKeyTable);
	else
		strSelect.Format(_T("SELECT %s,X,Y FROM [%s]"), headerprofile.m_strIdField, headerprofile.m_strKeyTable);

	double left = geoRect.left*(constants::radianTodegree);
	double right = geoRect.right*(constants::radianTodegree);
	double top = geoRect.top*(constants::radianTodegree);
	double bottom = geoRect.bottom*(constants::radianTodegree);
	CString strSQL;
	if(headerprofile.m_strWhere.IsEmpty()==FALSE)
		strSQL.Format(_T("%s WHERE %s And X>%g And X<%g And Y>%g And Y<%g"), strSelect, headerprofile.m_strWhere, left, right, top, bottom);
	else
		strSQL.Format(_T("%s WHERE X>%g And X<%g And Y>%g And Y<%g"), strSelect, left, right, top, bottom);
	CODBCDatabase::ParseSQL(pDatabase, strSQL);

	this->Clear();
	try
	{
		CRecordset rs(pDatabase);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CPoupin* pPin = new CPoupin();
			CString strValue;
			rs.GetFieldValue(headerprofile.m_strIdField, strValue);
			pPin->m_dwId = _tcstoul(strValue, nullptr, 10);
			rs.GetFieldValue(_T("X"), strValue);
			const double x = _tcstod(strValue, nullptr);
			rs.GetFieldValue(_T("Y"), strValue);
			const double y = _tcstod(strValue, nullptr);
			pPin->m_Point = CPointF(x*constants::degreeToradian, y*constants::degreeToradian);

			if(headerprofile.m_strAnnoField != _T(""))
			{
				CString strTag;
				rs.GetFieldValue(headerprofile.m_strAnnoField, strTag);
				strTag = strTag.Trim();
				if(strTag == _T("Null"))
					pPin->m_strNote = _T("");
				else
					pPin->m_strNote = strTag;
			}
			m_Pinlist.AddTail(pPin);

			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		//	ex->ReportError();
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}

	m_ValidRect = geoRect;
}

void CPOU::UpdatePosition(const DWORD& dwId, const CPointF& point)
{
	POSITION pos = m_Pinlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPoupin* pPin = m_Pinlist.GetNext(pos);
		if(pPin->m_dwId==dwId)
		{
			pPin->m_Point = point;
			break;
		}
	}
}

void CPOU::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CRectF& inGeoRect)
{
	if(m_bVisible==false)
		return;
	const CHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	if(headerprofile.m_strKeyTable.IsEmpty()==TRUE)
		return;

	if(m_ValidRect.Contains(inGeoRect)==false)
	{
		this->LoadPinlist(inGeoRect);
	}
	if(m_Pinlist.GetCount()==0)
		return;

	POSITION pos = m_Pinlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPoupin* pPin = m_Pinlist.GetNext(pos);
		const CPointF geoPoint = pPin->m_Point;
		if(inGeoRect.PtInRect(geoPoint)==FALSE)
			continue;
		const Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
		if(m_pSpot!=nullptr)
		{
			g.TranslateTransform(cliPoint.X, cliPoint.Y);
			m_pSpot->Draw(g, m_poulist.m_library, 0, 1, 1.0f, viewinfo.m_sizeDPI);
			g.TranslateTransform(-cliPoint.X, -cliPoint.Y);
		}
		else
		{
			Gdiplus::SolidBrush brush(Gdiplus::Color::DarkGray);
			g.FillEllipse(&brush, Gdiplus::Rect(cliPoint.X-5, cliPoint.Y-5, 10, 10));
		}
	}
}

void CPOU::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CRectF& geoRect)
{
	if(m_bVisible==false)
		return;
	else if(m_bShowTag==false)
		return;

	const CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	if(headerprofile.m_strAnnoField==_T(""))
		return;
	else if(m_Pinlist.GetCount()==0)
		return;

	CHint* pHint = m_pHint!=nullptr ? m_pHint : CHint::Default();
	const CRect wanRect = m_pSpot!=nullptr ? m_pSpot->GetWanRect(m_poulist.m_library) : CRect(-50000, -50000, 50000, 50000);
	const CSize sizeSymbol = CSize(wanRect.Width()*viewinfo.m_sizeDPI.cx/(10000*72.f), wanRect.Height()*viewinfo.m_sizeDPI.cy/(10000*viewinfo.m_sizeDPI.cy));

	POSITION pos = m_Pinlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPoupin* pPin = m_Pinlist.GetNext(pos);
		if(pPin->m_strNote==_T(""))
			continue;
		const CPointF geoPoint = pPin->m_Point;
		if(geoRect.PtInRect(geoPoint)==FALSE)
			continue;
		const Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
		const Gdiplus::RectF markRect(cliPoint.X-sizeSymbol.cx/2.0, cliPoint.Y-sizeSymbol.cy/2.0, sizeSymbol.cx, sizeSymbol.cy);
		Gdiplus::PointF point = CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(markRect, ANCHOR_6);
		pHint->DrawString(g, viewinfo, pPin->m_strNote, point, 0, HALIGN::HA_LEFT, VALIGN::VA_CENTER);
	}
}

int CPOU::Pick(CWnd* pWnd, CPoint& docPoint, const CViewinfo& viewinfo) const
{
	if(m_Pinlist.GetCount()==0)
		return -1;
	else if(m_bVisible==false)
		return -1;
	const CRect wanRect = m_pSpot!=nullptr ? m_pSpot->GetWanRect(m_poulist.m_library) : CRect(-50000, -50000, 50000, 50000);
	const CSize sizeSymbol = CSize(wanRect.Width()*viewinfo.m_sizeDPI.cx/(10000*72.f), wanRect.Height()*viewinfo.m_sizeDPI.cy/(10000*72.f));
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);

	POSITION pos = m_Pinlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPoupin* pPin = m_Pinlist.GetNext(pos);
		const Gdiplus::PointF point = viewinfo.WGS84ToClient(pPin->m_Point);
		Gdiplus::Rect markRect(point.X-sizeSymbol.cx/2.0, point.Y-sizeSymbol.cy/2.0, sizeSymbol.cx, sizeSymbol.cy);
		if(markRect.Contains(cliPoint)==TRUE)
		{
			return pPin->m_dwId;
		}
	}
	return -1;
}

void CPOU::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_dwId;
		ar<<m_strName;

		ar<<m_bVisible;
		ar<<m_bShowTag;
		ar<<m_minLevelTag;
		ar<<m_maxLevelTag;
		const BYTE typeSpot = m_pSpot==nullptr ? 0XFF : m_pSpot->Gettype();
		const BYTE typeHint = m_pHint==nullptr ? 0X00 : 0X01;
		ar<<typeSpot;
		ar<<typeHint;
	}
	else
	{
		ar>>m_dwId;
		ar>>m_strName;
		ar>>m_bVisible;
		ar>>m_bShowTag;
		ar>>m_minLevelTag;
		ar>>m_maxLevelTag;

		delete m_pSpot;
		delete m_pHint;

		BYTE typeSpot;
		BYTE typeHint;
		ar>>typeSpot;
		ar>>typeHint;
		m_pSpot = CSpot::Apply(typeSpot);
		m_pHint = typeHint==0x01 ? new CHint : nullptr;
	}

	m_datasource.Serialize(ar, dwVersion);
	if(m_pSpot!=nullptr)
		m_pSpot->Serialize(ar, dwVersion);
	if(m_pHint!=nullptr)
		m_pHint->Serialize(ar, dwVersion);
}

void CPOU::ReleaseWeb(CFile& file, const CDatainfo& datainfo)
{
	DWORD dwId = m_dwId;
	ReverseOrder(dwId);
	file.Write(&dwId, sizeof(DWORD));
	const CRectF mapRect = datainfo.GetMapRect();
	double minimumRatioTag = CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_minLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	double maximumRatioTag = CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_maxLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	minimumRatioTag = ReverseOrder(minimumRatioTag);
	maximumRatioTag = ReverseOrder(maximumRatioTag);
	file.Write(&minimumRatioTag, sizeof(double));
	file.Write(&maximumRatioTag, sizeof(double));
	const bool bVisible = m_bVisible;
	const bool bShowTag = m_bShowTag;
	file.Write(&bVisible, sizeof(BYTE));
	file.Write(&bShowTag, sizeof(BYTE));

	const CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	SaveAsUTF8(file, m_strName);
	SaveAsUTF8(file, headerprofile.m_strKeyTable);
	SaveAsUTF8(file, headerprofile.m_strWhere);
	SaveAsUTF8(file, headerprofile.m_strAnnoField);

	const BYTE type1 = m_pSpot==nullptr ? -1 : m_pSpot->Gettype();
	const BYTE type2 = m_pHint==nullptr ? -1 : 01;
	file.Write(&type1, sizeof(BYTE));
	file.Write(&type2, sizeof(BYTE));

	if(m_pSpot!=nullptr)
	{
		m_pSpot->ReleaseWeb(file);
	}
	if(m_pHint!=nullptr)
	{
		m_pHint->ReleaseWeb(file);
	}
}
void CPOU::ReleaseWeb(BinaryStream& stream, const CDatainfo& datainfo)
{
	stream<<m_dwId;
	const CRectF mapRect = datainfo.GetMapRect();
	double minimumRatioTag = CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_minLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	double maximumRatioTag = CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_maxLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	minimumRatioTag = ReverseOrder(minimumRatioTag);
	maximumRatioTag = ReverseOrder(maximumRatioTag);
	stream<<minimumRatioTag;
	stream<<maximumRatioTag;
	stream<<m_bVisible;
	stream<<m_bShowTag;

	const CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	SaveAsUTF8(stream, m_strName);
	SaveAsUTF8(stream, headerprofile.m_strKeyTable);
	SaveAsUTF8(stream, headerprofile.m_strWhere);
	SaveAsUTF8(stream, headerprofile.m_strAnnoField);
	const BYTE type1 = m_pSpot==nullptr ? -1 : m_pSpot->Gettype();
	const BYTE type2 = m_pHint==nullptr ? -1 : 01;
	stream<<type1;
	stream<<type2;

	if(m_pSpot!=nullptr)
	{
		m_pSpot->ReleaseWeb(stream);
	}
	if(m_pHint!=nullptr)
	{
		m_pHint->ReleaseWeb(stream);
	}
}

void CPOU::ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo)
{
	boost::json::object child;
	child["PID"] = m_dwId;
	const CRectF mapRect = datainfo.GetMapRect();
	double minimumRatioTag = CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_minLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	double maximumRatioTag = CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_maxLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	minimumRatioTag = ReverseOrder(minimumRatioTag);
	maximumRatioTag = ReverseOrder(maximumRatioTag);
	child["minRatioTag"] = minimumRatioTag;
	child["maxRatioTag"] = maximumRatioTag;
	const bool bVisible = m_bVisible;
	const bool bShowTag = m_bShowTag;
	child["Visible"] = bVisible;
	child["ShowTag"] = bShowTag;

	const CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();

	SaveAsEscapedASCII(child, "Name", m_strName);
	SaveAsEscapedASCII(child, "KeyTable", headerprofile.m_strKeyTable);
	SaveAsEscapedASCII(child, "Where", headerprofile.m_strWhere);
	SaveAsEscapedASCII(child, "AnnField", headerprofile.m_strAnnoField);

	if(m_pSpot != nullptr)
	{
		boost::json::object child1;
		const BYTE type = m_pSpot->Gettype();
		child1["Type"] = type;
		m_pSpot->ReleaseWeb(child1);
		json["Spot"] = child1;
	}
	if(m_pHint != nullptr)
	{
		boost::json::object child1;		
		m_pHint->ReleaseWeb(child1);
		json["Hint"] = child1;
	}
	json["POU"] = child;
}

void CPOU::ReleaseWeb(pbf::writer& writer, const CDatainfo& datainfo) const
{
	std::string cdata;
	pbf::writer child(cdata);

	child.add_variant_uint32(m_dwId);

	const CRectF mapRect=datainfo.GetMapRect();
	double minimumRatioTag=CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_minLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	double maximumRatioTag=CDatainfo::GetMapToViewFactorFromScale(datainfo.m_pProjection, m_maxLevelTag, mapRect.CenterPoint(), CSize(72, 72));
	minimumRatioTag=ReverseOrder(minimumRatioTag);
	maximumRatioTag=ReverseOrder(maximumRatioTag);
	child.add_double(minimumRatioTag);
	child.add_double(maximumRatioTag);
	const bool bVisible=m_bVisible;
	const bool bShowTag=m_bShowTag;
	child.add_bool(bVisible);
	child.add_bool(bShowTag);

	const CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();

	child.add_string(EscapedASCII(m_strName));
	child.add_string(EscapedASCII(headerprofile.m_strKeyTable));
	child.add_string(EscapedASCII(headerprofile.m_strWhere));
	child.add_string(EscapedASCII(headerprofile.m_strAnnoField));

	if(m_pSpot != nullptr)
	{
		std::string cdata1;
		pbf::writer child1(cdata1);

		const BYTE type=m_pSpot->Gettype();
		child1.add_byte(type);
		m_pSpot->ReleaseWeb(child1);
	//	child.add_block(cdata1);
	}
	if(m_pHint != nullptr)
	{
		std::string cdata1;
		pbf::writer child1(cdata1);
		m_pHint->ReleaseWeb(child1);
	//	child.add_block(pou::pbftag::hint, cdata1, cdata.length);
	}

//	writer.add_block(cdata);
}
bool CPOU::SetSpot(CDocument* pDocument)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CSpotCtrl dlg(nullptr, true, m_poulist.m_library, m_pSpot);
	if(dlg.DoModal()==IDOK)
	{
		if(m_pSpot!=nullptr)
		{
			m_pSpot->Decrease(m_poulist.m_library);
			delete m_pSpot;
			m_pSpot = nullptr;
		}

		m_pSpot = dlg.d_pSpot;
		dlg.d_pSpot = nullptr;

		if(pDocument!=nullptr)
		{
			pDocument->SetModifiedFlag(TRUE);
			AfxGetMainWnd()->PostMessage(WM_CROSSTHREAD, 1, (UINT)pDocument);//==pDocument->UpdateAllViews(nullptr);
		}
		else
		{
			CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
			CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
			if(pChildFrame!=nullptr)
			{
				CView* pView = pChildFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pView->Invalidate();
				}
			}
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	return true;
}

bool CPOU::SetHint(CDocument* pDocument)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CHint* pHint = m_pHint!=nullptr ? m_pHint->Clone() : new CHint();
	CHintDlg dlg(nullptr, *pHint);
	if(dlg.DoModal()==IDOK)
	{
		delete m_pHint;
		m_pHint = pHint;

		if(pDocument!=nullptr)
		{
			pDocument->SetModifiedFlag(TRUE);
			AfxGetMainWnd()->PostMessage(WM_CROSSTHREAD, 1, (UINT)pDocument);//==pDocument->UpdateAllViews(nullptr);
		}
		else
		{
			CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
			CFrameWnd* pChildFrame = pMainFrame->GetActiveFrame();
			if(pChildFrame!=nullptr)
			{
				CView* pView = pChildFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pView->Invalidate();
				}
			}
			else
			{
				pChildFrame->Invalidate();
			}
		}
	}
	else
	{
		delete pHint;
		pHint = nullptr;
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	return true;
}

bool CPOU::SetWhere(CDocument* pDocument)
{
	CODBCDatabase* pDatabase = m_poulist.GetDatabase(m_datasource.m_strDatabase);
	if(pDatabase==nullptr)
		return false;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	CWhereDlg dlg(nullptr, pDatabase, headerprofile.m_strKeyTable, headerprofile.m_strWhere);
	if(dlg.DoModal()==IDOK)
	{
		this->Clear();

		if(headerprofile.m_strKeyTable!=dlg.m_strTable)
		{
			headerprofile.m_strAnnoField.Empty();
		}

		headerprofile.m_strKeyTable = dlg.m_strTable;
		headerprofile.m_strWhere = dlg.m_strWhere;

		if(headerprofile.m_strKeyTable.IsEmpty()==TRUE)
		{
			headerprofile.m_strWhere.Empty();
			headerprofile.m_strAnnoField.Empty();
		}

		if(pDocument!=nullptr)
		{
			pDocument->SetModifiedFlag(TRUE);
			AfxGetMainWnd()->PostMessage(WM_CROSSTHREAD, 1, (UINT)pDocument);//==pDocument->UpdateAllViews(nullptr);
		}
		else
		{
			CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
			CFrameWnd* pChildFrame = pMainFrame->GetActiveFrame();
			if(pChildFrame!=nullptr)
			{
				CView* pView = pChildFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pView->Invalidate();
				}
			}
			else
			{
				pChildFrame->Invalidate();
			}
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	return true;;
}

bool CPOU::SetTag(CDocument* pDocument)
{
	CODBCDatabase* pDatabase = m_poulist.GetDatabase(m_datasource.m_strDatabase);
	if(pDatabase==nullptr)
		return false;

	CPOUHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	CFieldsPicker dlg(nullptr, pDatabase, headerprofile.m_strKeyTable, headerprofile.m_strAnnoField, m_minLevelTag, m_maxLevelTag);
	if(dlg.DoModal()==IDOK)
	{
		headerprofile.m_strAnnoField = dlg.m_strField;
		m_minLevelTag = dlg.m_minimumScale;
		m_maxLevelTag = dlg.m_maximumScale;

		if(pDocument!=nullptr)
		{
			pDocument->SetModifiedFlag(TRUE);
			AfxGetMainWnd()->PostMessage(WM_CROSSTHREAD, 1, (UINT)pDocument);//==pDocument->UpdateAllViews(nullptr);
		}
		else
		{
			CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
			CFrameWnd* pChildFrame = pMainFrame->GetActiveFrame();
			if(pChildFrame!=nullptr)
			{
				CView* pView = pChildFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pView->Invalidate();
				}
			}
			else
			{
				pChildFrame->Invalidate();
			}
		}
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return true;;
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return false;
	}
}

void CPOU::Invalidate(CDocument* pDocument) const
{
	ASSERT_VALID(this);

	if(pDocument!=nullptr)
	{
		pDocument->UpdateAllViews(nullptr);
	}
	else
	{
		CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
		CFrameWnd* pChildFrame = pMainFrame->GetActiveFrame();
		if(pChildFrame!=nullptr)
		{
			CView* pView = pChildFrame->GetActiveView();
			if(pView!=nullptr)
			{
				pView->Invalidate();
			}
		}
		else
		{
			CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
			CFrameWnd* pChildFrame = pMainFrame->GetActiveFrame();
			if(pChildFrame!=nullptr)
			{
				CView* pView = pChildFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pView->Invalidate();
				}
			}
			else
			{
				pChildFrame->Invalidate();
			}
		}
	}
}
