#include "stdafx.h"
#include "Global.h"

#include "../Style/LayerTree.h"
#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Respt.h"
#include "SelectDlg.h"
 
#include <String.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRespt::CRespt(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CCoverage(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath += _T("\\Respt");
}

CRespt::~CRespt() 	
{ 
}

long CRespt::ReadPAT()	// 读取多边形与弧段数据
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
		pat.Name[34]  = 0X00;
		pat.MapTN[11] = 0X00;

		file.Seek(8,CFile::current);	

		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		if(true)//25万
		{
			file.Read(pat.Code,  3);
			file.Read(pat.GB,    5);
			file.Read(pat.TN,    6);
			file.Read(pat.Name, 34);
			file.Read(pat.MapTN,11);

			file.Seek(1,CFile::current);
			 
			filepoint +=76;
		}
		else
		{
			file.Read(pat.GB,  5);
			file.Read(pat.Name,  50);

			file.Seek(50,CFile::current);
			file.Seek(4,CFile::current);
			file.Seek(1,CFile::current);

			file.Read(pat.Code,  2);

			filepoint += 128;
		}
	
		pat_list.AddTail(pat);
	}

	file.Close();
	return 2;
}

BOOL CRespt::Import(CLayerTree& layertree)
{
	CMapLayer layermap;

	CLayer* layer = CreateLayers(layertree,layermap,_T("Respt"));
	if(layer == nullptr)
		return false;

	CLayer* delLab = layertree.GetByName(_T("delLab"));
	if(delLab == nullptr)
	{
		delLab = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
		delLab->m_wId     = layertree.ApplyId();
		delLab->m_wParent = layer->m_wId;
		delLab->m_bVisible   = false;
		delLab->m_strName = _T("delLab");
		layertree.AddTail(delLab);			
	}
	ImportLab(library,layermap,delLab);
	
	layermap.RemoveAll();
	return TRUE;
}