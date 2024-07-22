#include "StdAfx.h"
#include <cmath>
#include "ImportGdal.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"
#include "../Geometry/Group.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/Tag.h"
#include "../Geometry/Compound.h"

#include "../Style/FontCombox.h"
#include "../Style/Line.h"
#include "../Style/LineEmpty.h"
#include "../Style/FillEmpty.h"
#include "../Style/FillAlone.h"
#include "../Style/Type.h"
#include "../Style/Spot.h"
#include "../Style/Dash.h"
#include "../Style/ColorSpot.h"
#include "../Style/SpotFont.h"
#include "../Style/SpotPattern.h"
#include "../Style/Hint.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"

#include "../Projection/Projection1.h"
#include "../Projection/Geographic.h"
#include "../Projection/Geocentric.h"

#include "../Dataview/Datainfo.h"
#include "../DataView/DatainfoDlg.h"
#include "../Utility/Scale.hpp"
#include "../Database/Datasource.h"

#include "../../Thirdparty/GDAL/3.3.3/gcore/gdal.h"
#include "../../Thirdparty/GDAL/3.3.3/gcore/gdal_priv.h"
#include "../../Thirdparty/GDAL/3.3.3/ogr/ogr_api.h"
#include "../../Thirdparty/GDAL/3.3.3/ogr/ogr_srs_api.h"
#include "../../Thirdparty/GDAL/3.3.3/ogr/ogr_feature.h"
#include "../../Thirdparty/GDAL/3.3.3/ogr/ogrsf_frmts/ogrsf_frmts.h"
#include "../../Thirdparty/GDAL/3.3.3/port/cpl_conv.h"
#include "FieldsPickerDlg.h"
#include <filesystem> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImportGdal::CImportGdal(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId)
	: CImport(datainfo, layertree, dwMaxGeomId)
{
	m_fXScale = 10000;
	m_fYScale = 10000;

	m_fXOffset = 0;
	m_fYOffset = 0;
	OGRRegisterAll();
}

CImportGdal::~CImportGdal()
{
}

BOOL CImportGdal::Support(CString strFile)
{
	CT2CA caFile(strFile);
	GDALDataset* pDataset = (GDALDataset*)GDALOpen(caFile.m_psz, GA_ReadOnly);
	if(pDataset==nullptr)
		return FALSE;
	else
	{
		GDALClose(pDataset);
		return TRUE;
	}
}
BOOL CImportGdal::Import(CString& strFile, CString& dbname, bool into)
{
	CString strName = strFile.ReverseFind(_T('\\'))!=-1 ? strFile.Mid(strFile.ReverseFind(_T('\\'))+1) : strFile;
	if(strName.ReverseFind(_T('.'))!=-1)
		strName = strName.Left(strName.ReverseFind(_T('.')));
	dbname = strName;
	CString strPath = strFile.ReverseFind(_T('\\'))!=-1 ? strFile.Left(strFile.ReverseFind(_T('\\'))) : CString("");

	if(m_datainfo.m_pProjection!=nullptr)
	{
		delete m_datainfo.m_pProjection;
		m_datainfo.m_pProjection = nullptr;
	}

	CProjection* pProjection = GetProjection(strFile);
	if(pProjection!=nullptr)
	{
		m_datainfo.ResetProjection(pProjection, false);
		m_datainfo.m_scaleMaximum = 1000;
		m_datainfo.m_scaleSource = 10000;
		m_datainfo.m_scaleMinimum = 800000000;
		m_fXScale = pProjection->m_strUnit=="Degree" ? 10000000 : pProjection->GetDilationMapToDoc();
		m_fYScale = pProjection->m_strUnit=="Degree" ? 10000000 : pProjection->GetDilationMapToDoc();
	}
	//else if(ignoreProjection==false)
	//{
	//	pProjection = new CGeographic();
	//	m_datainfo.ResetProjection(pProjection, false);
	//	m_datainfo.m_scaleMaximum = 1000;
	//	m_datainfo.m_scaleSource = 10000;
	//	m_datainfo.m_scaleMinimum = 800000000;
	//	m_fXScale = pProjection->m_strUnit=="Degree" ? 10000000 : pProjection->GetDilationMapToDoc();
	//	m_fYScale = pProjection->m_strUnit=="Degree" ? 10000000 : pProjection->GetDilationMapToDoc();
	//}
	else
	{
		m_datainfo.m_scaleMinimum = 128;
		m_datainfo.m_scaleSource = 1;
		m_datainfo.m_scaleMaximum = 0.01;
		m_fXScale = 10000;
		m_fYScale = 10000;
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

	if(m_datainfo.m_pProjection!=nullptr)
	{
		m_fXScale = m_datainfo.m_pProjection->m_strUnit=="Degree" ? 10000000 : m_datainfo.m_pProjection->GetDilationMapToDoc();
		m_fYScale = m_datainfo.m_pProjection->m_strUnit=="Degree" ? 10000000 : m_datainfo.m_pProjection->GetDilationMapToDoc();
	}
	else
	{
		m_fXScale = 10000;
		m_fYScale = 10000;
	}

	CDaoDatabase* pMDBDataBase = CImportGdal::CreateDatabase(strPath, strName);
	ReadLayer(strFile, pMDBDataBase, true);
	if(pMDBDataBase!=nullptr)
	{
		pMDBDataBase->Close();
		delete pMDBDataBase;
		pMDBDataBase = nullptr;
	}

	return CImport::Import(strFile);
}
BOOL CImportGdal::Import(LPCTSTR lpszPath, CStringList& files, bool ignoreProjection, CString& dbname, bool into)
{
	CString strName(lpszPath);
	if(files.GetCount()>1)
	{
		if(strName.ReverseFind(_T('\\'))!=-1)
			strName = strName.Mid(strName.ReverseFind(_T('\\'))+1);
	}
	else if(files.GetCount()==1)
	{
		strName = files.GetHead();
		if(strName.ReverseFind(_T('\\'))!=-1)
			strName = strName.Mid(strName.ReverseFind(_T('\\'))+1);
		if(strName.ReverseFind(_T('.'))!=-1)
			strName = strName.Left(strName.ReverseFind(_T('.')));
	}
	else
		return FALSE;

	dbname = strName;
	if(into==false)
	{
		if(m_datainfo.m_pProjection!=nullptr)
		{
			delete m_datainfo.m_pProjection;
			m_datainfo.m_pProjection = nullptr;
		}

		CProjection* pMainProjection = ignoreProjection ? nullptr : GetMainProjection(lpszPath, files);
		if(pMainProjection!=nullptr)
		{
			m_datainfo.ResetProjection(pMainProjection, false);
			m_datainfo.m_scaleMaximum = 1000;
			m_datainfo.m_scaleSource = 10000;
			m_datainfo.m_scaleMinimum = 800000000;
			m_fXScale = pMainProjection->m_strUnit=="Degree" ? 10000000 : pMainProjection->GetDilationMapToDoc();
			m_fYScale = pMainProjection->m_strUnit=="Degree" ? 10000000 : pMainProjection->GetDilationMapToDoc();
		}
		else if(ignoreProjection==false)
		{
			pMainProjection = new CGeographic();
			m_datainfo.ResetProjection(pMainProjection, false);
			m_datainfo.m_scaleMaximum = 1000;
			m_datainfo.m_scaleSource = 10000;
			m_datainfo.m_scaleMinimum = 800000000;
			m_fXScale = pMainProjection->m_strUnit=="Degree" ? 10000000 : pMainProjection->GetDilationMapToDoc();
			m_fYScale = pMainProjection->m_strUnit=="Degree" ? 10000000 : pMainProjection->GetDilationMapToDoc();
		}
		else
		{
			m_datainfo.m_scaleMinimum = 128;
			m_datainfo.m_scaleSource = 1;
			m_datainfo.m_scaleMaximum = 0.01;
			m_fXScale = 10000;
			m_fYScale = 10000;
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

	if(m_datainfo.m_pProjection!=nullptr)
	{
		m_fXScale = m_datainfo.m_pProjection->m_strUnit=="Degree" ? 10000000 : m_datainfo.m_pProjection->GetDilationMapToDoc();
		m_fYScale = m_datainfo.m_pProjection->m_strUnit=="Degree" ? 10000000 : m_datainfo.m_pProjection->GetDilationMapToDoc();
	}
	else
	{
		m_fXScale = 10000;
		m_fYScale = 10000;
	}

	CDaoDatabase* pMDBDataBase = CImportGdal::CreateDatabase(lpszPath, strName);
	POSITION pos = files.GetHeadPosition();
	while(pos!=nullptr)
	{
		CString strFile = files.GetNext(pos);
		ReadLayer(CString(lpszPath)+"\\"+strFile, pMDBDataBase, ignoreProjection, into);
	}
	if(pMDBDataBase!=nullptr)
	{
		pMDBDataBase->Close();
		delete pMDBDataBase;
		pMDBDataBase = nullptr;
	}

	return CImport::Import(lpszPath);
}

void CImportGdal::ReadLayer(LPCTSTR lpszFile, CDaoDatabase* pMDBDataBase, bool ignoreProjection, bool into)
{
	CT2CA caTable(lpszFile);

	GDALDatasetUniquePtr poDataset(GDALDataset::Open(caTable.m_psz, GA_ReadOnly|GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
	if(poDataset==nullptr)
	{
		CString strError;
		strError.Format(_T("Open %s failed."), lpszFile);
		AfxMessageBox(strError);
		return;
	}
	bool intoed1 = into;
	const std::filesystem::path filepath(lpszFile);
	const std::string ext = filepath.extension().string();
	const std::string name1 = filepath.filename().string();
	const std::string name2 = name1.substr(0, name1.length()-ext.length());
	double fPointToDoc = m_datainfo.CalPointToDocZoom(m_datainfo.m_pProjection);
	CLayer* pParent = into ? m_layertree.GetByName(CString(name2.c_str())) : nullptr;
	if(pParent==nullptr)
	{
		pParent = new CLayer(m_layertree);
		pParent->m_wId = m_layertree.ApplyId();
		pParent->m_strName = CString(name2.c_str());
		pParent->m_bDynamic &= ~LineDynamic;
		pParent->m_bDynamic &= ~FillDynamic;
		pParent->m_bCentroid = true;
		m_layertree.m_root.AddChild(nullptr, pParent);
	}
	else
	{
		intoed1 = true;
	}

	for(OGRLayer* poLayer:poDataset->GetLayers())
	{
		CString strLayer = CString(poLayer->GetName());
		CLayer* pLayer = nullptr;
		bool intoed2 = intoed1;
		if(poDataset->GetLayerCount()==1)
			pLayer = pParent;
		else
		{
			pLayer = into ? pParent->GetChild(strLayer) : nullptr;
			if(pLayer==nullptr)
			{
				CString strName2(poLayer->GetName());
				pLayer = new CLayer(m_layertree);
				pLayer->m_wId = m_layertree.ApplyId();
				pLayer->m_strName = strLayer;
				pLayer->m_bDynamic &= ~LineDynamic;
				pLayer->m_bDynamic &= ~FillDynamic;
				pLayer->m_bCentroid = true;
				pParent->AddChild(nullptr, pLayer);

				delete pLayer->m_pLine;
				pLayer->m_pLine = nullptr;
				delete pLayer->m_pFill;
				pLayer->m_pFill = nullptr;
				delete pLayer->m_pSpot;
				pLayer->m_pSpot = nullptr;
				delete pLayer->m_pHint;
				pLayer->m_pHint = nullptr;

				intoed2 = false;
			}
		}
		CString fieldAid = _T("");
		CString fieldAnno = _T("");
		CString fieldGeocode = _T("");
		CString fieldCentroidX = _T("");
		CString fieldCentroidY = _T("");

		CDaoRecordset rs(pMDBDataBase);
		OGRFeatureDefn* poFeatureDefn = poLayer->GetLayerDefn();
		if(pMDBDataBase!=nullptr&&poFeatureDefn!=nullptr&&CreateTable(poFeatureDefn, strLayer, pMDBDataBase)==TRUE)
		{
			CString strDBName = pMDBDataBase->GetName();
			strDBName = strDBName.Mid(strDBName.ReverseFind('\\')+1);
			strDBName = strDBName.Left(strDBName.ReverseFind('.'));
			pLayer->EnableAttribute(strDBName);
			CATTHeaderProfile& headerprofile = pLayer->GetAttDatasource()->GetHeaderProfile();
			headerprofile.m_strKeyTable = strLayer;
			headerprofile.m_strTables = strLayer;

			CString strSQL;
			strSQL.Format(_T("SELECT * FROM [%s]"), headerprofile.m_strKeyTable);

			rs.Open(dbOpenDynaset, strSQL);
			fieldAnno = GetNameField(pMDBDataBase, headerprofile.m_strKeyTable);
			fieldGeocode = GetGeocodeField(pMDBDataBase, headerprofile.m_strKeyTable);
			fieldAid = GetAIDField(pMDBDataBase, headerprofile.m_strKeyTable);
			fieldCentroidX = GetMiddleYField(pMDBDataBase, headerprofile.m_strKeyTable);
			fieldCentroidY = GetMiddleXField(pMDBDataBase, headerprofile.m_strKeyTable);
			rs.Close();

			if(hiddenlevel<0)
			{
				HINSTANCE hInst = ::LoadLibrary(_T("Port.dll"));
				HINSTANCE hInstOld = AfxGetResourceHandle();
				AfxSetResourceHandle(hInst);

				std::list<std::list<CString>> samples;
				int count = 0;
				poLayer->ResetReading();
				OGRFeature* poFeature = poLayer->GetNextFeature();
				while(poFeature!=nullptr&&count++<10)
				{
					std::list<CString> values;
					for(int nField = 0; nField<poFeatureDefn->GetFieldCount(); nField++)
					{
						OGRFieldDefn* poFieldDefn = poFeatureDefn->GetFieldDefn(nField);
						if(poFieldDefn==nullptr)
							continue;
						auto bures = poFeature->GetFieldAsString(nField);						
						OLECHAR* pOlechars = MultiByteToUnicode(CP_UTF8, bures);
						values.push_back(pOlechars);
						delete[] pOlechars;
					}
					samples.push_back(values);

					poFeature = poLayer->GetNextFeature();
				}
				poLayer->ResetReading();

				std::list<std::pair<CString, CString>> fileds = GetAllFields(pMDBDataBase, headerprofile.m_strKeyTable);
				CFieldsPickerDlg dlg(nullptr, fileds, fieldAid, fieldAnno, fieldGeocode, fieldCentroidX, fieldCentroidY, samples, strLayer);
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
			else if(hiddenlevel>0)
			{
				fieldAnno.Format(_T("NAME_%d"), hiddenlevel);
			}
			headerprofile.m_strAnnoField = fieldAnno;
			headerprofile.m_strCodeField = fieldGeocode;

			CDaoTableDef td(pMDBDataBase);
			td.Open(headerprofile.m_strKeyTable);
			if(td.CanUpdate()==TRUE)
			{
				if(fieldAid==_T(""))
				{
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
					headerprofile.m_strIdField = _T("AID");
				}
				else
				{
					headerprofile.m_strIdField = fieldAid;
				}
				td.Close();
			}
			rs.Open(dbOpenDynaset, strSQL);
		}

		OGRSpatialReference* pReference = poLayer->GetSpatialRef();
		CProjection* pProjection = (pReference!=nullptr&&ignoreProjection==false) ? CImportGdal::NewProjFromOGRSpatialReference(pReference, m_fXOffset, m_fYOffset) : nullptr;
		if(ignoreProjection)
		{
		}
		else if(pProjection!=nullptr&&m_datainfo.m_pProjection!=nullptr)
		{
			char* pszLinearUnits = nullptr;
			double dfLinearConv = pReference->GetLinearUnits(&pszLinearUnits);
			m_fXScale = (pProjection->m_strUnit=="Degree" ? 10000000 : pProjection->GetDilationMapToDoc())*dfLinearConv;
			m_fYScale = (pProjection->m_strUnit=="Degree" ? 10000000 : pProjection->GetDilationMapToDoc())*dfLinearConv;
		}
		else if(pProjection!=nullptr)
		{
			delete pProjection;
			pProjection = nullptr;
			m_fXScale = 10000;
			m_fYScale = 10000;
		}
		else if(m_datainfo.m_pProjection!=nullptr)
		{
			m_fXScale = m_datainfo.m_pProjection->m_strUnit=="Degree" ? 10000000 : m_datainfo.m_pProjection->GetDilationMapToDoc();
			m_fYScale = m_datainfo.m_pProjection->m_strUnit=="Degree" ? 10000000 : m_datainfo.m_pProjection->GetDilationMapToDoc();
		}
		else
		{
			m_fXScale = 10000;
			m_fYScale = 10000;
		}

		poLayer->ResetReading();
		OGRFeature* poFeature = poLayer->GetNextFeature();
		while(poFeature!=nullptr)
		{
			OGRGeometry* poGeometry = poFeature->GetGeometryRef();
			if(poGeometry==nullptr)
			{
				OGRFeature::DestroyFeature(poFeature);
				poFeature = poLayer->GetNextFeature();
				continue;
			}
			const char* pszStyleString = poFeature->GetStyleString();
			OGRStyleMgr* poStyleMgr = nullptr;
			if(pszStyleString!=nullptr)
			{
				poStyleMgr = new OGRStyleMgr;
				poStyleMgr->InitStyleString(pszStyleString);
			}

			CGeom* pGeom = ReadGeometry(m_layertree.m_library, poGeometry, poStyleMgr, fPointToDoc);
			delete poStyleMgr;
			poStyleMgr = nullptr;
			if(pGeom==nullptr)
			{
				OGRFeature::DestroyFeature(poFeature);
				poFeature = poLayer->GetNextFeature();
				continue;
			}

			if(rs.IsOpen()==TRUE)
			{
				try
				{
					rs.AddNew();

					double x = 0;
					double y = 0;
					for(int nField = 0; nField<poFeatureDefn->GetFieldCount(); nField++)
					{
						OGRFieldDefn* poFieldDefn = poFeatureDefn->GetFieldDefn(nField);
						if(poFieldDefn==nullptr)
							continue;
						CString strField(poFieldDefn->GetNameRef());
						switch(poFieldDefn->GetType())
						{
							case OFTInteger: // Simple 32bit integer 
								{
									int value = poFeature->GetFieldAsInteger(nField);
									rs.SetFieldValue(strField, COleVariant((long)value, VT_I4));
									if(strField.CompareNoCase(fieldAid)==0)
									{
										pGeom->m_attId = value;
									}
								}
								break;
							case OFTInteger64:
								{
									int value = poFeature->GetFieldAsInteger64(nField);
									rs.SetFieldValue(strField, COleVariant((long)value, VT_I4));
									if(strField.CompareNoCase(fieldAid)==0)
									{
										pGeom->m_attId = value;
									}
								}
								break;
							case OFTReal: // Double Precision floating point 
								{
									if(strField.CompareNoCase(fieldCentroidX)==0)
									{
										x = poFeature->GetFieldAsDouble(nField);
										if(y!=0)
										{
											if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)))
											{
												((CPoly*)pGeom)->m_centroid = CPoint(pGeom->m_Rect.left+pGeom->m_Rect.Width()*x, pGeom->m_Rect.top+pGeom->m_Rect.Height()*y);
											}
											if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut)))
											{
												((CDoughnut*)pGeom)->m_centroid = CPoint(pGeom->m_Rect.left+pGeom->m_Rect.Width()*x, pGeom->m_Rect.top+pGeom->m_Rect.Height()*y);
											}
										}
									}
									else if(strField.CompareNoCase(fieldCentroidX)==0)
									{
										y = poFeature->GetFieldAsDouble(nField);
										if(x!=0)
										{
											if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)))
											{
												((CPoly*)pGeom)->m_centroid = CPoint(pGeom->m_Rect.left+pGeom->m_Rect.Width()*x, pGeom->m_Rect.top+pGeom->m_Rect.Height()*y);
											}
											if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut)))
											{
												((CDoughnut*)pGeom)->m_centroid = CPoint(pGeom->m_Rect.left+pGeom->m_Rect.Width()*x, pGeom->m_Rect.top+pGeom->m_Rect.Height()*y);
											}
										}
									}
									else
									{
										double value = poFeature->GetFieldAsDouble(nField);
										rs.SetFieldValue(strField, COleVariant(value));
									}
								}
								break;
							case OFTString: // String of ASCII chars  		
								{
									const char* chars = poFeature->GetFieldAsString(nField);
									if(chars!=nullptr)
									{
										if(strField.CompareNoCase(fieldAnno)==0)
										{
											OLECHAR* pOlechars = MultiByteToUnicode(CP_UTF8, chars);											
											rs.SetFieldValue(strField, COleVariant(pOlechars, VT_BSTRT));
											pGeom->m_strName = pOlechars;
											delete[] pOlechars;
										}
										else if(strField.CompareNoCase(fieldAid)==0)
										{
											DWORD dwId = atoi(chars);
											rs.SetFieldValue(fieldAid, COleVariant((long)dwId, VT_I4));
											pGeom->m_attId = dwId;
										}
										else if(strField.CompareNoCase(fieldGeocode)==0)
										{
											CStringA value(chars);
											rs.SetFieldValue(strField, COleVariant(CString(value), VT_BSTRT));
											pGeom->m_geoCode = value;
										}
										else
										{
											OLECHAR* pOlechars = MultiByteToUnicode(CP_UTF8, chars);											
											rs.SetFieldValue(strField, COleVariant(pOlechars, VT_BSTRT));
											delete[] pOlechars;
										}
									}
								}
								break;
							case OFTBinary: // Raw Binary data 
								{
									int count;
									GByte* pBytes = poFeature->GetFieldAsBinary(nField, &count);
									if(pBytes!=nullptr)
									{
										CLongBinary bin;
										bin.m_dwDataLength = count;
										bin.m_hData = pBytes;
										rs.SetFieldValue(strField, bin);
									}
								}
								break;
							case OFTDate: // Date    
							case OFTTime: // Time
							case OFTDateTime: // Date and Time 
								{
									int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, tz = 0;
									if(poFeature->GetFieldAsDateTime(nField, &year, &month, &day, &hour, &min, &sec, &tz)==TRUE)
									{
										COleDateTime(year, month, day, hour, min, sec);
										rs.SetFieldValue(strField, COleDateTime(year, month, day, hour, min, sec));
									}
								}
								break;
								//	case OFTLogical:
								//		break;
								//	case OFTMaxType:	
								//		break;
							case OFTIntegerList: // List of 32bit integers 
							case OFTRealList: // List of doubles  
							case OFTStringList: // Array of strings
							case OFTWideString: // deprecated   
							case OFTWideStringList: // deprecated   
								{
									const char* chars = poFeature->GetFieldAsString(nField);
									if(chars!=nullptr)
									{
										OLECHAR* pOlechars = MultiByteToUnicode(CP_UTF8, chars);										
										rs.SetFieldValue(strField, COleVariant(pOlechars, VT_BSTRT));
										delete[] pOlechars;
									}
								}
								break;

							default:
								break;
						}
					}

					rs.Update();
					if(fieldAid==_T(""))
					{
						COleVariant bookmark = rs.GetLastModifiedBookmark();
						rs.SetBookmark(bookmark);
						COleVariant varKey = rs.GetFieldValue(_T("AID"));
						pGeom->m_attId = varKey.lVal;
					}
					if(fieldGeocode.IsEmpty())
					{
						pGeom->m_geoCode.Format("%d", pGeom->m_attId);
					}
				}
				catch(CDaoException* ex)
				{
					//					OutputDebugString(ex->m_strError);
					ex->Delete();
				}
			}

			pLayer->m_geomlist.AddTail(pGeom);

			OGRFeature::DestroyFeature(poFeature);
			poFeature = poLayer->GetNextFeature();
		}
		if(rs.IsOpen()==TRUE)
		{
			rs.Close();
		}
		if(ignoreProjection) {
		}
		else if(pProjection!=nullptr)
		{
			if(m_datainfo.m_pProjection!=nullptr&&*pProjection!=*m_datainfo.m_pProjection)
			{
				CDatainfo datainfo;
				delete datainfo.m_pProjection;

				datainfo.m_pProjection = pProjection;
				datainfo.m_scaleMaximum = 1000;
				datainfo.m_scaleSource = 10000;
				datainfo.m_scaleMinimum = 800000000;
				pLayer->Project(datainfo, m_datainfo, Interpolation::Nothing, Scale::PrecisionFromScale(datainfo.m_scaleSource));
				datainfo.m_pProjection = nullptr;
			}

			delete pProjection;
			pProjection = nullptr;
		}
		if(intoed2==false)
		{
			CValueCounter<CLine> lines;
			CValueCounter<CFillCompact> fills;
			CValueCounter<CSpot> spots;
			CValueCounter<CType> types;
			CValueCounter<CHint> hints;
			POSITION pos1 = pLayer->m_geomlist.GetHeadPosition();
			while(pos1!=nullptr)
			{
				CGeom* pGeom = pLayer->m_geomlist.GetNext(pos1);
				CountInStyles(pGeom, lines, fills, spots, types, hints);
			}

			const CLine* pLine = lines.GetMaxObject();
			const CFillCompact* pFill = fills.GetMaxObject();
			const CSpot* pSpot = spots.GetMaxObject();
			const CType* pType = types.GetMaxObject();
			const CHint* pHint = hints.GetMaxObject();
			pLayer->m_pSpot = (CSpot*)pSpot;
			pLayer->m_pLine = (CLine*)pLine;
			pLayer->m_pFill = (CFillCompact*)pFill;
			pLayer->m_pType = (CType*)pType;
			pLayer->m_pHint = (CHint*)pHint;
			POSITION pos2 = pLayer->m_geomlist.GetHeadPosition();
			while(pos2!=nullptr)
			{
				CGeom* pGeom = pLayer->m_geomlist.GetNext(pos2);
				RemoveStyles(pGeom, pLine, pFill, pSpot, pType, pHint);
			}
			if(pLayer->m_pLine==nullptr)
			{
				pLayer->m_pLine = new CLine();
			}
			if(pLayer->m_pFill==nullptr)
			{
				pLayer->m_pFill = new CFillAlone();
			}
			if(pLayer->m_pSpot==nullptr)
			{
				pLayer->m_pSpot = new CSpotFont();
				bool bHasmarks = false;
				POSITION Pos3 = pLayer->m_geomlist.GetHeadPosition();
				while(Pos3!=nullptr)
				{
					CGeom* pGeom = pLayer->m_geomlist.GetNext(Pos3);
					if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
					{
						((CMark*)pGeom)->CalCorner(pLayer->GetSpot(), m_datainfo, m_layertree.m_library);
						bHasmarks = true;
					}
				}
				if(bHasmarks==false)
				{
					delete pLayer->m_pSpot;
					pLayer->m_pSpot = nullptr;
				}
			}
		}
	}
}

CProjection* CImportGdal::GetProjection(CString& strFile)
{
	CValueCounter<CProjection> valuecounter;

	CT2CA caFile(strFile);
	GDALDataset* pDataset = (GDALDataset*)GDALOpenEx(caFile.m_psz, GA_ReadOnly|GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
	if(pDataset!=nullptr)
	{
		const char* projection1 = pDataset->GetProjectionRef();
		const char* projection2 = pDataset->GetGCPProjection();
		for(int index = 0; index<pDataset->GetLayerCount(); index++)
		{
			OGRLayer* poLayer = pDataset->GetLayer(index);
			if(poLayer==nullptr)
				continue;
			OGRSpatialReference* reference = poLayer->GetSpatialRef();
			if(reference!=nullptr)
			{
				CProjection* pProjection = CImportGdal::NewProjFromOGRSpatialReference(reference, this->m_fXOffset, this->m_fYOffset);
				if(pProjection!=nullptr)
				{
					CProjection* pProjection1 = (CProjection*)valuecounter.CountIn(pProjection);
					delete pProjection1;
				}
			}
		}
		GDALClose(pDataset);
	}

	CProjection* pProjection = (CProjection*)valuecounter.GetMaxObject();
	valuecounter.RemoveObject(pProjection);
	valuecounter.Clear();
	return pProjection;
}
CProjection* CImportGdal::GetMainProjection(LPCTSTR lpszPath, CStringList& files)
{
	CValueCounter<CProjection> valuecounter;
	POSITION pos = files.GetHeadPosition();
	while(pos!=nullptr)
	{
		CString strFile = files.GetNext(pos);
		strFile = CString(lpszPath)+_T('\\')+strFile;
		CT2CA caFile(strFile);
		GDALDataset* pDataset = (GDALDataset*)GDALOpenEx(caFile.m_psz, GA_ReadOnly|GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
		if(pDataset!=nullptr)
		{
			const char* projection1 = pDataset->GetProjectionRef();
			const char* projection2 = pDataset->GetGCPProjection();
			for(int index = 0; index<pDataset->GetLayerCount(); index++)
			{
				OGRLayer* poLayer = pDataset->GetLayer(index);
				if(poLayer==nullptr)
					continue;
				OGRSpatialReference* reference = poLayer->GetSpatialRef();
				if(reference!=nullptr)
				{
					CProjection* pProjection = CImportGdal::NewProjFromOGRSpatialReference(reference, this->m_fXOffset, this->m_fYOffset);
					if(pProjection!=nullptr)
					{
						CProjection* pProjection1 = (CProjection*)valuecounter.CountIn(pProjection);
						delete pProjection1;
					}
				}
			}
			GDALClose(pDataset);
		}
	}

	CProjection* pProjection = (CProjection*)valuecounter.GetMaxObject();
	valuecounter.RemoveObject(pProjection);
	valuecounter.Clear();
	return pProjection;
}

CDaoDatabase* CImportGdal::CreateDatabase(CString strPath, CString strName) const
{
	CDaoDatabase* pMDBDataBase = nullptr;
	try
	{
		const CString strDBFile = strPath+_T('\\')+strName+_T(".Mdb");
		if(_taccess(strDBFile, 00)!=-1)
		{
			pMDBDataBase = new CDaoDatabase();
			pMDBDataBase->Open(strDBFile);
		}
		else
		{
			pMDBDataBase = new CDaoDatabase();
			pMDBDataBase->Create(strDBFile);
		}
	}
	catch(CDaoException* ex)
	{
		//		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
		delete pMDBDataBase;
		pMDBDataBase = nullptr;
		AfxMessageBox(_T("Error happened!"));
	}

	return pMDBDataBase;
}

BOOL IsExistentField(CDaoTableDef& tabledef, CString fieldAnno)
{
	BOOL bDuplicateFieldName = TRUE;
	CDaoFieldInfo fieldInfo; // only needed for the call
	try
	{
		tabledef.GetFieldInfo(fieldAnno, fieldInfo);
	}
	catch(CDaoException* ex)
	{
		//		OutputDebugString(ex->);
		if(ex->m_pErrorInfo->m_lErrorCode==3265)
			bDuplicateFieldName = FALSE;
		ex->Delete();
	}
	catch(CMemoryException* ex)
	{
		ex->Delete();
	}

	return bDuplicateFieldName;
}

bool CImportGdal::CreateTable(OGRFeatureDefn* poFeatureDefn, CString strTable, CDaoDatabase* pMDBDataBase)
{
	if(pMDBDataBase==nullptr)
		return FALSE;
	if(poFeatureDefn==nullptr)
		return FALSE;

	try
	{
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
		if(poFeatureDefn->GetFieldCount()>0)
		{
			CDaoTableDef td(pMDBDataBase);
			td.Create(strTable);
			for(int nField = 0; nField<poFeatureDefn->GetFieldCount(); nField++)
			{
				OGRFieldDefn* poFieldDefn = poFeatureDefn->GetFieldDefn(nField);
				if(poFieldDefn==nullptr)
					continue;
				CString strField(poFieldDefn->GetNameRef());
				if(IsExistentField(td, strField)==TRUE)
					continue;

				OGRFieldType fieldtype = poFieldDefn->GetType();
				switch(fieldtype)
				{
					case OFTInteger: // Simple 32bit integer 
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
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
						break;
					case OFTInteger64:
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
							fieldInfo.m_lAttributes = dbFixedField;
							fieldInfo.m_nOrdinalPosition = 0;
							fieldInfo.m_bRequired = FALSE;
							fieldInfo.m_lCollatingOrder = 0;
							fieldInfo.m_nType = dbLong;
							fieldInfo.m_lSize = 8;
							fieldInfo.m_lAttributes = dbFixedField;
							fieldInfo.m_bAllowZeroLength = FALSE;
							td.CreateField(fieldInfo);
						}
						break;
					case OFTReal: // Double Precision floating point 
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
							fieldInfo.m_lAttributes = dbFixedField;
							fieldInfo.m_nOrdinalPosition = 0;
							fieldInfo.m_bRequired = FALSE;
							fieldInfo.m_lCollatingOrder = 0;
							fieldInfo.m_nType = dbDouble;
							fieldInfo.m_lSize = 8;
							fieldInfo.m_bAllowZeroLength = FALSE;
							td.CreateField(fieldInfo);
						}
						break;
					case OFTString: // String of ASCII chars  
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
							fieldInfo.m_nOrdinalPosition = 0;
							fieldInfo.m_bRequired = FALSE;
							fieldInfo.m_lCollatingOrder = 0;
							fieldInfo.m_nType = dbText;
							fieldInfo.m_lSize = poFieldDefn->GetWidth();
							fieldInfo.m_lAttributes = dbVariableField;
							fieldInfo.m_bAllowZeroLength = TRUE;
							td.CreateField(fieldInfo);
						}
						break;
					case OFTBinary: // Raw Binary data
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
							fieldInfo.m_lAttributes = dbFixedField;
							fieldInfo.m_nOrdinalPosition = 0;
							fieldInfo.m_bRequired = FALSE;
							fieldInfo.m_lCollatingOrder = 0;
							fieldInfo.m_nType = dbLongBinary;
							fieldInfo.m_lSize = 0;
							fieldInfo.m_bAllowZeroLength = TRUE;
							td.CreateField(fieldInfo);
						}
						break;
					case OFTDate: // Date    		 
					case OFTTime: // Time
					case OFTDateTime: // Date and Time 
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
							fieldInfo.m_lAttributes = dbFixedField;
							fieldInfo.m_nOrdinalPosition = 0;
							fieldInfo.m_bRequired = FALSE;
							fieldInfo.m_lCollatingOrder = 0;
							fieldInfo.m_nType = dbDate;
							fieldInfo.m_lSize = 8;
							fieldInfo.m_bAllowZeroLength = FALSE;
							td.CreateField(fieldInfo);
						}
						break;
						/*case OFTLogical:
						{
						CDaoFieldInfo fieldInfo;
						fieldInfo.m_strName = strField;
						fieldInfo.m_lAttributes = dbFixedField;
						fieldInfo.m_nOrdinalPosition = 0;
						fieldInfo.m_bRequired        = FALSE;
						fieldInfo.m_lCollatingOrder  = 0;
						fieldInfo.m_nType = dbBoolean;
						fieldInfo.m_lSize =  1;
						fieldInfo.m_bAllowZeroLength = FALSE;
						td.CreateField(fieldInfo);
						}
						break;*/
						/*case OFTMaxType:
						break;*/
					case OFTIntegerList: // List of 32bit integers 
					case OFTRealList: // List of doubles  
					case OFTStringList: // Array of strings
					case OFTWideString: // deprecated   
					case OFTWideStringList: // deprecated   
						{
							CDaoFieldInfo fieldInfo;
							fieldInfo.m_strName = strField;
							fieldInfo.m_nOrdinalPosition = 0;
							fieldInfo.m_bRequired = FALSE;
							fieldInfo.m_lCollatingOrder = 0;
							fieldInfo.m_nType = dbText;
							fieldInfo.m_lSize = poFieldDefn->GetWidth()==0 ? 255 : poFieldDefn->GetWidth();
							fieldInfo.m_lAttributes = dbVariableField;
							fieldInfo.m_bAllowZeroLength = TRUE;
							td.CreateField(fieldInfo);
						}
					default:
						break;
				}
			}
			td.Append();
			td.Close();
		}
		else
			return false;
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}

	return true;
}
std::list<std::pair<CString, CString>> CImportGdal::GetAllFields(CDaoDatabase* pMDBDataBase, CString strTable)
{
	std::list<std::pair<CString, CString>> allfields;
	CDaoTableDef td(pMDBDataBase);
	td.Open(strTable);
	for(int index = 0; index<td.GetFieldCount(); index++)
	{
		CDaoFieldInfo fieldinfo;
		td.GetFieldInfo(index, fieldinfo);
		CString strField = fieldinfo.m_strName;
		switch(fieldinfo.m_nType)
		{
			case dbInteger:
			case dbLong:
			case dbBigInt:
				allfields.push_back(std::pair<CString, CString>(fieldinfo.m_strName, _T("integer")));
				break;
			case dbText:
				allfields.push_back(std::pair<CString, CString>(fieldinfo.m_strName, _T("string")));
				break;
			case dbDouble:
				allfields.push_back(std::pair<CString, CString>(fieldinfo.m_strName, _T("double")));
				break;
			default:
				break;
		}
	}
	td.Close();

	return allfields;
}
CString CImportGdal::GetAIDField(CDaoDatabase* pMDBDataBase, CString strTable)
{
	CString fieldAid = _T("");
	CString fieldId = _T("");
	CString fieldOther = _T("");
	CDaoTableDef td(pMDBDataBase);
	td.Open(strTable);
	for(int index = 0; index<td.GetFieldCount(); index++)
	{
		CDaoFieldInfo fieldinfo;
		td.GetFieldInfo(index, fieldinfo);
		CString strField = fieldinfo.m_strName;
		if(strField.CompareNoCase(_T("aid"))==0)
			fieldAid = strField;
		else if(strField.CompareNoCase(_T("id"))==0)
			fieldId = strField;
		else if(strField.GetLength()>2&&strField.Right(2)==_T("ID"))
			fieldOther = strField;
	}
	td.Close();

	return fieldAid.IsEmpty()==FALSE ? fieldAid : (fieldId.IsEmpty()==FALSE ? fieldId : fieldOther);
}

CString CImportGdal::GetNameField(CDaoDatabase* pMDBDataBase, CString strTable)
{
	CString fieldAnno = _T("");

	CDaoTableDef td(pMDBDataBase);
	td.Open(strTable);
	for(int index = 0; index<td.GetFieldCount(); index++)
	{
		CDaoFieldInfo fieldinfo;
		td.GetFieldInfo(index, fieldinfo);
		CString strField = fieldinfo.m_strName;
		strField.MakeUpper();
		if(strField.CompareNoCase(_T("NAME"))==0)
			fieldAnno = fieldinfo.m_strName;
		else if(fieldAnno==_T("")&&strField.Find(_T("NAME"))!=-1)
			fieldAnno = fieldinfo.m_strName;
		else if(fieldAnno==_T("")&&strField.Find(_T("名称"))!=-1)
			fieldAnno = fieldinfo.m_strName;
		else if(fieldAnno==_T("")&&strField.Find(_T("地名"))!=-1)
			fieldAnno = fieldinfo.m_strName;
	}
	td.Close();

	return fieldAnno;
}
CString CImportGdal::GetGeocodeField(CDaoDatabase* pMDBDataBase, CString strTable)
{
	CString field = _T("");

	CDaoTableDef td(pMDBDataBase);
	td.Open(strTable);
	for(int index = 0; index<td.GetFieldCount(); index++)
	{
		CDaoFieldInfo fieldinfo;
		td.GetFieldInfo(index, fieldinfo);
		CString strField = fieldinfo.m_strName;
		strField.MakeUpper();
		if(strField.CompareNoCase(_T("GEOCODE"))==0)
		{
			field = fieldinfo.m_strName;
		}
		else if(strField.CompareNoCase(_T("KEY"))==0)
		{
			field = fieldinfo.m_strName;
		}
		if(strField.CompareNoCase(_T("GEOID"))==0)
		{
			//	field = fieldinfo.m_strName;
		}
	}
	td.Close();

	return field;
}
CString CImportGdal::GetMiddleXField(CDaoDatabase* pMDBDataBase, CString strTable)
{
	CString field = _T("");

	CDaoTableDef td(pMDBDataBase);
	td.Open(strTable);
	for(int index = 0; index<td.GetFieldCount(); index++)
	{
		CDaoFieldInfo fieldinfo;
		td.GetFieldInfo(index, fieldinfo);
		CString strField = fieldinfo.m_strName;
		strField.MakeUpper();
		if(strField.CompareNoCase(_T("middle-x"))==0)
		{
			field = fieldinfo.m_strName;
		}
		else if(strField.CompareNoCase(_T("middle-lon"))==0)
		{
			field = fieldinfo.m_strName;
		}

	}
	td.Close();

	return field;
}
CString CImportGdal::GetMiddleYField(CDaoDatabase* pMDBDataBase, CString strTable)
{
	CString field = _T("");

	CDaoTableDef td(pMDBDataBase);
	td.Open(strTable);
	for(int index = 0; index<td.GetFieldCount(); index++)
	{
		CDaoFieldInfo fieldinfo;
		td.GetFieldInfo(index, fieldinfo);
		CString strField = fieldinfo.m_strName;
		strField.MakeUpper();
		if(strField.CompareNoCase(_T("middle-y"))==0)
		{
			field = fieldinfo.m_strName;
		}
		else if(strField.CompareNoCase(_T("middle-lat"))==0)
		{
			field = fieldinfo.m_strName;
		}
	}
	td.Close();

	return field;
}
CGeom* CImportGdal::ReadGeometry(CLibrary& library, OGRGeometry* poGeometry, OGRStyleMgr* poStyleMgr, double fPointToDoc)
{
	if(poGeometry==nullptr)
		return nullptr;

	switch(wkbFlatten(poGeometry->getGeometryType()))
	{
		case wkbUnknown: /**< unknown type, non-standard */
			{
				return nullptr;
			}
			break;
		case wkbPoint: /**< 0-dimensional geometric object, standard WKB */
			{
				return ReadPoint((OGRPoint*)poGeometry, poStyleMgr, library, fPointToDoc);
			}
			break;
		case wkbLineString: /**< 1-dimensional geometric object with linear	*  interpolation between Points, standard WKB */
			{
				CPoly* pPoly = ReadLineString((OGRLineString*)poGeometry);
				if(pPoly!=nullptr)
					pPoly->m_pLine = CImportGdal::ReadStroke(poStyleMgr);
				return pPoly;
			}
			break;
		case wkbLinearRing: /**< non-standard, just for createGeometry() */
			{
				CPoly* pPoly = ReadLinearRing((OGRLinearRing*)poGeometry);
				if(pPoly!=nullptr)
				{
					pPoly->m_pLine = CImportGdal::ReadStroke(poStyleMgr);
					pPoly->m_pFill = CImportGdal::ReadFill(poStyleMgr);
				}
				return pPoly;
			}
			break;
		case wkbPolygon: /**< planar 2-dimensional geometric object defined	 *  by 1 exterior boundary and 0 or more interior	   boundaries, standard WKB */
			{
				OGRPolygon* poPolygon = (OGRPolygon*)poGeometry;
				CDoughnut* pDonut = poPolygon->getNumInteriorRings()>0 ? new CDoughnut() : nullptr;
				CGeom* pGeom = pDonut;

				OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
				if(poExteriorRing!=nullptr)
				{
					CPoly* pPoly = ReadLinearRing(poExteriorRing);
					if(pDonut!=nullptr)
					{
						pDonut->m_geoId = ++m_dwMaxGeomId;
						pDonut->AddMember(pPoly);
					}
					else
					{
						pGeom = pPoly;
					}
				}
				for(int part = 0; part<poPolygon->getNumInteriorRings(); part++)
				{
					OGRLinearRing* poInteriorRing = poPolygon->getInteriorRing(part);
					CPoly* pPoly = ReadLinearRing(poInteriorRing);
					if(pDonut!=nullptr)
						pDonut->AddMember(pPoly);
					else
						delete pPoly;
				}
				if(pGeom!=nullptr)
				{
					pGeom->RecalRect();
					pGeom->RecalCentroid();
					pGeom->m_pLine = CImportGdal::ReadStroke(poStyleMgr);
					pGeom->m_pFill = CImportGdal::ReadFill(poStyleMgr);
				}
				return pGeom;
			}
			break;
		case wkbMultiPoint: /**< GeometryCollection of Points, standard WKB */
			{
				OGRGeometryCollection* poGeometryCollectio = (OGRGeometryCollection*)poGeometry;
				CGroup* pGroup = new CGroup();
				pGroup->m_geoId = ++m_dwMaxGeomId;

				for(int part = 0; part<poGeometryCollectio->getNumGeometries(); part++)
				{
					OGRGeometry* pGeometry = poGeometryCollectio->getGeometryRef(part);
					if(pGeometry==nullptr)
						continue;
					CGeom* pGeom = ReadGeometry(library, pGeometry, poStyleMgr, fPointToDoc);
					if(pGeom!=nullptr)
						pGroup->AddMember(pGeom);
				}
				pGroup->RecalRect();
				pGroup->RecalCentroid();
				return pGroup;
			}
		case wkbMultiLineString: /**< GeometryCollection of LineStrings, standard WKB*/
			{
				OGRGeometryCollection* poGeometryCollectio = (OGRGeometryCollection*)poGeometry;
				CGroup* pGroup = new CGroup();
				pGroup->m_geoId = ++m_dwMaxGeomId;

				for(int part = 0; part<poGeometryCollectio->getNumGeometries(); part++)
				{
					OGRGeometry* pGeometry = poGeometryCollectio->getGeometryRef(part);
					if(pGeometry==nullptr)
						continue;
					CGeom* pGeom = ReadGeometry(library, pGeometry, nullptr, fPointToDoc);
					if(pGeom!=nullptr)
						pGroup->AddMember(pGeom);
				}
				pGroup->m_bClosed = false;
				pGroup->RecalRect();
				pGroup->RecalCentroid();
				pGroup->m_pLine = CImportGdal::ReadStroke(poStyleMgr);
				return pGroup;
			}
		case wkbMultiPolygon: /**< GeometryCollection of Polygons, standard WKB*/
			{
				OGRGeometryCollection* poGeometryCollectio = (OGRGeometryCollection*)poGeometry;
				CCompound* pCompound = new CCompound();//not only simple polygons(path), some are doghnuts
				pCompound->m_geoId = ++m_dwMaxGeomId;

				for(int part = 0; part<poGeometryCollectio->getNumGeometries(); part++)
				{
					OGRGeometry* pGeometry = poGeometryCollectio->getGeometryRef(part);
					if(pGeometry==nullptr)
						continue;
					CGeom* pGeom = ReadGeometry(library, pGeometry, nullptr, fPointToDoc);
					if(pGeom!=nullptr)
						pCompound->AddMember(pGeom);
				}
				pCompound->m_bClosed = true;
				pCompound->RecalRect();
				pCompound->RecalCentroid();
				pCompound->m_pLine = CImportGdal::ReadStroke(poStyleMgr);
				pCompound->m_pFill = CImportGdal::ReadFill(poStyleMgr);
				return pCompound;
			}
		case wkbGeometryCollection: /**< geometric object that is a collection of 1	   or more geometric objects, standard WKB*/
			{
				OGRGeometryCollection* poGeometryCollectio = (OGRGeometryCollection*)poGeometry;
				CGroup* pGroup = new CGroup();
				pGroup->m_geoId = ++m_dwMaxGeomId;

				for(int part = 0; part<poGeometryCollectio->getNumGeometries(); part++)
				{
					OGRGeometry* pGeometry = poGeometryCollectio->getGeometryRef(part);
					if(pGeometry==nullptr)
						continue;
					CGeom* pGeom = ReadGeometry(library, pGeometry, poStyleMgr, fPointToDoc);
					if(pGeom!=nullptr)
						pGroup->AddMember(pGeom);
				}
				pGroup->RecalRect();
				pGroup->RecalCentroid();
				return pGroup;
			}
			break;
		case wkbNone: /**< non-standard, for pure attribute records */
			break;
		default:
			break;
	}

	return nullptr;
}

CPoly* CImportGdal::ReadLineString(OGRLineString* poLineString)
{
	if(poLineString==nullptr)
		return nullptr;
	const int nAnchors = poLineString->getNumPoints();
	if(nAnchors<2)
		return nullptr;

	CPoly* pPoly = new CPoly();
	pPoly->m_geoId = ++m_dwMaxGeomId;
	for(int index = 0; index<nAnchors; index++)
	{
		OGRPoint ogrpoint;
		poLineString->getPoint(index, &ogrpoint);
		CPoint point = Change(ogrpoint.getX(), ogrpoint.getY());
		pPoly->AddAnchor(point, true, true);
	}
	pPoly->m_bClosed = false;
	return pPoly;
}

CPoly* CImportGdal::ReadLinearRing(OGRLinearRing* poLinearRing)
{
	if(poLinearRing==nullptr)
		return nullptr;
	const int nAnchors = poLinearRing->getNumPoints();
	if(nAnchors<3)
		return nullptr;

	CPoly* pPoly = ReadLineString(poLinearRing);
	if(pPoly!=nullptr)
	{
		if(pPoly->m_pPoints[0]!=pPoly->m_pPoints[pPoly->m_nAnchors-1])
		{
			pPoly->AddAnchor(pPoly->m_pPoints[0], true, true);
		}
		pPoly->m_bClosed = true;
		pPoly->RecalCentroid();
		return pPoly;
	}
	else
	{
		return nullptr;
	}
}

CProjection* CImportGdal::NewProjFromOGRSpatialReference(OGRSpatialReference* reference, double& offsetX, double& offsetY)
{
	if(reference==nullptr)
		return nullptr;
	else if(reference->GetRoot()==nullptr)
		return nullptr;

	const char* pszProjection = reference->GetAttrValue("PROJECTION");
	if(pszProjection==nullptr&&reference->IsGeographic()==FALSE)
		return nullptr;
	else if(pszProjection==nullptr&&reference->IsGeographic()==TRUE)
		return new CGeographic();
	else
	{
		offsetX = reference->GetNormProjParm(SRS_PP_FALSE_EASTING, 0.0);
		offsetY = reference->GetNormProjParm(SRS_PP_FALSE_NORTHING, 0.0);

		double lng0 = reference->GetNormProjParm(SRS_PP_CENTRAL_MERIDIAN, 0.0);
		double lat0 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0);
		const double lngc = reference->GetNormProjParm(SRS_PP_CENTRAL_MERIDIAN, 0.0);
		double lat1 = reference->GetNormProjParm(SRS_PP_STANDARD_PARALLEL_1, 0.0);
		double lat2 = reference->GetNormProjParm(SRS_PP_STANDARD_PARALLEL_2, 0.0);
		double lng1 = 0;
		double lng2 = 0;
		double lats = 0;
		const double k0 = reference->GetNormProjParm(SRS_PP_SCALE_FACTOR, 1.0);
		const double h = reference->GetNormProjParm(SRS_PP_SATELLITE_HEIGHT, 35785831.0);
		const double alpha = reference->GetNormProjParm(SRS_PP_AZIMUTH, 0.0);
		const double gamma = reference->GetNormProjParm(SRS_PP_RECTIFIED_GRID_ANGLE, 1000.0);
		int zone = 0;
		int bNorth = true;
		char key[256];

		if(EQUAL(pszProjection, SRS_PT_CYLINDRICAL_EQUAL_AREA))
		{
			strcpy_s(key, "cea");
			lats = reference->GetNormProjParm(SRS_PP_STANDARD_PARALLEL_1, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_BONNE))
			strcpy_s(key, "bonne");
		else if(EQUAL(pszProjection, SRS_PT_CASSINI_SOLDNER))
			strcpy_s(key, "cass");
		else if(EQUAL(pszProjection, SRS_PT_NEW_ZEALAND_MAP_GRID))
			strcpy_s(key, "nzmg");
		else if(EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR)||
			EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR_MI_21)||
			EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR_MI_22)||
			EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR_MI_23)||
			EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR_MI_24)||
			EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR_MI_25))
		{
			zone = reference->GetUTMZone(&bNorth);
			if(zone!=0)
				strcpy_s(key, "utm");
			else
				strcpy_s(key, "tmerc");
		}
		else if(EQUAL(pszProjection, SRS_PT_TRANSVERSE_MERCATOR_SOUTH_ORIENTED))
			strcpy_s(key, "tmerc");
		else if(EQUAL(pszProjection, SRS_PT_MERCATOR_1SP))
		{
			if(reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0)==0.0)
				strcpy_s(key, "merc");
			else if(reference->GetNormProjParm(SRS_PP_SCALE_FACTOR, 1.0)==1.0)
			{
				strcpy_s(key, "merc");
				lats = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0);
			}
			else
				return nullptr;
		}
		else if(EQUAL(pszProjection, SRS_PT_MERCATOR_2SP))
		{
			strcpy_s(key, "merc");
			lats = reference->GetNormProjParm(SRS_PP_STANDARD_PARALLEL_1, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_OBLIQUE_STEREOGRAPHIC))
			strcpy_s(key, "sterea");
		else if(EQUAL(pszProjection, SRS_PT_STEREOGRAPHIC))
			strcpy_s(key, "stere");
		else if(EQUAL(pszProjection, SRS_PT_POLAR_STEREOGRAPHIC))
		{
			if(reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0)>=0.0)
			{
				strcpy_s(key, "stere");
				lat0 = 90;
				lats = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 90.0);
			}
			else
			{
				strcpy_s(key, "stere");
				lat0 = -90;
				lats = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, -90.0);
			}
		}
		else if(EQUAL(pszProjection, SRS_PT_EQUIRECTANGULAR))
		{
			strcpy_s(key, "eqc");
			lats = reference->GetNormProjParm(SRS_PP_STANDARD_PARALLEL_1, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_GAUSSSCHREIBERTMERCATOR))
		{
			strcpy_s(key, "gstmerc");
			lat0 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, -21.116666667);
			lng0 = reference->GetNormProjParm(SRS_PP_CENTRAL_MERIDIAN, 55.53333333309);

			offsetX = reference->GetNormProjParm(SRS_PP_FALSE_EASTING, 160000.000);
			offsetY = reference->GetNormProjParm(SRS_PP_FALSE_NORTHING, 50000.000);
		}
		else if(EQUAL(pszProjection, SRS_PT_GNOMONIC))
			strcpy_s(key, "gnom");
		else if(EQUAL(pszProjection, SRS_PT_ORTHOGRAPHIC))
			strcpy_s(key, "ortho");
		else if(EQUAL(pszProjection, SRS_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA))
			strcpy_s(key, "laea");
		else if(EQUAL(pszProjection, SRS_PT_AZIMUTHAL_EQUIDISTANT))
			strcpy_s(key, "aeqd");
		else if(EQUAL(pszProjection, SRS_PT_EQUIDISTANT_CONIC))
		{
			strcpy_s(key, "eqdc");
			lng0 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_CENTER, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_MILLER_CYLINDRICAL))
			strcpy_s(key, "mill");
		else if(EQUAL(pszProjection, SRS_PT_MOLLWEIDE))
			strcpy_s(key, "moll");
		else if(EQUAL(pszProjection, SRS_PT_ECKERT_I))
			strcpy_s(key, "eck1");
		else if(EQUAL(pszProjection, SRS_PT_ECKERT_II))
			strcpy_s(key, "eck2");
		else if(EQUAL(pszProjection, SRS_PT_ECKERT_III))
			strcpy_s(key, "eck3");
		else if(EQUAL(pszProjection, SRS_PT_ECKERT_IV))
			strcpy_s(key, "eck4");
		else if(EQUAL(pszProjection, SRS_PT_ECKERT_V))
			strcpy_s(key, "eck5");
		else if(EQUAL(pszProjection, SRS_PT_ECKERT_VI))
			strcpy_s(key, "eck6");
		else if(EQUAL(pszProjection, SRS_PT_POLYCONIC))
			strcpy_s(key, "poly");
		else if(EQUAL(pszProjection, SRS_PT_ALBERS_CONIC_EQUAL_AREA))
			strcpy_s(key, "aea");
		else if(EQUAL(pszProjection, SRS_PT_ROBINSON))
			strcpy_s(key, "robin");
		else if(EQUAL(pszProjection, SRS_PT_VANDERGRINTEN))
			strcpy_s(key, "vandg");
		else if(EQUAL(pszProjection, SRS_PT_SINUSOIDAL))
		{
			strcpy_s(key, "sinu");
			lng0 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_CENTER, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_GALL_STEREOGRAPHIC))
			strcpy_s(key, "gall");
		else if(EQUAL(pszProjection, SRS_PT_GOODE_HOMOLOSINE))
			strcpy_s(key, "goode");
		else if(EQUAL(pszProjection, SRS_PT_GEOSTATIONARY_SATELLITE))
			strcpy_s(key, "geos");
		else if(EQUAL(pszProjection, SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP)
			||EQUAL(pszProjection, SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP_BELGIUM))
			strcpy_s(key, "lcc");
		else if(EQUAL(pszProjection, SRS_PT_LAMBERT_CONFORMAL_CONIC_1SP))
		{
			strcpy_s(key, "lcc");
			lat1 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_HOTINE_OBLIQUE_MERCATOR))
		{
			/* special case for swiss oblique mercator : see bug 423 */
			if(std::abs(reference->GetNormProjParm(SRS_PP_AZIMUTH, 0.0)-90.0)<0.0001
				&&std::abs(reference->GetNormProjParm(SRS_PP_RECTIFIED_GRID_ANGLE, 0.0)-90.0)<0.0001)
				strcpy_s(key, "somerc");
			else
				strcpy_s(key, "omerc");
		}
		else if(EQUAL(pszProjection, SRS_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN))
		{
			strcpy_s(key, "omerc");
			lat1 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0);
			lng1 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0);
			lat2 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_POINT_2, 0.0);
			lng2 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_KROVAK))
		{
			strcpy_s(key, "krovak");
			lng0 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_CENTER, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_TWO_POINT_EQUIDISTANT))
		{
			strcpy_s(key, "tpeqd");
			lat1 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0);
			lng1 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0);
			lat2 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_POINT_2, 0.0);
			lng2 = reference->GetNormProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_IMW_POLYCONIC))
		{
			strcpy_s(key, "imw_p");
			lat1 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0);
			lat2 = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_POINT_2, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_I))
			strcpy_s(key, "wag1");
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_II))
			strcpy_s(key, "wag2");
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_III))
		{
			strcpy_s(key, "wag3");
			lats = reference->GetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0);
		}
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_IV))
			strcpy_s(key, "wag4");
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_V))
			strcpy_s(key, "wag5");
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_VI))
			strcpy_s(key, "wag6");
		else if(EQUAL(pszProjection, SRS_PT_WAGNER_VII))
			strcpy_s(key, "wag7");
		/* Note: This never really gets used currently.  See bug 423 */
		else if(EQUAL(pszProjection, SRS_PT_SWISS_OBLIQUE_CYLINDRICAL))
			strcpy_s(key, "somerc");
		else
			return nullptr;

		CGeocentric::ProjType* pProjType = CGeocentric::GetProjType(key);
		if(pProjType!=nullptr)
		{
			CGeocentric* pGeocentric = new CGeocentric(key);
			pGeocentric->m_lng0 = lng0;
			pGeocentric->m_lat0 = lat0;
			pGeocentric->m_lat1 = lat1;
			pGeocentric->m_lat2 = lat2;
			pGeocentric->m_lats = lats;
			pGeocentric->m_lngc = lngc;
			pGeocentric->m_lng1 = lng1;
			pGeocentric->m_lng2 = lng2;
			pGeocentric->m_k0 = k0;
			pGeocentric->m_south = bNorth==TRUE ? FALSE : TRUE;
			pGeocentric->m_zone = zone;
			pGeocentric->m_h = h;
			pGeocentric->m_alpha = alpha;
			pGeocentric->m_gamma = gamma;
			const double a = reference->GetSemiMajor();
			const double inf = reference->GetInvFlattening();
			bool bFound = false;
			const Spheroid* pSpheroidList = CProjection::GetSpheroidList();
			for(int index = 0; pSpheroidList[index].key!=nullptr; index++)
			{
				if(std::abs(pSpheroidList[index].a-a)>0.01)
					continue;
				else if(pSpheroidList[index].f==0&&inf!=0)
					continue;
				else if(pSpheroidList[index].f!=0&&inf==0)
					continue;
				else if(pSpheroidList[index].f==0&&inf==0)
				{
					pGeocentric->GCS.Setup(pSpheroidList[index].key);
					bFound = true;
					break;
				}
				else if(std::abs(1.0/pSpheroidList[index].f-inf)<0.0001)
				{
					pGeocentric->GCS.Setup(pSpheroidList[index].key);
					bFound = true;
					break;
				}
			}

			if(bFound==false)
			{
				pGeocentric->GCS.Setup("sphere");
			}
			pGeocentric->Reconfig();
			return pGeocentric;
		}
		else
			return nullptr;
	}
}

CGeom* CImportGdal::ReadPoint(OGRPoint* poGeometry, OGRStyleMgr* poStyleMgr, CLibrary& library, double fPointToDoc)
{
	if(poGeometry==nullptr)
		return nullptr;
	const CPoint point = Change(poGeometry->getX(), poGeometry->getY());
	if(poStyleMgr==nullptr)
	{
		CMark* pMark = new CMark(point);
		pMark->CalCorner(nullptr, m_datainfo, library);
		return pMark;
	}
	CSpot* pSpot = ReadSpot(poStyleMgr, library);
	CGeom* pGeom = nullptr;
	if(pSpot!=nullptr)
	{
		pGeom = new CMark(point);
		((CMark*)pGeom)->m_pSpot = pSpot;
	}
	else
	{
		pGeom = new CText(point);
	}
	pGeom->m_geoId = ++m_dwMaxGeomId;

	for(int i = 0; i<poStyleMgr->GetPartCount(nullptr); i++)
	{
		OGRStyleTool* poStyleTool = poStyleMgr->GetPart(i, nullptr);
		if(poStyleTool==nullptr)
			continue;
		if(poStyleTool->GetType()==OGRSTCSymbol&&pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
		{
			OGRStyleSymbol* poStyleSymbol = (OGRStyleSymbol*)poStyleTool;
			GBool bDeault = FALSE;
			const double angle = poStyleSymbol->Angle(bDeault);
			if(angle!=0.0)
			{
				const CPoint center = ((CMark*)pGeom)->m_Origin;
				((CMark*)pGeom)->Move(CSize(-center.x, -center.y));
				double fSin = sin((float)angle*M_PI/180);
				double fCos = cos((float)angle*M_PI/180);
				((CMark*)pGeom)->Transform(fCos, -fSin, 0, fSin, fCos, 0);
				((CMark*)pGeom)->m_Origin = center;
			}
		}
		else if(poStyleTool->GetType()==OGRSTCLabel)
		{
			OGRStyleLabel* poStylLabel = (OGRStyleLabel*)poStyleTool;
			OGRSTUnitId unitid = poStylLabel->GetUnit();
			poStylLabel->SetUnit(OGRSTUGround);
			GBool bDeault = FALSE;
			const char* pszText = poStylLabel->TextString(bDeault);
			if(pszText!=nullptr)
			{
				pGeom->m_strName = CString(pszText);
			}
			const CString strFont = CString(poStylLabel->FontName(bDeault));
			const float fSize = poStylLabel->Size(bDeault);
			const char* pszForecolor = poStylLabel->ForeColor(bDeault);
			if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
			{
				CHint* pHint = new CHint();
				pHint->m_fontdesc.SetFaceName(strFont);
				pHint->m_fSize = std::abs(fSize);
				pHint->m_pColor = pszForecolor==nullptr ? nullptr : new CColorSpot(pszForecolor);
				const char* pszBackcolor = poStylLabel->BackColor(bDeault);
				if(bDeault==FALSE&&pszBackcolor!=nullptr)
				{
					pHint->m_filter = CHint::Filter::Frame;
					pHint->m_frame.m_bBackground = TRUE;
					//pHint->m_frame.m_argbBackground; = CColor::AGBToAGRB(strtoul(pszBackcolor+1, nullptr, 16));
				}
				pGeom->m_pHint = pHint;
				const int anchor = poStylLabel->Anchor(bDeault);
				CTag* pTag = new CTag(pGeom->m_strName, (ANCHOR)((anchor-1)%9+1));
				pGeom->m_pTag = pTag;

				((CMark*)pGeom)->CalCorner(nullptr, m_datainfo, library);
			}
			else if(pGeom->IsKindOf(RUNTIME_CLASS(CText)))
			{
				CType* pType = new CType();
				pType->m_fontdesc.SetFaceName(strFont);
				pType->m_fSize = abs(fSize);
				pType->m_pColor = pszForecolor==nullptr ? nullptr : new CColorSpot(pszForecolor);
				pType->m_deform.m_nHorzscale = poStylLabel->Stretch(bDeault)*100;
				pType->m_deform.m_nHorzscale = bDeault==true ? 100 : pType->m_deform.m_nHorzscale;
				const double angle = poStylLabel->Angle(bDeault);
				if(bDeault==false&&angle!=0.0)
				{
					const CPoint origin = ((CText*)pGeom)->m_Origin;
					((CText*)pGeom)->Move(CSize(-origin.x, -origin.y));
					double fSin = sin((float)angle*M_PI/180);
					double fCos = cos((float)angle*M_PI/180);
					((CText*)pGeom)->Transform(fCos, -fSin, 0, fSin, fCos, 0);
					((CText*)pGeom)->m_Origin = origin;
				}
				pGeom->m_pType = pType;
				((CText*)pGeom)->CalCorner(nullptr, fPointToDoc, 1.f);
			}
		}
	}

	return pGeom;
}

CSpot* CImportGdal::ReadSpot(OGRStyleMgr* poStyleMgr, CLibrary& library) const
{
	if(poStyleMgr==nullptr)
		return nullptr;

	CSpot* pSpot = nullptr;
	for(int i = 0; i<poStyleMgr->GetPartCount(nullptr); i++)
	{
		OGRStyleTool* poStyleTool = poStyleMgr->GetPart(i, nullptr);
		if(poStyleTool==nullptr)
			continue;

		if(poStyleTool->GetType()==OGRSTCSymbol)
		{
			OGRStyleSymbol* poStyleSymbol = (OGRStyleSymbol*)poStyleTool;
			poStyleSymbol->SetUnit(OGRSTUGround);
			GBool bDeault = FALSE;
			const int nSize = poStyleSymbol->Size(bDeault)*10;
			const char* pszSymbolId = poStyleSymbol->Id(bDeault);
			if(pszSymbolId!=nullptr&&(strstr(pszSymbolId, "mapinfo-sym-")||strstr(pszSymbolId, "ogr-sym-")))
			{
				CSpotFont* pSpotFont = new CSpotFont();
				pSpotFont->m_strFontReal = _T("MapInfo Symbols");
				pSpotFont->m_strFontFace = _T("MapInfo Symbols");
				pSpotFont->m_pColor = new CColorSpot(poStyleSymbol->Color(bDeault));
				pSpotFont->m_nSize = abs(nSize);
				if(strstr(pszSymbolId, "mapinfo-sym-"))
				{
					pSpotFont->m_wId = atoi(pszSymbolId+12);
				}
				else if(strstr(pszSymbolId, "ogr-sym-"))
				{
					pSpotFont->m_wId = atoi(pszSymbolId+8);
					// The OGR symbol is not the MapInfo one          
					// Here's some mapping             
					switch(pSpotFont->m_wId)
					{
						case 0:
							pSpotFont->m_wId = 31;
							break;
						case 1:
							pSpotFont->m_wId = 49;
							break;
						case 2:
							pSpotFont->m_wId = 50;
							break;
						case 3:
							pSpotFont->m_wId = 40;
							break;
						case 4:
							pSpotFont->m_wId = 34;
							break;
						case 5:
							pSpotFont->m_wId = 38;
							break;
						case 6:
							pSpotFont->m_wId = 32;
							break;
						case 7:
							pSpotFont->m_wId = 42;
							break;
						case 8:
							pSpotFont->m_wId = 36;
							break;
						case 9:
							pSpotFont->m_wId = 41;
							break;
						case 10:
							pSpotFont->m_wId = 35;
							break;
					}
				}

				pSpot = pSpotFont;
				delete poStyleTool;
				break;
			}
			else if(pszSymbolId!=nullptr&&strstr(pszSymbolId, "bmp-"))
			{
				const CString strSymbolFolder = _T("");
				const CString strSymbol = strSymbolFolder+CString(pszSymbolId+4);
				CPatternMid* pMid = nullptr;
				for(std::map<WORD, CPatternMid*>::const_iterator it = library.m_libSpotPattern.m_midlist.begin(); it!=library.m_libSpotPattern.m_midlist.end(); ++it)
				{
					CPatternMid* mid = it->second;
					if(strSymbol.CompareNoCase(mid->m_strFile)==0)
					{
						pMid = mid;
						break;
					}
				}
				if(pMid!=nullptr)
				{
					pMid->m_nUsers++;
				}
				else
				{
					pMid = new CPatternMid(strSymbol);
					pMid->m_wId = library.m_libSpotPattern.GetMaxId()+1;
					pMid->m_nUsers = 1;
					library.m_libSpotPattern.m_midlist[pMid->m_wId] = pMid;
				}
				CSpotPattern* pSpotPattern = new CSpotPattern();
				pSpotPattern->m_libId = pMid->m_wId;
				pSpotPattern->m_nIndex = 0;
				pSpotPattern->m_nScale = pMid->m_nHeight==0 ? 100*nSize/16 : 100*nSize/pMid->m_nHeight;

				pSpot = pSpotPattern;
				delete poStyleTool;
				break;
			}
			double angle = poStyleSymbol->Angle(bDeault);
		}

		delete poStyleTool;
	}

	return pSpot;
}

CLine* CImportGdal::ReadStroke(OGRStyleMgr* poStyleMgr) const
{
	if(poStyleMgr==nullptr)
		return nullptr;

	for(int i = 0; i<poStyleMgr->GetPartCount(nullptr); i++)
	{
		OGRStyleTool* poStyleTool = poStyleMgr->GetPart(i, nullptr);
		if(poStyleTool==nullptr)
			continue;
		if(poStyleTool->GetType()==OGRSTCPen)
		{
			OGRStylePen* poStylePen = (OGRStylePen*)poStyleTool;
			poStylePen->SetUnit(OGRSTUGround);
			GBool bDeault = FALSE;
			const int id = poStylePen->GetSpecificId(poStylePen->Id(bDeault), "ogr-pen");
			if(id==1)
			{
				delete poStyleTool;
				return new CLineEmpty();
			}
			else
			{
				CLine* pLine = new CLine();
				pLine->m_pColor = new CColorSpot(poStylePen->Color(bDeault));
				pLine->m_nWidth = max(poStylePen->Width(bDeault), 0.1)*10;

				const char* pszPattern = poStylePen->Pattern(bDeault);
				if(pszPattern!=nullptr&&strstr(pszPattern, " ")!=nullptr)
				{
					const char delimiters[] = " ,;:!-";
					int seg = 0;
					char values[256];
					memcpy(values, pszPattern, strlen(pszPattern));
					values[strlen(pszPattern)] = 0;
					char* value = strtok(values, delimiters);
					while(value!=nullptr)
					{
						CPLString plstring;
						plstring.assign(value);
						pLine->m_dash.m_segment[seg++] = abs(atof(plstring)*10);
						value = strtok(nullptr, delimiters);
					}
				}
				pLine->m_bCapindex = poStylePen->Cap(bDeault)=="b" ? 0 : (poStylePen->Cap(bDeault)=="r" ? 1 : 2);
				pLine->m_bJoinindex = poStylePen->Join(bDeault)=="m" ? 0 : (poStylePen->Cap(bDeault)=="r" ? 1 : 2);

				delete poStyleTool;
				return pLine;
			}
			break;
		}

		delete poStyleTool;
	}

	return nullptr;
}

CFillCompact* CImportGdal::ReadFill(OGRStyleMgr* poStyleMgr)
{
	if(poStyleMgr==nullptr)
		return nullptr;

	for(int i = 0; i<poStyleMgr->GetPartCount(nullptr); i++)
	{
		OGRStyleTool* poStyleTool = poStyleMgr->GetPart(i, nullptr);
		if(poStyleTool==nullptr)
			continue;
		if(poStyleTool->GetType()==OGRSTCBrush)
		{
			OGRStyleBrush* poStyleBrush = (OGRStyleBrush*)poStyleTool;
			poStyleBrush->SetUnit(OGRSTUGround);

			GBool bDeault = FALSE;
			const int id = poStyleBrush->GetSpecificId(poStyleBrush->Id(bDeault), "ogr-brush");
			if(id==1)
			{
				delete poStyleTool;
				return new CFillEmpty();
			}
			else
			{
				CFillAlone* pFill = new CFillAlone();
				pFill->m_pColor = new CColorSpot(poStyleBrush->ForeColor(bDeault));
				delete poStyleTool;
				return pFill;
			}
			break;
		}

		delete poStyleTool;
	}

	return nullptr;
}
