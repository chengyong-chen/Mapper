#include "Stdafx.h"
#include "ImportJson.h"
#include "FieldsPickerDlg.h"

#include "../Image/Dib.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/Text.h"
#include "../Style/ColorSpot.h"
#include "../Layer/LayerTree.h"
#include "../Database/Datasource.h"

#include <ogr_core.h>
#include <ogr_spatialref.h>
#include "ImportGdal.h"
#include <cstring>
#include <fstream>
#include <regex>
#include <cmath>
#include "../Projection/Projection1.h"
#include "../Topology/RegionTopoer.h"
#include "../Topology/Polygon.h"
#include "../Topology/Polying.h"
#include "../Topology/Layon.h"
#include "../DataView/DatainfoDlg.h"

#include "../../Thirdparty/GDAL/3.3.3/gcore/gdal.h"
#include "../../Thirdparty/GDAL/3.3.3/gcore/gdal_priv.h"
#include "../../Thirdparty/GDAL/3.3.3/ogr/ogr_feature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace boost;
using namespace std;
double ReadAsDouble(boost::json::value& value)
{
	if(value.is_double())
		return value.as_double();
	else if(value.is_int64())
		return value.as_int64();
	else if(value.is_uint64())
		return value.as_uint64();
	else
		return 0;
}
int ReadAsInt(boost::json::value& value)
{
	if(value.is_int64())
		return value.as_int64();
	else if(value.is_uint64())
		return value.as_uint64();
	else if(value.is_string())
	{
		const char* id = value.as_string().c_str();
		return atoi(id);
	}
}
const char* ReadAsString(boost::json::value& value)
{
	if(value.is_string())
		return  value.as_string().c_str();
	else
		return nullptr;
}

CPolying* ReadPolying(CRegionTopoer* pTopo, boost::json::array arcs)
{
	if(arcs.empty())
		return nullptr;

	CPolying* pPolying = new CPolying(*pTopo);
	for(boost::json::value& arc:arcs)
	{
		int dwEdgeId = ReadAsInt(arc);
		pPolying->m_edgelist.Add(dwEdgeId);
	}
	pPolying->RecalRect();
	return pPolying;
}
CPolygon* ReadPolygon(CRegionTopoer* pTopo, boost::json::array arcs)
{
	if(arcs.empty())
		return nullptr;

	CPolygon* polygon = new CPolygon(*pTopo);
	for(boost::json::value& arc:arcs)
	{
		int dwEdgeId = ReadAsInt(arc);
		polygon->m_edgelist.Add(dwEdgeId);
	}
	polygon->RecalRect();
	return polygon;
}
CImportJson::CImportJson(CDatainfo& datainfo, CLayerTree& layertree, std::list<CTopology*>& topolist, DWORD& dwMaxGeomID)
	: CImportGdal(datainfo, layertree, dwMaxGeomID), m_topolist(topolist)
{
}

CImportJson::~CImportJson()
{
}
BOOL CImportJson::Import(CString& strFile, CString& dbname, bool into)
{
	std::ifstream file(strFile);
	if(file.is_open()==false)
		return FALSE;

	const std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	boost::json::value jroot = boost::json::parse(contents);
	boost::json::object jbody = jroot.as_object();
	if(jbody.empty()==true)
		return FALSE;

	const char* type = jbody.at("type").as_string().c_str();
	if(strcmp(type, "Topology")==0)
	{
		CProjection* pProjection = GetProjection(strFile);
		CString strPath = strFile.Left(strFile.ReverseFind(_T('\\')));
		CString strName = strFile.Mid(strFile.ReverseFind(_T('\\'))+1);
		strName = strName.Left(strName.Find(_T('.')));
		boost::json::value jtitle = jbody.contains("title") ? jbody.at("title") : nullptr;
		if(jtitle!=nullptr)
		{
			const char* title = jtitle.as_string().c_str();
			m_datainfo.m_strName = title;
		}
		else
			m_datainfo.m_strName = strName;
		

		boost::json::object jtransform = jbody.at("transform").as_object();
		if(jtransform.contains("default"))
		{
			boost::json::object defaults = jbody.at("default").as_object();

			boost::json::array	scale = defaults.at("scale").as_array();
			m_scaleX = ReadAsDouble(scale.at(0));
			m_scaleY = ReadAsDouble(scale.at(1));
			boost::json::array translate = defaults.at("translate").as_array();
			m_fXOffset = -ReadAsDouble(translate.at(0));
			m_fYOffset = -ReadAsDouble(translate.at(1));
			m_datainfo.m_mapOrigin.x = m_fXOffset;
			m_datainfo.m_mapOrigin.y = m_fYOffset;
		}
		else
		{
			if(jtransform.contains("scale"))
			{
				boost::json::array jscales = jtransform.at("scale").as_array();
				m_scaleX = ReadAsDouble(jscales.at(0));
				m_scaleY = ReadAsDouble(jscales.at(1));
			}
			if(jtransform.contains("translate"))
			{
				boost::json::array jtranslates = jtransform.at("translate").as_array();
				m_fXOffset = -ReadAsDouble(jtranslates.at(0));
				m_fYOffset = -ReadAsDouble(jtranslates.at(1));
				m_datainfo.m_mapOrigin.x = m_fXOffset;
				m_datainfo.m_mapOrigin.y = m_fYOffset;
			}
		}

		if(hiddenlevel<0)
		{
			const HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
			AfxSetResourceHandle(hInst);

			CDatainfoDlg dlg(nullptr, m_datainfo);
			if(dlg.DoModal()!=IDOK)
			{
				AfxSetResourceHandle(hInstOld);
				::FreeLibrary(hInst);
				return FALSE;
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		m_fXScale = m_datainfo.m_dilationMapToDoc;
		m_fYScale = m_datainfo.m_dilationMapToDoc;
	//	m_fXScale = 10000000;//m_dilationMapToDoc is from radian to doc
	//	m_fYScale = 10000000;//need to be more smart, could be meter(100)

		CLayon* layon1 = new CLayon(m_layertree);
		layon1->m_wId = m_layertree.ApplyId();
		layon1->m_bVisible = true;
		layon1->m_strName = _T("polygons");
		layon1->m_bCentroid = true;
		layon1->m_pLine = new CLine(new CColorSpot(128, 128, 128, 255), 1);
		layon1->m_bDynamic |= LineDynamic;
		m_layertree.m_root.AddChild(nullptr, layon1);

		CLayon* layon2 = new CLayon(m_layertree);
		layon2->m_wId = m_layertree.ApplyId();
		layon2->m_bVisible = true;
		layon2->m_strName = _T("linestrings");
		layon2->m_pLine = new CLine(new CColorSpot(0, 0, 0, 255), 5);
		layon2->m_bDynamic |= LineDynamic;
		m_layertree.m_root.AddChild(nullptr, layon2);

		CLayer* layer = new CLayer(m_layertree, m_datainfo.m_scaleMinimum, m_datainfo.m_scaleMaximum, m_datainfo.m_scaleMinimum, m_datainfo.m_scaleMaximum);
		layer->m_wId = m_layertree.ApplyId();
		layer->m_bVisible = false;
		layer->m_strName = _T("arcs");
		m_layertree.m_root.AddChild(nullptr, layer);

		CLayer* backup = new CLayer(m_layertree, m_datainfo.m_scaleMinimum, m_datainfo.m_scaleMaximum, m_datainfo.m_scaleMinimum, m_datainfo.m_scaleMaximum);
		backup->m_wId = m_layertree.ApplyId();
		backup->m_bVisible = true;
		backup->m_strName = _T("backup");
		backup->m_pLine = new CLine(new CColorSpot(0, 0, 0, 255), 5);
		m_layertree.m_root.AddChild(nullptr, backup);

		std::list<CLayon*> layonlist;
		layonlist.push_back(layon1);
		layonlist.push_back(layon2);
		CRegionTopoer* pTopo = new CRegionTopoer(m_layertree, layonlist);
		pTopo->m_strName = strName;
		layon1->m_pRegionTopoer = pTopo;
		layon2->m_pRegionTopoer = pTopo;

		boost::json::array jarcs = jbody.at("arcs").as_array();
		for(auto& jarc:jarcs)
		{
			boost::json::array arrayarc = jarc.as_array();
			CPoly* pPoly = ReadPoly(arrayarc);
			pPoly->m_geoId = ++m_dwMaxGeomId;
			layer->m_geomlist.AddTail(pPoly);
			pTopo->AddAEdge(layer, pPoly);
		}
		boost::json::object jobjects = jbody.at("objects").as_object();
		if(jobjects.empty()==false)
		{
			CDaoDatabase* pMDBDataBase = CImportGdal::CreateDatabase(strPath, strName);
			dbname = strName;

			for(boost::json::object::iterator it = jobjects.begin(); it!=jobjects.end(); it++)
			{
				boost::json::object& jovalue = (*it).value().as_object();
				boost::json::string jotype = jovalue.at("type").as_string();
				if(strcmp(jotype.c_str(), "GeometryCollection")==0)
				{
					boost::json::array jgeoms = jovalue.at("geometries").as_array();

					CString strTable = CString(it->key_c_str());
					std::map<string, string> fieldtype;
					CString fieldAnno;
					CString fieldGeocode;
					CString fieldAid;
					CString fieldCentroidX;
					CString fieldCentroidY;
					CreateTable(jgeoms, pMDBDataBase, strTable, fieldtype, fieldAnno, fieldGeocode, fieldAid, fieldCentroidX, fieldCentroidY);
					layon1->EnableAttribute(strName);
					CATTHeaderProfile& headerprofile = layon1->GetAttDatasource()->GetHeaderProfile();
					headerprofile.m_strKeyTable = strTable;
					headerprofile.m_strTables = strTable;
					headerprofile.m_strAnnoField = fieldAnno;
					headerprofile.m_strCodeField = fieldGeocode;

					CString strSQL;
					strSQL.Format(_T("SELECT * FROM [%s]"), strTable);
					CDaoRecordset rs(pMDBDataBase);
					rs.Open(dbOpenDynaset, strSQL);
					for(boost::json::value& jgeom:jgeoms)
					{
						boost::json::object jbgeom = jgeom.as_object();
						boost::json::value* jid = jbgeom.if_contains("id");
						const char* geocode = jid!=nullptr ? ReadAsString(*jid) : nullptr;
						CGeom* pGeom = nullptr;
						CPolygon* pPolygon = nullptr;
						CGonroup* pGonroup = nullptr;
						CPolying* pPolying = nullptr;
						CIngroup* pIngroup = nullptr;
						if(strcmp(jbgeom.at("type").as_string().c_str(), "Polygon")==0)
						{
							pPolygon = ReadPolygon(pTopo, jbgeom.at("arcs").as_array().at(0).as_array());
							pPolygon->m_geoId = ++m_dwMaxGeomId;
							pPolygon->m_geoCode = geocode;
							layon1->m_geomlist.AddTail(pPolygon);
							pGeom = pPolygon;
						}
						else if(strcmp(jbgeom.at("type").as_string().c_str(), "MultiPolygon")==0)
						{
							pGonroup = new CGonroup(*pTopo);
							pGonroup->m_geoId = ++m_dwMaxGeomId;
							pGonroup->m_geoCode = geocode;
							for(boost::json::value& arcs:jbgeom.at("arcs").as_array())
							{
								CPolygon* pPolygon1 = ReadPolygon(pTopo, arcs.as_array().at(0).as_array());
								delete pPolygon1->m_pFill;
								pPolygon1->m_pFill = nullptr;
								pGonroup->m_geomlist.AddTail(pPolygon1);
							}
							pGonroup->RecalRect();
							pGonroup->OrderByArea();
							layon1->m_geomlist.AddTail(pGonroup);

							pGeom = pGonroup;
						}
						else if(strcmp(jbgeom.at("type").as_string().c_str(), "LineString")==0)
						{
							if(jbgeom.at("arcs").is_null()==false)
							{
								pPolying = ReadPolying(pTopo, jbgeom.at("arcs").as_array());
								pPolygon->m_geoId = ++m_dwMaxGeomId;
								pPolying->m_geoCode = geocode;
								layon2->m_geomlist.AddTail(pPolying);
								pGeom = pPolying;
							}
							else if(jbgeom.at("coordinates").is_null()==false)
							{
								CPoly* pPoly = ReadPoly(jbgeom.at("coordinates").as_array());
								if(pPoly!=nullptr)
								{
									backup->m_geomlist.AddTail(pPoly);
								}
							}
							else
								continue;
						}
						else if(strcmp(jbgeom.at("type").as_string().c_str(), "MultiLineString")==0)
						{
							pIngroup = new CIngroup(*pTopo);
							pIngroup->m_geoId = ++m_dwMaxGeomId;
							pIngroup->m_geoCode = geocode;
							for(boost::json::value& arcs:jbgeom.at("arcs").as_array())
							{
								CPolying* pPolying1 = ReadPolying(pTopo, arcs.as_array());
								pIngroup->m_geomlist.AddTail(pPolying1);
							}
							pIngroup->RecalRect();
							layon2->m_geomlist.AddTail(pIngroup);

							pGeom = pIngroup;
						}
						else
							continue;

						boost::json::value* jproperties = jbgeom.if_contains("properties");
						if(jproperties!=nullptr)
						{
							rs.AddNew();

							double x = 0;
							double y = 0;
							boost::json::object properties = jproperties->as_object();
							for(boost::json::object::iterator it = properties.begin(); it!=properties.end(); it++)
							{
								std::string field = it->key_c_str();
								field = std::regex_replace(field, std::regex("^hc-"), "");
								boost::json::value value = it->value();
								CString strFiled = CString(field.c_str());
								if(value.is_null())
								{
								}
								else if(fieldtype[field]=="string")
								{
									rs.SetFieldValue(strFiled, COleVariant(CString(value.as_string().c_str()), VT_BSTRT));

									if(fieldAnno.IsEmpty())
									{
									}
									else if(fieldAnno.CompareNoCase(strFiled)==0)
									{
										pGeom->m_strName = CString(value.as_string().c_str());
									}
									if(geocode!=nullptr)
									{
									}
									else if(fieldGeocode.IsEmpty())
									{
									}
									else if(fieldGeocode.CompareNoCase(strFiled)==0)
									{
										pGeom->m_geoCode = CString(value.as_string().c_str());
									}
								}
								else if(fieldtype[field]=="double")
								{

									rs.SetFieldValue(strFiled, COleVariant(value.to_number<double>()));

									if(fieldCentroidX.IsEmpty())
									{

									}
									else if(fieldCentroidY.IsEmpty())
									{

									}
									else if(strFiled.CompareNoCase(fieldCentroidX)==0)
									{
										x = value.to_number<double>();
									}
									else if(strFiled.CompareNoCase(fieldCentroidY)==0)
									{
										y = value.to_number<double>();
									}
								}
								else if(fieldtype[field]=="int")
								{
									rs.SetFieldValue(strFiled, COleVariant((long)value.as_int64(), VT_I4));
									if(fieldAid==strFiled)
									{
										pGeom->m_attId = (long)value.as_int64();
									}
								}
							}

							rs.Update();
							if(pGeom->m_attId==0XFFFFFFFF)
							{
								COleVariant bookmark = rs.GetLastModifiedBookmark();
								rs.SetBookmark(bookmark);
								COleVariant varKey = rs.GetFieldValue(_T("AID"));
								pGeom->m_attId = varKey.lVal;
							}
							if(x!=0.0||y!=0.0)
							{
								if(fieldCentroidX.CompareNoCase(_T("middle-lon"))==0&&fieldCentroidY.CompareNoCase(_T("middle-lat"))==0)
								{
									if(pPolygon!=nullptr)
										pPolygon->m_centroid = m_datainfo.GeoToDoc(x, y);
									else if(pGonroup!=nullptr)
										pGonroup->m_geomlist.GetHead()->m_centroid = m_datainfo.GeoToDoc(x, y);
								}
								else
								{
									if(pPolygon!=nullptr)
										pPolygon->m_centroid = CPoint(x, y);
									else if(pGonroup!=nullptr)
										pGonroup->m_geomlist.GetHead()->m_centroid = CPoint(x, y);
								}
							}
						}
					}
					rs.Close();
				}
			}
			if(pMDBDataBase!=nullptr)
			{
				pMDBDataBase->Close();
				delete pMDBDataBase;
				pMDBDataBase = nullptr;
			}
		}

		m_topolist.push_back(pTopo);
		return CImport::Import(strFile);
	}
	else
		return CImportGdal::Import(strFile, dbname, into);
}
void CImportJson::CreateTable(boost::json::array jgeoms, CDaoDatabase* pMDBDataBase, CString strTable, std::map<string, string>& fieldtype, CString& fieldAnno, CString& fieldGeocode, CString& fieldAid, CString& fieldCentroidX, CString& fieldCentroidY)
{
	if(pMDBDataBase==nullptr)
		return;
	CDaoTableDefInfo tableInfo;
	long nTableCount = pMDBDataBase->GetTableDefCount();
	for(long I = 0; I<nTableCount; I++)
	{
		pMDBDataBase->GetTableDefInfo(I, tableInfo);
		if(strTable.CompareNoCase(tableInfo.m_strName)==0)
		{
			pMDBDataBase->DeleteTableDef(strTable);
			break;
		}
	}
	std::list<std::string> fields;
	std::map<string, int> fieldsize;
	std::list<std::list<CString>> samples;
	for(boost::json::value& jgeom:jgeoms)
	{
		boost::json::object jbgeom = jgeom.as_object();
		boost::json::value* jproperties = jbgeom.if_contains("properties");
		if(jproperties!=nullptr)
		{
			std::list<CString> sample;

			boost::json::object properties = jproperties->as_object();
			for(boost::json::object::iterator it = properties.begin(); it!=properties.end(); it++)
			{
				boost::json::key_value_pair& jpair = (*it);
				string field = std::string(jpair.key());

				if(std::find(fields.begin(), fields.end(), field)==fields.end())
					fields.push_back(field);

				boost::json::value jvalue = jpair.value();
				if(jvalue.is_null()) {
				}
				else if(jvalue.is_string())
				{
					fieldtype[field] = "string";
					if(fieldsize.contains(field)==false)
						fieldsize[field] = jvalue.as_string().size();
					else
						fieldsize[field] = max(fieldsize[field], jvalue.as_string().size());
				}
				else if(fieldtype[field]=="string")
					continue;
				else if(jvalue.is_double())
					fieldtype[field] = "double";
				else if(fieldtype[field]=="double")
				{
				}
				else if(jvalue.is_number())
					fieldtype[field] = "int";
				else
				{

				}
				if(jvalue.is_null())
					sample.push_back(CString());
				else if(jvalue.is_string())
					sample.push_back(CString(jvalue.as_string().c_str()));
				else
					sample.push_back(CString(boost::json::serialize(jvalue).c_str()));

			}
			if(samples.size()<20)
				samples.push_back(sample);
		}
	}

	try
	{
		CDaoTableDef td(pMDBDataBase);
		td.Create(strTable);

		for(std::list<string>::iterator it = fields.begin(); it!=fields.end(); it++)
		{
			std::string field = *it;
			if(fieldtype.contains(field)==false)
			{
				CDaoFieldInfo fieldInfo;
				fieldInfo.m_strName = CString(field.c_str());
				fieldInfo.m_nOrdinalPosition = 0;
				fieldInfo.m_bRequired = FALSE;
				fieldInfo.m_lCollatingOrder = 0;
				fieldInfo.m_nType = dbText;
				fieldInfo.m_lSize = fieldsize[field];
				fieldInfo.m_lAttributes = dbVariableField;
				fieldInfo.m_bAllowZeroLength = TRUE;
				td.CreateField(fieldInfo);
				continue;
			}

			std::string type = fieldtype[field];
			if(type=="string")
			{
				CDaoFieldInfo fieldInfo;
				fieldInfo.m_strName = CString(field.c_str());
				fieldInfo.m_nOrdinalPosition = 0;
				fieldInfo.m_bRequired = FALSE;
				fieldInfo.m_lCollatingOrder = 0;
				fieldInfo.m_nType = dbText;
				fieldInfo.m_lSize = fieldsize[field];
				fieldInfo.m_lAttributes = dbVariableField;
				fieldInfo.m_bAllowZeroLength = TRUE;
				td.CreateField(fieldInfo);
			}
			else if(type=="double")
			{
				CDaoFieldInfo fieldInfo;
				fieldInfo.m_strName = CString(field.c_str());
				fieldInfo.m_lAttributes = dbFixedField;
				fieldInfo.m_nOrdinalPosition = 0;
				fieldInfo.m_bRequired = FALSE;
				fieldInfo.m_lCollatingOrder = 0;
				fieldInfo.m_nType = dbDouble;
				fieldInfo.m_lSize = 8;
				fieldInfo.m_bAllowZeroLength = FALSE;
				td.CreateField(fieldInfo);
			}
			else if(type=="int")
			{
				CDaoFieldInfo fieldInfo;
				fieldInfo.m_strName = CString(field.c_str());
				fieldInfo.m_lAttributes = dbFixedField;
				fieldInfo.m_nOrdinalPosition = 0;
				fieldInfo.m_bRequired = FALSE;
				fieldInfo.m_lCollatingOrder = 0;
				fieldInfo.m_nType = dbLong;
				fieldInfo.m_lSize = 4;
				fieldInfo.m_lAttributes = dbFixedField;
				fieldInfo.m_bAllowZeroLength = FALSE;
				td.CreateField(fieldInfo);
			}

		}
		td.Append();
		td.Close();

		fieldAnno = GetNameField(pMDBDataBase, strTable);
		fieldGeocode = GetGeocodeField(pMDBDataBase, strTable);
		fieldAid = GetAIDField(pMDBDataBase, strTable);
		fieldCentroidX = GetMiddleXField(pMDBDataBase, strTable);
		fieldCentroidY = GetMiddleYField(pMDBDataBase, strTable);
		if(hiddenlevel<0)
		{
			HINSTANCE hInst = ::LoadLibrary(_T("Port.dll"));
			HINSTANCE hInstOld = AfxGetResourceHandle();
			AfxSetResourceHandle(hInst);

			std::list<std::pair<CString, CString>> fileds = GetAllFields(pMDBDataBase, strTable);
			CFieldsPickerDlg dlg(nullptr, fileds, fieldAid, fieldAnno, fieldGeocode, fieldCentroidX, fieldCentroidY, samples, strTable);
			if(dlg.DoModal()==IDOK)
			{
				fieldAid = dlg.m_strFieldAid;
				fieldAnno = dlg.m_strFieldAnno;
				fieldGeocode = dlg.m_strFieldCode;
				fieldCentroidX = dlg.m_strFieldCentroidX;
				fieldCentroidY = dlg.m_strFieldCentroidY;
			}
			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}

		if(fieldAid.IsEmpty())
		{
			td.Open(strTable);

			CDaoFieldInfo fieldInfo;
			fieldInfo.m_strName = _T("AID");
			fieldInfo.m_nOrdinalPosition = 0;
			fieldInfo.m_lCollatingOrder = dbSortGeneral;
			fieldInfo.m_nType = dbLong;
			fieldInfo.m_lSize = 4;
			fieldInfo.m_lAttributes = dbFixedField|dbAutoIncrField;
			fieldInfo.m_bAllowZeroLength = FALSE;
			td.CreateField(fieldInfo);

			CDaoIndexFieldInfo difi;
			difi.m_strName = _T("AID");
			difi.m_bDescending = FALSE;

			CDaoIndexInfo idx;
			idx.m_strName = _T("PrimaryKey");
			idx.m_pFieldInfos = &difi;
			idx.m_nFields = 1;
			idx.m_bPrimary = TRUE;
			idx.m_bUnique = TRUE;
			idx.m_bClustered = FALSE;
			idx.m_bIgnoreNulls = TRUE; // doesn't matter
			idx.m_bRequired = TRUE;
			idx.m_bForeign = FALSE;
			idx.m_lDistinctCount = 0; // ignored
			td.CreateIndex(idx);

			//		td.Append();
			td.Close();

			fieldAid = _T("AID");
		}
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
}
CProjection* CImportJson::GetProjection(CString& strFile)
{
	std::ifstream file(strFile);
	if(file.is_open()==false)
		return nullptr;

	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	contents = std::regex_replace(contents, std::regex("^hc-"), "");

	boost::json::value jroot = boost::json::parse(contents);
	boost::json::object object = jroot.as_object();
	if(object.empty()==true)
		return nullptr;

	if(object.contains("title"))
	{
		m_datainfo.m_strName = CString(object.at("title").as_string().c_str());
	}
	if(object.contains("transform"))
	{
		CProjection* pProjection = nullptr;
		boost::json::object jtransform = object.at("transform").as_object();
		if(jtransform.empty()==false)
		{
			delete m_datainfo.m_pProjection;
			m_datainfo.m_pProjection = nullptr;
			if(jtransform.contains("default"))
			{
				boost::json::object jdefault = jtransform.at("default").as_object();
				OGRSpatialReference oSRS;
				if(oSRS.importFromProj4(jdefault.at("crs").as_string().c_str())==OGRERR_NONE)
				{
					double offsetX = 0;
					double offsetY = 0;
					pProjection = CImportGdal::NewProjFromOGRSpatialReference(&oSRS, offsetX, offsetY);
				}
				m_fXOffset = jdefault.at("xoffset").to_number<double>();
				m_fYOffset = jdefault.at("yoffset").to_number<double>();
				boost::json::value jscale = jdefault.at("scale");
				if(jscale!=nullptr)
				{
					m_scale = jscale.to_number<double>();
				}
				boost::json::value res = jdefault.at("jsonres");
				if(res!=nullptr)
				{
					m_jsonres = res.to_number<double>();
				}
				m_margin.cx = jdefault.at("jsonmarginX").to_number<double>();
				m_margin.cy = jdefault.at("jsonmarginY").to_number<double>();
			}
			else
			{
				if(jtransform.contains("scale"))
				{
					boost::json::array jscales = jtransform.at("scale").as_array();
					m_scaleX = ReadAsDouble(jscales.at(0));
					m_scaleY = ReadAsDouble(jscales.at(1));
				}
				if(jtransform.contains("translate"))
				{
					boost::json::array jtranslates = jtransform.at("translate").as_array();
					m_fXOffset = -ReadAsDouble(jtranslates.at(0));
					m_fYOffset = -ReadAsDouble(jtranslates.at(1));
					m_datainfo.m_mapOrigin.x = m_fXOffset;
					m_datainfo.m_mapOrigin.y = m_fYOffset;
				}
			}
		}
		return pProjection;
	}
	else
		return nullptr;

}
CPoint CImportJson::Change(const double& x, const double& y) const
{
	const long X = std::lround(((x-m_margin.cx)*m_scaleX/m_jsonres/m_scale-m_fXOffset)*m_fXScale);
	const long Y = std::lround(((y-m_margin.cy)*m_scaleY/m_jsonres/m_scale-m_fYOffset)*m_fYScale);
	return CPoint(X, Y);
}
CPoly* CImportJson::ReadPoly(boost::json::array coordinates)
{
	if(coordinates.empty())
		return nullptr;

	CPoly* pPoly = new CPoly();
	double prevX;
	double prevY;
	int index = 0;
	for(boost::json::value& xy:coordinates)
	{
		if(index==0)
		{
			prevX = ReadAsDouble(xy.as_array().at(0));
			prevY = ReadAsDouble(xy.as_array().at(1));
			pPoly->AddAnchor(Change(prevX, prevY), true);
		}
		else
		{
			double x = prevX+ReadAsDouble(xy.as_array().at(0));
			double y = prevY+ReadAsDouble(xy.as_array().at(1));
			pPoly->AddAnchor(Change(x, y), true);
			prevX = x;
			prevY = y;
		}
		index++;
	}

	pPoly->RecalRect();
	return pPoly;
}