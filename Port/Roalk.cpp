#include "stdafx.h"

#include "../Style/LayerTree.h"
#include "../Geometry/Geom.h"

#include "Coverage.h"
#include "Roalk.h"
#include "SelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRoalk::CRoalk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	   :CCoverage(datainfo,lpszCoveragePath)
{ 
	m_strCoveragePath += _T("\\Roaln");
}
	
CRoalk::~CRoalk() 
{
	POSITION Pos3 = ratnatmap.GetStartPosition();
	while(Pos3 != nullptr)
	{
		WORD rkey;
		CRoalk::RAT* rat;

		ratnatmap.GetNextAssoc(Pos3, rkey, rat);
		delete rat;
	}
	ratnatmap.RemoveAll();

	POSITION Pos4 = rathigmap.GetStartPosition();
	while(Pos4 != nullptr)
	{
		WORD rkey;
		CRoalk::RAT* rat;

		rathigmap.GetNextAssoc(Pos4, rkey, rat);
		delete rat;
	}
	rathigmap.RemoveAll();
}

long CRoalk::ReadPAT()	// ��ȡ������뻡������
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
		pat.Code[3] = 0X00;
		pat.GB[5] = 0X00;
		pat.TN[6] = 0X00;
		pat.Name[34] = 0X00;
		pat.MapTN[11] = 0X00;


		file.Seek(8,CFile::current);	
		filepoint += 8;
		
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		pat.Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pat.UserId = buff.valLong;

		file.Read(pat.Code,3);
		file.Read(pat.GB,    5);
		file.Read(pat.TN,    6);
		file.Read(pat.Name, 34);
		file.Read(pat.MapTN,11);

		file.Seek(9,CFile::current);
		 
		filepoint +=62;

		pat_list.AddTail(pat);
	}

	file.Close();	
	return 2;
}

long CRoalk::ReadAAT()
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
		aat.GB[5] = 0X00;
		aat.RN[6] = 0X00;
		aat.RN2[6] = 0X00;

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
			file.Read(aat.RN,  6);
			file.Read(aat.RN2, 6);
			 
			filepoint +=48;
		}
		else
		{	
			file.Read(aat.Code,5);
			file.Read(aat.Name,4);
			 
			file.Seek(1,CFile::current);

			filepoint +=38;
		}

		aat_list.AddTail(aat);
	}

	file.Close();	
	return 2;
}

long CRoalk::ReadRAT(CRoalk::CMapRat& ratmap, LPCTSTR lpszFile)	// ��ȡ������뻡������
{
	CString strFilePath = m_strCoveragePath + _T("\\")+lpszFile+_T(".rat");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	
	while(filepoint < filelength)	
	{
		CRoalk::RAT* rat = new CRoalk::RAT;
		rat->Code[4] = 0X00;
		rat->Name[20] = 0X00;
		rat->FName[20] = 0X00;
		rat->TName[20] = 0X00;

		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		rat->Id = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		rat->UserId = buff.valLong;
		
		file.Read(rat->Code,4);
		file.Read(rat->Name, 20);
		file.Read(rat->FName, 20);
		file.Read(rat->TName, 20);
		
		filepoint +=72;

		RAT* del;
		if(ratmap.Lookup(rat->Id,del) == TRUE)
			delete del;
		
		ratmap.SetAt(rat->Id,rat);
	}

	file.Close();	
	return 2;
}

BOOL CRoalk::Import(CLayerTree& layertree)
{
	CMapLayer layermap;

	CLayer* layer = CreateLayers(layertree,layermap,_T("Roaln"));
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
/*
	CLayer* NatWay = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	DWORD UserId = layertree.ApplyId();
	NatWay->m_dwId       = UserId;
	NatWay->m_dwParentId = layer->m_dwId;
	NatWay->m_bVisible   = false;
	layertree.AddTail(NatWay);			
	NatWay->m_strName=_T("NatWay");	

	CLayer* delNat = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	UserId = layertree.ApplyId();
	delNat->m_dwId       = UserId;
	delNat->m_dwParentId = NatWay->m_dwId;
	delNat->m_bVisible   = false;
	layertree.AddTail(delNat);			
	delNat->m_strName=_T("delNat");	

	CGeomList arclist;
	ImportSec(arclist,secnat_list);

	POSITION pos = arclist.GetHeadPosition();
	while(pos != nullptr)
	{
		CPoly* poly = (CPoly*)arclist.GetNext(pos);
	
		RAT* rat = nullptr;
		ratnatmap.Lookup(poly->m_dwId,rat);
		if(rat == nullptr)
		{
			delNat->m_geomlist.AddTail(poly);					
			continue;
		}		

		NatWay->m_geomlist.AddTail(poly);				
		
		LAB* lab;
		labmap.Lookup(poly->m_dwId, lab);
		if(lab!=nullptr)
		{
			CPoint point = Change(lab->points[0]);
			CTag*pTag = new CTag(point,rat->Name);			
		}
	}
	arclist.RemoveAll();

	CLayer* HigWay = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	UserId = layertree.ApplyId();
	HigWay->m_dwId       = UserId;
	HigWay->m_dwParentId = layer->m_dwId;
	HigWay->m_bVisible   = false;
	layertree.AddTail(HigWay);			
	HigWay->m_strName=_T("HigWay");	

	CLayer* delHig = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
	UserId = layertree.ApplyId();
	delHig->m_dwId       = UserId;
	delHig->m_dwParentId = HigWay->m_dwId;
	delHig->m_bVisible   = false;
	layertree.AddTail(delHig);			
	delHig->m_strName=_T("delHig");	


	ImportSec(arclist,sechig_list);

	pos = arclist.GetHeadPosition();
	while(pos != nullptr)
	{
		CPoly* poly = (CPoly*)arclist.GetNext(pos);
	
		RAT* rat = nullptr;
		rathigmap.Lookup(poly->m_dwId,rat);
		if(rat == nullptr)
		{
			delHig->m_geomlist.AddTail(poly);					
			continue;
		}		
		
		HigWay->m_geomlist.AddTail(poly);					

		LAB* lab;
		labmap.Lookup(poly->m_dwId, lab);
		if(lab!=nullptr)
		{
			CPoint point = Change(lab->points[0]);
			CTag* tip new CTag(point,rat->Name);		
		}
	}
	arclist.RemoveAll();
*/

	layermap.RemoveAll();
	return TRUE;
}