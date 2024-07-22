#include "stdafx.h"

#include "..\Layer\Layer.h"
#include "..\Layer\LayerTree.h"

#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Bount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBount::CBount(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CCoverage(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath += _T("\\Bount");
}

CBount::~CBount() 
{	
	patcntylist.RemoveAll();
	patdistlist.RemoveAll();
	patprovlist.RemoveAll();
}

long CBount::ReadPAT()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\Pat.adf");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	
	while(filepoint < filelength)	
	{
		CBountPat pat;
		pat.Pac[6] = 0X00;
		
		file.Seek(8,CFile::current);	

		revType buff;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;
		
		file.Read(pat.Pac,6);
		 
		filepoint +=22;

		pat_list.AddTail(pat);
	}

	file.Close();
	return 2;
}

long CBount::ReadAAT()	// 读取多边形与弧段数据
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

		file.Seek(20,CFile::current);
		
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		aat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		aat.UserId = buff.valLong;
	
		if(true)
		{
			file.Read(aat.Code,3);
			file.Read(aat.GB,  5);
			
			filepoint +=36;
		}
		else
		{
			file.Read(aat.Code,5);
			file.Seek(1,CFile::current);

			filepoint +=34;
		}
		aat_list.AddTail(aat);
	}

	file.Close();	
	return 2;
}

long CBount::ReadPATCNTY()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\Cnty.Pat");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	
	while(filepoint < filelength)	
	{
		PAT pat;
		pat.Code[6]   = 0X00;
		pat.Name[32]  = 0X00;
		
		file.Seek(8,CFile::current);	
	
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		file.Read(pat.Code, 6);
		file.Read(pat.Name,32);

		file.Seek(56,CFile::current);	
		 
		filepoint +=110;

		patcntylist.AddTail(pat);
	}

	file.Close();

	return 2;
}

long CBount::ReadPATDIST()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\Dist.Pat");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	
	while(filepoint < filelength)	
	{
		PAT pat;
		pat.Code[6]   = 0X00;
		pat.Name[22]  = 0X00;
		pat.CapName[20] = 0X00;

		file.Seek(8,CFile::current);	
		
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		file.Read(pat.Code, 6);
		file.Read(pat.Name,22);
		file.Read(pat.CapName,20);
		
		file.Seek(8,CFile::current);
		 
		filepoint +=64;

		
		patdistlist.AddTail(pat);
	}

	file.Close();	
	return 2;
}

long CBount::ReadPATPROV()	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\Prov.Pat");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	
	while(filepoint < filelength)	
	{
		PAT pat;
		pat.Code[6]   = 0X00;
		pat.Name[32]  = 0X00;
		
		file.Seek(8,CFile::current);	
	
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;
	
		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		file.Read(pat.Code,6);
		file.Read(pat.Name,32);
		
		file.Seek(50,CFile::current);
		file.Seek(32,CFile::current);
		file.Seek(50,CFile::current);
		
		 
		filepoint +=186;

		patprovlist.AddTail(pat);
	}

	file.Close();	
	return 2;
}

BOOL CBount::Import(CLayerTree& layertree)
{
	CMapLayer layermap;

	CLayer* layer = CreateLayers(layertree,layermap,_T("Bount"));
	if(layer == nullptr)
		return false;

	CLayer* delArc = layertree.GetByName(_T("delArc"));
	if(delArc == nullptr)
	{
		delArc = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delArc->m_wId     = layertree.ApplyId();
		delArc->m_wParent = layer->m_wId;
		delArc->m_bVisible   = false;
		delArc->m_strName = _T("delArc");
		layertree.AddTail(delArc);			
	}
	ImportArc(layermap,delArc);

	CLayer* delPoly = layertree.GetByName(_T("delPoly"));
	if(delPoly == nullptr)
	{	
		delPoly = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delPoly->m_wId     = layertree.ApplyId();
		delPoly->m_wParent = layer->m_wId;
		delPoly->m_bVisible   = false;
		layertree.AddTail(delPoly);			
		delPoly->m_strName=_T("delPoly");
	}
	ImportPolygon(layermap,delPoly);
/*
	CLayer* sunlayer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	Id = layertree.ApplyId();
	sunlayer->m_dwId = UserId;
	sunlayer->m_dwParentId = layer->m_dwId;
	layertree.AddTail(sunlayer);			
	sunlayer->m_strName=_T("xzqh");

	CLayer* delXzq = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	Id = layertree.ApplyId();
	delXzq->m_dwId       = UserId;
	delXzq->m_dwParentId = sunlayer->m_dwId;
	delXzq->m_bVisible   = false;
	layertree.AddTail(delXzq);			
	delXzq->m_strName=_T("delXzqh");

	ImportPolygon(palxzqh_list,patxzqhlist,sunlayer,delXzq);
*/
	
	CLayer* cntylayer = layertree.GetByName(_T("县级区域"));
	if(cntylayer == nullptr)
	{
		cntylayer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);
		cntylayer->m_wId = layertree.ApplyId();
		cntylayer->m_wParent = layer->m_wId;
		cntylayer->m_strName = _T("县级区域");
		layertree.AddTail(cntylayer);			
	}

	CLayer* delCnty = layertree.GetByName(_T("delCnty"));
	if(delCnty == nullptr)
	{
		delCnty = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delCnty->m_wId     = layertree.ApplyId();
		delCnty->m_wParent = cntylayer->m_wId;
		delCnty->m_bVisible = false;
		layertree.AddTail(delCnty);			
		delCnty->m_strName=_T("delCnty");
	}
	ImportPolygon(palcnty_list,patcntylist,cntylayer,delCnty);
	/*
	sunlayer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	Id = layertree.ApplyId();
	sunlayer->m_dwId = Id;
	sunlayer->m_dwParentId = layer->m_dwId;
	layertree.AddTail(sunlayer);			
	sunlayer->m_strName=_T("Dist");
		
	CLayer* delDist = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	Id = layertree.ApplyId();
	delDist->m_dwId       = Id;
	delDist->m_dwParentId = sunlayer->m_dwId;
	delDist->m_bVisible   = false;
	layertree.AddTail(delDist);			
	delDist->m_strName=_T("delDist");

	ImportPolygon(paldist_list,patdistlist,sunlayer,delCnty);
	*/
	  
	CLayer* provlayer = layertree.GetByName(_T("省级区域"));
	if(provlayer == nullptr)
	{
		provlayer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		provlayer->m_wId = layertree.ApplyId();
		provlayer->m_wParent = layer->m_wId;
		layertree.AddTail(provlayer);			
		provlayer->m_strName=_T("省级区域");
	}
	CLayer* delProv = layertree.GetByName(_T("delProv"));
	if(delProv == nullptr)
	{
		delProv = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delProv->m_wId     = layertree.ApplyId();
		delProv->m_wParent = provlayer->m_wId;
		delProv->m_bVisible = false;
		layertree.AddTail(delProv);			
		delProv->m_strName=_T("delProv");
	}
	ImportPolygon(palprov_list,patprovlist,provlayer,delProv);

	layermap.RemoveAll();
	return TRUE;
}

