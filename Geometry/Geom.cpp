#include "stdafx.h"
#include "Geom.h"
#include "Global.h"

#include "OleObj.h"
#include "PRect.h"
#include "Ellipse.h"
#include "Imager.h"
#include "Mark.h"
#include "RRect.h"
#include "Doughnut.h"
#include "Poly.h"
#include "Bezier.h"
#include "Text.h"
#include "TextPoly.h"
#include "Pole.h"
#include "Compound.h"
#include "PropertyDlg.h"
#include "Holder.h"

#include "Tag.h"
#include "TextEditDlg.h"
#include "StraightLine.hpp"

#include <cmath>
#include "../Database/ODBCRecordset.h"
#include "../Mapper/Global.h"
#include "../Public/BinaryStream.h"
#include "../Style/Color.h"
#include "../Style/ColorSpot.h"
#include "../Style/Line.h"
#include "../Style/Fill.h"
#include "../Style/FillCompact.h"
#include "../Style/FillComplex.h"
#include "../Style/FillLinear.h"
#include "../Style/FillRadial.h"
#include "../Style/Type.h"
#include "../Style/Hint.h"
#include "../Style/HintDlg.h"
#include "../Style/StyleDlg.h"
#include "../Style/Psboard.h"

#include "../Dataview/Global.h"
#include "../Dataview/Datainfo.h"
#include "../Projection/Projection1.h"
#include "../Rectify/Tin.h"

#include "../Action/Modify.h"
#include "../Action/ActionStack.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Geom.h"

#include "../Utility/Lnglat.hpp"
#include "../Utility/cstring.hpp"
#include "../Pbf/writer.hpp"

#include "Clip.h"
#include "Mask.h"
#include "TextGaped.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"
#include "../Public/Enumeration.h"
#include "../Public/BinaryStream.h"


#include <boost/json.hpp>
using namespace boost;
using namespace Clipper2Lib;
using namespace Holder;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ANCHOR PrioritizedAnchors[];
extern CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;
//extern CColorSpot* CColorSpot::Black;

IMPLEMENT_DYNAMIC(CGeom, CObject)

CGeom::CGeom()
{
	m_Rect.SetRectEmpty();
	m_strName.Empty();
}
CGeom::CGeom(CLine* pLine, CFillCompact* pFill)
	:m_pLine(pLine), m_pFill(pFill)
{
	m_Rect.SetRectEmpty();
	m_strName.Empty();
}
CGeom::CGeom(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: m_Rect(rect), m_pLine(pLine), m_pFill(pFill)
{
}

CGeom::~CGeom()
{
	delete m_pLine;
	delete m_pFill;
	delete m_pType;
	delete m_pHint;
	delete m_pTag;
}
BOOL CGeom::operator==(const CGeom& geom) const
{
	if(m_Rect!=geom.m_Rect)
		return FALSE;
	else if(m_strName!=geom.m_strName)
		return FALSE;
	else
		return TRUE;
}
void CGeom::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_byte(this->Gettype());
	if(m_strName.IsEmpty()==FALSE)
	{
		cstr::sha1(m_strName, sha1);
	}
}
void CGeom::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1);
}
CGeom* CGeom::Apply(BYTE type)
{
	CGeom* geom = nullptr;

	switch(type)
	{
		case 0:		//CGeom
			break;
		case 1:
			geom = new CPoly();
			break;
		case 2:
			geom = new CBezier();
			break;
		case 3:
			geom = new CImager();
			break;
		case 4:
			geom = new CDoughnut();
			break;
		case 5:
			geom = new CGroup();
			break;
		case 6:
			geom = new COleObj();
			break;
		case 7:
			geom = new CPRect();
			break;
		case 8:
			geom = new CMark();
			break;
		case 9:
			geom = new CText();
			break;
		case 10:
			geom = new CTextPoly();
			break;
		case 11:
			geom = new CTextPoly();
			break;
		case 12:
			geom = new CRRect();
			break;
		case 13:
			geom = new CEllipse();
			break;
		case 15:
			geom = new CPole();
			break;
		case 16:
			geom = new CClip();
			break;
		case 17:
			geom = new CMask();
			break;
		case 18:
			geom = new CTextGaped();
			break;
		case 19:	//CCluster
			break;
		case 20:
			geom = new CCompound();
			break;
		case 41:	//Mapper::CSubmap
			geom = new Holder::Submap();
			break;
		case 51:	//Topology::CPolygon
		case 52:	//Topology::CGonroup
			break;
		case 61:	//Topology::CPolying
		case 62:	//Topology::CIngroup
			break;
		case 101:	//Topology::CTiledPoly
		case 102:	//Topology::CTiledBezier
			break;
		default:
			ASSERT(0);
	}

	return geom;
}

void CGeom::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		unsigned short toggler = 0;
		if(m_bLocked==true)				toggler |= 0X0001;
		if(m_bClosed==true)				toggler |= 0X0002;
		if(m_attId!=0XFFFFFFFF)			toggler |= 0X0004;
		if(m_pTag!=nullptr)				toggler |= 0X0008;
		if(m_pLine!=nullptr)			toggler |= 0X0010;
		if(m_pFill!=nullptr)			toggler |= 0X0020;
		if(m_pType!=nullptr)			toggler |= 0X0040;
		if(m_pHint!=nullptr)			toggler |= 0X0080;
		if(m_strName.IsEmpty()==false)	toggler |= 0X0100;
		if(m_filter!=FilterType::None)	toggler |= 0X0200;
		if(m_geoCode.IsEmpty()==false)	toggler |= 0X0400;

		ar<<toggler;
		ar<<m_geoId;
		if(m_attId!=0XFFFFFFFF)
			ar<<m_attId;
		ar<<m_Rect;
		if(m_strName.IsEmpty()==false)
			ar<<m_strName;
		if(m_filter!=FilterType::None)
			ar<<m_filter;
		if(m_geoCode.IsEmpty()==false)
			ar<<m_geoCode;
		if(m_pLine!=nullptr)
		{
			const CLine::LINETYPE linetype = m_pLine->Gettype();
			ar<<(BYTE)linetype;
		}
		if(m_pFill!=nullptr)
		{
			const CFill::FILLTYPE filltype = m_pFill->Gettype();
			ar<<(BYTE)filltype;
		}
	}
	else
	{
		unsigned short toggler = 0;
		ar>>toggler;
		ar>>m_geoId;
		if((toggler&0X0004)==0X0004)
			ar>>m_attId;
		ar>>m_Rect;
		if((toggler&0X0100)==0X0100)
			ar>>m_strName;
		if((toggler&0X0200)==0X0200)
			ar>>m_filter;
		m_bLocked = (toggler&0X01)==0X01;
		m_bClosed = (toggler&0X02)==0X02;
		if((toggler&0X0400)==0X0400)
			ar>>m_geoCode;
		if((toggler&0X08)==0X08)
		{
			m_pTag = new CTag(m_strName, ANCHOR_5);
		}
		if((toggler&0X10)==0X10)
		{
			char lineindex;
			ar>>lineindex;
			m_pLine = CLine::Apply(lineindex);
		}
		else if(m_pLine!=nullptr)
		{
			delete m_pLine;
			m_pLine = nullptr;
		}
		if((toggler&0X20)==0X20)
		{
			char fillindex;
			ar>>fillindex;
			m_pFill = CFillCompact::Apply(fillindex);
		}
		else if(m_pFill!=nullptr)
		{
			delete m_pFill;
			m_pFill = nullptr;
		}
		if((toggler&0X40)==0X40)
		{
			m_pType = new CType;
		}
		if((toggler&0X80)==0X80)
		{
			m_pHint = new CHint;
		}
		if(m_strName==_T("Shanghai"))
		{
			int i = 0;
		}
	}

	if(m_pTag!=nullptr)
	{
		m_pTag->Serialize(ar, dwVersion);
		m_bTag = m_pTag->GetShort();
	}

	if(m_pLine!=nullptr)
		m_pLine->Serialize(ar, dwVersion);
	if(m_pFill!=nullptr)
		m_pFill->Serialize(ar, dwVersion);
	if(m_pType!=nullptr)
		m_pType->Serialize(ar, dwVersion);
	if(m_pHint!=nullptr)
		m_pHint->Serialize(ar, dwVersion);
}

void CGeom::Attribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	CPropertyDlg dlg(pWnd, m_geoId, m_attId, m_geoCode, std::nullopt);
	if(dlg.DoModal()==IDOK)
	{
		m_geoCode = dlg.m_strGeoCode;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGeom::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar<<m_bClosed;
		const int left = m_Rect.left/10000;
		const int top = m_Rect.top/10000;
		const int width = m_Rect.Width()/10000;
		const int height = m_Rect.Height()/10000;
		ar<<m_geoId;
		ar<<left;
		ar<<top;
		ar<<width;
		ar<<height;
		SerializeStrCE(ar, m_strName);
	}
	else
	{
	}

	//	if(m_pLine != nullptr)
	//		m_pLine->Serialize(ar);
	//	if(m_pFill != nullptr)
	//		m_pFill->Serialize(ar);
	//	if(m_pType != nullptr)
	//		m_pType->Serialize(ar);
}

void CGeom::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		char toggles = 0;
		if(m_bClosed==true) toggles |= 0X02;
		if(m_pLine!=nullptr) toggles |= 0X10;
		if(m_pFill!=nullptr) toggles |= 0X20;
		if(m_pType!=nullptr) toggles |= 0X40;
		//	if(m_pHint != nullptr)    toggles |= 0X40;

		if(m_strName.IsEmpty()==FALSE) toggles |= 0X80;

		ar<<m_geoId;//	ar << m_Rect; //´óÔ¼Õ¼×ÜÊý¾ÓÁ¿µÄ16%
		ar<<toggles;
		if(m_strName.IsEmpty()==FALSE)
		{
			ar<<m_strName;
		}

		if(m_pLine!=nullptr)
		{
			const CLine::LINETYPE linetype = m_pLine->Gettype();
			ar<<(BYTE)linetype;
		}
		if(m_pFill!=nullptr)
		{
			const CFill::FILLTYPE filltype = m_pFill->Gettype();
			ar<<(BYTE)filltype;
		}
	}
	else
	{
		char toggles = 0;

		ar>>m_geoId;//	ar >> m_Rect;
		ar>>toggles;

		m_bClosed = (toggles&0X02)==0X02 ? true : false;
		if((toggles&0X80)==0X80)
		{
			ar>>m_strName;
		}

		delete m_pLine;
		m_pLine = nullptr;
		if((toggles&0X10)==0X10)
		{
			char lineindex;
			ar>>lineindex;
			m_pLine = CLine::Apply(lineindex);
		}

		delete m_pFill;
		m_pFill = nullptr;
		if((toggles&0X20)==0X20)
		{
			char fillindex;
			ar>>fillindex;
			m_pFill = CFillCompact::Apply(fillindex);
		}
		/*
		if((toggles & 0X40) == 0X40)
		{
		if(m_pHint == nullptr)
		{
		m_pHint = new CHint;
		}
		}
		else if(m_pHint != nullptr)
		{
		delete m_pHint;
		m_pHint = nullptr;
		}*/
	}

	if(m_pLine!=nullptr)
		m_pLine->ReleaseDCOM(ar);
	if(m_pFill!=nullptr)
		m_pFill->ReleaseDCOM(ar);
	if(m_pHint!=nullptr)
		m_pHint->ReleaseDCOM(ar);
}

CGeom* CGeom::Load(CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	try
	{
		const short type = rs.Field(_T("Shape")).AsShort();
		CGeom* pGeom = CGeom::Apply(type);
		if(pGeom!=nullptr)
		{
			if(pGeom->GetValues(rs, datainfo, strTag)==TRUE)
			{
				return pGeom;
			}
			else
			{
				delete pGeom;
				pGeom = nullptr;
			}
		}
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
	return nullptr;
}

bool CGeom::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	try
	{
		m_geoId = rs.Field(_T("GID")).AsInt();
		m_Rect.left = datainfo.MapToDoc(rs.Field(_T("minX")).AsDouble());
		m_Rect.top = datainfo.MapToDoc(rs.Field(_T("minY")).AsDouble());
		m_Rect.right = datainfo.MapToDoc(rs.Field(_T("maxX")).AsDouble());
		m_Rect.bottom = datainfo.MapToDoc(rs.Field(_T("maxY")).AsDouble());

		if(strTag.IsEmpty()==FALSE)
		{
			m_strName = rs.Field(strTag).AsString();
			m_strName.Trim();
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

bool CGeom::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	try
	{
		const double minX = datainfo.DocToMap(m_Rect.left);
		const double minY = datainfo.DocToMap(m_Rect.top);
		const double maxX = datainfo.DocToMap(m_Rect.right);
		const double maxY = datainfo.DocToMap(m_Rect.bottom);
		rs.Field(_T("minX")) = minX;
		rs.Field(_T("minY")) = minY;
		rs.Field(_T("maxX")) = maxX;
		rs.Field(_T("maxY")) = maxY;
		rs.Field(_T("Shape")) = 7;
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->Delete();
		return false;
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return true;
}

bool CGeom::IntoDatabase(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	try
	{
		rs.AddNew();
		PutValues(rs, datainfo);
		rs.Update();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->Delete();
		return false;
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return true;
}

bool CGeom::BeforeDispose(CDatabase* pDatabase, CString strTable, const CDatainfo& datainfo)
{
	if(pDatabase==nullptr)
		return false;
	if(m_bModified==false)
		return false;

	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s Where GID=%d"), strTable, m_geoId);

		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, dbOpenDynaset);
		if(rs.IsEOF()==FALSE)
		{
			PutValues(rs, datainfo);
			rs.Update();
			rs.Close();
			m_bModified = false;
			return true;
		}
		else
		{
			rs.Close();

		}
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

DWORD CGeom::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile!=nullptr)
	{
		DWORD size = PackStrPC(m_strName, pFile);

		BYTE toggles = 0;
		if(m_bLocked==true) toggles |= 0X01;
		if(m_bClosed==true) toggles |= 0X02;
		if(m_pTag!=nullptr) toggles |= 0X08;

		if(m_pLine!=nullptr) toggles |= 0X10;
		if(m_pFill!=nullptr) toggles |= 0X20;
		if(m_pType!=nullptr) toggles |= 0X40;

		pFile->Write(&toggles, sizeof(BYTE));
		size += sizeof(BYTE);

		if(m_pTag!=nullptr)
		{
			size += m_pTag->PackPC(pFile, bytes);
		}

		if(m_pLine!=nullptr)
		{
			const CLine::LINETYPE linetype = m_pLine->Gettype();
			pFile->Write(&linetype, sizeof(BYTE));

			size += sizeof(BYTE);
		}
		if(m_pFill!=nullptr)
		{
			const CFill::FILLTYPE filltype = m_pFill->Gettype();
			pFile->Write(&filltype, sizeof(BYTE));

			size += sizeof(BYTE);
		}

		if(m_pLine!=nullptr)
		{
			size += m_pLine->PackPC(pFile, bytes);
		}
		if(m_pFill!=nullptr)
		{
			size += m_pFill->PackPC(pFile, bytes);
		}
		if(m_pType!=nullptr)
		{
			size += m_pType->PackPC(pFile, bytes);
		}

		return size;
	}
	else
	{
		m_strName = UnpackStrPC(bytes);
		const BYTE toggles = *(BYTE*)bytes;
		m_bClosed = (toggles&0X02)==0X02 ? true : false;
		m_pTag = (toggles&0X08)==0X08 ? new CTag(m_strName, ANCHOR_5) : nullptr;

		bytes += sizeof(BYTE);

		if(m_pTag!=nullptr)
		{
			m_pTag->PackPC(nullptr, bytes);
		}

		delete m_pLine;
		m_pLine = nullptr;
		if((toggles&0X10)==0X10)
		{
			const BYTE lineindex = *(BYTE*)bytes;
			m_pLine = CLine::Apply(lineindex);

			bytes += sizeof(BYTE);
		}

		delete m_pFill;
		m_pFill = nullptr;
		if((toggles&0X20)==0X20)
		{
			const BYTE fillindex = *(BYTE*)bytes;

			m_pFill = CFillCompact::Apply(fillindex);

			bytes += sizeof(BYTE);
		}
		if((toggles&0X40)==0X40)
		{
			if(m_pType==nullptr)
			{
				m_pType = new CType;
			}
		}
		else if(m_pType!=nullptr)
		{
			delete m_pType;
			m_pType = nullptr;
		}

		if(m_pLine!=nullptr)
		{
			m_pLine->PackPC(nullptr, bytes);
		}
		if(m_pFill!=nullptr)
		{
			m_pFill->PackPC(nullptr, bytes);
		}
		if(m_pType!=nullptr)
		{
			m_pType->PackPC(nullptr, bytes);
		}
		return 0;
	}
}

DWORD CGeom::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = 0;
	size += PackStrCE(m_strName, file);

	BYTE toggles = 0;
	if(m_bClosed==true) toggles |= 0X02;

	file.Write(&toggles, sizeof(BYTE));
	size += sizeof(BYTE);

	return size;
}

void CGeom::ReleaseWeb(CFile& file, CSize offset) const
{
	file.Write(&m_bType, 1);

	DWORD geoId = m_geoId;
	ReverseOrder(geoId);
	file.Write(&geoId, sizeof(DWORD));

	DWORD attId = m_attId;
	ReverseOrder(attId);
	file.Write(&attId, sizeof(DWORD));
	SaveAsUTF8(file, m_strName);

	BYTE toggles = 0;
	if(m_bClosed==true) toggles |= 0X02;
	if(m_pLine!=nullptr) toggles |= 0X10;
	if(m_pFill!=nullptr) toggles |= 0X20;
	file.Write(&toggles, sizeof(BYTE));

	if(m_pLine!=nullptr)
	{
		const CLine::LINETYPE linetype = m_pLine->Gettype();
		file.Write(&linetype, sizeof(BYTE));
	}
	if(m_pFill!=nullptr)
	{
		const CFill::FILLTYPE filltype = m_pFill->Gettype();
		file.Write(&filltype, sizeof(BYTE));
	}

	if(m_pLine!=nullptr)
	{
		m_pLine->ReleaseWeb(file);
	}
	if(m_pFill!=nullptr)
	{
		m_pFill->ReleaseWeb(file);
	}
}
void CGeom::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	stream<<m_bType;
	stream<<m_geoId;
	stream<<m_attId;
	SaveAsUTF8(stream, m_strName);

	BYTE toggles = 0;
	if(m_bClosed==true) toggles |= 0X02;
	if(m_pLine!=nullptr) toggles |= 0X10;
	if(m_pFill!=nullptr) toggles |= 0X20;
	stream<<toggles;

	if(m_pLine!=nullptr)
	{
		const CLine::LINETYPE linetype = m_pLine->Gettype();
		stream<<linetype;
	}
	if(m_pFill!=nullptr)
	{
		const CFill::FILLTYPE filltype = m_pFill->Gettype();
		stream<<filltype;
	}

	if(m_pLine!=nullptr)
	{
		m_pLine->ReleaseWeb(stream);
	}
	if(m_pFill!=nullptr)
	{
		m_pFill->ReleaseWeb(stream);
	}
}
void CGeom::ReleaseWeb(boost::json::object& json) const
{
	json["Type"] = m_bType;
	if(m_geoId!=0X00000000)
		json["GeoID"] = m_geoId;
	if(m_attId!=0X00000000)
		json["AttID"] = m_attId;
	if(m_geoCode.IsEmpty()==FALSE)
		json["Geocode"] = m_geoCode;
	SaveAsEscapedASCII(json, "Name", m_strName);
	{
		std::vector<long> child;
		child.push_back(m_Rect.left);
		child.push_back(m_Rect.top);
		child.push_back(m_Rect.right);
		child.push_back(m_Rect.bottom);
		json["Rect"] = boost::json::value_from(child);
	}
	BYTE toggles = 0;
	if(m_bClosed==true) toggles |= 0X02;
	if(m_pLine!=nullptr) toggles |= 0X10;
	if(m_pFill!=nullptr) toggles |= 0X20;
	json["Wswitch"] = toggles;

	if(m_pLine!=nullptr)
	{
		boost::json::object child;
		const CLine::LINETYPE type = m_pLine->Gettype();
		child["Type"] = type;
		m_pLine->ReleaseWeb(child);
		json["Line"] = child;
	}
	if(m_pFill!=nullptr&&m_bClosed==true)
	{
		boost::json::object child;
		const CFill::FILLTYPE type = m_pFill->Gettype();
		child["Type"] = type;
		m_pFill->Transform(1, 0, 0, 0, 1, 0);
		m_pFill->ReleaseWeb(child);
		json["Fill"] = child;
	}
}
void CGeom::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	writer.add_byte(m_bType);//it is here on purpose, becase type can change

	writer.add_variant_uint32(m_geoId);

	BYTE toggles = 0;
	toggles |= m_attId==0XFFFFFFFF ? 0X00 : 0X01;
	toggles |= m_bClosed==true ? 0X02 : 0X00;
	toggles |= m_geoCode.IsEmpty() ? 0X00 : 0X04;
	toggles |= m_strName.IsEmpty() ? 0X00 : 0X08;
	toggles |= m_pLine!=nullptr ? 0X10 : 0X00;
	toggles |= (m_pFill!=nullptr&&m_bClosed==true) ? 0X20 : 0X00;
	toggles |= m_Rect.IsRectEmpty() ? 0X00 : 0X40;
	toggles |= m_bTag==0X00 ? 0X00 : 0X80;
	writer.add_byte(toggles);
	if(m_attId!=0XFFFFFFFF)
	{
		writer.add_variant_uint32(m_attId);
	}
	if(m_geoCode.IsEmpty()==false)
	{
		writer.add_string(m_geoCode);
	}
	if(m_strName.IsEmpty()==false)
	{
		writer.add_string(EscapedASCII(m_strName));
	}
	if(m_Rect.IsRectEmpty()==false)
	{
		writer.add_variant_sint32(m_Rect.left-offset.cx);
		writer.add_variant_sint32(m_Rect.top-offset.cy);
		writer.add_variant_sint32(m_Rect.right-offset.cx);
		writer.add_variant_sint32(m_Rect.bottom-offset.cy);
	}
	if(m_bTag!=0X00)
	{
		writer.add_byte(m_bTag);
	}
	if(m_pLine!=nullptr)
	{
		const CLine::LINETYPE type = m_pLine->Gettype();
		writer.add_byte(type);
		m_pLine->ReleaseWeb(writer);
	}
	if(m_pFill!=nullptr&&m_bClosed==true)
	{
		const CFill::FILLTYPE type = m_pFill->Gettype();
		writer.add_byte(type);
		m_pFill->Transform(1, 0, 0, 0, 1, 0);
		m_pFill->ReleaseWeb(writer);
	}
}
void CGeom::ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo, CSize& offset, CDatabase* pDatabase, CString& strSQL, CString& strIdField) const
{
	if(pDatabase!=nullptr&&pDatabase->IsOpen()&&strSQL.IsEmpty()==FALSE&&this->m_attId!=0XFFFFFFFF)
	{
		CRecordset rs(pDatabase);
		rs.m_strFilter.Format(_T("%s=%d"), strIdField, this->m_attId);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(!rs.IsEOF()) {
			rs.MoveFirst();
			boost::json::object child;
			for(short index = 0; index<rs.GetODBCFieldCount(); index++)
			{
				CODBCFieldInfo field;
				rs.GetODBCFieldInfo(index, field);
				if(field.m_strName==strIdField)
					continue;

				std::string name(CW2A(field.m_strName));
				CDBVariant varValue;
				rs.GetFieldValue(index, varValue);
				switch(varValue.m_dwType)
				{
					case DBVT_NULL:
						break;
					case DBVT_BOOL:
						child[name] = varValue.m_boolVal;
						break;
					case DBVT_UCHAR:
					case DBVT_SHORT:
						child[name] = varValue.m_iVal;
						break;
					case DBVT_LONG:
						child[name] = varValue.m_lVal;
						break;
					case DBVT_SINGLE:
						child[name] = varValue.m_fltVal;
						break;
					case DBVT_DOUBLE:
						child[name] = varValue.m_dblVal;
						break;
					case DBVT_DATE:
						break;
					case DBVT_STRING:
						SaveAsEscapedASCII(child, name.c_str(), *varValue.m_pstring);
						break;
					case DBVT_ASTRING:
						child[name] = *varValue.m_pstringA;
						break;
					case DBVT_WSTRING:
						SaveAsEscapedASCII(child, name.c_str(), *varValue.m_pstringW);
						break;
					case DBVT_BINARY:
						break;
					default:
						break;
				}
			}

			json["properties"] = child;
			rs.Close();
		}
	}
}
void CGeom::ReleaseWeb(pbf::writer& writer, CDatabase* pDatabase, CString& strSQL, CString& strIdField) const
{
	if(pDatabase!=nullptr&&pDatabase->IsOpen()&&strSQL.IsEmpty()==FALSE&&this->m_attId!=0XFFFFFFFF)
	{
		CRecordset rs(pDatabase);
		rs.m_strFilter.Format(_T("%s=%d"), strIdField, this->m_attId);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(!rs.IsEOF()) {
			rs.MoveFirst();

			std::list<std::pair<std::string, std::string>> properties;
			for(short index=0; index < rs.GetODBCFieldCount(); index++)
			{
				CODBCFieldInfo field;
				rs.GetODBCFieldInfo(index, field);
				if(field.m_strName == strIdField)
					continue;

				std::string name(CW2A(field.m_strName));
				CDBVariant varValue;
				rs.GetFieldValue(index, varValue);
				switch(varValue.m_dwType)
				{
					case DBVT_NULL:
						break;
					case DBVT_BOOL:
						properties.push_back(std::make_pair(name,std::to_string(varValue.m_boolVal)));
						break;
					case DBVT_UCHAR:
					case DBVT_SHORT:
						properties.push_back(std::make_pair(name, std::to_string(varValue.m_iVal)));
						break;
					case DBVT_LONG:
						properties.push_back(std::make_pair(name, std::to_string(varValue.m_lVal)));
						break;
					case DBVT_SINGLE:
						properties.push_back(std::make_pair(name, std::to_string(varValue.m_fltVal)));
						break;
					case DBVT_DOUBLE:
						properties.push_back(std::make_pair(name, std::to_string(varValue.m_dblVal)));
						break;
					case DBVT_DATE:
						break;
					case DBVT_STRING:
						properties.push_back(std::make_pair(name, EscapedASCII(*varValue.m_pstring)));
						break;
					case DBVT_ASTRING:
						properties.push_back(std::make_pair(name, (*varValue.m_pstringA).GetString()));
						break;
					case DBVT_WSTRING:
						properties.push_back(std::make_pair(name, EscapedASCII(*varValue.m_pstringW)));
						break;
					case DBVT_BINARY:
						break;
					default:
						break;
				}
			}
			rs.Close();

			writer.add_variant_int16(properties.size());
			for(const auto& pair:properties)
			{
				writer.add_string(pair.first);
				writer.add_string(pair.second);
			}
		}
		else 
			writer.add_variant_uint32(0);
	}
	else {
		writer.add_variant_uint32(0);
	}
}
void CGeom::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
}
void CGeom::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot) const
{
	if(m_strName.IsEmpty()==TRUE)
		return;

	const Gdiplus::Size textSize = pHint->MeasureText<Gdiplus::Size>(g, viewinfo, m_strName, 0);
	const Gdiplus::Rect geomRect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
	if(textSize.Width>geomRect.Width)
		return;
	if(textSize.Height>geomRect.Height)
		return;

	HALIGN hAlign;
	VALIGN vAlign;
	Gdiplus::PointF origin = GetTagAnchor(viewinfo, hAlign, vAlign);
	pHint->DrawString(g, viewinfo, m_strName, origin, 0, hAlign, vAlign);
}
void CGeom::DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const
{
	if(tAnchor<fAnchor)
		return;
	if(tAnchor==fAnchor&&et<=st)
		return;
	const Gdiplus::PointF fpoint = viewinfo.DocToClient<Gdiplus::PointF>(this->GetPoint(fAnchor, st));
	const Gdiplus::PointF tpoint = viewinfo.DocToClient<Gdiplus::PointF>(this->GetPoint(tAnchor, et));

	Gdiplus::PointF* points = new Gdiplus::PointF[tAnchor-fAnchor+2];
	points[0] = fpoint;
	points[tAnchor-fAnchor+2-1] = tpoint;
	for(int nAnchor = fAnchor+1; nAnchor<=tAnchor; nAnchor++)
	{
		points[nAnchor-fAnchor] = viewinfo.DocToClient<Gdiplus::PointF>(GetAnchor(nAnchor));
	}
	g.DrawLines(pen, points, tAnchor-fAnchor+2);
	::delete[] points;
}
void CGeom::DrawPath(CDC* pDC, const CViewinfo& viewinfo) const
{
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);
	const CPoint point1 = CPoint(m_Rect.left, m_Rect.top);
	const CPoint point2 = CPoint(m_Rect.right, m_Rect.bottom);
	const Gdiplus::PointF cliPoint1 = viewinfo.DocToClient<Gdiplus::PointF>(point1);
	const Gdiplus::PointF cliPoint2 = viewinfo.DocToClient<Gdiplus::PointF>(point2);

	CRect rect = CRect(cliPoint1.X, cliPoint1.Y, cliPoint2.X, cliPoint2.Y);
	rect.NormalizeRect();

	pDC->Rectangle(rect);

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

void CGeom::DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const
{
	const Gdiplus::HatchBrush brush(Gdiplus::HatchStyleDiagonalCross, Gdiplus::Color(100, 128, 0, 0), Gdiplus::Color(100, 255, 255, 0));

	Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
	const Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
	path.AddRectangle(rect);

	g.FillPath(&brush, &path);
}
void CGeom::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	CLine* pLine = m_pLine!=nullptr ? m_pLine : context.pLine;
	if(pLine==nullptr)
		return;

	CColor* pOldLineColor = this->m_pLine==nullptr ? nullptr : this->m_pLine->m_pColor;
	if(this->m_pLine!=nullptr)
	{
		const_cast<CColor*&>(this->m_pLine->m_pColor) = CColorSpot::White();
	}
	CColor* pOldTypeColor = this->m_pType==nullptr ? nullptr : this->m_pType->m_pColor;
	if(this->m_pType!=nullptr)
	{
		const_cast<CColor*&>(this->m_pType->m_pColor) = CColorSpot::White();
	}
	auto pOldFill1 = this->m_pFill;
	auto pOldFill2 = context.pFill;
	auto pOldBrush = context.brushFill;
	context.pFill = nullptr;
	const_cast<CFillCompact*&>(this->m_pFill) = nullptr;
	context.brushFill = nullptr;

	Draw(g, viewinfo, library, context);

	const_cast<CFillCompact*&>(this->m_pFill) = pOldFill1;
	context.pFill = pOldFill2;
	context.brushFill = pOldBrush;

	if(this->m_pLine!=nullptr)
	{
		const_cast<CColor*&>(this->m_pLine->m_pColor) = pOldLineColor;
	}
	if(this->m_pType!=nullptr)
	{
		const_cast<CColor*&>(this->m_pType->m_pColor) = pOldTypeColor;
	}
}
void CGeom::Mono(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	CColor* pOldLineColor = this->m_pLine==nullptr ? nullptr : this->m_pLine->m_pColor;
	if(this->m_pLine!=nullptr)
	{
		const_cast<CColor*&>(this->m_pLine->m_pColor) = CColorSpot::White();
	}
	CColor* pOldFillColor = this->m_pFill==nullptr ? nullptr : this->m_pFill->GetColor();
	if(this->m_pFill!=nullptr)
	{
		this->m_pFill->SetColor(CColorSpot::White());
	}
	CColor* pOldTypeColor = this->m_pType==nullptr ? nullptr : this->m_pType->m_pColor;
	if(this->m_pType!=nullptr)
	{
		const_cast<CColor*&>(this->m_pType->m_pColor) = CColorSpot::White();
	}
	
	Draw(g, matrix, context, dpi);

	if(this->m_pLine!=nullptr)
	{
		const_cast<CColor*&>(this->m_pLine->m_pColor) = pOldLineColor;
	}
	if(this->m_pFill!=nullptr)
	{
		this->m_pFill->SetColor(pOldFillColor);
	}
	if(this->m_pType!=nullptr)
	{
		const_cast<CColor*&>(this->m_pType->m_pColor) = pOldTypeColor;
	}
}
void CGeom::Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
}
void CGeom::Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, const int tolerance) const
{
}
void CGeom::SetStyle(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CStyleDlg dlg(pWnd, false, library, m_pLine, m_pFill);
	if(dlg.DoModal()==IDOK)
	{
		if(m_pLine!=nullptr)
		{
			Invalidate(pWnd, viewinfo, 3);
			//			m_pLine->Decrease(nullptr);
			delete m_pLine;
			m_pLine = nullptr;
		}
		if(m_pFill!=nullptr)
		{
			Invalidate(pWnd, viewinfo, 0);
			//			m_pFill->Decrease(nullptr);
			delete m_pFill;
			m_pFill = nullptr;
		}

		m_pLine = dlg.d_pLine;
		m_pFill = (CFillCompact*)dlg.d_pFill;
		dlg.d_pLine = nullptr;
		dlg.d_pFill = nullptr;

		if(m_pLine!=nullptr)
		{
			Invalidate(pWnd, viewinfo, 3);
			//			m_pLine->SetWidth(library.);
		}
		if(m_pFill!=nullptr)
		{
			Invalidate(pWnd, viewinfo, 0);
		}

		m_bModified = true;
		pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGeom::SetHint(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CHint* pHint = m_pHint!=nullptr ? m_pHint->Clone() : new CHint();
	CHintDlg dlg(pWnd, *pHint);
	if(dlg.DoModal()==IDOK)
	{
		if(m_pHint!=nullptr)
		{
			Invalidate(pWnd, viewinfo, 3);
			delete m_pHint;
			m_pHint = nullptr;
		}

		m_pHint = pHint;
		if(m_pHint!=nullptr)
		{
			Invalidate(pWnd, viewinfo, 3);
		}

		pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
	}
	else
	{
		delete pHint;
		pHint = nullptr;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

bool CGeom::IsDissident() const
{
	if(m_pLine!=nullptr)
		return true;
	if(m_pFill!=nullptr)
		return true;
	if(m_pType!=nullptr)
		return true;
	if(m_pHint!=nullptr)
		return true;

	return false;
}

void CGeom::Regress(CWnd* pWnd, CLibrary& library)
{
	if(m_pLine!=nullptr)
	{
		m_pLine->Decrease(library);
		delete m_pLine;
		m_pLine = nullptr;

		m_bModified = true;
	}

	if(m_pFill!=nullptr)
	{
		m_pFill->Decrease(library);
		delete m_pFill;
		m_pFill = nullptr;

		m_bModified = true;
	}

	if(m_pType!=nullptr)
	{
		delete m_pType;
		m_pType = nullptr;

		m_bModified = true;
	}
}

void CGeom::CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* m_pSpot, const CType* pType) const
{
	if(m_pLine!=nullptr)
		lines.CountIn(m_pLine);
	else
		lines.CountIn(pLine);

	if(m_bClosed==true)
	{
		if(m_pFill!=nullptr)
			fills.CountIn(m_pFill);
		else
			fills.CountIn(pFill);
	}
}

bool CGeom::HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const
{
	pLine2 = m_pLine!=nullptr ? m_pLine : pLine2;
	pFill2 = m_pFill!=nullptr ? m_pFill : pFill2;

	switch(activeall)
	{
		case ACTIVEALL::Stroke:
			if(pLine1==nullptr)
				return false;
			else if(pLine2==nullptr)
				return false;
			else if(pLine1==pLine2)
				return true;
			else
				return (*pLine1==*pLine2);
			break;
		case ACTIVEALL::Fill:
			if(m_bClosed==false)
				return false;
			if(pFill1==nullptr)
				return false;
			else if(pFill2==nullptr)
				return false;
			else if(pFill1==pFill2)
				return true;
			else
				return (*pFill1==*pFill2);
			break;
		case ACTIVEALL::Style:
			if(m_bClosed==false)
				return false;
			if(pLine1==nullptr)
				return false;
			else if(pFill1==nullptr)
				return false;
			else if(pLine2==nullptr)
				return false;
			else if(pFill2==nullptr)
				return false;
			else if(pLine1==pLine2&&pFill1==pFill2)
				return true;
			else
				return (*pLine1==*pLine2)&&(*pFill1==*pFill2);
			break;
		default:
			break;
	}

	return false;
}

void CGeom::Invalid(CDocument& document) const
{
	POSITION pos = document.GetFirstViewPosition();
	while(pos!=nullptr)
	{
		CView* pView = document.GetNextView(pos);
		CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
		if(pViewinfo!=nullptr)
		{
			this->Invalidate(pView, *pViewinfo, 10);
		}
	}
}

void CGeom::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const
{
	if(pWnd==nullptr)
		return;

	ASSERT_VALID(this);
	if(m_Rect.IsRectEmpty())
		return;

	/*if(pWnd->IsKindOf(RUNTIME_CLASS(CScrollView)) == FALSE)
	{
	CRect rect;
	pWnd->GetClientRect(rect);
	pWnd->InvalidateRect(rect,TRUE);
	}
	else*/
	{
		Gdiplus::Rect cliRect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
		cliRect.Inflate(inflate, inflate);
		pWnd->InvalidateRect(CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom()), FALSE);
	}
}

void CGeom::InvalidateSegment(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const
{
	Invalidate(pWnd, viewinfo, inflate);
}

void CGeom::InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const
{
	Invalidate(pWnd, viewinfo, inflate);
}

void CGeom::InvalidateTag(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_pTag!=nullptr)
	{
		HALIGN hAlign;
		VALIGN vAlign;
		const Gdiplus::PointF point = this->GetTagAnchor(viewinfo, hAlign, vAlign);
		m_pTag->Invalidate(pWnd, viewinfo, point, pHint);
	}
}

void CGeom::DrawAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor) const
{
	const unsigned int nAnchors = GetAnchors();
	if(nAnchor==nAnchors&&m_bClosed==true)
		return;
	if(nAnchor>nAnchors)
		return;
	const CPoint& point = GetAnchor(nAnchor);
	const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(point);
	CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
	cliRect.InflateRect(2, 2);

	pDC->InvertRect(cliRect);
}

void CGeom::DrawAnchors(CDC* pDC, const CViewinfo& viewinfo) const
{
	const unsigned int nAnchors = GetAnchors();
	for(unsigned int nAnchor = 1; nAnchor<=nAnchors; nAnchor++)
	{
		if(nAnchor==nAnchors&&m_bClosed==true)
			continue;

		const CPoint& point = GetAnchor(nAnchor);
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(point);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);

		pDC->InvertRect(cliRect);
	}
}

void CGeom::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_pTag!=nullptr)
	{
		HALIGN hAlign;
		VALIGN vAlign;
		const Gdiplus::PointF point = this->GetTagAnchor(viewinfo, hAlign, vAlign);
		m_pTag->Draw(g, viewinfo, m_pHint!=nullptr ? m_pHint : pHint, point);
	}
}

bool CGeom::PickOn(const CPoint& point, const unsigned long& gap) const
{
	//	if(m_pLine != nullptr && m_pLine->Gettype() == CLine::LINETYPE::Blank)
	//		return false;

	CRect rect = m_Rect;
	rect.InflateRect(gap, gap);

	return rect.PtInRect(point);
}

bool CGeom::PickIn(const CPoint& point) const
{
	if(m_bClosed==false)
		return false;
	else if(m_pFill!=nullptr&&m_pFill->Gettype()==CFill::FILLTYPE::Blank)
		return false;
	else
		return m_Rect.PtInRect(point);
}

unsigned int CGeom::PickAnchor(const CDatainfo& datainfo, const CViewinfo& viewinfo, const Gdiplus::Point& cliPoint) const
{
	if(m_bLocked==true)
		return 0;

	Gdiplus::Rect cliRect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
	cliRect.Inflate(5, 5);
	if(cliRect.Contains(cliPoint)==FALSE)
		return 0;

	for(unsigned int nAnchor = 1; nAnchor<=GetAnchors(); nAnchor++)
	{
		const Gdiplus::Point cliHandle = viewinfo.DocToClient<Gdiplus::Point>(GetAnchor(nAnchor));
		Gdiplus::Rect rect(Gdiplus::Point(cliHandle.X, cliHandle.Y), Gdiplus::Size(0, 0));
		rect.Inflate(5, 5);
		if(rect.Contains(cliPoint)==TRUE)
		{
			return nAnchor;
		}
	}

	return 0;
}

bool CGeom::PickIn(const CRect& rect) const
{
	if(m_Rect.IsRectEmpty()==TRUE)
	{
		if(rect.PtInRect(CPoint(m_Rect.left, m_Rect.top))==TRUE)
			return TRUE;
		else if(rect.PtInRect(CPoint(m_Rect.right, m_Rect.top))==TRUE)
			return TRUE;
		else if(rect.PtInRect(CPoint(m_Rect.right, m_Rect.bottom))==TRUE)
			return TRUE;
		else if(rect.PtInRect(CPoint(m_Rect.left, m_Rect.bottom))==TRUE)
			return TRUE;
		else
			return FALSE;
	}
	else
		return !(m_Rect&rect).IsRectEmpty();
}

bool CGeom::PickIn(const CPoint& center, const unsigned long& radius) const
{
	CRect rect(center, CSize(0, 0));
	rect.InflateRect(radius, radius);
	if(rect.IntersectRect(rect, m_Rect)==FALSE)
		return false;

	if(m_Rect.PtInRect(center)==TRUE)
		return true;

	rect = m_Rect;
	rect.OffsetRect(-center.x, -center.y);

	if(abs(rect.left)<radius)
	{
		if(rect.top*rect.bottom<0)
			return true;
	}

	if(abs(rect.right)<radius)
	{
		if(rect.top*rect.bottom<0)
			return true;
	}

	if(abs(rect.top)<radius)
	{
		if(rect.left*rect.right<0)
			return true;
	}

	if(abs(rect.bottom)<radius)
	{
		if(rect.left*rect.right<0)
			return true;
	}

	CPoint point;

	point = rect.TopLeft();
	if(point.x*point.x+point.y*point.y<=radius*radius)
		return true;

	point = CPoint(rect.right, rect.top);
	if(point.x*point.x+point.y*point.y<=radius*radius)
		return true;

	point = rect.BottomRight();
	if(point.x*point.x+point.y*point.y<=radius*radius)
		return true;

	point = CPoint(rect.left, rect.bottom);;
	if(point.x*point.x+point.y*point.y<=radius*radius)
		return true;

	return false;
}

bool CGeom::PickIn(const CPoly& polygon) const
{
	CRect rect = polygon.m_Rect;
	rect = m_Rect&rect;
	if(rect.IsRectEmpty()==TRUE)
		return false;

	return polygon.PickIn(m_Rect);
}
bool CGeom::IsIn(const CViewinfo& viewinfo, const Gdiplus::Rect& vewRect) const
{
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.left, m_Rect.top));
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.right, m_Rect.top));
	const Gdiplus::Point point3 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.right, m_Rect.bottom));
	const Gdiplus::Point point4 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.left, m_Rect.bottom));
	if(min(min(point1.X, point2.X), min(point3.X, point4.X))>vewRect.GetRight())
		return false;
	else if(max(max(point1.X, point2.X), max(point3.X, point4.X))<vewRect.GetLeft())
		return false;
	else if(min(min(point1.Y, point2.Y), min(point3.Y, point4.Y))>vewRect.GetBottom())
		return false;
	else if(max(max(point1.Y, point2.Y), max(point3.Y, point4.Y))<vewRect.GetTop())
		return false;
	else
		return true;
}

unsigned int CGeom::GetAnchors() const
{
	return 5;
}
CPoint& CGeom::GetAnchor(const unsigned int& nAnchor) const
{
	ASSERT_VALID(this);
	static CPoint point;

	switch(nAnchor)
	{
		case 1:
		case 5:
			point.x = m_Rect.left;
			point.y = m_Rect.top;
			break;
		case 2:
			point.x = m_Rect.left;
			point.y = m_Rect.bottom;
			break;
		case 3:
			point.x = m_Rect.right;
			point.y = m_Rect.bottom;
			break;
		case 4:
			point.x = m_Rect.right;
			point.y = m_Rect.top;
			break;
		default:
			ASSERT(FALSE);
			break;
	}

	return point;
}

bool CGeom::GetAandT(const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const
{
	if(fAnchor>=5)
	{
		tAnchor = 5;
		et = 0;
		return false;
	}
	else if(length==0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	if(length==0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	for(unsigned int nAnchor = fAnchor; nAnchor<5; nAnchor++)
	{
		long segment = 0;
		switch(nAnchor)
		{
			case 1:
			case 3:
				segment = m_Rect.Width();
				break;
			case 2:
			case 4:
				segment = m_Rect.Height();
				break;
			default:
				segment = 0;
				break;
		}
		const long leftlen = segment*(1-st);
		if(length<leftlen)
		{
			tAnchor = nAnchor;
			et = st+(double)length/segment;

			return true;
		}

		if(length==leftlen)
		{
			tAnchor = nAnchor;
			et = 1;

			return true;
		}
		length -= leftlen;
		st = 0.0f;
	}

	tAnchor = 4;
	et = 1.0f;

	return false;
}

CPoint CGeom::GetPoint(const unsigned int& nAnchor, const double& t) const
{
	CPoint point;
	switch(nAnchor)
	{
		case 1:
			{
				if(t==0.0f)
				{
					point.x = m_Rect.left;
					point.y = m_Rect.top;
				}
				else if(t==1.0f)
				{
					point.x = m_Rect.left;
					point.y = m_Rect.bottom;
				}
				else
				{
					point.x = m_Rect.left;
					point.y = m_Rect.top+round(m_Rect.Height()*t);
				}
			}
			break;
		case 2:
			{
				if(t==0.0f)
				{
					point.x = m_Rect.left;
					point.y = m_Rect.bottom;
				}
				else if(t==1.0f)
				{
					point.x = m_Rect.right;
					point.y = m_Rect.bottom;
				}
				else
				{
					point.x = m_Rect.left+round(m_Rect.Width()*t);
					point.y = m_Rect.bottom;
				}
			}
			break;
		case 3:
			{
				if(t==0.0f)
				{
					point.x = m_Rect.right;
					point.y = m_Rect.bottom;
				}
				else if(t==1.0f)
				{
					point.x = m_Rect.right;
					point.y = m_Rect.top;
				}
				else
				{
					point.x = m_Rect.right;
					point.y = m_Rect.bottom-round(m_Rect.Height()*t);
				}
			}
			break;
		case 4:
			{
				if(t==0.0f)
				{
					point.x = m_Rect.right;
					point.y = m_Rect.top;
				}
				else if(t==1.0f)
				{
					point.x = m_Rect.left;
					point.y = m_Rect.top;
				}
				else
				{
					point.x = m_Rect.right-round(m_Rect.Width()*t);
					point.y = m_Rect.top;
				}
			}
			break;
	}
	return point;
}

CPoint CGeom::GetGeogroid() const
{
	return m_Rect.CenterPoint();
}
CPoint CGeom::GetLabeloid() const
{
	return m_Rect.CenterPoint();
}

double CGeom::GetAngle(const unsigned int& nAnchor, const double& t) const
{
	double angle = 0.0f;
	switch(nAnchor)
	{
		case 1:
			{
				if(t==0.0f)
					angle = M_PI*3/4;
				else if(t==1.0f)
					angle = M_PI/4;
				else
					angle = M_PI/2;
			}
			break;
		case 2:
			{
				if(t==0.0f)
					angle = M_PI/4;
				else if(t==1.0f)
					angle = M_PI*7/4;
				else
					angle = 0.f;
			}
			break;
		case 3:
			{
				if(t==0.0f)
					angle = M_PI*7/4;
				else if(t==1.0f)
					angle = M_PI*5/4;
				else
					angle = M_PI*3/2;
			}
			break;
		case 4:
			{
				if(t==0.0f)
					angle = M_PI*5/4;
				else if(t==1.0f)
					angle = M_PI*3/4;
				else
					angle = M_PI;
			}
			break;
		default: break;
	}

	return angle;
}

bool CGeom::IsValid() const
{
	return true;
}

unsigned long CGeom::GetLength(const double& tolerance) const
{
	return 2*(m_Rect.Width()+m_Rect.Height());
}

double CGeom::GetLength(const CDatainfo& datainfo, const double& tolerance) const
{
	CPoint* points = new CPoint[5];
	points[0] = CPoint(m_Rect.left, m_Rect.bottom);
	points[1] = CPoint(m_Rect.right, m_Rect.bottom);
	points[2] = CPoint(m_Rect.right, m_Rect.top);
	points[3] = CPoint(m_Rect.left, m_Rect.top);
	points[4] = points[0];

	return datainfo.CalLength(points, 5);
}

double CGeom::GetArea() const
{
	const double a = m_Rect.Width();
	const double b = m_Rect.Height();
	return a*b;
}

double CGeom::GetArea(const CDatainfo& datainfo) const
{
	CPoint* points = new CPoint[5];
	points[0] = CPoint(m_Rect.left, m_Rect.top);
	points[1] = CPoint(m_Rect.left, m_Rect.bottom);
	points[2] = CPoint(m_Rect.right, m_Rect.bottom);
	points[3] = CPoint(m_Rect.right, m_Rect.top);
	points[4] = points[0];
	return datainfo.CalArea(points, 5);
}
Gdiplus::PointF CGeom::GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const
{
	hAlign = HALIGN::HA_LEFT;
	vAlign = VALIGN::VA_CENTER;
	const Gdiplus::RectF rect = viewinfo.DocToClient<Gdiplus::RectF>(m_Rect);
	return CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(rect, m_pTag!=nullptr ? m_pTag->GetAnchor() : ANCHOR_6);
}

void CGeom::ChangeAnchor(const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch, Undoredo::CActionStack* pActionstack)
{
	if(Δ==CSize(0, 0))
		return;
	const unsigned int fAnchor = (nAnchor+2)%4==0 ? 4 : (nAnchor+2)%4;

	CPoint local = GetAnchor(nAnchor)+Δ;
	CPoint face = GetAnchor(fAnchor);

	local.x -= face.x;
	local.y -= face.y;

	if(bMatch==true)
	{
		if(abs(local.x)>abs(local.y))
			local.y = abs(local.x)*(local.y>=0 ? 1 : -1);
		else
			local.x = abs(local.y)*(local.x>=0 ? 1 : -1);
	}

	local = local+face;

	if(::GetKeyState(VK_CONTROL)<0)
		face = face-(local-GetAnchor(nAnchor));
	CRect rect;
	rect.left = face.x;
	rect.top = face.y;
	rect.right = local.x;
	rect.bottom = local.y;
	rect.NormalizeRect();

	if(pActionstack!=nullptr)
	{
		Undoredo::CModify<CWnd*, CGeom*, CRect>* pModify = new Undoredo::CModify<CWnd*, CGeom*, CRect>(nullptr, this, m_Rect, rect);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Geom::Undo_Modify_Rect;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Geom::Redo_Modify_Rect;
		pActionstack->Record(pModify);
	}

	m_Rect = rect;
	m_bModified = true;
}

CRect CGeom::RegulizeRect(CPoint point1, CPoint point2, bool bSquare, bool bCentral)
{
	int cx = point2.x-point1.x;
	int cy = point2.y-point1.y;

	if(bSquare==true)
	{
		if(abs(cx)>abs(cy))
			cy = abs(cx)*(cy>=0 ? 1 : -1);
		else
			cx = abs(cy)*(cx>=0 ? 1 : -1);
	}
	point2.x = point1.x+cx;
	point2.y = point1.y+cy;
	if(bCentral==true)
	{
		point1.x -= cx;
		point1.y -= cy;
	}
	CRect rect(point1.x, point1.y, point2.x, point2.y);
	rect.NormalizeRect();
	return rect;
}
void CGeom::Move(const int& dx, const int& dy)
{
	m_Rect.OffsetRect(dx, dy);
	if(m_pFill!=nullptr)
	{
		m_pFill->Transform(1, 0, dx, 0, 1, dy);
	}
	m_bModified = true;
}
void CGeom::Move(const CSize& Δ)
{
	m_Rect.OffsetRect(Δ);
	if(m_pFill!=nullptr)
	{
		m_pFill->Transform(1, 0, Δ.cx, 0, 1, Δ.cy);
	}
	m_bModified = true;
}

void CGeom::MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch)
{
	const CRect rect = m_Rect;
	CGeom::ChangeAnchor(nAnchor, Δ, bMatch, nullptr);

	DrawPath(pDC, viewinfo);
	DrawAnchors(pDC, viewinfo);

	m_Rect = rect;
}

BOOL CGeom::operator !=(const CGeom& geom) const
{
	return !(*this==geom);
}

CGeom* CGeom::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CGeom* pClone = (CGeom*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	return pClone;
}

void CGeom::CopyTo(CGeom* pGeom, bool ignore) const
{
	pGeom->m_geoId = m_geoId;
	pGeom->m_attId = m_attId;
	if(ignore==false)
	{
		pGeom->m_Rect = m_Rect;
	}
	pGeom->m_bLocked = m_bLocked;
	pGeom->m_bClosed = m_bClosed;
	pGeom->m_bActive = m_bActive;
	pGeom->m_strName = m_strName;
	pGeom->m_filter = m_filter;
	delete pGeom->m_pTag;
	pGeom->m_pTag = nullptr;
	delete pGeom->m_pLine;
	pGeom->m_pLine = nullptr;
	delete pGeom->m_pFill;
	pGeom->m_pFill = nullptr;
	delete pGeom->m_pType;
	pGeom->m_pType = nullptr;
	delete pGeom->m_pHint;
	pGeom->m_pHint = nullptr;

	if(m_pTag!=nullptr)
	{
		pGeom->m_pTag = m_pTag->Clone(pGeom->m_strName);
	}

	if(m_pLine!=nullptr)
		pGeom->m_pLine = m_pLine->Clone();
	if(m_pFill!=nullptr)
		pGeom->m_pFill = (CFillCompact*)m_pFill->Clone();
	if(m_pType!=nullptr)
		pGeom->m_pType = (CType*)m_pType->Clone();
	if(m_pHint!=nullptr)
		pGeom->m_pHint = m_pHint->Clone();
}

void CGeom::Inherit(CGeom* pGeom) const
{
	pGeom->m_geoId = m_geoId;
	if(pGeom->m_attId==0XFFFFFFFF)
		pGeom->m_attId = m_attId;
	pGeom->m_bLocked = m_bLocked;
	pGeom->m_bActive = m_bActive;
	if(pGeom->m_strName.IsEmpty())
		pGeom->m_strName = m_strName;
	if(pGeom->m_filter==FilterType::None)
		pGeom->m_filter = m_filter;

	if(pGeom->m_pTag==nullptr&&m_pTag!=nullptr)
		pGeom->m_pTag = m_pTag->Clone(pGeom->m_strName);
	if(pGeom->m_pLine==nullptr&&m_pLine!=nullptr)
		pGeom->m_pLine = m_pLine->Clone();
	if(pGeom->m_pFill==nullptr&&m_pFill!=nullptr)
		pGeom->m_pFill = (CFillCompact*)m_pFill->Clone();
	if(pGeom->m_pType==nullptr&&m_pType!=nullptr)
		pGeom->m_pType = (CType*)m_pType->Clone();
	if(pGeom->m_pHint==nullptr&&m_pHint!=nullptr)
		pGeom->m_pHint = m_pHint->Clone();
}

void CGeom::Swap(CGeom* pGeom)
{
	Swapdata<DWORD>(pGeom->m_geoId, m_geoId);
	Swapdata<DWORD>(pGeom->m_attId, m_attId);
	Swapclass<CRect>(pGeom->m_Rect, m_Rect);
	Swapclass<bool>(pGeom->m_bLocked, m_bLocked);
	Swapclass<bool>(pGeom->m_bClosed, m_bClosed);
	Swapclass<bool>(pGeom->m_bActive, m_bActive);
	Swapclass<CString>(pGeom->m_strName, m_strName);

	Swapclass<CTag*>(pGeom->m_pTag, m_pTag);
	Swapclass<CLine*>(pGeom->m_pLine, m_pLine);
	Swapclass<CFillCompact*>(pGeom->m_pFill, m_pFill);
	Swapclass<CType*>(pGeom->m_pType, m_pType);
	Swapclass<CHint*>(pGeom->m_pHint, m_pHint);
}

bool CGeom::Junction(const CPoint& point1, const CPoint& point2)
{
	return false;
}

Gdiplus::RectF CGeom::GetVewBoundary(const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	CLine* line = m_pLine!=nullptr ? m_pLine : (CLine*)context.pLine;
	float inflation = (line==nullptr||line->Gettype()==CLine::LINETYPE::Blank) ? 0 : line->m_nWidth/10.f;
	inflation *= context.ratioLine;
	Gdiplus::RectF rect = viewinfo.DocToClient<Gdiplus::RectF>(m_Rect);
	rect.Inflate(inflation, inflation);
	return rect;
}

bool CGeom::CreateName(CWnd* pWnd, const float& fontSize, const float& zoomPointToDoc, CObList& oblist)
{
	if(m_strName.IsEmpty()==FALSE)
	{
		CPoint center;
		center.x = m_Rect.left+m_Rect.Width()/2;
		center.y = m_Rect.top+m_Rect.Height()/2;

		CText* text = new CText();
		text->m_strName = m_strName;
		text->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);
		text->CalCorner(nullptr, zoomPointToDoc, 1.f);
		const int x = m_Rect.left+m_Rect.Width()/2;
		const int y = m_Rect.top+m_Rect.Height()/2;
		text->Move(CSize(x, y));

		oblist.AddTail(text);

		return true;
	}

	return false;
}

void CGeom::CreateOptimizedTag(CArray<Gdiplus::RectF, Gdiplus::RectF&>& OccupiedRect, const int& sizeFont)
{
	if(m_strName.IsEmpty()==TRUE)
		return;
	if(m_pTag!=nullptr)
		return;
	const Gdiplus::RectF markRect = Gdiplus::RectF(m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height());
	const unsigned short chars = m_strName.GetLength();

	bool bHasEnglish = false;
	for(int j = 0; j<chars; j++)
	{
		if((sizeof(TCHAR)==1&&(BYTE)m_strName[j]<0X81)||(sizeof(TCHAR)==2&&m_strName[j]<(TCHAR)0X0040))
		{
			bHasEnglish = true;
			break;
		}
	}

	const Gdiplus::SizeF textSize(m_strName.GetLength()*sizeFont/2, sizeFont);
	Gdiplus::RectF maxRect = markRect;
	if(bHasEnglish==true)
	{
		maxRect.Inflate(textSize.Width, textSize.Height);
	}
	else
	{
		const Gdiplus::REAL ex = max(textSize.Width, textSize.Height);
		maxRect.Inflate(ex, ex);
	}

	CArray<Gdiplus::RectF, Gdiplus::RectF&> rectarray;
	const int count = OccupiedRect.GetSize();
	for(int index = 0; index<count; index++)
	{
		Gdiplus::RectF rect2 = OccupiedRect.GetAt(index);
		if(rect2.IntersectsWith(maxRect)==TRUE)
			rectarray.Add(rect2);
	}

	double minArea = 9999999999999999.0f;
	ANCHOR Anchor = ANCHOR::ANCHOR_0;
	Gdiplus::RectF tagRect;

	bHasEnglish = true;
	for(int index = 0; index<8; index++)
	{
		ANCHOR anchor = PrioritizedAnchors[index];
		HALIGN hAlign;
		VALIGN vAlign;
		CTag::GetDefaultAlign(anchor, hAlign, vAlign);
		Gdiplus::RectF rect = ::GetTagRect(markRect, textSize, anchor, hAlign, vAlign);
		const int count = rectarray.GetSize();
		double area = 0.0f;
		for(int j = 0; j<count; j++)
		{
			Gdiplus::RectF rect2 = rectarray.GetAt(j);
			rect2.Intersect(rect);
			if(rect2.IsEmptyArea()==FALSE)
			{
				area += (double)rect2.Width*(double)rect2.Height;
			}
		}

		if(area==0.0f)
		{
			Anchor = anchor;
			tagRect = rect;
			minArea = 0;
			break;
		}
		else if(area<minArea)
		{
			Anchor = anchor;
			tagRect = rect;
			minArea = area;
		}
	}
	rectarray.RemoveAll();

	m_pTag = new CTag(m_strName, Anchor);
	OccupiedRect.Add(tagRect);
}

void CGeom::CreateTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	if(m_strName.IsEmpty()==TRUE)
		return;

	if(m_pTag!=nullptr)
		return;

	m_pTag = new CTag(m_strName, anchor);
}

void CGeom::SetupTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	if(m_pTag==nullptr)
		return;

	m_pTag->SetAnchor(anchor);
}
bool CGeom::HasTag() const
{
	return m_pTag!=nullptr;
}
bool CGeom::Tagable() const
{
	return m_strName.IsEmpty()==FALSE;
}
void CGeom::DeleteTag()
{
	delete m_pTag;
	m_pTag = nullptr;
}

void CGeom::Information(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	CTextEditDlg dlg(pWnd, m_strName);
	if(dlg.DoModal()==IDOK)
	{
		if(m_pTag!=nullptr&&pHint!=nullptr)
		{
			const Gdiplus::PointF point = m_pTag->GetAnchorPoint<Gdiplus::Rect, Gdiplus::PointF>(viewinfo, m_Rect);
			m_pTag->Invalidate(pWnd, viewinfo, point, pHint);
			m_strName = dlg.m_String;
			m_pTag->Invalidate(pWnd, viewinfo, point, pHint);
		}

		m_strName = dlg.m_String;

		m_bModified = true;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGeom::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	const CRect replace = m_Rect;
	const CPoint point1 = CPoint(m_Rect.left, m_Rect.top);
	const CPoint point2 = CPoint(m_Rect.right, m_Rect.bottom);
	const long x1 = point1.x*m11+point1.y*m21+m31;
	const long y1 = point1.x*m12+point1.y*m22+m32;
	const long x2 = point2.x*m11+point2.y*m21+m31;
	const long y2 = point2.x*m12+point2.y*m22+m32;

	m_Rect = CRect(x1, y1, x2, y2);;
	m_Rect.NormalizeRect();

	DrawPath(pDC, viewinfo);

	m_Rect = replace;
}

void CGeom::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m21==0.0f&&m12==0.f)
	{
		auto rect = m_Rect;
		rect.left = round(m_Rect.left*m11+m31);
		rect.right = round(m_Rect.right*m11+m31);
		rect.top = round(m_Rect.top*m22+m32);
		rect.bottom = round(m_Rect.bottom*m22+m32);
		m_Rect = rect;
		m_Rect.NormalizeRect();
	}
	else
	{
		CPoint point;

		CPoint point1;
		point = CPoint(m_Rect.left, m_Rect.top);
		point1.x = round(point.x*m11+point.y*m21+m31);
		point1.y = round(point.x*m12+point.y*m22+m32);

		CPoint point2;
		point = CPoint(m_Rect.right, m_Rect.top);
		point2.x = round(point.x*m11+point.y*m21+m31);
		point2.y = round(point.x*m12+point.y*m22+m32);

		CPoint point3;
		point = CPoint(m_Rect.right, m_Rect.bottom);
		point3.x = round(point.x*m11+point.y*m21+m31);
		point3.y = round(point.x*m12+point.y*m22+m32);

		CPoint point4;
		point = CPoint(m_Rect.left, m_Rect.bottom);
		point4.x = round(point.x*m11+point.y*m21+m31);
		point4.y = round(point.x*m12+point.y*m22+m32);

		m_Rect.left = min(min(point1.x, point2.x), min(point3.x, point4.x));
		m_Rect.top = min(min(point1.y, point2.y), min(point3.y, point4.y));
		m_Rect.right = max(max(point1.x, point2.x), max(point3.x, point4.x));
		m_Rect.bottom = max(max(point1.y, point2.y), max(point3.y, point4.y));
	}
	if(m_pLine!=nullptr)
	{
		const float sx = sqrt(m11*m11+m12*m12);
		const float sy = sqrt(m21*m21+m22*m22);

		m_pLine->Scale((sx+sy)/2.f);
	}
	if(m_pType!=nullptr)
	{
		const float sx = sqrt(m11*m11+m12*m12);
		const float sy = sqrt(m21*m21+m22*m22);
		m_pType->Scale((sx+sy)/2.f);
	}
	if(m_pFill!=nullptr)
	{
		m_pFill->Transform(m11, m21, m31, m12, m22, m32);
	}
	m_bModified = true;
}

void CGeom::Transform(const CViewinfo& viewinfo)
{
	if(m_pFill!=nullptr)
	{
		m_pFill->Transform(viewinfo, m_Rect);
	}

	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.left, m_Rect.top));
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.right, m_Rect.top));
	const Gdiplus::Point point3 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.right, m_Rect.bottom));
	const Gdiplus::Point point4 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.left, m_Rect.bottom));

	m_Rect.left = min(min(point1.X, point2.X), min(point3.X, point4.X));
	m_Rect.top = min(min(point1.Y, point2.Y), min(point3.Y, point4.Y));
	m_Rect.right = max(max(point1.X, point2.X), max(point3.X, point4.X));
	m_Rect.bottom = max(max(point1.Y, point2.Y), max(point3.Y, point4.Y));
	m_bModified = true;
}

void CGeom::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	CPoint point;

	point = CPoint(m_Rect.left, m_Rect.top);
	const CPoint point1 = Rotate(mapinfo, deltaLng, deltaLat, facing, point);

	point = CPoint(m_Rect.right, m_Rect.top);
	const CPoint point2 = Rotate(mapinfo, deltaLng, deltaLat, facing, point);

	point = CPoint(m_Rect.right, m_Rect.bottom);
	const CPoint point3 = Rotate(mapinfo, deltaLng, deltaLat, facing, point);

	point = CPoint(m_Rect.left, m_Rect.bottom);
	const CPoint point4 = Rotate(mapinfo, deltaLng, deltaLat, facing, point);

	m_Rect = FPointRect(point1, point2, point3, point4);
	m_Rect.NormalizeRect();

	m_bModified = true;
}
CGeom* CGeom::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	CPoint point;

	point = CPoint(m_Rect.left, m_Rect.top);
	const CPoint point1 = CDatainfo::Project(fMapinfo, tMapinfo, point);

	point = CPoint(m_Rect.right, m_Rect.top);
	const CPoint point2 = CDatainfo::Project(fMapinfo, tMapinfo, point);

	point = CPoint(m_Rect.right, m_Rect.bottom);
	const CPoint point3 = CDatainfo::Project(fMapinfo, tMapinfo, point);

	point = CPoint(m_Rect.left, m_Rect.bottom);
	const CPoint point4 = CDatainfo::Project(fMapinfo, tMapinfo, point);

	m_Rect = FPointRect(point1, point2, point3, point4);
	m_Rect.NormalizeRect();

	m_bModified = true;
	return this;
}

void CGeom::Rectify(CTin& tin)
{
	CPoint point;

	point = CPoint(m_Rect.left, m_Rect.top);
	const CPoint point1 = tin.Rectify(point);

	point = CPoint(m_Rect.right, m_Rect.top);
	const CPoint point2 = tin.Rectify(point);

	point = CPoint(m_Rect.right, m_Rect.bottom);
	const CPoint point3 = tin.Rectify(point);

	point = CPoint(m_Rect.left, m_Rect.bottom);
	const CPoint point4 = tin.Rectify(point);

	m_Rect = FPointRect(point1, point2, point3, point4);
	m_Rect.NormalizeRect();

	m_bModified = true;
}
#ifdef _DEBUG
void CGeom::AssertValid() const
{
	ASSERT(m_Rect.left<=m_Rect.right);
	ASSERT(m_Rect.top<=m_Rect.bottom);
}
#endif

//CGeom* CGeom::Sculpt(const Clipper2Lib::Path64& clipper, const Clipper2Lib::ClipType& cliptype) const
//{
////	const Clipper2Lib::Paths<int> polygons = this->GetPolygons();
////	if(polygons.size() == 0)
////		return nullptr;
////
////	Clipper2Lib::Clipper<int> c;
////	c.AddPolygons(clipper, Clipper2Lib::PathType::Clip);
////	Clipper2Lib::Path<int> solution;
////	c.AddPolygons(polygons, Clipper2Lib::PathType::Subject);
////	if(c.Execute(cliptype, solution) == true)
////	{
////		if(solution.size() == 1)
////		{
////			Clipper2Lib::Path<int> polygon = *solution.begin();
////			CPoly* poly = new CPoly();
////			CGeom::CopyTo(poly);
////			poly->m_nAnchors = poly->m_nAllocs = polygon.size();
////			poly->m_pPoints = new CPoint[poly->m_nAnchors];
////			int index = 0;
////			for(std::vector<IntPoint>::iterator it = polygon.begin(); it != polygon.end(); it++)
////			{
////				const IntPoint point = *it;
////				poly->m_pPoints[index].x = point.X;
////				poly->m_pPoints[index].y = point.Y;
////				index++;
////			}
////			poly->RecalRect();
////			return poly;
////		}
////		else if(solution.size() > 1)
////		{
////			CCompound* pCompound = new CCompound();
////			CGeom::CopyTo(pCompound);
////			for(std::vector<Clipper2Lib::Path<int>>::iterator it1 = solution.begin(); it1 != solution.end(); it1++)
////			{
////				Clipper2Lib::Path<int> polygon = *it1;
////				CPoly* poly = new CPoly();
////				CGeom::CopyTo(poly);
////				poly->m_nAnchors = poly->m_nAllocs = polygon.size();
////				poly->m_pPoints = new CPoint[poly->m_nAnchors];
////				int index = 0;
////				for(std::vector<IntPoint>::iterator it = polygon.begin(); it != polygon.end(); it++)
////				{
////					const IntPoint point = *it;
////					poly->m_pPoints[index].x = point.X;
////					poly->m_pPoints[index].y = point.Y;
////					index++;
////				}
////				poly->RecalRect();
////				pCompound->AddMember(poly);
////			}
////			pCompound->RecalRect();
////			return pCompound;
////		}
////		else
////			return nullptr;
////	}
////	else
//		return nullptr;
//}

std::list<std::tuple<int, int, CGeom*>> CGeom::Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const
{
	int minX = m_Rect.left/cxTile;
	int maxX = m_Rect.right/cxTile;
	int minY = m_Rect.top/cyTile;
	int maxY = m_Rect.bottom/cyTile;
	maxX = m_Rect.right%cxTile==0 ? maxX-1 : maxX;
	maxY = m_Rect.bottom%cyTile==0 ? maxY-1 : maxY;

	std::list<std::tuple<int, int, CGeom*>> cellgeoms;
	if(maxX<=minX&&maxY<=minY)
		cellgeoms.push_back(make_tuple(minX, minY, this->Clone()));
	else
	{
		for(int row = minY; row<=maxY; row++)
		{
			for(int col = minX; col<=maxX; col++)
			{
				CGeom* pGeom = this->Clone();
				cellgeoms.push_back(make_tuple(col, row, pGeom));
			}
		}
	}
	return cellgeoms;
}

CGeom* CGeom::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(in==true&&Util::Rect::Intersect(m_Rect, bound)==false)
		return nullptr;
	else if(in==false&&Util::Rect::Intersect(m_Rect, bound)==false)
		return this;
	else
	{
		Clipper2Lib::Path64 path(5);
		path[0].x = m_Rect.left;
		path[0].y = m_Rect.top;
		path[1].x = m_Rect.right;
		path[1].y = m_Rect.top;
		path[2].x = m_Rect.right;
		path[2].y = m_Rect.bottom;
		path[3].x = m_Rect.left;
		path[3].y = m_Rect.bottom;
		path[4].x = m_Rect.left;
		path[4].y = m_Rect.top;
		Clipper2Lib::Paths64 subjects;
		subjects.push_back(path);

		Clipper64 clipper;
		clipper.AddClip(clips);
		clipper.AddSubject(subjects);
		Paths64 dummy;
		return (clipper.Execute(in ? ClipType::Intersection : ClipType::Difference, FillRule::NonZero, dummy)&&dummy.size()>0) ? this : nullptr;
	}
}

bool CGeom::Query(CString strKey)
{//22786c++
	if(strKey==_T('*'))
		return true;

	CString strId;
	strId.Format(_T("%d"), m_geoId);
	if(strId==strKey)
		return true;

	if(m_strName.IsEmpty()==TRUE)
		return false;

	CString string = m_strName;
	string.Replace(_T("\r\n"), _T(" "));
	//	string.Replace(_T("\r\n"),nullptr);
	//	string.Replace(_T(" "),nullptr);
	//	string.Replace(_T("¡¡"),nullptr);
	string.MakeUpper();

	return QueryRecursion(string, strKey);
}

bool CGeom::QueryRecursion(const CString& string, CString strKey)
{
	if(string.IsEmpty()==TRUE)
		return false;

	int pos = strKey.Find(_T('|'));
	if(pos!=-1)
	{
		while(pos!=-1)
		{
			CString strLeft = strKey.Left(pos);
			CString strRight = strKey.Mid(pos+1);
			strLeft.Trim();
			strRight.Trim();
			if(QueryRecursion(string, strLeft)==true)
				return true;
			if(QueryRecursion(string, strRight)==true)
				return true;
		}

		return false;
	}
	else
	{
		pos = strKey.FindOneOf(_T("+& "));
		while(pos!=-1)
		{
			CString str = strKey.Left(pos);
			str.Trim();
			if(string.Find(str)==-1)
				return false;

			strKey = strKey.Mid(pos+1);
			strKey.Trim();
			pos = strKey.FindOneOf(_T("+& "));
		}

		return string.Find(strKey)!=-1 ? true : false;
	}
}

inline float CGeom::GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const
{
	const float fPts = m_pLine!=nullptr ? m_pLine->m_nWidth*0.05 : (pLine!=nullptr ? pLine->m_nWidth*0.05 : 0.f);// /20.f
	return fPts*ratioLine;
}

void CGeom::RecalRect()
{
}
void CGeom::GatherFonts(std::list<CStringA>& fontlist, bool bEnglish) const
{
	if(m_pHint != nullptr)
	{
		const CStringA strFont = m_pHint->m_fontdesc.GetRealName();
		if(strFont.IsEmpty() == FALSE && std::find(fontlist.begin(), fontlist.end(), strFont) == fontlist.end())
		{
			fontlist.push_back(strFont);
		}
	}
}
void CGeom::ExportPlaintext(FILE* file, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid()==false)
		return;

	_ftprintf(file, _T("Id:%u ATT:%u Type:%d C:%d %s\n"), m_geoId, m_attId, m_bType, m_bClosed, m_strName);
	if(m_bClosed==true)
		this->ExportPts(file, matrix, _T("EndGeom"));
	else
		this->ExportPts(file, matrix, _T("EndGeom"));
}

void CGeom::ExportPdfTag(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(this->IsValid()==false)
		return;

	if(m_pTag!=nullptr&&pHint!=nullptr)
	{
		HALIGN hAlign;
		VALIGN vAlign;
		const Gdiplus::PointF point = this->GetTagAnchor(viewinfo, hAlign, vAlign);
		m_pTag->ExportPdf(pdf, page, viewinfo, point, pHint);
	}
}

void CGeom::ExportMid(FILE* fileMid) const
{
	if(this->IsValid()==false)
		return;

	CString string = m_strName;
	string.Replace(_T("\r\n"), _T(" "));
	//	string.Replace(_T("\r\n"),nullptr);
	//	string.Replace(_T(" "),nullptr);
	//	string.Replace(_T("¡¡"),nullptr);
	fprintf(fileMid, "%d,\"%s\"\n", m_geoId, string);
}

double CGeom::Shortcut(const CPoint& point, CPoint& trend) const
{
	if(m_Rect.PtInRect(point)==TRUE)
	{
		return 0;
	}
	else
	{
		if(point.x<m_Rect.left)
		{
			trend.x = m_Rect.left;
			trend.y = point.y;
			if(point.y>m_Rect.bottom)
				trend.y = m_Rect.bottom;
			else if(point.y<m_Rect.top)
				trend.y = m_Rect.top;
		}
		else if(point.x>m_Rect.right)
		{
			trend.x = m_Rect.right;
			trend.y = point.y;
			if(point.y>m_Rect.bottom)
				trend.y = m_Rect.bottom;
			else if(point.y<m_Rect.top)
				trend.y = m_Rect.top;
		}

		return abs(::LineLength1(point, CPoint(trend.x, trend.y)));
	}
}

CPoint CGeom::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& angle, const CPoint& point)
{
	double Δλ = deltaLng*constants::degreeToradian;
	double Δφ = deltaLat*constants::degreeToradian;
	CPointF mapPoint = mapinfo.DocToMap(point);
	if(mapinfo.m_pProjection!=nullptr)
	{
		CPointF geoPoint = mapinfo.m_pProjection->MapToGeo(mapPoint);
		double λ = geoPoint.x;
		double ψ = geoPoint.y;
		if(Δλ!=0.f)
		{
			λ += Δλ;
			Lnglat::Regularizeλ(λ);
		}
		if(Δφ!=0)
		{
			const double cosΔφ = cos(Δφ);
			const double sinΔφ = sin(Δφ);
			const double cosγ = cos(angle*constants::degreeToradian);
			const double sinγ = sin(angle*constants::degreeToradian);

			const double cosψ = cos(ψ);
			const double x = cos(λ)*cosψ;
			const double y = sin(λ)*cosψ;
			const double z = sin(ψ);
			const double k = z*cosΔφ+x*sinΔφ;
			λ = atan2(y*cosγ-k*sinγ, x*cosΔφ-z*sinΔφ);
			ψ = asin(k*cosγ+y*sinγ);
			Lnglat::Regularizeλ(λ);
			mapPoint = mapinfo.m_pProjection->GeoToMap(λ, ψ);
		}
		return mapinfo.MapToDoc(mapPoint);
	}
	else
		return point;
}

Gdiplus::Rect CGeom::Transform(const Gdiplus::Matrix& matrix, CRect rect)
{
	Gdiplus::Point point1 = Gdiplus::Point(rect.left, rect.top);
	Gdiplus::Point point2 = Gdiplus::Point(rect.right, rect.bottom);

	matrix.TransformPoints(&point1);
	matrix.TransformPoints(&point2);
	const int minX = min(point1.X, point2.X);
	const int minY = min(point1.Y, point2.Y);
	const int maxX = max(point1.X, point2.X);
	const int maxY = max(point1.Y, point2.Y);

	return Gdiplus::Rect(minX, minY, maxX-minX, maxY-minY);
}

Gdiplus::PointF* CGeom::Transform(const Gdiplus::Matrix& matrix, CPoint* pPoints, int count)
{
	Gdiplus::PointF* points = new Gdiplus::PointF[count];
	for(int index = 0; index<count; index++)
	{
		points[index].X = pPoints[index].x;
		points[index].Y = pPoints[index].y;
	}
	matrix.TransformPoints(points, count);
	return points;
}

void CGeom::Serializelist(CArchive& ar, const DWORD dwVersion, CObList& oblist)
{
	if(ar.IsStoring())
	{
		ar<<(int)oblist.GetCount();
		POSITION pos = oblist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = (CGeom*)oblist.GetNext(pos);
			const BYTE type = pGeom->Gettype();
			ar<<type;
			pGeom->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int nCount;
		ar>>nCount;
		for(int index = 0; index<nCount; index++)
		{
			BYTE type;
			ar>>type;
			CGeom* pGeom = CGeom::Apply(type);
			if(pGeom!=nullptr)
			{
				pGeom->Serialize(ar, dwVersion);
				oblist.AddTail(pGeom);
			}
		}
	}
}

Gdiplus::PathGradientBrush* CGeom::SetGradientBrush(RadialBrush* radial, const Gdiplus::Matrix& matrix, Gdiplus::GraphicsPath* path) const
{
	return nullptr;
}

Gdiplus::PathGradientBrush* CGeom::SetGradientBrush(RadialBrush* radial, const CViewinfo& viewinfo, Gdiplus::GraphicsPath* path) const//because Gdiplus::PathGradientBrush dosn't support setpath we have to recreate a new one
{
	if(radial==nullptr)
		return nullptr;


	Gdiplus::Point center;
	switch(radial->m_centermode)//m_centermode
	{
		case CFillRadial::CENTERMODE::Centroid:
			{
				const CPoint centroid = CGeom::GetGeogroid();
				center = viewinfo.DocToClient<Gdiplus::Point>(centroid);
			}
			break;
		case CFillRadial::CENTERMODE::Geomcenter:
			{
				center = viewinfo.DocToClient<Gdiplus::Point>(m_Rect.CenterPoint());
			}
			break;
		case CFillRadial::CENTERMODE::CenterPercent:
			{
				center = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.left+m_Rect.Width()*radial->m_cx, m_Rect.top+m_Rect.Height()*radial->m_cy));
			}
			break;
		case CFillRadial::CENTERMODE::CenterOffset:
			center = viewinfo.DocToClient<Gdiplus::Point>(CPoint(m_Rect.left+radial->m_cx, m_Rect.top+radial->m_cy));
			break;
		case CFillRadial::CENTERMODE::CenterCoordinate:
			center = viewinfo.DocToClient <Gdiplus::Point>(CPoint(radial->m_cx, radial->m_cy));
			break;
		default:
			{
				center = viewinfo.DocToClient <Gdiplus::Point>(m_Rect.CenterPoint());
			}
			break;
	};

	const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(m_Rect);
	Gdiplus::GraphicsPath brushpath(Gdiplus::FillMode::FillModeAlternate);
	switch(radial->m_radiusmode)//m_radiusmode
	{
		case CFillRadial::RADIUSMODE::OUTERELLIPSE:
			{
				const int A = ceil(rect.Width/sqrt(2.f));
				const int B = ceil(rect.Height/sqrt(2.f));

				const Gdiplus::Rect brushrect(center.X-A, center.Y-B, 2*A, 2*B);
				brushpath.AddEllipse(brushrect);
				path = &brushpath;
			}
			break;
		case CFillRadial::RADIUSMODE::INNERELLIPSE:
			{
				brushpath.AddEllipse(rect);
				path = &brushpath;
			}
			break;
		case CFillRadial::RADIUSMODE::OUTERCIRCLE:
			{
				const int A = ceil(rect.Width/sqrt(2.f));
				const int B = ceil(rect.Height/sqrt(2.f));
				const int R = max(A, B);

				const Gdiplus::Rect brushrect(center.X-R, center.Y-R, 2*R, 2*R);
				Gdiplus::GraphicsPath brushpath(Gdiplus::FillMode::FillModeAlternate);
				brushpath.AddEllipse(brushrect);
				path = &brushpath;
			}
			break;
		case CFillRadial::RADIUSMODE::INNERCIRCLE:
			{
				const int R = min(rect.Width/2, rect.Height/2);
				const Gdiplus::Rect brushrect(center.X-R, center.Y-R, 2*R, 2*R);

				Gdiplus::GraphicsPath brushpath(Gdiplus::FillMode::FillModeAlternate);
				brushpath.AddEllipse(brushrect);
				path = &brushpath;
			}
			break;
		case CFillRadial::RADIUSMODE::RadiusPercent:
			{
				const int A = rect.Width*radial->m_cr;
				const int B = rect.Height*radial->m_fr;

				const Gdiplus::Rect brushrect(center.X-A, center.Y-B, 2*A, 2*B);
				brushpath.AddEllipse(brushrect);
				path = &brushpath;
			}
			break;
		case CFillRadial::RADIUSMODE::RadiusValue:
			{
				const CPoint center = CPoint(rect.X+rect.Width/2, rect.Y+rect.Height/2);
				CRect rect(center, center);
				rect.InflateRect(radial->m_cr, radial->m_fr);
				const Gdiplus::Rect brushrect = viewinfo.DocToClient <Gdiplus::Rect>(rect);
				brushpath.AddEllipse(brushrect);
				path = &brushpath;
			}
			break;
		case CFillRadial::RADIUSMODE::OUTPATH:
		default:
			break;
	};
	switch(radial->m_focusmode)//m_focusmode // in svg focus point means center point of gdiplus
	{
		case CFillRadial::FOCUSMODE::CENTER:
			break;
		case CFillRadial::FOCUSMODE::FocusPercent:
			{
				center.X = rect.X+rect.Width*radial->m_fx;
				center.Y = rect.Y+rect.Height*radial->m_fy;
			}
			break;
		case CFillRadial::FOCUSMODE::FocusOffset:
			center = viewinfo.DocToClient <Gdiplus::Point>(CPoint(rect.X+radial->m_fx, rect.Y+radial->m_fy));
			break;
		case CFillRadial::FOCUSMODE::FocusCoordinate:
			center = viewinfo.DocToClient <Gdiplus::Point>(CPoint(radial->m_fx, radial->m_fy));
			break;
	}


	Gdiplus::PathGradientBrush* brush = ::new Gdiplus::PathGradientBrush(path);
	brush->SetGammaCorrection(TRUE);
	brush->SetCenterPoint(center);
	const Gdiplus::Color color1 = radial->m_stops.begin()->second->GetColor();
	const Gdiplus::Color color2 = radial->m_stops.rbegin()->second->GetColor();
	brush->SetCenterColor(color1);
	const Gdiplus::Color colors1[] = {color2};
	int count = 1;
	brush->SetSurroundColors(colors1, &count);

	const int size = radial->m_stops.size();
	if(size>2)
	{

		float* positions = new float[size];
		Gdiplus::Color* colors2 = new Gdiplus::Color[size];
		std::map<unsigned int, CColor*>::iterator it = radial->m_stops.begin();
		for(int index = 0; index<size; index++)
		{
			positions[size-index-1] = 1-it->first/100.f;
			colors2[size-index-1] = it->second->GetColor();
			++it;
		}
		brush->SetInterpolationColors(colors2, positions, size);
	}
	return brush;
}

Gdiplus::LinearGradientBrush* CGeom::SetGradientBrush(LinearBrush* linear, const CViewinfo& viewinfo) const
{
	if(linear==nullptr)
		return nullptr;

	Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(m_Rect);
	Gdiplus::Point point1(0, 0);
	Gdiplus::Point point2(0, 0);
	switch(linear->m_valuemode)//valuemode
	{
		case CFillLinear::VALUEMODE::Angle:
			{
				const Gdiplus::Point origin(rect.GetLeft(), rect.GetTop());
				rect.Offset(-origin.X, -origin.Y);
				if(linear->m_nAngle>=0&&linear->m_nAngle<90)
				{
					const double tg = tan(linear->m_nAngle*M_PI/constants::pi);
					point1 = Gdiplus::Point(0, 0);
					point2 = Gdiplus::Point(rect.GetRight(), rect.Width*tg);
				}
				else if(linear->m_nAngle>=90&&linear->m_nAngle<constants::pi)
				{
					const double tg = tan((constants::pi-linear->m_nAngle)*M_PI/constants::pi);
					point1 = Gdiplus::Point(rect.GetRight(), 0);
					point2 = Gdiplus::Point(0, rect.Width*tg);
				}
				else if(linear->m_nAngle>=constants::pi&&linear->m_nAngle<270)
				{
					const double tg = tan((linear->m_nAngle-constants::pi)*M_PI/constants::pi);
					point1 = Gdiplus::Point(rect.GetRight(), rect.GetBottom());
					point2 = Gdiplus::Point(0, rect.Width*tg);
				}
				else if(linear->m_nAngle>=270&&linear->m_nAngle<constants::twopi)
				{
					const double tg = tan((linear->m_nAngle-constants::pi)*M_PI/constants::pi);
					point1 = Gdiplus::Point(0, rect.GetBottom());
					point2 = Gdiplus::Point(rect.GetRight(), rect.Width*tg);
				}
				point1 = point1+origin;
				point2 = point2+origin;
				rect.Offset(origin.X, origin.Y);
			}
			break;
		case CFillLinear::VALUEMODE::VALUEPercent:
			{
				point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_Rect.left+m_Rect.Width()*linear->m_x1, m_Rect.top+m_Rect.Height()*linear->m_y1));
				point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_Rect.left+m_Rect.Width()*linear->m_x2, m_Rect.top+m_Rect.Height()*linear->m_y2));
			}
			break;
		case CFillLinear::VALUEMODE::VALUEOffset:
			{
				point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_Rect.left+linear->m_x1, m_Rect.top+linear->m_y1));
				point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_Rect.left+linear->m_x2, m_Rect.top+linear->m_y2));
			}
			break;
		case CFillLinear::VALUEMODE::VALUEActual:
		default:
			{
				point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(linear->m_x1, linear->m_y1));
				point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(linear->m_x2, linear->m_y2));
			}
			break;
	}
	CStraightLine<int> straightline(point1.X, point1.Y, point2.X, point2.Y);
	int angle = straightline.GetAngle()+constants::pi;
	while(angle>constants::pi)
		angle -= constants::pi;
	CStraightLine<int>* perpendicularline1 = nullptr;
	CStraightLine<int>* perpendicularline2 = nullptr;
	if(angle>=0&&angle<=90)
	{
		perpendicularline1 = straightline.GetPerpendicular(rect.GetLeft(), rect.GetTop());
		perpendicularline2 = straightline.GetPerpendicular(rect.GetRight(), rect.GetBottom());
	}
	else
	{
		perpendicularline1 = straightline.GetPerpendicular(rect.GetLeft(), rect.GetBottom());
		perpendicularline2 = straightline.GetPerpendicular(rect.GetRight(), rect.GetTop());
	}
	Gdiplus::Point inter1 = point1;
	Gdiplus::Point inter2 = point2;
	straightline.IntersectWith(*perpendicularline1, inter1.X, inter1.Y);
	straightline.IntersectWith(*perpendicularline2, inter2.X, inter2.Y);
	double t1 = 0.0;
	double t2 = 1.0;
	PointOnLine(inter1.X, inter1.Y, point1.X, point1.Y, point2.X, point2.Y, 10000, t1);
	PointOnLine(inter2.X, inter2.Y, point1.X, point1.Y, point2.X, point2.Y, 10000, t2);
	std::map<double, Gdiplus::Point> points;
	points[0] = point1;
	points[t1] = inter1;
	points[t2] = inter2;
	points[1] = point2;

	const Gdiplus::Point final1 = points.begin()->second;
	const Gdiplus::Point final2 = points.rbegin()->second;
	Gdiplus::LinearGradientBrush* local = ::new Gdiplus::LinearGradientBrush(final1, final2, linear->m_stops.begin()->second->GetColor(), linear->m_stops.rbegin()->second->GetColor()); //gdiplus and svg don't support the start and end point yet
	const int count = linear->m_stops.size();

	Gdiplus::Color* colors2 = new Gdiplus::Color[count];
	Gdiplus::REAL* positions = new Gdiplus::REAL[count];
	int index = 0;
	for(const auto& pair:linear->m_stops)
	{
		positions[index] = pair.first/100.f;
		colors2[index] = pair.second->GetColor();
		index++;
	}

	local->SetInterpolationColors(colors2, positions, count);
	delete[] colors2;
	delete[] positions;
	//int newcount = linear->m_stops.size();
		//if(final1.Equals(point1)==FALSE)
		//	newcount += 1;
		//if(final2.Equals(point2)==FALSE)
		//	newcount += 1;

		//Gdiplus::Color* colors2 = new Gdiplus::Color[newcount];
		//Gdiplus::REAL* positions = new Gdiplus::REAL[newcount];
		//if(final1.Equals(point1)==FALSE||final2.Equals(point2)==FALSE)
		//{
		//	const double len1 = ::LineLength1(final1, point1);
		//	const double len2 = ::LineLength1(point1, point2);
		//	const double len3 = ::LineLength1(point2, final2);
		//	for(int index = 0; index<linear->m_stops.size(); index++)
		//	{
		//		positions[index] = (len1+positions[index]*len2)/(len1+len2+len3);
		//	}
		//}
		//if(final1.Equals(point1)==FALSE)
		//{
		//	for(int index = nCount-1; index>=0; index--)
		//	{
		//		positions[index+1] = positions[index];
		//		colors2[index+1] = colors2[index];
		//	}
		//	positions[0] = 0;
		//	colors2[0] = colors2[0];
		//}
		//if(final2.Equals(point2)==FALSE)
		//{
		//	positions[newcount-1] = 1;
		//	colors2[newcount-1] = colors2[newcount-1-1];
		//}
	return local;
}

Gdiplus::LinearGradientBrush* CGeom::SetGradientBrush(LinearBrush* linear, const Gdiplus::Matrix& matrix) const
{
	return nullptr;
}