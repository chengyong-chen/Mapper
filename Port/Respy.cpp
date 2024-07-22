#include "stdafx.h"

#include "../Style/LayerTree.h"
#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Respt.h"
#include "Respy.h"
#include "SelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRespy::CRespy(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CRespt(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath.Replace(_T("Respt"),_T("Respy"));
}

CRespy::~CRespy()
{
	patpopulist.RemoveAll();
}


long CRespy::ReadPATPOPU()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\Popu.Pat");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	
	while(filepoint < filelength)	
	{
		CPOPUPat pat;
		pat.RName[40] = 0X00;
		pat.PYName[60] = 0X00;
		pat.ADClass[2] = 0X00;
		pat.MapTN[11] = 0X00;

		file.Seek(8,CFile::current);	

		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		file.Read(pat.RName, 40);
		file.Read(pat.PYName,60);
		file.Read(pat.ADClass, 2);
		file.Read(pat.MapTN,11);

		file.Seek(1,CFile::current);
		 
		filepoint +=130;

		patpopulist.AddTail(pat);
	}

	file.Close();	
	return 2;
}

BOOL CRespy::Import(CLayerTree& layertree)
{
	CMapLayer layermap;

	CLayer* layer = CreateLayers(layertree,layermap,_T("Respy"));
	if(layer == nullptr)
		return false;

	CLayer* delLab = layertree.GetByName(_T("delLab"));
	if(delLab == nullptr)
	{
		delLab = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);
		delLab->m_wId = layertree.ApplyId();
		delLab->m_wParent = layer->m_wId;
		delLab->m_bVisible = false;
		delLab->m_strName = _T("delLab");
		layertree.AddTail(delLab);			
	}
//	ImportLab(layermap,delLab);

	CLayer* delPoly = layertree.GetByName(_T("delPoly"));
	if(delPoly == nullptr)
	{
		delPoly = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delPoly->m_wId     = layertree.ApplyId();
		delPoly->m_wParent = layer->m_wId;
		delPoly->m_bVisible   = false;
		delPoly->m_strName = _T("delPoly");	
		layertree.AddTail(delPoly);			
	}
	ImportPolygon(layermap,delPoly);

//	ImportPolygon(polylist,palpopu_list);
	return TRUE;
}

// only 100万
long CRespy::ReadPAT()	// 读取多边形与弧段数据
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
		pat.Code[5] = 0X00;

		file.Seek(8,CFile::current);	

		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		file.Read(pat.Code,  5);
		
		file.Seek(4,CFile::current);
		file.Seek(1,CFile::current);

		filepoint += 30;

		pat_list.AddTail(pat);
	}

	file.Close();
	return 2;
}