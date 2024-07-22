#include "stdafx.h"
#include "Resource.h"
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "Global.h"

#include "ImportMapInfoMif.h"
#include "NameFieldDlg.h"

#include "../Geometry/Poly.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"
#include "../Geometry/Doughnut.h"

#include "../Style/ColorSpot.h"
#include "../Style/Spot.h"
#include "../Style/Type.h"
#include "../Style/Fill.h"
#include "../Style/Line.h"

#include "../Projection/Projection1.h"
#include "../DataView/DatainfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//39.01811111
//117.13883889


CImportMapInfoMif::CImportMapInfoMif(const CDatainfo& datainfo)
		   :CImportMapInfo(datainfo)
{
}

BOOL CImportMapInfoMif::Import(LPCTSTR lpszPath,DWORD& maxId)
{
	delete m_datainfo.m_pProjection;
	m_datainfo.m_pProjection = new CProjection();
	m_datainfo.m_pProjection->SetupGCS(2);
	m_datainfo.m_scaleMaximum = 1000;
	m_datainfo.m_scaleSource  = 10000;
	m_datainfo.m_scaleMinimum = 800000000;

	HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
	AfxSetResourceHandle(hInst);

	CDatainfoDlg dlg(nullptr,m_datainfo);
	if(dlg.DoModal() != IDOK)
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return FALSE;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	if(m_datainfo.m_pProjection != nullptr)
		m_fScale = m_datainfo.m_pProjection->m_dilationMapToDoc;
	else
		m_fScale = 10000;

	m_maxId = maxId;

	CString strPath = lpszPath;
	if(strPath.ReverseFind(_T('\\')) != -1)
	{
		strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	}

	CString strFolder = strPath;
	if(strFolder.ReverseFind(_T('\\')) != -1)
	{
		strFolder = strFolder.Mid(strFolder.ReverseFind(_T('\\'))+1);
	}
	CDaoDatabase* pMDBDataBase = CImportMapInfo::CreateDatabase(strPath,strFolder);

	CFileException e;
	WIN32_FIND_DATA FindDate;
	HANDLE hFile = FindFirstFile(strPath + _T("\\*.mif"),&FindDate);
	do
    {
		if((FindDate.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			continue;
		
		CString strMif = strPath + _T('\\') + FindDate.cFileName;
		
		CString strLayer = FindDate.cFileName;
		strLayer =strLayer.Left(strLayer.GetLength() - 4);
		
		CLayer* layer = layertree.GetByName(strLayer);
		if(strLayer.CompareNoCase(_T("Block")) == 0)
			layer = layertree.GetByName(_T("居民色块"));
		else if(strLayer.CompareNoCase(_T("Landuse")) == 0)
			layer = layertree.GetByName(_T("土地利用"));
		else if(strLayer.CompareNoCase(_T("Road")) == 0)
			layer = layertree.GetByName(_T("交通"));
		else if(strLayer.CompareNoCase(_T("Adm_Area")) == 0)
			layer = layertree.GetByName(_T("行政区划"));
		else if(strLayer.CompareNoCase(_T("Railway")) == 0)
			layer = layertree.GetByName(_T("铁路"));
		else if(strLayer.CompareNoCase(_T("Guojie")) == 0)
			layer = layertree.GetByName(_T("国界"));

		if(layer == nullptr && strLayer.CompareNoCase(_T("Node")) != 0)
		{
			layer= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
			layer->m_wId = layertree.ApplyId();
			layer->m_strName = strLayer;

			layertree.AddHead(layer);	
		}
		
		ReadMif(library,layer,strMif,pMDBDataBase);
		
		if(strLayer.CompareNoCase(_T("Landuse")) == 0)
		{
	//		layer->m_attTable = _T("");
	//		DistinguishLanduse(layer,pMDBDataBase,layertree);
		}
		else if(strLayer.CompareNoCase(_T("Adm_Area")) == 0)
		{
	//		layer->m_attTable = _T("");
	//		DistinguishAdmArea(layer,pMDBDataBase,layertree);
		}
		else if(strLayer.CompareNoCase(_T("Road")) == 0)
		{
	//		layer->m_attTable = _T("");
	//		DistinguishRoad(lpszPath,layer,pMDBDataBase,layertree);
		}
	}while(FindNextFile(hFile,&FindDate)==TRUE);

	if(pMDBDataBase != nullptr)
	{
		pMDBDataBase->Close();
		delete pMDBDataBase;
	}

	maxId = m_maxId;
	return CImportMapInfo::Import(lpszPath);
}


BOOL CImportMapInfoMif::ReadMif(CLibrary& library,CLayer* layer,LPCTSTR lpszShapePath,CDaoDatabase* pDataBase)
{
	CString strTable = lpszShapePath;
	strTable = strTable.Right(strTable.GetLength() - strTable.ReverseFind(_T('\\')) - 1);
	strTable = strTable.Left(strTable.GetLength() - 4);

	CStdioFile mifFile;
	CStdioFile midFile;
	
	long nameField = -1;
	bool bRecord = false;	
	CDaoRecordset rs(pDataBase);
	if(CreateTable(lpszShapePath,pDataBase) == TRUE)
	{
		if(layer != nullptr)
		{	
			layer->m_attTable = strTable;
		}
		
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), strTable);
		rs.Open(dbOpenDynaset, strSQL);		

		CString strMid = lpszShapePath;
		strMid.MakeUpper();
		strMid.Replace(_T(".MIF"),_T(".mid"));
		midFile.Open(strMid, CFile::modeRead | CFile::shareDenyWrite);

		nameField = GetNameField(pDataBase,strTable);
		bRecord = true;
	}	
	
	if(mifFile.Open(lpszShapePath, CFile::modeRead | CFile::shareDenyWrite) == false)
	{
		midFile.Close();
		return false;
	}
	
	
	CString rRead;
	while(mifFile.ReadString(rRead))
	{
		CGeom* pGeom = nullptr;

		double x, y;
		CString rQ = rRead.SpanExcluding(_T(" "));
		if(!rQ.CompareNoCase(_T("Region")))
		{
			rRead.TrimLeft(rQ);
          
			long nRegionCount = _ttoi(rRead);
			if(nRegionCount == 1)
			{
				pGeom = new CPoly;
			}
			else
			{
				pGeom = new CDoughnut;
			}
			pGeom->m_bClosed = TRUE;
			pGeom->m_geoId = ++m_maxId;
			
			for(long I = 0; I < nRegionCount; I ++)
			{
				if(!mifFile.ReadString(rRead)) 
					return false;
				
				long nPointCount = _ttoi(rRead);
				if(!nPointCount) 
					continue;
				
				CPoly* poly = nullptr;
				if(nRegionCount == 1)
				{
					poly = (CPoly*)pGeom;
				}
				else if(nRegionCount >  1)
				{
					poly = new CPoly;
					poly->m_bClosed = TRUE;
					poly->m_geoId = ++m_maxId;
					((CDoughnut*)pGeom)->m_geomlist.AddTail(poly);
				}

				for(long J = 0; J < nPointCount; J ++)
				{
					if(!mifFile.ReadString(rRead)) 						
						return false;

					double x1 = 0.0f;
					double y1 = 0.0f;
					_stscanf(rRead, _T("%lf %lf"), &x1, &y1);
					CPoint point = Change(x1, y1);
					poly->AddAnchor(point, true, true);
				}
			}

			if(strTable.CompareNoCase(_T("Adm_Area")) == 0)
			{
				this->ReadBrush(mifFile,pGeom);
			}
		}
		else if(!rQ.CompareNoCase(_T("Line")))
		{
			rRead.TrimLeft(rQ);
			
			pGeom = new CPoly;
			pGeom->m_geoId = ++m_maxId;

			double x1, y1;
			double x2, y2;
			_stscanf(rRead, _T("%lf %lf %lf %lf"), &x1, &y1, &x2, &y2);
			CPoint point1 = Change(x1, y1);				
			pGeom->AddAnchor(point1, true, true);
			CPoint point2 = Change(x2, y2);
			pGeom->AddAnchor(point2, true, true);
		}
		else if(!rQ.CompareNoCase(_T("Pline")))
		{
			long nPolyCount = 1;
			rRead.TrimLeft(rQ);
			rRead.TrimLeft();
			rQ = rRead.SpanExcluding(_T(" "));
			if(!rQ.CompareNoCase(_T("Multiple")))
			{
				rRead.TrimLeft(rQ);
				rRead.TrimLeft();
				nPolyCount = _ttoi(rRead);
				mifFile.ReadString(rRead);
			}

			if(nPolyCount == 1)
			{
				pGeom = new CPoly;
				pGeom->m_bClosed = false;
			}
			else
			{
				pGeom = new CDoughnut;
				pGeom->m_bClosed = false;
			}
			
			pGeom->m_geoId = ++m_maxId;
			
			CPoly* poly = nullptr;
			for(long i = 0;i<nPolyCount;i++)
			{
				if(i != 0)
				{
					mifFile.ReadString(rRead);
				}

				long nPointCount = _ttoi(rRead);
				if(!nPointCount) 
					continue;

				if(nPolyCount == 1)
				{
					poly = (CPoly*)pGeom;
				}
				if(nPolyCount > 1)
				{
					poly = new CPoly;
					poly->m_bClosed = false;
					poly->m_geoId = ++m_maxId;
					((CDoughnut*)pGeom)->m_geomlist.AddTail(poly);
				}
	
				for(long J = 0; J < nPointCount; J ++)
				{
					if(!mifFile.ReadString(rRead)) 
						return false;

					double x1, y1;
					_stscanf(rRead, _T("%lf %lf"), &x1, &y1);
					CPoint point = Change(x1, y1);
					poly->AddAnchor(point, true, true);
				}
			}
		}
		else if(!rQ.CompareNoCase(_T("CPoint")))
		{
			rRead.TrimLeft(rQ);
			
			_stscanf(rRead, _T("%lf %lf"), &x, &y);
			CPoint point=Change(x,y);
			
			pGeom = new CMark(point);
			pGeom->m_geoId = ++m_maxId;
			if(layer != nullptr)
			{
				((CMark*)pGeom)->CalCorner(layer->m_pSpot,m_datainfo,&library);
			}
		}
		else if(!rQ.CompareNoCase(_T("Text")))
		{
			rRead.TrimLeft(rQ);		
			if(!mifFile.ReadString(rRead)) 
				return false;

			long nFirst=rRead.Find(_T('"'));
			long nEnd=rRead.ReverseFind(_T('"'));

		
			CString word = rRead.Mid(nFirst+1, nEnd-nFirst-1);
			
			double x1, y1;
			double x2, y2;
			
			mifFile.ReadString(rRead);
			rRead.TrimLeft();
			rRead.TrimRight();
			if(rRead.Find(_T(' ')) >= 3)
			{
				_stscanf(rRead, _T("%lf %lf %lf %lf"), &x1,&y1,&x2, &y2);
			}
			else
			{
				_stscanf(rRead, _T("%lf %lf"), &x1,  &y1);
				mifFile.ReadString(rRead);
				_stscanf(rRead, _T("%lf %lf"), &x2, &y2);
			}
			
			CPoint point1 = Change(x1,y1);
			CPoint point2 = Change(x2,y2);
			
			pGeom = new CText(CPoint(point1.x,point1.y));
			pGeom->m_geoId = ++m_maxId;

			pGeom->m_strName = word;	
			if(layer != nullptr)
			{
				((CText*)pGeom)->CalCorner(layer->GetType(),m_datainfo.m_zoomPointToDoc);
			}
		}
		else if(!rQ.CompareNoCase(_T("Rect")))
		{
			rRead.TrimLeft(rQ);
			
			double x1, y1;
			double x2, y2;
			_stscanf(rRead, _T("%lf %lf %lf %lf"), &x1, &y1, &x2, &y2);

			CPoint point1 = Change(x1,y1);
			CPoint point2 = Change(x2,y2);
			pGeom = new CPRect(CRect(point1.x, point1.y, point2.x, point2.y));
			pGeom->m_geoId = ++m_maxId;
		}
		else if(!rQ.CompareNoCase(_T("None")))
		{
			if(bRecord == true)
			{
				midFile.ReadString(rRead);
			}
			continue;
		}
		else
		{
			continue;
		}
		
		if(pGeom != nullptr && bRecord == TRUE)
		{	
			midFile.ReadString(rRead);
			if(rRead.IsEmpty() == TRUE)
			{
				AfxMessageBox(_T("There is an error in the relation between mif and mid!"));
				continue;
			}

			rRead.Replace(_T(",\"\"\""),_T(",\""));
			
			//滤掉多余的""
			int pos = rRead.Find(_T("\"\""));
			while(pos != -1)
			{
				if(pos != 0)
				{
					if(rRead[pos-1] == _T(','))
					{
						pos += 2;
						pos = rRead.Find(_T("\"\""),pos);
						continue;
					}
				}
				if(pos+2<rRead.GetLength())
				{
					if(rRead[pos+2] == _T(','))
					{
						pos += 2;
						pos = rRead.Find(_T("\"\""),pos);
						continue;
					}
				}
				rRead.Delete(pos,2);
				pos = rRead.Find(_T("\"\""),pos);
			}//滤掉多余的""
			
			rs.AddNew();

			long field = 0;
			if(pGeom != nullptr)
			{
				COleVariant var = COleVariant((long)(pGeom->m_geoId), VT_I4);;
				rs.SetFieldValue(field++,var);	
			}

			CString strMid = rRead;
			while(strMid.IsEmpty() == false)
			{
				if(strMid.Find(_T(",")) != -1)
				{
					if(strMid.Left(1) == _T('\"'))
					{
						strMid.SetAt(0,_T(' '));
						if(strMid.Find(_T('\"')) != -1)
						{
							rQ = strMid.Left(strMid.Find(_T('\"'))+1);
							rQ.SetAt(0,'\"');
						}
						else if(strMid.Find(_T('?')) != -1)
						{
							rQ = strMid.Left(strMid.Find(_T('?'))+1);
							rQ.SetAt(0,'\"');
						}

						strMid = strMid.Mid(strMid.Find(_T('\"'))+1);
						if(strMid.IsEmpty() == FALSE)
							strMid = strMid.Mid(1);
					}
					else
					{
						rQ = strMid.SpanExcluding(_T(","));
						strMid = strMid.Mid(strMid.Find(_T(","))+1);
					}
				}
				else
				{
					rQ = strMid;
					strMid.Empty();
				}
				
				COleVariant var;
				CDaoFieldInfo fieldinfo;
				rs.GetFieldInfo(field,fieldinfo);
				switch(fieldinfo.m_nType)
				{
				case dbBoolean:
					{
						BOOL value;
						if(rQ == _T("T"))
							value = TRUE;
						else
							value = FALSE;

						var = COleVariant((short)value,VT_BOOL);
					}
					break;
				case dbByte:	
					break;
				case dbInteger:
					{
						int value = _tcstol(rQ,nullptr,10);
						var = COleVariant((short)value,VT_I2);
					}
					break;
				case dbLong:
					{
						long value = _tcstol(rQ,nullptr,10);
						var = COleVariant(value,VT_I4);
					}
				  break;
				case dbCurrency:
				  break;
				case dbSingle:
					{
						float value = _tcstod(rQ,nullptr);
						var = COleVariant(value);
					}
				  break;
				case dbDouble:
					{
						double value = _tcstod(rQ,nullptr);
						var = COleVariant(value);
					}
				  break;
				case dbDate:
					{
						int value = _tcstol(rQ,nullptr,10);
						int year = value/10000;
						int month = value/100 - year*100;
						int day = value - year*10000 - month*100;
						
						COleDateTime datatime(year,month,day,0,0,0);
						var = COleVariant(datatime);
					}
				  break;
				case dbText:
					if(rQ.Find(_T('\"')) != -1)
					{
						rQ = rQ.Mid(rQ.Find(_T('\"'))+1);
						rQ = rQ.Left(rQ.ReverseFind(_T('\"')));
						var = COleVariant((LPCTSTR)rQ, VT_BSTRT);
					}
				  break;
				case dbLongBinary:
				  break;
				case dbMemo:
				  break;
				case dbGUID:
				  break;
				}

				if(field == nameField)
				{
					if(pGeom != nullptr && pGeom->m_strName.IsEmpty() == TRUE)
					{
						pGeom->m_strName = rQ;
					}
				}

				try
				{
					rs.SetFieldValue(field++,var);		
				}
				catch(CDaoException* e)
				{
					CString strMessage;
					strMessage.Format(_T("发现mid文件错误: 文件->%s, 语句->%s"),lpszShapePath,rRead);
					AfxMessageBox(strMessage);
					strMid.Empty();
					e->ReportError();
					e->Delete();
					break;
				}
			}

			if(layer == nullptr)
			{
				COleVariant var;
				var = COleVariant(x);
				rs.SetFieldValue(_T("X"),var);
				var = COleVariant(y);
				rs.SetFieldValue(_T("Y"),var);
			}
			
			rs.Update();
		}

		if(layer != nullptr && pGeom != nullptr)
		{
			layer->m_geomlist.AddTail(pGeom);	
		}
		else if(pGeom != nullptr)
		{
			delete pGeom;
			pGeom = nullptr;
		}
	}		
  
	mifFile.Close();	

	if(bRecord == TRUE)
	{
		midFile.Close();
		rs.Close();
	}
	return TRUE;
}

void CImportMapInfoMif::DistinguishLanduse(CLayer* layer,CDaoDatabase* pMDBDataBase,CLayerTree& layertree)
{
	CLayer* layer22 = layertree.GetByName(_T("文化娱乐"));
	CLayer* layer24 = layertree.GetByName(_T("体育场"));
	CLayer* layer27 = layertree.GetByName(_T("文物古迹"));
	CLayer* layer42 = layertree.GetByName(_T("机场"));
	CLayer* layer51 = layertree.GetByName(_T("广场"));
	CLayer* layer71 = layertree.GetByName(_T("公园"));
	CLayer* layer72 = layertree.GetByName(_T("绿地"));
	CLayer* layer81 = layertree.GetByName(_T("海洋"));

	CLayer* shuanxianhe = layertree.GetByName(_T("双线河"));
	CLayer* danxianhe = layertree.GetByName(_T("单线河"));
	CLayer* yunhe = layertree.GetByName(_T("运河"));
	CLayer* qudao = layertree.GetByName(_T("渠道"));
	
	CLayer* shuiku = layertree.GetByName(_T("水库"));
	CLayer* hupo = layertree.GetByName(_T("湖泊"));
	
	CLayer* daoyu = layertree.GetByName(_T("岛屿"));
	
	CLayer* senlin = layertree.GetByName(_T("森林"));
	
	CDaoRecordset rs(pMDBDataBase);
	
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM %s"), _T("Landuse"));
	rs.Open(dbOpenDynaset, strSQL);		

	POSITION pos1 = layer->m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* geom = layer->m_geomlist.GetNext(pos1);
		layer->m_geomlist.RemoveAt(pos2);

		COleVariant var = rs.GetFieldValue(_T("Type"));
		int oldType = V_I2(&var);
		if(oldType == 22)
		{
			layer22->m_geomlist.AddTail(geom);
		}
		else if(oldType == 24)
		{
			layer24->m_geomlist.AddTail(geom);
		}
		else if(oldType == 27)
		{
			layer27->m_geomlist.AddTail(geom);
		}
		else if(oldType == 42)
		{
			layer42->m_geomlist.AddTail(geom);
		}
		else if(oldType == 51)
		{
			layer51->m_geomlist.AddTail(geom);
		}
		else if(oldType == 71)
		{
			layer71->m_geomlist.AddTail(geom);
		}
		else if(oldType == 72)
		{
			layer72->m_geomlist.AddTail(geom);
		}
		else if(oldType == 81)
		{
			layer81->m_geomlist.AddTail(geom);
		}
		else if(oldType == 82)
		{
			if(geom->m_bClosed == true)
				shuanxianhe->m_geomlist.AddTail(geom);
			else
				danxianhe->m_geomlist.AddTail(geom);
		}
		else if(oldType == 83 || 
				oldType == 85 || 
				oldType == 86)
		{
			if(geom->m_strName.Find(_T("渠")) != -1)
				qudao->m_geomlist.AddTail(geom);
			else if(geom->m_strName.Find(_T("水道")) != -1)
				qudao->m_geomlist.AddTail(geom);
			else if(geom->m_strName.Find(_T("运河")) != -1)
				yunhe->m_geomlist.AddTail(geom);
			else if(geom->m_strName.Find(_T("水库")) != -1)
				shuiku->m_geomlist.AddTail(geom);
			else if(geom->m_bClosed == true)
				shuanxianhe->m_geomlist.AddTail(geom);
			else
				danxianhe->m_geomlist.AddTail(geom);
		}
		else if(oldType == 84)
		{
			if(geom->m_strName.Find(_T("水库")) != -1)
				shuiku->m_geomlist.AddTail(geom);
			else
				hupo->m_geomlist.AddTail(geom);
		}
		else if(oldType == 95)
		{
			daoyu->m_geomlist.AddTail(geom);
		}
		else if(oldType == 96)
		{
			senlin->m_geomlist.AddTail(geom);
		}
		else if(oldType == 0  ||
				oldType == 11 ||
				oldType == 21 ||
				oldType == 23 ||
				oldType == 25 ||
				oldType == 26 ||
				oldType == 31 ||
				oldType == 32 ||
				oldType == 41 ||
				oldType == 43 ||
				oldType == 52 ||
				oldType == 61 ||
				oldType == 91 ||
				oldType == 92 ||
				oldType == 93 ||
				oldType == 94)
		{
			delete geom;
		}
		else
		{
		//	AfxMessageBox("没有考虑到的代码！");
		}

		rs.MoveNext();
	}

	rs.Close();
}


void CImportMapInfoMif::DistinguishRoad(LPCTSTR lpszShapePath,CLayer* layer,CDaoDatabase* pMDBDataBase,CLayerTree& layertree)
{
	//首先对road表进行过滤，因为有很多重复的线段
	{
		CDaoRecordset rs(pMDBDataBase);
		
		POSITION pos1;
		POSITION pos2;
		DWORD  dwF = 0;
		DWORD  dwT = 0;
		double fL = 0.0f;

		CString strSQL = _T("select * from road order by fnode asc,tnode asc,ID asc");
		rs.Open(dbOpenDynaset, strSQL);
		while(rs.IsEOF() == FALSE)
		{	
			COleVariant varID = rs.GetFieldValue(_T("ID"));
			DWORD dwId = V_I4(&varID);

			COleVariant varFNode = rs.GetFieldValue(_T("FNODE"));
			DWORD dwFNode = V_I4(&varFNode);

			COleVariant varTNode = rs.GetFieldValue(_T("TNODE"));			
			DWORD dwTNode = V_I4(&varTNode);

			COleVariant varLeng = rs.GetFieldValue(_T("LENGTH"));			
			double fLen = V_R8(&varLeng);

			if(dwFNode == dwF && dwTNode == dwT && std::abs(fLen-fL)<0.0000001f)
			{
				rs.Delete();
			
				while((pos2 = pos1) != nullptr)
				{
					CGeom* geom = layer->m_geomlist.GetNext(pos1);
					if(geom->m_geoId == dwId)
					{
						layer->m_geomlist.RemoveAt(pos2);
						delete geom;
						geom=nullptr;
						break;
					}
				}
			}
			else
			{
				pos2 = pos1 = layer->m_geomlist.GetHeadPosition();
				dwF = dwFNode;
				dwT = dwTNode;
				fL = fLen;
			}
			
			rs.MoveNext();
		}	

		rs.Close();
	}

/*

	CTopology* pTopology = new CRoadTopo;
	pTopology->m_strName = _T("Route");
	pTopology->m_bVisible = false;
	pTopology->m_Rect.SetRectEmpty();

	CString str = lpszShapePath;

	//建立所有的节点
	{	
		CDaoRecordset rs(pMDBDataBase);
		
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), _T("Node"));
		rs.Open(dbOpenDynaset, strSQL);	
		while(rs.IsEOF() == FALSE)
		{
			CRoadNode* pNode = new CRoadNode();
			
			COleVariant varID = rs.GetFieldValue(_T("NODE_ID"));			
			pNode->m_dwId = V_I4(&varID);
			
			COleVariant varX = rs.GetFieldValue(_T("X"));
			double x  = V_R8(&varX);
			COleVariant varY = rs.GetFieldValue(_T("Y"));
			double y = V_R8(&varY);
			pNode->m_Point = Change(x,y);
		
			pTopology->m_mapIdNode[pNode->m_dwId] = pNode;
	
			rs.MoveNext();
		}
		rs.Close();
	}

	////建立所有的边
	{
		CDaoRecordset rs(pMDBDataBase);
	
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), _T("Road"));
		rs.Open(dbOpenDynaset, strSQL);		
		while(rs.IsEOF() == FALSE)
		{
			CRoadEdge* pEdge = new CRoadEdge;
		
			COleVariant var;
			var = rs.GetFieldValue(_T("ROAD_ID"));
			pEdge->m_dwId = V_I4(&var);
	
			var = rs.GetFieldValue(_T("FNODE"));
			pEdge->m_dwFromNode = V_I4(&var);

			var = rs.GetFieldValue(_T("TNODE"));
			pEdge->m_dwToNode = V_I4(&var);

			CNode* pNode1 = pTopology->GetNode(pEdge->m_dwFromNode);
			if(pNode1 != nullptr)
			{
				pNode1->m_edgelist.Add(pEdge->m_dwId);
			}
			else
			{
				AfxMessageBox("数据有误");
			}

			CNode* pNode2 = pTopology->GetNode(pEdge->m_dwToNode);
			if(pNode2 != nullptr)
			{
				pNode2->m_edgelist.Add(pEdge->m_dwId);
			}
			else
			{
				AfxMessageBox("数据有误");
			}

			pTopology->m_mapIdEdge[pEdge->m_dwId] = pEdge;

			rs.MoveNext();
		}

		rs.Close();
	}
	//为所有的节点设定Demention
	{
		for(CTopology::CNodeMap::iterator it = pTopology->m_mapIdNode.begin();it != pTopology->m_mapIdNode.end();it++)
		{
			CRoadNode* pNode = (CRoadNode*)it->second;
			int nCount = pNode->m_edgelist.GetCount();
			pNode->m_Forbid.SetDimention(nCount);
		}
	}
*/

	CLayer* gaosu = layertree.GetByName(_T("高速公路"));
	CLayer* guodao = layertree.GetByName(_T("国道"));
	CLayer* shengdao = layertree.GetByName(_T("省道"));
	CLayer* xianxiangdao = layertree.GetByName(_T("县乡道"));
	CLayer* chengshikuaisu = layertree.GetByName(_T("城市快速路"));
	CLayer* chengshizhugan = layertree.GetByName(_T("城市主干道"));
	CLayer* chengshizhi = layertree.GetByName(_T("城市支路"));
	CLayer* linongxiang = layertree.GetByName(_T("里弄巷道"));
	CLayer* qita = layertree.GetByName(_T("其他道路"));
	
	gaosu->m_attTable = _T("Road");
	guodao->m_attTable = _T("Road");
	shengdao->m_attTable = _T("Road");
	xianxiangdao->m_attTable = _T("Road");
	chengshikuaisu->m_attTable = _T("Road");
	chengshizhugan->m_attTable = _T("Road");
	chengshizhi->m_attTable = _T("Road");
	linongxiang->m_attTable = _T("Road");
	qita->m_attTable = _T("Road");

	CDaoRecordset rs(pMDBDataBase);
	
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM %s"), _T("Road"));
	rs.Open(dbOpenDynaset, strSQL);		

	POSITION pos1 = layer->m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* geom = layer->m_geomlist.GetNext(pos1);
		if(geom == nullptr)
		{
			AfxMessageBox(_T("数据有误!"));
			rs.MoveNext();
			continue;
		}
/*		
		COleVariant var;
		var = rs.GetFieldValue(_T("ROAD_ID"));
		DWORD dwId = V_I4(&var);
		CRoadEdge* pEdge = (CRoadEdge*)pTopology->GetEdge(dwId);
		if(pEdge == nullptr)
		{
			AfxMessageBox(_T("数据有误!"));
			rs.MoveNext();
			continue;
		}	

		pTopology->m_Rect.UnionRect(pTopology->m_Rect,geom->m_Rect);
		pEdge->m_Rect = geom->m_Rect;
		pEdge->m_dwPoly = geom->m_dwId;
*/
		COleVariant var;
		var = rs.GetFieldValue(_T("ATTRIBUTE"));
		CString strAttribute  = V_BSTRT(&var);
		if(strAttribute.IsEmpty() == TRUE)
		{
			rs.MoveNext();
			continue;
		}

		layer->m_geomlist.RemoveAt(pos2);

		char cType = strAttribute[4]; 
		switch(cType)
		{
		case '1':
			{
				gaosu->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = gaosu->m_wId;
//				pEdge->m_designedSpeed = 120;
			}
			break;
		case '2':
			{
				guodao->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = guodao->m_wId;
//				pEdge->m_designedSpeed = 80;
			}
			break;
		case '3':
			{
				shengdao->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = shengdao->m_wId;
//				pEdge->m_designedSpeed = 60;
			}
			break;
		case '4':
			{
				chengshikuaisu->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = chengshikuaisu->m_wId;
//				pEdge->m_designedSpeed = 80;
			}
			break;
		case '5':
			{
				chengshizhugan->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = chengshizhugan->m_wId;
//				pEdge->m_designedSpeed = 60;
			}
			break;
		case '6':
			{
				chengshizhi->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = chengshizhi->m_wId;
//				pEdge->m_designedSpeed = 40;
			}
			break;
		case '7':
			{
				xianxiangdao->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = xianxiangdao->m_wId;
//				pEdge->m_designedSpeed = 40;
			}
			break;
		case '8':
			{
				linongxiang->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = linongxiang->m_wId;
//				pEdge->m_designedSpeed = 20;
			}
			break;
		case '9':
			{
				qita->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = qita->m_wId;
//				pEdge->m_designedSpeed = 20;
			}
			break;
		default:
			{
				qita->m_geomlist.AddTail(geom);
//				pEdge->m_wLayer = qita->m_wId;
//				pEdge->m_designedSpeed = 20;
			}
			break;
		}

		/*
		//道路等级
		{
			CString cClass = strAttribute.Mid(0,1); 
			int nClass = _ttoi (cClass);
			nClass -= 1;
			BYTE byte = nClass;
			byte &= ROAD_CLASS;
			pEdge->m_bAttribute3 |= byte;
		}
		*/
		//道路连通性
/*
		{
			CRoadNode* fNode = (CRoadNode*)pTopology->GetNode(pEdge->m_dwFromNode);
			CRoadNode* tNode = (CRoadNode*)pTopology->GetNode(pEdge->m_dwToNode);
			BYTE fIndex = fNode->GetEdgeIndex(pEdge->m_dwId);
			BYTE tIndex = tNode->GetEdgeIndex(pEdge->m_dwId);

			char cDirection = strAttribute[1]; 
			BYTE byte = 0X00;
			switch(cDirection)
			{
			case '1':
				{
					fNode->m_Forbid.SetValue(fIndex,fIndex,0);//应该在节点属性中设置过，这里重复
					tNode->m_Forbid.SetValue(tIndex,tIndex,0);//应该在节点属性中设置过，这里重复
				}
				break;
			case '2':
				{
					fNode->m_Forbid.SetValue(fIndex,fIndex,1);//应该在节点属性中设置过，这里重复
					tNode->m_Forbid.SetValue(tIndex,tIndex,1);//应该在节点属性中设置过，这里重复
				}
				break;
			case '3':
				{
					fNode->m_Forbid.SetValue(fIndex,fIndex,0);//应该在节点属性中设置过，这里重复
					tNode->m_Forbid.SetValue(tIndex,tIndex,1);//应该在节点属性中设置过，这里重复
				}
				break;
			case '4':
				{
					fNode->m_Forbid.SetValue(fIndex,fIndex,1);//应该在节点属性中设置过，这里重复
					tNode->m_Forbid.SetValue(tIndex,tIndex,0);//应该在节点属性中设置过，这里重复
				}
				break;
			case '5':
				{
					fNode->m_Forbid.SetValue(fIndex,fIndex,0);//应该在节点属性中设置过，这里重复
					tNode->m_Forbid.SetValue(tIndex,tIndex,0);//应该在节点属性中设置过，这里重复
				}
				break;
			case '6':
				{
					fNode->m_Forbid.SetValue(fIndex,fIndex,0);//应该在节点属性中设置过，这里重复
					tNode->m_Forbid.SetValue(tIndex,tIndex,0);//应该在节点属性中设置过，这里重复
				}
				break;
			}
		}
		/*
		//道路类型
		{
			CString strRoadType = strAttribute.Mid(2,2);
			pEdge->m_bRoadType = _ttoi (strRoadType);
		}
		//单向车道数量
		{
			CString cWay = strAttribute.Mid(5,1); 
			int nWay = _ttoi (cWay);
			BYTE byte = nWay;
			byte = byte<<4;
			byte &= ROAD_WAYS;
			pEdge->m_bAttribute3 |= byte;
		}
		//是否有红绿灯
		{
		}
		//是否有隔离带	
		{
			char cPatition = strAttribute[11]; 
			if(cPatition == '1')
				pEdge->m_bAttribute1 |= 0X00;
			else
				pEdge->m_bAttribute1 |= 0X40;
		}
		//道路形状
		{
			char cShape = strAttribute[13]; 
			BYTE bType = 0;
			switch(cShape)
			{
			case '1':
				pEdge->m_bAttribute2 |= 0X00;
				break;
			case '2':
			case '4':
				pEdge->m_bAttribute2 |= 0X01;
				break;
			case '3':
			case '5':
				pEdge->m_bAttribute2 |= 0X02;
				break;
			case '6':
				pEdge->m_bAttribute2 |= 0X03;
				break;
			}
		}
		//道路高度
		{
			char cHeight = strAttribute[14]; 
			switch(cHeight)
			{
			case '1':
				pEdge->m_bAttribute2 |= 0X00;
				break;
			case '2':
				pEdge->m_bAttribute2 |= 0X04;
				break;
			case '3':
				pEdge->m_bAttribute2 |= 0X08;
				break;
			}
		}
		//道路坡度
		{
			char cSlope = strAttribute[15]; 
			switch(cSlope)
			{
			case '1':
				pEdge->m_bAttribute2 |= 0X00;
				break;
			case '2':
				pEdge->m_bAttribute2 |= 0X10;
				break;
			case '3':
				pEdge->m_bAttribute2 |= 0X20;
				break;
			}
			
		}
		//单车道宽度
		{
			char cWidth = strAttribute[16]; 
			int nWidth = cWidth - '0';
			BYTE byte = nWidth;
			byte = byte<<6;
			byte &= ROAD_WIDTH;
			pEdge->m_bAttribute2 |= byte;
		}
		*/
		rs.MoveNext();
	}
	rs.Close();

/*	
	{	
		CDaoRecordset rs(pMDBDataBase);
		
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), _T("Node"));
		rs.Open(dbOpenDynaset, strSQL);		
		while(rs.IsEOF() == FALSE)
		{
			COleVariant varID = rs.GetFieldValue(_T("NODE_ID"));	
			DWORD dwId = V_I4(&varID);
			CRoadNode* pNode = (CRoadNode*)pTopology->GetNode(dwId);
			if(pNode == nullptr)
			{				
				rs.MoveNext();
				continue;
			}

			if(true)//只是针对中国图
			{
			//	rs.MoveNext();
			//	continue;
			}
			
			int countNode = 0;
			DWORD linkNode[8];
			for(int index1=1;index1<=8;index1++)
			{
				CString strNode;
				strNode.Format(_T("Node%d"),index1);
		
				COleVariant var = rs.GetFieldValue(strNode);					
				DWORD dwNode = V_I4(&var);
				if(dwNode == 0)
					break;

				if(index1>pNode->m_edgelist.GetCount())
				{
					AfxMessageBox(_T("节点的规则数量不应该大于实际连接的边数!"));
				}

				linkNode[index1-1] = dwNode;
				countNode++;
			}
			
			for(int index1=1;index1<=countNode;index1++)
			{
				CString strRule;
				strRule.Format(_T("Rule%d"),index1);

				COleVariant var = rs.GetFieldValue(strRule);
				CString rule = V_BSTRT(&var);
				if(rule.IsEmpty() == TRUE)
					break;

				for(int index2=1;index2<=countNode;index2++)
				{
					//这个地方有错误,因为Node存的是关联边,但linkNode存的是关联节点
					char c = rule[index2-1];
					if(c == '1')
					{		
						pNode->SetForbid(linkNode[index1-1],linkNode[index2-1],false);
					}
					else
					{		
						pNode->SetForbid(linkNode[index1-1],linkNode[index2-1],true);			
					}
				}
			}

			rs.MoveNext();
		}
		rs.Close();
	}
	
	pMDBDataBase->Execute(_T("Select Road_ID as ID,FNODE,TNODE,LENGTH,ATTRIBUTE,CODE,SPEED,LIMIT,TOLL,DOORPLATE,UPDATE Into Edge From Road"));
	pMDBDataBase->Execute(_T("ALTER TABLE Road DROP Road_ID,FNODE,TNODE,LENGTH,SPEED,LIMIT,TOLL,DOORPLATE"));     

	pMDBDataBase->Execute(_T("UPDATE Node set ID=Node_ID"));
	pMDBDataBase->Execute(_T("ALTER TABLE Node DROP Node_ID"));     
	
	CString strTopFile = lpszShapePath;
	strTopFile = strTopFile.Left(strTopFile.ReverseFind(_T('\\')));
	strTopFile += strTopFile.Mid(strTopFile.ReverseFind(_T('\\'))) + _T("0.Top");

	CFile file;
	if(file.Open(strTopFile, CFile::modeCreate | CFile::modeWrite) == TRUE)
	{		
		CArchive Ar(&file, CArchive::store);

		pTopology->Serialize(Ar);

		Ar.Close();
		file.Close();
	}

	if(pTopology != nullptr)
	{
		delete pTopology;
		pTopology = nullptr;
	}
	*/

}

void CImportMapInfoMif::DistinguishAdmArea(CLayer* layer,CDaoDatabase* pMDBDataBase,CLayerTree& layertree)
{
	CLayer* shengji = layertree.GetByName(_T("省级区划"));
	CLayer* diji = layertree.GetByName(_T("地级区划"));
	CLayer* xianji = layertree.GetByName(_T("县级区划"));
	CLayer* chengshi = layertree.GetByName(_T("城市区域"));
	
	shengji->m_attTable = _T("Adm_Area");
	diji->m_attTable = _T("Adm_Area");
	xianji->m_attTable = _T("Adm_Area");
	chengshi->m_attTable = _T("Adm_Area");
	
	CDaoRecordset rs(pMDBDataBase);
	
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM %s"), _T("Adm_Area"));
	rs.Open(dbOpenDynaset, strSQL);		

	POSITION pos1 = layer->m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* geom = layer->m_geomlist.GetNext(pos1);
		layer->m_geomlist.RemoveAt(pos2);

		COleVariant var = rs.GetFieldValue(_T("TYPE"));
		int oldType = V_I2(&var);
		if(oldType == 1)
		{
			chengshi->m_geomlist.AddTail(geom);
		}
		else if(oldType == 2)
		{
			shengji->m_geomlist.AddTail(geom);
		}
		else if(oldType == 3)
		{
			diji->m_geomlist.AddTail(geom);
		}
		else if(oldType == 4)
		{
			chengshi->m_geomlist.AddTail(geom);
		}
		else if(oldType == 5)
		{
			xianji->m_geomlist.AddTail(geom);
		}
		else
		{
			layer->m_geomlist.AddHead(geom);
		}

		
		rs.MoveNext();
	}
	rs.Close();
}

void CImportMapInfoMif::ReadPen(CStdioFile& file,CGeom* pGeom)
{

}

void CImportMapInfoMif::ReadBrush(CStdioFile& file,CGeom* pGeom)
{
	CString strRead;
	while(file.ReadString(strRead))
	{
		strRead.TrimLeft();
		CString strQ = strRead.SpanExcluding(_T(" "));
		if(!strQ.CompareNoCase(_T("Brush")))
		{
			strRead.TrimLeft(strQ);
			
			strRead.Replace(_T('\('),nullptr);
			strRead.Replace(_T('\)'),nullptr);
			strRead.TrimLeft();

			int type;
			DWORD value;
			_stscanf(strRead,_T("%d,%ld"),&type,&value);
			if(type == 2)
			{
				delete pGeom->m_pFill;
				
				CColorSpot* color = new CColorSpot();
				color->m_bR = (value>>16) & 0XFF;
				color->m_bG = (value>> 8) & 0XFF;
				color->m_bB = (value>> 0) & 0XFF;

				pGeom->m_pFill = new CFill();
				pGeom->m_pFill->m_pColor = color;
			}
			break;
		}
	}
}

void CImportMapInfoMif::ReadSpot(CStdioFile& file,CGeom* pGeom)
{

}

void CImportMapInfoMif::ReadType(CStdioFile& file,CGeom* pGeom)
{

}

BOOL CImportMapInfoMif::CreateTable(LPCTSTR lpszShapePath,CDaoDatabase* pDataBase)
{
	if(pDataBase == nullptr)
		return FALSE;

	CString strMid = lpszShapePath;
	strMid.MakeUpper();
	strMid.Replace(_T(".MIF"),_T(".mid"));
	if(_taccess(strMid,00) == -1)
		return FALSE;

	CString strTable = lpszShapePath;
	strTable = strTable.Right(strTable.GetLength() - strTable.ReverseFind(_T('\\')) - 1);
	strTable = strTable.Left(strTable.GetLength() - 4);

	CStdioFile file;
	if(file.Open(lpszShapePath, CFile::modeRead | CFile::shareDenyWrite) == false)
		return FALSE;
	

	CDaoTableDefInfo tableInfo;
	long nTableCount = pDataBase->GetTableDefCount();
	for(long I = 0; I < nTableCount; I++)
	{
		pDataBase->GetTableDefInfo(I, tableInfo);
		if(strTable.CompareNoCase(tableInfo.m_strName) == 0)
		{
			CString strSQL;
			strSQL.Format(_T("Delete From [%s]"),strTable);
			pDataBase->Execute(strSQL);
			return TRUE;
		}
	}
	
	CDaoTableDef td(pDataBase);	
	td.Create(strTable, dbAttachExclusive, _T(""), _T("DATABASE = "));
	
	CString rRead;
	while(file.ReadString(rRead))// 文件头读入开始
	{
		CString rQ = rRead.SpanExcluding(_T(" "));
		if(!rQ.CompareNoCase(_T("Version")))
		{
			rRead.TrimLeft(rQ);
			TRACE(rQ);
		}
		else if(!rQ.CompareNoCase(_T("Charset")))
		{
			rRead.TrimLeft(rQ);
			TRACE(rQ);
		}
		else if(!rQ.CompareNoCase(_T("Delimiter")))
		{
			rRead.TrimLeft(rQ);
			TRACE(rQ);
		}
		else if(!rQ.CompareNoCase(_T("Index")))
		{
			rRead.TrimLeft(rQ);
			TRACE(rQ);
		}
		else if(!rQ.CompareNoCase(_T("CoordSys")))
		{
			rRead.TrimLeft(rQ);
			TRACE(rQ);
		}
		else if(!rQ.CompareNoCase(_T("Columns")))
		{
			CDaoFieldInfo fieldInfo;		
			
			fieldInfo.m_strName = _T("ID");
			fieldInfo.m_nType = dbLong;
			fieldInfo.m_lSize = 4;
			fieldInfo.m_lAttributes = dbFixedField;  
			fieldInfo.m_bAllowZeroLength = FALSE;
			fieldInfo.m_nOrdinalPosition = 0;
			fieldInfo.m_bRequired        = FALSE;
			fieldInfo.m_lCollatingOrder  = 0;

			td.CreateField(fieldInfo);

			rRead.TrimLeft(rQ);
			long nColumns = _ttoi(rRead);
			for(long I = 0; I < nColumns; I ++)
			{
				file.ReadString(rRead);
				
				if(rRead.IsEmpty() == false)
				{
					rRead.TrimLeft();
					rQ = rRead.SpanExcluding(_T(" "));
					fieldInfo.m_strName = rQ;
					
					CString Name = rQ;
					
					rRead = rRead.Mid(rRead.Find(_T(" ")));
					rRead.TrimLeft();
					rQ = rRead.SpanExcluding(_T("\("));
					if(!rQ.CompareNoCase(_T("Char")))
					{
						rRead = rRead.Mid(rRead.Find(_T("\("))+1);
						rQ = rRead.SpanExcluding(_T("\)"));
						rRead = rRead.Mid(rRead.Find(_T("\)"))+1);
						long width = _ttoi(rQ);
												
						fieldInfo.m_nType = dbText;
						fieldInfo.m_lSize = width;
						fieldInfo.m_lAttributes = dbVariableField  ;
						fieldInfo.m_bAllowZeroLength = TRUE;					
					}

					if(!rQ.CompareNoCase(_T("Smallint")))
					{
						fieldInfo.m_nType = dbInteger;
						fieldInfo.m_lSize =  2;
						fieldInfo.m_lAttributes = dbFixedField; 
						fieldInfo.m_bAllowZeroLength = FALSE;
					}
					if(!rQ.CompareNoCase(_T("Integer")))
					{
						fieldInfo.m_nType = dbLong;
						fieldInfo.m_lSize =  4;
						fieldInfo.m_lAttributes = dbFixedField; 
						fieldInfo.m_bAllowZeroLength = FALSE;
					}
					if(!rQ.CompareNoCase(_T("Float")))
					{
						fieldInfo.m_nType = dbDouble;
						fieldInfo.m_lSize =  8;
						fieldInfo.m_bAllowZeroLength = FALSE;
					}
					if(!rQ.CompareNoCase(_T("Decimal")))
					{
						fieldInfo.m_nType = dbDouble;
						fieldInfo.m_lSize =  8;
						fieldInfo.m_bAllowZeroLength = FALSE;
					}
					if(!rQ.CompareNoCase(_T("Logical")))
					{
						fieldInfo.m_nType = dbBoolean;
						fieldInfo.m_lSize =  1;
						fieldInfo.m_bAllowZeroLength = FALSE;
					}
					if(!rQ.CompareNoCase(_T("Date")))
					{
						fieldInfo.m_nType = dbDate;
						fieldInfo.m_lSize =  8;
						fieldInfo.m_bAllowZeroLength = FALSE;	
					}
					fieldInfo.m_nOrdinalPosition = 0;
					fieldInfo.m_bRequired        = FALSE;
					fieldInfo.m_lCollatingOrder  = 0;

					td.CreateField(fieldInfo);
				}
			}

			if(strTable == _T("Node"))
			{//对于Node
				fieldInfo.m_lAttributes = dbFixedField;  
				fieldInfo.m_bAllowZeroLength = FALSE;
				fieldInfo.m_nOrdinalPosition = 0;
				fieldInfo.m_bRequired        = FALSE;
				fieldInfo.m_lCollatingOrder  = 0;

				fieldInfo.m_strName = _T("X");
				fieldInfo.m_nType = dbDouble;
				fieldInfo.m_lSize =  8;
				td.CreateField(fieldInfo);

				fieldInfo.m_strName = _T("Y");
				td.CreateField(fieldInfo);
			}
			
			td.Append();	
			td.Close();

			file.Close();
			return TRUE;
		}
		else if(!rQ.CompareNoCase(_T("Data")))
		{
			break;
		}	
	}

	file.Close();
	return FALSE;
}
