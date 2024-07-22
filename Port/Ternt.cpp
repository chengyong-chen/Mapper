#include "stdafx.h"

#include "../Style/LayerTree.h"
#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Ternt.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTernt::CTernt(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CCoverage(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath += _T("\\Ternt");
}

CTernt::~CTernt() 
{	
}

long CTernt::ReadPAT()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\pat.adf");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;

	long filelength = file.GetLength();
	long filepoint = 0;

	while(filepoint < filelength)	
	{
		PAT pat;
		pat.Code[3]   = 0X00;
		pat.GB[5]     = 0X00;
		pat.HYDC[6]   = 0X00;
		pat.TN[6]     = 0X00;
		pat.Name[20]  = 0X00;
		pat.MapTN[11] = 0X00;


		file.Seek(8,CFile::current);

		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserID = buff.valLong;
		if(true)//25万
		{
			file.Read(pat.Code, 3);
			file.Read(pat.GB,   5);
			 
			filepoint += 8;
		}
		else
		{
			file.Read(pat.Code, 5);

			file.Seek(1,CFile::current);
			 
			filepoint += 22;
		}

		pat_list.AddTail(pat);
	}

	file.Close();
	return 2;
}

long CTernt::ReadAAT()
{
	CString strFilePath = m_strCoveragePath + _T("\\Aat.adf");

		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;

	long filelength = file.GetLength();
	long filepoint = 0;

	while(filepoint < filelength)	
	{
		AAT aat;
		aat.Code[3]   = 0X00;
		aat.GB[5]     = 0X00;
		aat.HYDC[6]   = 0X00;
		aat.TN[6]     = 0X00;
		aat.Name[20]  = 0X00;
		aat.MapTN[11] = 0X00;
	
		file.Seek(20,CFile::current);

		revType buff;
		
		file.Read(buff.str,4);
		Reverse(buff);
		aat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		aat.UserID = buff.valLong;
		if(true)
		{		
			file.Read(aat.Code,  3);
			file.Read(aat.GB,    5);

			 
			filepoint +=36;
		}
		else
		{
			file.Read(aat.Code,  5);
			
			file.Seek(1,CFile::current);
			 
			filepoint +=30;
		}

		aat_list.AddTail(aat);
	}
	
	file.Close();
	return 2;
}

BOOL CTernt::Import(CLayerTree& layertree)
{
	CMapLayer layermap;
	
	CLayer* layer = CreateLayers(layertree,layermap,_T("Ternt"));
	if(layer == nullptr)
		return false;
/*	
	CLayer* delLab= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	DWORD Id = layertree.ApplyId();
	delLab.m_dwId       = Id;
	delLab.m_dwParentID = layer->m_dwId;
	delLab->m_bVisible   = false;
	layertree.AddTail(delLab);			
	delLab->m_strName=_T("delLab");

	ImportLab(library,layermap,delLab);
*/
	CLayer* delArc = layertree.GetByName(_T("delArc"));
	if(delArc == nullptr)
	{
		delArc= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delArc->m_wId = layertree.ApplyId();
		delArc->m_wParent = layer->m_wId;
		delArc->m_bVisible = false;
		delArc->m_strName = _T("delArc");
		layertree.AddTail(delArc);
	}
	ImportArc(layermap,delArc);

	CLayer* delPoly = layertree.GetByName(_T("delPoly"));
	if(delPoly == nullptr)
	{
		delPoly= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delPoly->m_wId     = layertree.ApplyId();
		delPoly->m_wParent = layer->m_wId;
		delPoly->m_bVisible = false;
		delPoly->m_strName = _T("delPoly");
		layertree.AddTail(delPoly);
	}
	ImportPolygon(layermap,delPoly);

	layermap.RemoveAll();
	return TRUE;
}