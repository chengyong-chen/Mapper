#include "stdafx.h"

#include "..\Layer\LayerTree.h"
#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Hydlk.h"
#include "SelectDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHydlk::CHydlk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CCoverage(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath += _T("\\Hydlk");
}

CHydlk::~CHydlk() 
{	
}

long CHydlk::ReadPAT()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\Pat.adf");
		
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
		pat.UserId = buff.valLong;
		
		file.Read(pat.Code,  3);
		file.Read(pat.GB,    5);
		file.Read(pat.TN,    6);
		file.Read(pat.Name, 20);
		file.Read(pat.MapTN,11);

		file.Seek(1,CFile::current);
		 
		filepoint +=62;

		pat_list.AddTail(pat);
	}

	file.Close();	
	return 2;
}

long CHydlk::ReadAAT()	// 读取多边形与弧段数据
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
		aat.TN[6]     = 0X00;
		aat.Name[20]  = 0X00;
		aat.MapTN[11] = 0X00;

		file.Seek(24,CFile::current);	
		
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		aat.UserId = buff.valLong;
		
		file.Read(aat.Code,  3);
		file.Read(aat.GB,    5);
		file.Read(aat.TN,    6);
		file.Read(aat.Name, 20);
		file.Read(aat.MapTN,11);
		 
		file.Seek(1,CFile::current);	
		filepoint +=74;

		aat_list.AddTail(aat);
	}

	file.Close();	
	return 2;
}

BOOL CHydlk::Import(CLayerTree& layertree)
{
	CMapLayer layermap;

	CLayer* layer = CreateLayers(layertree,layermap,_T("Hydlk"));
	if(layer == nullptr)
		return false;

	CLayer* delLab = layertree.GetByName(_T("delLab"));
	if(delLab == nullptr)
	{	
		delLab= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);
		delLab->m_wId = layertree.ApplyId();
		delLab->m_wParent = layer->m_wId;
		delLab->m_bVisible = false;
		layertree.AddTail(delLab);			
		delLab->m_strName = _T("delLab");
	}
	ImportLab(library,layermap,delLab);

	CLayer* delArc = layertree.GetByName(_T("delArc"));
	if(delArc == nullptr)
	{
		delArc= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delArc->m_wId     = layertree.ApplyId();
		delArc->m_wParent = layer->m_wId;
		delArc->m_bVisible = false;
		layertree.AddTail(delArc);			
		delArc->m_strName = _T("delArc");
	}
	ImportArc(layermap,delArc);

	CLayer* delPoly = layertree.GetByName(_T("delPoly"));
	if(delPoly == nullptr)
	{
		delPoly= new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delPoly->m_wId     = layertree.ApplyId();
		delPoly->m_wParent = layer->m_wId;
		delPoly->m_bVisible = false;
		layertree.AddTail(delPoly);			
		delPoly->m_strName = _T("delPoly");
	}
	ImportPolygon(layermap,delPoly);
	
	layermap.RemoveAll();
	
	return TRUE;
}

