#include "stdafx.h"
#include "Resource.h"
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "Global.h"

#include "../../Thirdparty\GDAL\3.3.3\gcore\gdal.h"
#include "../../Thirdparty\GDAL\3.3.3\gcore\gdal_priv.h"

#include "../../Thirdparty\GDAL\3.3.3\ogr\ogr_api.h"
#include "../../Thirdparty\GDAL\3.3.3\ogr\ogr_srs_api.h"


#include "ImportMapInfo.h"
#include "NameFieldDlg.h"

#include "../Geometry/Poly.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/Bezier.h"

#include "../Style/ColorSpot.h"
#include "../Style/Spot.h"
#include "../Style/Type.h"
#include "../Style/Fill.h"
#include "../Style/Line.h"
#include "../Style/LineEmpty.h"
#include "../Style/SpotChar.h"
#include "../Style/FillEmpty.h"
#include "../Style/SpotPattern.h"

#include "../Projection/Projection1.h"
#include "../Projection/Geographic.h"
#include "../Projection/Geocentric.h"

#include "../DataView/DatainfoDlg.h"

#include "../Public/Global.h"

//#include "../../Thirdparty\Mitab\mitab_capi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//39.01811111
//117.13883889

CString strSymbolFolder = _T("");
CImportMapInfo::CImportMapInfo(CDatainfo& datainfo)
	:CImport(datainfo)
{
	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(nullptr,buff,sizeof(buff)); 	
	CString strPath = buff;

	strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);	 
	strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE,_T("Diwatu"),_T(""),_T(""),strPath);
	strSymbolFolder = strPath + _T("Symbols\\MapInfo\\");
}

void CImportMapInfo::ReadLayer(CLayer* layer,LPCTSTR lpszTab,CDaoDatabase* pDataBase)
{
	CT2CA caTable(lpszTab);
//	mitab_handle dataset = mitab_c_open(caTable.m_psz);
	GDALDataset* pDataset = (GDALDataset*)GDALOpen(caTable.m_psz, GA_ReadOnly );
	if( pDataset != nullptr ) //error
	{
		CString strError;
	//	strError.Format(_T("mitab_c_open(%s) failed.\n%s\n"),lpszTab, mitab_c_getlasterrormsg()); 
		AfxMessageBox(strError);
		return;
	}

	CString strTable = lpszTab;
	strTable = strTable.Right(strTable.GetLength() - strTable.ReverseFind(_T('\\')) - 1);
	strTable = strTable.Left(strTable.GetLength() - 4);

	long nameField = -1;
	CDaoRecordset rs(pDataBase);
	if(pDataBase != nullptr)
	{
		/*if(CreateTable(dataset,strTable,pDataBase) == TRUE)
		{
			if(layer != nullptr)
			{	
				layer->m_attTable = strTable;
			}

			CString strSQL;
			strSQL.Format(_T("SELECT * FROM [%s]"), strTable);
			rs.Open(dbOpenDynaset, strSQL);		
			nameField = GetNameField(pDataBase,strTable);
		}*/
	}
	m_offset.cx = 0;
	m_offset.cy = 0;
	CProjection* pProjection = this->CreateProjection(pDataset);
	if(pProjection != nullptr)
		m_fScale = pProjection->m_dilationMapToDoc;
	else
		m_fScale = 10000;

	//for(int dwId = mitab_c_next_feature_id(dataset,-1); dwId != -1;  dwId = mitab_c_next_feature_id(dataset,dwId) )
	//{
	//	mitab_feature feature = mitab_c_read_feature(dataset, dwId );
	//	if( feature == nullptr )
	//	{
	//		printf( "Failed to read feature %d.\n%s\n",dwId, mitab_c_getlasterrormsg() );
	//		exit( 1 );
	//	}

	//	CGeom* pGeom = nullptr;
	//	int type = mitab_c_get_type(feature);
	//	switch(type)
	//	{
	//	case TABFC_NoGeom: 
	//		break;
	//	case TABFC_Point: 
	//	case TABFC_FontPoint: 
	//	case TABFC_CustomPoint: 
	//		{
	//			double x = mitab_c_get_vertex_x(feature, 0, 0);
	//			double y = mitab_c_get_vertex_y(feature, 0, 0);
	//			CPoint point = this->Change(x,y);
	//			CMark* pMark = new CMark(point);
	//			pMark->m_geoId = ++m_maxId;
	//			pGeom = pMark;
	//			if(type == TABFC_FontPoint)
	//			{
	//				double angle = mitab_c_get_symbol_angle(feature); 
	//				if(angle != 0)
	//				{
	//				}
	//			}
	//		}
	//		break;
	//	case TABFC_Text: 
	//		{
	//			double x = mitab_c_get_vertex_x(feature, 0, 0);
	//			double y = mitab_c_get_vertex_y(feature, 0, 0);
	//			CPoint point = this->Change(x,y);
	//			const char* text = mitab_c_get_text(feature);  
	//			CText* pText = new CText(point);
	//			pText->m_strName = CString(text);
	//			pText->m_geoId = ++m_maxId;
	//			pGeom = pText;
	//		}
	//		break;
	//	case TABFC_Polyline: 
	//		{
	//			CGroup* pGroup = nullptr;
	//			int parts = mitab_c_get_parts(feature);
	//			if(parts >1)
	//			{
	//				pGroup = new CGroup();
	//				pGroup->m_geoId = ++m_maxId;
	//				pGeom = pGroup;
	//			}

	//			for(int part=0;part<parts;part++)
	//			{
	//				CPoly* pPoly = new CPoly();
	//				pPoly->m_geoId = ++m_maxId;

	//				int num_points = mitab_c_get_vertex_count(feature, part);
	//				for(int pointno = 0; pointno < num_points; pointno++)
	//				{
	//					double x = mitab_c_get_vertex_x(feature, part, pointno);
	//					double y = mitab_c_get_vertex_y(feature, part, pointno);
	//					CPoint point = this->Change(x,y);
	//					pPoly->AddAnchor(point, true, true);
	//				}
	//				if(pGroup != nullptr)
	//				{
	//					pGroup->AddMember(pPoly);
	//				}
	//				else
	//					pGeom = pPoly;
	//			}
	//			pGeom->RecalRect();
	//		}
	//		break;
	//	case TABFC_Arc: 
	//		{
	//			CBezier* pBezier = new CBezier();
	//			pBezier->m_geoId = ++m_maxId;
	//			pGeom = pBezier;
	//		}
	//		break;
	//	case TABFC_Region: 
	//		{
	//			CDoughnut* pDonut = nullptr;
	//			int parts = mitab_c_get_parts(feature);
	//			if(parts >1)
	//			{
	//				pDonut = new CDoughnut();
	//				pDonut->m_geoId = ++m_maxId;
	//				pDonut->m_bClosed = TRUE;
	//				pGeom = pDonut;
	//			}

	//			for(int part=0;part<parts;part++)
	//			{
	//				CPoly* pPoly = new CPoly();
	//				pPoly->m_geoId = ++m_maxId;
	//				pPoly->m_bClosed = TRUE;

	//				int num_points = mitab_c_get_vertex_count(feature, part);
	//				for(int pointno = 0; pointno < num_points; pointno++)
	//				{
	//					double x = mitab_c_get_vertex_x(feature, part, pointno);
	//					double y = mitab_c_get_vertex_y(feature, part, pointno);
	//					CPoint point = this->Change(x,y);
	//					pPoly->AddAnchor(point, true, true);
	//				}
	//				if(pPoly->GetAnchor(1) != pPoly->GetAnchor(pPoly->m_nAnchors))
	//					pPoly->AddAnchor(pPoly->GetAnchor(1),true, true);

	//				pPoly->RecalRect();

	//				if(pDonut != nullptr)
	//					pDonut->AddMember(pPoly);
	//				else
	//					pGeom = pPoly;
	//			}
	//			if(pGeom != nullptr)
	//				pGeom->RecalRect();
	//		}
	//		break;
	//	case TABFC_Rectangle:
	//		{

	//		}
	//		break;
	//	case TABFC_Ellipse: 
	//		{

	//		}
	//		break;
	//	case TABFC_MultiPoint: 
	//		{
	//			CGroup* pGroup = new CGroup();
	//			pGroup->m_geoId = ++m_maxId;
	//			int parts = mitab_c_get_parts(feature);
	//			for(int part=0;part<parts;part++)
	//			{	
	//				CMark* pMark = new CMark();
	//				int color = mitab_c_get_symbol_color(feature); 
	//				int symbol = mitab_c_get_symbol_no(feature); 
	//				int size = mitab_c_get_symbol_size(feature); 
	//				pGroup->AddMember(pMark);
	//			}
	//			pGeom = pGroup;
	//		}
	//		break;
	//	case TABFC_Collection: 
	//		printf("\n");
	//		break;
	//	default:
	//		printf("\n");
	//		break;
	//	}

	//	if(pGeom != nullptr)
	//	{
	//		switch(type)
	//		{
	//		case TABFC_NoGeom: 
	//			break;
	//		case TABFC_Point: 
	//			{
	//				int color = mitab_c_get_symbol_color(feature); 
	//				int symbol = mitab_c_get_symbol_no(feature); 
	//				int size = mitab_c_get_symbol_size(feature);
	//				CSpotChar* pSpot = new CSpotChar();
	//				pSpot->m_pColor = new CColorSpot(color);
	//				pSpot->m_nSize = size*10;
	//				pSpot->m_wId = symbol;
	//				pSpot->m_strFont = _T("MapInfo Cartographic");
	//				((CMark*)pGeom)->m_pSpot = pSpot;
	//				((CMark*)pGeom)->CalCorner(layer->m_pSpot,&m_datainfo,&library);
	//			}
	//			break;
	//		case TABFC_FontPoint: 
	//			{
	//				const char* font = mitab_c_get_font(feature); 
	//				int angle = mitab_c_get_symbol_angle(feature);
	//				int style = mitab_c_get_symbol_style(feature); 					
	//				int color = mitab_c_get_symbol_color(feature); 
	//				int symbol = mitab_c_get_symbol_no(feature); 
	//				int size = mitab_c_get_symbol_size(feature);

	//				CSpotChar* pSpot = new CSpotChar();
	//				pSpot->m_strFont = CString(font);
	//				pSpot->m_pColor = new CColorSpot(color);
	//				pSpot->m_nSize = size*10;
	//				pSpot->m_wId = symbol;
	//				((CMark*)pGeom)->m_pSpot = pSpot;
	//				((CMark*)pGeom)->CalCorner(layer->m_pSpot,&m_datainfo,&library);
	//			}
	//			break;
	//		case TABFC_CustomPoint: 
	//			{
	//				const char* font = mitab_c_get_font(feature); 
	//				int size = mitab_c_get_symbol_size(feature);
	//				CString strSymbol = strSymbolFolder + CString(font);
	//				CPatternTag* pTag = nullptr;
	//				for(std::list<CPatternTag*>::iterator it = library.m_libSpotPattern.m_midlist.begin(); it != library.m_libSpotPattern.m_midlist.end(); ++it)
	//				{
	//					CPatternTag* tag = *it;
	//					if(tag == nullptr)
	//						continue;
	//					if(strSymbol.CompareNoCase(tag->m_strFile) == 0)
	//					{
	//						pTag = tag;
	//						break;
	//					}
	//				}
	//				if(pTag != nullptr)
	//				{
	//					pTag->m_nUsers++;
	//				}
	//				else
	//				{
	//					pTag = new CPatternTag(strSymbol);
	//					pTag->m_wId = library.m_libSpotPattern.GetMaxId()+1;
	//					pTag->m_nUsers = 1;
	//					library.m_libSpotPattern.m_midlist.push_back(pTag);
	//				}
	//				CSpotPattern* pSpot = new CSpotPattern();
	//				pSpot->m_libId = pTag->m_wId;
	//				pSpot->m_nIndex = 0;
	//				pSpot->m_nScale = pTag->m_nHeight == 0 ? 100*size/16 : 100*size/pTag->m_nHeight;
	//				((CMark*)pGeom)->m_pSpot = pSpot;
	//				((CMark*)pGeom)->CalCorner(layer->m_pSpot,&m_datainfo,&library);
	//			}
	//			break;
	//		case TABFC_Text: 
	//			{
	//				CType* pType = this->ReadType(feature);
	//				((CText*)pGeom)->m_pType = pType;
	//				((CText*)pGeom)->CalCorner(layer->m_pType,1);
	//			}
	//			break;
	//		case TABFC_Polyline: 
	//		case TABFC_Arc: 
	//			{
	//				CLine* pLine = this->ReadStroke(feature);
	//				pGeom->m_pLine = pLine;
	//			}
	//			break;
	//		case TABFC_Region: 
	//		case TABFC_Rectangle:
	//		case TABFC_Ellipse: 
	//			{
	//				CLine* pLine = this->ReadStroke(feature);
	//				pGeom->m_pLine = pLine;					
	//				CFill* pFill = this->ReadFill(feature);
	//				pGeom->m_pFill = pFill;
	//			}
	//			break;
	//		case TABFC_MultiPoint: 
	//			{
	//				int color = mitab_c_get_symbol_color(feature); 
	//				int symbol = mitab_c_get_symbol_no(feature); 
	//				int size = mitab_c_get_symbol_size(feature);
	//				CSpotChar* pSpot = new CSpotChar();
	//				pSpot->m_pColor = new CColorSpot(color);
	//				pSpot->m_nSize = size*10;
	//				pSpot->m_wId = symbol;
	//				pSpot->m_strFont = _T("MapInfo Cartographic");
	//				POSITION pos = ((CGroup*)pGeom)->m_geomlist.GetHeadPosition();
	//				while(pos != nullptr)
	//				{
	//					CMark* pMark = (CMark*)(((CGroup*)pGeom)->m_geomlist.GetNext(pos));
	//					pMark->m_pSpot = pSpot->Clone();
	//					pMark->CalCorner(layer->m_pSpot,&m_datainfo,&library);
	//				}  
	//				if(pSpot != nullptr)	
	//				{
	//					delete pSpot;
	//				}
	//			}
	//			break;
	//		case TABFC_Collection: 
	//			printf("\n");
	//			break;
	//		default:
	//			printf("\n");
	//			break;
	//		}

	//		try
	//		{
	//			if(rs.IsOpen() == TRUE)
	//			{
	//				rs.AddNew();

	//				int index = 0;
	//				int	fields = mitab_c_get_field_count(dataset);
	//				for(int fieldno = 0; fieldno < fields; fieldno++)//Dump the feature attributes...
	//				{
	//					const char* strName = mitab_c_get_field_name(dataset, fieldno);
	//					const char* strValue = mitab_c_get_field_as_string(feature, fieldno);
	//					if(_strnicmp(strName,"AID",2) == 0)
	//					{
	//						unsigned __int64  ddwId=_atoi64(strValue);
	//						ddwId &= 0X00000000FFFFFFFF;
	//						pGeom->m_attId = ddwId;
	//						char value[12];
	//						_ui64toa(ddwId,value,10);
	//						strValue = value;
	//					}
	//					else if(index == nameField && pGeom->IsKindOf(RUNTIME_CLASS(CText)) == FALSE)
	//						pGeom->m_strName = CString(strValue);

	//					int type = mitab_c_get_field_type(dataset,fieldno); 
	//					switch(type)
	//					{
	//					case TABFT_Char: 
	//						rs.SetFieldValue(index++,COleVariant(CString(strValue), VT_BSTRT));
	//						break;
	//					case TABFT_Integer:
	//						rs.SetFieldValue(index++,COleVariant((long)atoi(strValue), VT_I4));
	//						break;
	//					case TABFT_SmallInt:
	//						rs.SetFieldValue(index++,COleVariant((short)atoi(strValue), VT_I2));
	//						break;
	//					case TABFT_Decimal:
	//						rs.SetFieldValue(index++,COleVariant(strtod(strValue,nullptr)));
	//						break;
	//					case TABFT_Float:
	//						rs.SetFieldValue(index++,COleVariant(strtod(strValue,nullptr)));
	//						break;
	//					case TABFT_Date:
	//						{
	//							long value = strtol(strValue,nullptr,10);
	//							int year = value/10000;
	//							int month = value/100 - year*100;
	//							int day = value - year*10000 - month*100;

	//							COleDateTime(year,month,day,0,0,0);

	//							rs.SetFieldValue(index++,COleDateTime(year,month,day,0,0,0));
	//						}
	//						break;
	//					case TABFT_Logical:
	//						{
	//							BOOL value;
	//							if(strcmp(strValue,"T") == 0)
	//								value = TRUE;
	//							else
	//								value = FALSE;

	//							rs.SetFieldValue(index++,COleVariant((short)value,VT_BOOL));
	//						}
	//						break;
	//					default:
	//						break;
	//					}		
	//				}
	//				rs.Update();
	//			}
	//		}
	//		catch(CDBException* ex)
	//		{
	//			e->ReportError();
	//		}
	//			
	//		layer->m_geomlist.AddTail(pGeom);
	//	}

	//	mitab_c_destroy_feature( feature );
	//}
	/*rs.Close();

	mitab_c_close( dataset );
	if( mitab_c_getlasterrormsg() != nullptr  && strlen(mitab_c_getlasterrormsg()) > 0 )
	{
		fprintf( stderr, "Last Error: %s\n", mitab_c_getlasterrormsg() );
	}*/
	
}

//BOOL CImportMapInfo::CreateTable(mitab_handle dataset,CString strTable,CDaoDatabase* pDataBase)
//{
//	if(pDataBase == nullptr)
//		return FALSE;
//	if(dataset == nullptr)
//		return FALSE;
//	try
//	{
//		CDaoTableDefInfo tableInfo;
//		long nTableCount = pDataBase->GetTableDefCount();
//		for(long I = 0; I < nTableCount; I++)
//		{
//			pDataBase->GetTableDefInfo(I, tableInfo);
//			if(strTable.CompareNoCase(tableInfo.m_strName) == 0)
//			{	
//				pDataBase->DeleteTableDef(strTable);
//				break;
//			}
//		}
//
//		CDaoTableDef td(pDataBase);	
//		td.Create(strTable, dbAttachExclusive, _T(""), _T("DATABASE = "));
		//int	fields = mitab_c_get_field_count(dataset);
		//for(int fieldno = 0; fieldno < fields; fieldno++)//Dump the feature attributes...
		//{	
		//	const char* strName = mitab_c_get_field_name(dataset, fieldno);
		//	int tolerance = mitab_c_get_field_precision(dataset,fieldno); 
		//	int type = mitab_c_get_field_type(dataset,fieldno); 
		//	switch(type)
		//	{
		//	case TABFT_Char: 
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbText;
		//			fieldInfo.m_lSize = mitab_c_get_field_width(dataset,fieldno);  ;
		//			fieldInfo.m_lAttributes = dbVariableField;
		//			fieldInfo.m_bAllowZeroLength = TRUE;	
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	case TABFT_Integer:
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_lAttributes = dbFixedField;  
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbLong;
		//			fieldInfo.m_lSize =  4;
		//			fieldInfo.m_lAttributes = dbFixedField; 
		//			fieldInfo.m_bAllowZeroLength = FALSE;
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	case TABFT_SmallInt:
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_lAttributes = dbFixedField;  
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbInteger;
		//			fieldInfo.m_lSize =  2;
		//			fieldInfo.m_lAttributes = dbFixedField; 
		//			fieldInfo.m_bAllowZeroLength = FALSE;
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	case TABFT_Decimal:
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_lAttributes = dbFixedField;  
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbDouble;
		//			fieldInfo.m_lSize =  8;
		//			fieldInfo.m_bAllowZeroLength = FALSE;
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	case TABFT_Float:
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_lAttributes = dbFixedField;  
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbDouble;
		//			fieldInfo.m_lSize =  8;
		//			fieldInfo.m_bAllowZeroLength = FALSE;
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	case TABFT_Date:
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_lAttributes = dbFixedField;  
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbDate;
		//			fieldInfo.m_lSize =  8;
		//			fieldInfo.m_bAllowZeroLength = FALSE;	
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	case TABFT_Logical:
		//		{
		//			CDaoFieldInfo fieldInfo;		
		//			fieldInfo.m_strName = CString(strName);
		//			fieldInfo.m_lAttributes = dbFixedField;  
		//			fieldInfo.m_nOrdinalPosition = 0;
		//			fieldInfo.m_bRequired        = FALSE;
		//			fieldInfo.m_lCollatingOrder  = 0;
		//			fieldInfo.m_nType = dbBoolean;
		//			fieldInfo.m_lSize =  1;
		//			fieldInfo.m_bAllowZeroLength = FALSE;
		//			td.CreateField(fieldInfo);
		//		}
		//		break;
		//	default:
		//		break;
		//	}		
		//}
//		td.Append();	
//		td.Close();
//	}
//	catch(CDBException* ex)
//	{
//		e->ReportError();
//	}
//		return TRUE;
//}
/*
CLine* CImportMapInfo::ReadStroke(mitab_feature feature)
{
	int pattern = mitab_c_get_pen_pattern(feature);
	switch(pattern)
	{
	case 2:
		{
			int color = mitab_c_get_pen_color(feature);  
			int width = mitab_c_get_pen_width(feature); 
			if(width>10)
				width = width-10;
			else
				width *= 10;

			CLine* pLine = new CLine();
			pLine->m_pColor = new CColorSpot(color);
			pLine->m_nWidth = width;
			return pLine;
		}
	case 1:
		{
			return new CLineEmpty();
		}
		break;
	default:
		break;
	}
	return nullptr;
}
CFill* CImportMapInfo::ReadFill(mitab_feature feature)
{
	//int pattern2 = mitab_c_get_brush_pattern(feature);  
	//if(pattern2 == 2)//2 is solid 1 in none
	//{
	//	int color2 = mitab_c_get_brush_fgcolor(feature); 
	//	CFill* pFill = new CFill();
	//	pFill->m_pColor = new CColorSpot(color2);  					
	//	int transparent = mitab_c_get_brush_transparent(feature);   
	//	//		if(transparent == 0)
	//	{	
	//		int color3 = mitab_c_get_brush_bgcolor(feature);  
	//	}
	//	//	else if(transparent == 1)
	//	//		return new CFillEmpty();
	//	//	else
	//	//		return nullptr;

	//	return pFill;
	//}
	//else 
		return nullptr;


}
CType* CImportMapInfo::ReadType(mitab_feature feature)
{
	/*const char* font = mitab_c_get_font(feature);   
	double angle = mitab_c_get_text_angle(feature); 
	int color1 = mitab_c_get_text_fgcolor (feature);  
	int color2 = mitab_c_get_text_bgcolor(feature);  

	double width = mitab_c_get_text_width(feature);     
	double height = mitab_c_get_text_height(feature);    
	int spacing = mitab_c_get_text_spacing(feature);     
	int linetype = mitab_c_get_text_linetype(feature);     
	int justification = mitab_c_get_text_justification(feature);     

	CType* pType = new CType();
	pType->m_pColor = new CColorSpot(color1);
	pType->m_fontdesc.m_strName = CString(font);
	pType->m_nSize = height*10;
	pType->m_nHSkewAngle = width*100/height;
	pType->m_nLeterspace = spacing;
	pType->m_nWordspace  = spacing;

	return pType;
}

	 */
