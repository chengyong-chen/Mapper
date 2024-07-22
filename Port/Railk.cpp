#include "stdafx.h"

#include "..\Layer\LayerTree.h"
#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Roalk.h"
#include "Railk.h"

#include "SelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRailk::CRailk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CRoalk(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath = lpszCoveragePath;
	m_strCoveragePath += _T("\\Railn");
}

	
CRailk::~CRailk() 
{	
	POSITION Pos3 = ratraimap.GetStartPosition();
	while(Pos3 != nullptr)
	{
		WORD rkey;
		CRailk::RAT* rat;

		ratraimap.GetNextAssoc(Pos3, rkey, rat);
		delete rat;
	}
	ratraimap.RemoveAll();
}

long CRailk::ReadPAT()	// 读取多边形与弧段数据
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
		pat.Code[3] = 0X00;
		pat.GB[5]   = 0X00;
		pat.RN[6]   = 0X00;
			
		file.Seek(8,CFile::current);	


		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;


		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;
		
		file.Read(pat.Code,3);
		file.Read(pat.GB,  5);
		file.Read(pat.RN,  6);

		file.Seek(8,CFile::current);
		 
		filepoint +=30;

		pat_list.AddTail(pat);
	}

	file.Close();	
	return 2;
}

long CRailk::ReadAAT()	// 读取多边形与弧段数据
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
		aat.Code[3] = 0X00;
		aat.GB[5]   = 0X00;
		aat.RN[6]   = 0X00;

		file.Seek(20,CFile::current);		
		
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		aat.Id = buff.valLong;
		
		file.Read(buff.str,4);
		Reverse(buff);
		aat.UserId = buff.valLong;
	
		if(true)//25万
		{
			file.Read(aat.Code,3);
			file.Read(aat.GB,  5);
			file.Read(aat.RN,  6);
			 
			filepoint +=42;
		}
		else
		{
			file.Read(aat.Code,5);
			file.Read(aat.Name,  3);
			 
			filepoint += 36;
		}

		aat_list.AddTail(aat);
	}

	file.Close();	
	return 2;
}

BOOL CRailk::Import(CLayerTree& layertree)
{
	CMapLayer layermap;

	CLayer* layer = CreateLayers(layertree,layermap,_T("Railn"));
	if(layer == nullptr)
		return false;

	CLayer* delArc = layertree.GetByName(_T("delArc"));
	if(delArc == nullptr)
	{
		delArc = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delArc->m_wId     = layertree.ApplyId();
		delArc->m_wParent = layer->m_wId;
		delArc->m_bVisible = false;
		delArc->m_strName = _T("delArc");
		layertree.AddTail(delArc);			
	}
	ImportArc(layermap,delArc);

	layermap.RemoveAll();
	return TRUE;
}

