#include "stdafx.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"

#include "..\Layer\Layer.h"
#include "..\Layer\LayerTree.h"

#include "Coverage.h"

#include "SelectDlg.h"
#include "Hydnt.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_SERIAL(CCoverage, CObject, 0)

double CCoverage::m_xOffset = 0.0f;
double CCoverage::m_yOffset = 0.0f;
double CCoverage::m_fScale = 1.0f;

long ggg = 0;

CCoverage::CCoverage(const CDatainfo& datainfo)
	:m_datainfo(datainfo)
{
}

CCoverage::CCoverage(CDatainfo& datainfo,LPCTSTR lpszCoveragePath) 
	:m_datainfo(datainfo)
{ 
	m_strCoveragePath = lpszCoveragePath;

	CString strCoverageName;
	strCoverageName = m_strCoveragePath.Left(m_strCoveragePath.ReverseFind('\\'));
	strCoverageName = strCoverageName.Right(strCoverageName.GetLength() - strCoverageName.ReverseFind('\\') - 1);	
}

CCoverage::~CCoverage() 
{ 
	pat_list.RemoveAll();
	aat_list.RemoveAll();
	pal_list.RemoveAll();

	POSITION Pos3 = arcmap.GetStartPosition();
	while(Pos3 != nullptr)
	{
		WORD rkey;
		CCoverage::ARC* arc = nullptr;

		arcmap.GetNextAssoc(Pos3, rkey, arc);
		delete arc;
		arc = nullptr;
	}
	arcmap.RemoveAll();

	POSITION Pos4 = labmap.GetStartPosition();
	while(Pos4 != nullptr)
	{
		WORD rkey;
		CCoverage::LAB* lab = nullptr;
		labmap.GetNextAssoc(Pos4, rkey, lab);
		delete lab;
		lab = nullptr;
	}
	labmap.RemoveAll();
}

long CCoverage::ReadTic(CPointF* pTic)
{
	CString strFilePath = m_strCoveragePath + _T("Tic.adf");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
		
	long   FID[4];
	CPointF fPoint[4];
	revType buff;
	file.Read(buff.str,4);
	Reverse(buff);
	FID[0] = buff.valLong;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[0].x = buff.valFloat;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[0].y = buff.valFloat;

	file.Read(buff.str,4);
	Reverse(buff);
	FID[1] = buff.valLong;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[1].x = buff.valFloat;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[1].y = buff.valFloat;

	file.Read(buff.str,4);
	Reverse(buff);
	FID[2] = buff.valLong;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[2].x = buff.valFloat;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[2].y = buff.valFloat;

	file.Read(buff.str,4);
	Reverse(buff);
	FID[3] = buff.valLong;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[3].x = buff.valFloat;
	file.Read(buff.str,4);
	Reverse(buff);
	fPoint[3].y = buff.valFloat;

	file.Close();
	return 4;
}

long CCoverage::ReadBnd(CPointF* pBnd)
{
	CString strFilePath = m_strCoveragePath + _T("Bnd.adf");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
		
	float MaxX;
	float MinX;
	float MaxY;
	float MinY;
	revType buff;
	
	file.Read(buff.str,4);
	Reverse(buff);
	MinX = buff.valFloat;
	file.Read(buff.str,4);
	Reverse(buff);
	MinY = buff.valFloat;

	file.Read(buff.str,4);
	Reverse(buff);
	MaxX = buff.valFloat;
	file.Read(buff.str,4);
	Reverse(buff);
	MaxY = buff.valFloat;

	file.Close();
	return 2;
}

long CCoverage::ReadLAB()	// 读取标识点数据
{
	CString strFilePath = m_strCoveragePath + _T("\\lab.adf");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;

	long filelength = file.GetLength();
	long filepoint = 0;
	file.Seek(100,CFile::current);
	filepoint +=100;
	LONG id = 1;
	while(filepoint < filelength)
	{
		CCoverage::LAB* lab = new LAB;

		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		lab->Id = buff.valLong;
		lab->Id = id++;
		
		file.Read(buff.str,4);
		Reverse(buff);
		lab->polyId = buff.valLong;
		
		file.Read(buff.str,4);
		Reverse(buff);
		lab->points[0].x = buff.valFloat;
		file.Read(buff.str,4);
		Reverse(buff);
		lab->points[0].y = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		lab->points[1].x = buff.valFloat;
		file.Read(buff.str,4);
		Reverse(buff);
		lab->points[1].y = buff.valFloat;
	
		file.Read(buff.str,4);
		Reverse(buff);
		lab->points[2].x = buff.valFloat;
		file.Read(buff.str,4);
		Reverse(buff);
		lab->points[2].y = buff.valFloat;
		
		filepoint +=32;

		LAB* del;
		if(labmap.Lookup(lab->Id,del) == TRUE)
		{
			delete lab;
			lab = nullptr;
		}
		else
		{
			labmap.SetAt(lab->Id,lab);
		}
	}

	file.Close();
	return 2;
}


long CCoverage::ReadARC()
{
	CString strFilePath = m_strCoveragePath + _T("\\Arc.adf");

	CFile file;
	if(file.Open(strFilePath, CFile::modeRead | CFile::shareDenyWrite)==FALSE)
		return 0;
	arcmap.RemoveAll();

	long nArcCount = 0;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	file.Seek(100L, CFile::begin);
	filepoint +=100;
	while(filepoint < filelength)
	{
		ARC* arc = new ARC;;
		
		revType buff;
		file.Read(buff.str,4);
		Reverse(buff);
		arc->Id = buff.valLong;
	
		file.Read(buff.str,4);
		Reverse(buff);
		arc->lLength = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arc->ID_ = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arc->lFNode = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arc->lTNode = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arc->lLPoly = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arc->lRPoly = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arc->lPosCount = buff.valLong;

		if(arc->lPosCount)
		{
	    	arc->points = new CPointF[arc->lPosCount];
			for(long i=0; i<arc->lPosCount;i++)
			{
				file.Read(buff.str,4);
				Reverse(buff);
				arc->points[i].x = buff.valFloat;

				file.Read(buff.str,4);
				Reverse(buff);
				arc->points[i].y = buff.valFloat;
			}

		}

		filepoint += 8*sizeof(DWORD) + arc->lPosCount*8;

		ARC* del;
		if(arcmap.Lookup(arc->Id,del) == TRUE)
			delete del;
		

		arcmap.SetAt(arc->Id,arc);
		nArcCount ++;
	}

	file.Close();
	return nArcCount;
}

long CCoverage::ReadArx(CCoverage::ARX_LIST& arx_list)
{
	CString strFilePath = m_strCoveragePath + _T("Arx.adf");

	CFile file;
	if(file.Open(strFilePath, CFile::modeRead | CFile::shareDenyWrite) == FALSE)
		return 0;
	
	arx_list.RemoveAll();

	DWORD dwLength;	// 数据长度 (LONG 单位)
	file.Seek(2L, CFile::begin);
	file.Read(&dwLength, sizeof(DWORD));
	dwLength = (dwLength - 50L)/2L;
//	file.Seek(356L, CFile::begin);

	long nArxCount = 0;
	
	long filelength = file.GetLength();
	long filepoint = 0;
	file.Seek(100L, CFile::begin);
	filepoint +=100;
	while(filepoint < filelength)
	{
		ARX arx;
		revType buff;
		
		file.Read(buff.str,4);
		Reverse(buff);
		arx.Id = buff.valLong;
	
		file.Read(buff.str,4);
		Reverse(buff);
		arx.lLength = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arx.ID_ = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		arx.lFNode = buff.valLong;

		filepoint += 16;

		arx_list.AddTail(arx);
		nArxCount ++;
	}

	file.Close();	
	return nArxCount;
}


// 读取注记数据
// FAIL:	return -1
//	TRUE:	return 注记数
long CCoverage::ReadTxt(CCoverage::TXT_LIST& txt_list)
{
	CString strFilePath = m_strCoveragePath + _T("Txt.adf");

	CFile file;
	file.Open(strFilePath, CFile::modeRead | CFile::shareDenyWrite);
	if(file.m_hFile == CFile::hFileNull)
	{
		AfxMessageBox(_T("在指定路径找不到文件, 请确认文件路径是否正确!"));
		return -1;
	}
	txt_list.RemoveAll();

	DWORD dwLength;	// 数据长度 (LONG 单位)
	file.Seek(2L, CFile::begin);
	file.Read(&dwLength, sizeof(DWORD));
	dwLength = (dwLength - 50L)/2L;
	file.Seek(356L, CFile::begin);

	long nTxtCount = 0;
	while(dwLength)
	{
		DWORD nAlloc;
		TXT	txt;
		file.Read(&txt, sizeof(LONG)*7 + sizeof(CPointF)*8);
		if(txt.lCharCount)
		{
			nAlloc = txt.lCharCount;
			if(nAlloc%4 != 0)
				nAlloc = (nAlloc/4 + 1)*4L;
			txt.pzAnno = new TCHAR[nAlloc + 1];
			file.Read(txt.pzAnno, nAlloc);
			txt.pzAnno[txt.lCharCount] = '\0';
		}
		dwLength -= 7 + 2*8 + nAlloc/4L;

		txt_list.AddTail(txt);
		nTxtCount ++;
	}

	file.Close();
	return nTxtCount;
}

long CCoverage::ReadPAL(CCoverage::PAL_LIST& pal_list,LPCTSTR lpszFile)
{
	CString strFilePath;

	if(lpszFile == nullptr)
		strFilePath = m_strCoveragePath + _T("\\") + _T("Pal.Adf");
	else
		strFilePath = m_strCoveragePath + _T("\\") + lpszFile+ _T(".Pal");
	
	CFile file;
	if(file.Open(strFilePath, CFile::modeRead | CFile::shareDenyWrite) == FALSE)
		return 0;
		
	pal_list.RemoveAll();

	long filelength = file.GetLength();
	long filepoint  = 0;
	file.Seek(100L, CFile::begin);
	filepoint +=100;
	long nPalCount = 0;
	while(filepoint < filelength)
	{
		PAL pal;
		revType buff;
		
		file.Read(buff.str,4);
		Reverse(buff);
		pal.polyId = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pal.lReserved = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		pal.point[0].x = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		pal.point[0].y = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		pal.point[1].x = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		pal.point[1].y = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		pal.lRelationArcCount = buff.valLong;
		
		if(pal.lRelationArcCount<0)
			continue;
	
		pal.relationship = new PAL::ARC_POLY[pal.lRelationArcCount];
		for(long i=0;i<pal.lRelationArcCount;i++)
		{
			file.Read(buff.str,4);
			Reverse(buff);
			pal.relationship[i].Id = buff.valLong;
				
			file.Read(buff.str,4);
			Reverse(buff);
			pal.relationship[i].lNodeId = buff.valLong;

			file.Read(buff.str,4);
			Reverse(buff);
			pal.relationship[i].polyId = buff.valLong;
		}
			
		pal_list.AddTail(pal);
		nPalCount ++;

		filepoint += 7*4 + (pal.lRelationArcCount*sizeof(PAL::ARC_POLY));
	}

	file.Close();
	return nPalCount;
}

long CCoverage::ReadSEC(CCoverage::SEC_LIST& sec_list,LPCTSTR lpszFile)	// 读取多边形与弧段数据
{
	CString strFilePath = m_strCoveragePath + _T("\\") + lpszFile + _T(".sec");
		
	CFile file;
	if(file.Open(strFilePath,CFile::modeRead | CFile::shareDenyWrite) == FALSE)	
		return -1;
	CCoverage::SEC sec;
	
	long filelength = file.GetLength();
	long filepoint = 0;	
	while(filepoint < filelength)	
	{
		revType buff;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.RouteLink = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.ArcLink = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.F_Meas = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.T_Meas = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.F_Pos = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.T_Pos = buff.valFloat;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.Sub = buff.valLong;

		file.Read(buff.str,4);
		Reverse(buff);
		sec.SubId = buff.valLong;
				
		filepoint +=32;
		
		if(sec.RouteLink!=2)
			continue;
		sec_list.AddTail(sec);
	}

	file.Close();	
	return 2;
}

void CCoverage::Reverse(revType& rev)
{
	char c;
	c = rev.str[0];
	rev.str[0] = rev.str[3];
	rev.str[3] = c;
	c = rev.str[1];
	rev.str[1] = rev.str[2];
	rev.str[2] = c;
}

BOOL CCoverage::ImportArc(CMapLayer& layermap,CLayer* delArc)
{
	POSITION pos = arcmap.GetStartPosition();
	while(pos != nullptr)
	{
		WORD dwId;
		CCoverage::ARC* arc = nullptr;
		arcmap.GetNextAssoc(pos,dwId,arc);
		if(arc == nullptr)
			continue;
		POSITION pos = aat_list.FindIndex(arc->Id-1);
		if(pos == nullptr)
			continue;
		AAT& aat = aat_list.GetAt(pos);
	
		CLayer* sunlayer = nullptr;
		if(layermap.Lookup(aat.Code,sunlayer) == false)
			continue;

		CPoly* poly = new CPoly();
		poly->m_geoId = arc->Id;
		poly->m_strName = aat.Name;
		poly->m_nAnchors = arc->lPosCount;
		poly->m_nAllocs = arc->lPosCount+1;
		poly->m_pPoints = new CPoint[arc->lPosCount+1];
		for(long i=0;i<arc->lPosCount;i++)
		{
			CPoint point = Change(arc->points[i]);						
			poly->m_pPoints[i] = point;
		}	
		poly->RecalRect();

		CString str;
	//	if(pat.IsKindOf(RUNTIME_CLASS(CHydntAat)))
		{
//			str =  pat.HYDC;
//			str.TrimLeft();
//			str.TrimRight();
//			if(str.IsEmpty())
//			{
//				delArc->m_geomlist.AddTail(poly);					
//				continue;
//			}
		
//			str = str.Right(1);
//			long level = _ttoi (str);
//			if(level==0)
//				continue;
		}
		sunlayer->m_geomlist.AddTail(poly);					
	}

	return TRUE;
}
	
BOOL CCoverage::ImportSec(CGeomList& polylist,CCoverage::SEC_LIST& sec_list)
{
	DWORD oldSec  = 0;
	float oldMeas= 0;
	CPoly* poly  = nullptr;
	POSITION pos = sec_list.GetHeadPosition();
	while(pos != nullptr)
	{
		CCoverage::SEC& sec	= sec_list.GetNext(pos);
		if(sec.RouteLink != oldSec)
		{
			if(poly!=nullptr)
				polylist.AddTail(poly);
			poly = new CPoly();
			poly->m_geoId = sec.RouteLink;
			oldSec = sec.RouteLink;			
		}
		
		CCoverage::ARC* arc;
		if(arcmap.Lookup(sec.ArcLink,arc)==FALSE)
			continue;
			
		if(sec.F_Pos==0 && sec.T_Pos==100)
		{
			for(long j=0;j<arc->lPosCount;j++)
			{
				CPoint point = Change(arc->points[j]);						
				poly->AddAnchor(point, true, true);
			}
			continue;
		}

		if(sec.F_Pos==100 && sec.T_Pos==0)
		{
			for(long j=arc->lPosCount-1;j>=0;j--)
			{
				CPoint point = Change(arc->points[j]);						
				poly->AddAnchor(point, true, true);
			}
			continue;
		}
	}
		
	if(poly!=nullptr)
	{
		polylist.AddTail(poly);
	}
	return TRUE;
}

BOOL CCoverage::ImportPolygon(CMapLayer& layermap,CLayer* delPoly)
{
	POSITION pos1 = pal_list.GetHeadPosition();
	while(pos1 != nullptr)
	{
		CCoverage::PAL& pal	= pal_list.GetNext(pos1);
		POSITION pos = pat_list.FindIndex(pal.polyId-1);
		if(pos == nullptr)
		{
//			AfxMessageBox(_T("Error!"));
			continue;
		}

		PAT& pat = pat_list.GetAt(pos);
		if(pal.polyId != pat.Id)
		{
//			AfxMessageBox(_T("Error!"));
			continue;
		}

		CLayer* sunlayer;
		if(layermap.Lookup(pat.Code,sunlayer)==false)
			continue;	

		CDoughnut* donut = nullptr;
		CPoly* poly = new CPoly();
		poly->m_bClosed = TRUE;
		poly->m_geoId = pal.polyId;
		poly->m_strName = pat.Name;
		poly->m_strName.TrimRight();

		for(long i=0;i<pal.lRelationArcCount;i++)
		{
			CCoverage::PAL::ARC_POLY arcpoly = pal.relationship[i];

			if(arcpoly.Id == 0 && arcpoly.Id == arcpoly.lNodeId && arcpoly.lNodeId == arcpoly.polyId && i!=0)
			{
				if(donut == nullptr)
				{
					donut = new CDoughnut;
					donut->m_geoId = poly->m_geoId;
					donut->m_strName = poly->m_strName;
					donut->m_strName.TrimRight();
					donut->m_bClosed = TRUE;	
					donut->m_geomlist.AddTail(poly);
				}

				poly = new CPoly();
				poly->m_bClosed = TRUE;
				poly->m_geoId = pal.polyId;
				donut->m_geomlist.AddTail(poly);
				continue;
			}

			CCoverage::ARC* arc;
			if(arcmap.Lookup(abs(arcpoly.Id),arc)==FALSE)
				continue;

			if(arcpoly.Id>=0)
			{
				for(long j=0;j<arc->lPosCount;j++)
				{
					CPoint point = Change(arc->points[j]);						
					poly->AddAnchor(point, true, true);
				}
			}
			else
			{
				for(long j=arc->lPosCount-1;j>=0;j--)
				{
					CPoint point = Change(arc->points[j]);						
					poly->AddAnchor(point, true, true);
				}
			}
		}	

		if(donut != nullptr)
		{
			donut->RecalRect();
			sunlayer->m_geomlist.AddTail(donut);					
		}
		else
		{
			sunlayer->m_geomlist.AddTail(poly);					
		}
	}

	return TRUE;
}

BOOL CCoverage::ImportPolygon(CCoverage::PAL_LIST& pal_list,PAT_LIST& pat_list, CLayer* layer,CLayer* delPoly)
{
	POSITION pos1 = pal_list.GetHeadPosition();
	while(pos1 != nullptr)
	{
		CCoverage::PAL& pal	= pal_list.GetNext(pos1);

		POSITION pos = pat_list.FindIndex(pal.polyId-1);
		if(pos == nullptr)
			continue;
		PAT& pat = pat_list.GetAt(pos);
		if(pal.polyId != pat.Id)
		{
		//	AfxMessageBox(_T("Error!"));
			continue;
		}

		CDoughnut* donut = nullptr;
		CPoly* poly = new CPoly();
		poly->m_bClosed = TRUE;
		poly->m_geoId = pal.polyId;
		poly->m_strName = pat.Name;
		poly->m_strName.TrimRight();
		poly->m_strName.TrimLeft();

		for(long i=0;i<pal.lRelationArcCount;i++)
		{
			CCoverage::PAL::ARC_POLY arcpoly = pal.relationship[i];

			if(arcpoly.Id == 0 && arcpoly.Id == arcpoly.lNodeId && arcpoly.lNodeId == arcpoly.polyId && i!=0)
			{
				if(donut == nullptr)
				{
					donut = new CDoughnut;
					donut->m_geoId = poly->m_geoId;
					donut->m_strName = poly->m_strName;
					donut->m_strName.TrimRight();
					donut->m_strName.TrimLeft();
					donut->m_bClosed = TRUE;	
					donut->m_geomlist.AddTail(poly);
				}

				poly = new CPoly();
				poly->m_bClosed = TRUE;
				poly->m_geoId = pal.polyId;
				donut->m_geomlist.AddTail(poly);
				continue;
			}

			CCoverage::ARC* arc;
			if(arcmap.Lookup(abs(arcpoly.Id),arc)==FALSE)
				continue;

			if(arcpoly.Id>=0)
			{
				for(long j=0;j<arc->lPosCount;j++)
				{
					CPoint point = Change(arc->points[j]);						
					poly->AddAnchor(point, true, true);
				}
			}
			else
			{
				for(long j=arc->lPosCount-1;j>=0;j--)
				{
					CPoint point = Change(arc->points[j]);						
					poly->AddAnchor(point, true, true);
				}
			}
		}	
			
		if(layer->m_strName == _T("依比例水库（含建筑中的）") && donut != nullptr)
		{
			long len = poly->GetLength();
			if(len<400)
			{
				delete poly;
				continue;
			}
		}

		if(donut != nullptr)
		{
			donut->RecalRect();
			layer->m_geomlist.AddTail(donut);					
		}
		else
		{
			layer->m_geomlist.AddTail(poly);					
		}
	}

	return TRUE;
}

CLayer* CCoverage::CreateLayers(CLayerTree& layertree,CMapLayer& layermap,CString strTable)
{
	CLayer* Parlayer = layertree.GetByName(strTable);
	if(Parlayer == nullptr)
	{
		Parlayer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);		
		Parlayer->m_wParent = 0;
		Parlayer->m_wId = layertree.ApplyId();
		layertree.AddTail(Parlayer);			
		Parlayer->m_strName = strTable;
	}
	
/*	HINSTANCE hInst = ::LoadLibrary(_T("Import.dll"));
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CSelectDlg Dlg(nullptr,strLayer);
	if(Dlg.DoModal() == IDOK)
	{
		POSITION pos = Dlg.m_Kinds.GetHeadPosition();
		while(pos != nullptr)
		{
			CSelectDlg::Kind kind = Dlg.m_Kinds.GetNext(pos);
			if(kind.Check == TRUE)
			{
				CLayer* sunlayer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
				DWORD UserId = layertree.ApplyId();
				sunlayer->m_geoId       = UserId;
				sunlayer->m_dwParentID = layer->m_geoId;
				sunlayer->m_strName    = kind.Name;
				sunlayer->m_pSpot->m_geoId=4;
				layertree.AddTail(sunlayer);			

				layermap.SetAt(kind.Code,sunlayer);
			}
		}
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return nullptr;
	}	
*/
	CString strConnect;
	strConnect.Format(_T("DSN=MS Access Database;DBQ=%s;"),m_strDb);
	
	CDatabase pDB;
	BOOL bOpen = pDB.OpenEx(strConnect, CDatabase::openReadOnly | CDatabase::noOdbcDialog);
	if(bOpen == false)
	{
		AfxMessageBox(_T("The database dosn't exist!"));
		return false;
	}

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM %s"), strTable);

	CRecordset rs(&pDB);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL);

		while(!rs.IsEOF()) 
		{
			CString var;
			rs.GetFieldValue((short)4,var);
			if(var != _T("1"))
			{
				rs.MoveNext();
 				continue;
			}
			
			rs.GetFieldValue((short)1,var);
			var.TrimLeft();
			var.TrimRight();

			CString strCode = var;
			
			rs.GetFieldValue((short)2,var);
			var.TrimLeft();
			var.TrimRight();

			CLayer* layer = layertree.GetByName(var);
			if(layer == nullptr)
			{
				layer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
				layer->m_wId = layertree.ApplyId();
				layer->m_wParent = Parlayer->m_wId;
				layertree.AddTail(layer);			
				layer->m_strName = var;
			}
			layermap.SetAt(strCode,layer);

			rs.MoveNext();
		}

		rs.Close();	
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError+"\r\n");
		ex->ReportError();
		ex->Delete();
	}

	pDB.Close();	

	return Parlayer;
}

BOOL CCoverage::ImportLab(CLibrary& library,CMapLayer& layermap,CLayer* delLab)
{
/*	POSITION pos1 = labmap.GetStartPosition();

	while(pos1 != nullptr)
	{
		WORD rkey;
		LAB* lab;
		labmap.GetNextAssoc(pos1, rkey, lab);
		if(lab->polyId!=0)
			continue;

		POSITION pos = pat_list.FindIndex(i++);
		PAT pat = pat_list.GetAt(pos);
		CLayer* layer;
		if(layermap.Lookup(atol(pat.Code),layer)==false)
			continue;	

		CPoint point = Change(lab->points[0]);
		CMark* mark = new CMark(point);
				
		CTag* pTag = new CTag(point,pat.Name);
		pTag->m_UserId = mark->m_geoId;

		mark->CalCorner(layer->GetSpot());
		layer->m_geomlist.AddTail(mark);				
	}*/

	POSITION pos1 = pat_list.GetHeadPosition();
	while(pos1 != nullptr)
	{
		PAT& pat = pat_list.GetNext(pos1);

		LAB* lab = nullptr;
		labmap.Lookup(pat.Id, lab);
		if(lab == nullptr)
			continue;
		CLayer* layer = nullptr;
		layermap.Lookup(pat.Code,layer);
		if(layer==nullptr)
			continue;	
		CPoint point = Change(lab->points[0]);
		CMark* mark = new CMark(point);
		mark->m_geoId = pat.UserId;
		mark->m_strName = pat.Name;				
		mark->m_strName.TrimRight();
		mark->SetupTag(ANCHOR_3, HALIGN::HA_LEFT, VALIGN::VA_CENTER);

   		mark->CalCorner(layer->GetSpot(),m_datainfo,&library);
		layer->m_geomlist.AddTail(mark);				
	}

	return TRUE;
}


CPoint CCoverage::Change(CPointF Scr)
{
	Scr.x = (float)(Scr.x - m_xOffset);
	Scr.y = (float)(Scr.y - m_yOffset);

	Scr.x = Scr.x*m_fScale*10000;
	Scr.y = Scr.y*m_fScale*10000;

	return CPoint((long)Scr.x,(long)Scr.y);
}