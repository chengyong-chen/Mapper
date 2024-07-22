#include "stdafx.h"
#include "Resource.h"
#include "Global.h"
#include "SelectDlg.h"

#include "ImportArcInfo.h"
#include "Coverage.h"
#include "Respt.h"
#include "Respy.h"
#include "Roalk.h"
#include "Railk.h"
#include "Bount.h"
#include "Hydnt.h"
#include "Hydlk.h"
#include "Ternt.h"
#include "Terlk.h"
#include "Ternt.h"
#include "Othnt.h"

#include <stdlib.h>
#include <stdio.h>
#include  <io.h>
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/Mark.h"

#include "../Style/Spot.h"
#include "../Style/Type.h"

CString CSelectDlg::m_strDb;
CString CCoverage::m_strDb;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CImportArcInfo::CImportArcInfo(CDatainfo& datainfo)
		   :CImport(datainfo)
{
	m_fScale = 10000000;
}

BOOL CImportArcInfo::Import(LPCTSTR lpszPath,LPCTSTR lpszExt)
{
	CString strDB = lpszPath;
	strDB = strDB.Left(strDB.ReverseFind(_T('\\')));
	strDB = strDB.Left(strDB.ReverseFind(_T('\\')));
	CSelectDlg::m_strDb = strDB+_T("\\25万分类");
	CCoverage::m_strDb = strDB+_T("\\25万分类");	
	
	m_maxId = maxId;

	BOOL wTransform = TRUE;
	
 // GetParameter();
	CString fold = lpszPath;
	fold +=_T("\\");
	/*
	if(_taccess(fold+_T("Respt"),00)!=-1)
	{	
		CRespt respt(lpszCoveragePath);;
		respt.ReadPAT();
		respt.ReadLAB();
		respt.Import(layertree);
	}

	if(_taccess(fold+_T("Bount"),00)!=-1)
	{
		CBount bount(lpszCoveragePath);
		bount.ReadARC();
		bount.ReadPAL(bount.pal_list);
		bount.ReadPAT();
		bount.ReadAAT();
		bount.ReadPAL(bount.palcnty_list,_T("Cnty"));
		bount.ReadPATCNTY();
	//	bount.ReadPAL(bount.paldist_list,_T("Dist"));
	//	bount.ReadPATDIST();
		bount.ReadPAL(bount.palprov_list,_T("Prov"));
		bount.ReadPATPROV();
		bount.Import(layertree);	
	}
	
	if(_taccess(fold+_T("Railn"),00)!=-1)
	{
		CRailk railk(lpszCoveragePath);
		railk.ReadARC();
		railk.ReadPAT();
		railk.ReadAAT();
	//	railk.ReadRAT(railk.ratraimap,_T("Rai"));
	//	railk.ReadSEC(railk.secrai_list,_T("Rai"));
		railk.Import(layertree);
	}

	if(_taccess(fold+_T("Roaln"),00)!=-1)
	{
		CRoalk roalk(lpszCoveragePath);
		roalk.ReadARC();
		roalk.ReadPAT();
		roalk.ReadAAT();
	//	roalk.ReadRAT(roalk.ratnatmap,_T("Nat"));
	//	roalk.ReadSEC(roalk.secnat_list,_T("Nat"));
	//	roalk.ReadRAT(roalk.rathigmap,_T("Hig"));
	//	roalk.ReadSEC(roalk.sechig_list,_T("Hig"));
		roalk.Import(layertree);	
	}

	if(_taccess(fold+_T("Hydlk"),00)!=-1)
	{
		CHydlk hydlk(lpszCoveragePath);
		hydlk.ReadLAB();
		hydlk.ReadARC();
		hydlk.ReadPAT();
		hydlk.ReadAAT();
		hydlk.Import(layertree);
	}
*/
/*
	if(_taccess(fold+_T("Hydnt"),00)!=-1)
	{
		CHydnt hydnt(lpszCoveragePath);
		hydnt.ReadLAB();	
		hydnt.ReadARC();
		hydnt.ReadPAL(hydnt.pal_list);
		hydnt.ReadPAT();
		hydnt.ReadAAT();
//		hydnt.ReadPAL(hydnt.pallake_list, _T("Lake"));
//		hydnt.ReadPATLAKE();
//		hydnt.ReadRAT(hydnt.rathydmap,  _T("Hyd"));
//		hydnt.ReadSEC(hydnt.sechyd_list,_T("Hyd"));
		hydnt.Import(layertree);
	}

	if(_taccess(fold+_T("Respy"),00)!=-1)
	{
		CRespy respy(lpszCoveragePath);
		respy.ReadPAT();
		respy.ReadLAB();
		respy.ReadARC();
		respy.ReadPAL(respy.pal_list);
	//	respy.ReadPATPOPU();
	//	respy.ReadPAL(respy.palpopu_list, _T("Popu"));
		respy.Import(layertree);
	}*/

	if(_taccess(fold+_T("Terlk"),00)!=-1)
	{
		CTerlk terlk(m_datainfo,lpszPath);
		terlk.ReadPAT();
		terlk.ReadARC();
		terlk.ReadAAT();
		terlk.ReadPAL(terlk.pal_list);
		terlk.Import(layertree);
	}
	/*
	if(_taccess(fold+_T("Ternt"),00)!=-1)
	{
		CTernt ternt(lpszCoveragePath);
		ternt.ReadPAT();
		ternt.ReadARC();
		ternt.ReadAAT();
		ternt.ReadPAL(ternt.pal_list);
		ternt.Import(layertree);
	}
	*/
	
	if(_taccess(fold+_T("Othnt"),00)!=-1)
	{
		COthnt othnt(m_datainfo,lpszPath);
		othnt.ReadPAT();
		othnt.ReadARC();
		othnt.ReadAAT();
		othnt.ReadPAL(othnt.pal_list);
		othnt.Import(layertree);
	}
	maxId = m_maxId;
	return TRUE;
}
CImportArcInfo250000::CImportArcInfo250000(const CDatainfo& datainfo)
		   :CImportArcInfo(datainfo)
{
}
BOOL CImportArcInfo250000::Import(LPCTSTR lpszCoveragePath)
{
	CString strDB = lpszCoveragePath;
//	strDB = strDB.Left(strDB.ReverseFind(_T('\\')));
	CCoverage::m_strDb = strDB+_T("\\25万分类");	

	for(char lat='A';lat<='N';lat++)
	{
		for(long lng=43;lng<=53;lng++)
		{
			TCHAR str[3];
			_itot(lng,str,10);
			CString title100(lat);
			title100 += str;
			CString fold100 = lpszCoveragePath;
			fold100 += _T('\\');
			fold100 += title100;
			if(_taccess(fold100,00)==-1)
				continue;

			for(long div=1;div<=16;div++)
			{
				TCHAR str[3];
				_itot(div,str,10);

				CString title25 =title100;
				if(str[1]==0X00)
					title25 += _T('0');
				title25 += str;

				CString fold25 = fold100;
				fold25 += _T('\\');
				fold25 += title25;
				if(_taccess(fold25,00)==-1)
					continue;

				long x = (long)((lng-43)*4  + (div-1)%4);
				long y = (long)((lat-'A')*4 + (16-div)/4);
				
				Import(fold25,maxId);
			}
		}
	}

	return TRUE;
}

CImportArcInfo1000000::CImportArcInfo1000000(const CDatainfo& datainfo)
		   :CImportArcInfo(datainfo)
{
}
BOOL CImportArcInfo1000000::Import(LPCTSTR lpszCoveragePath)
{	
	CString strDB = lpszCoveragePath;
//	strDB = strDB.Left(strDB.ReverseFind(_T('\\')));
	CCoverage::m_strDb = strDB+_T("\\100万分类");	

	for(char lat='A';lat<='F';lat++)	
	{
		for(long lng=43;lng<=53;lng++)
		{
			TCHAR str[3];
			_itot(lng,str,10);
			CString title100(lat);
			title100 += str;
			CString fold100 = lpszCoveragePath;
			fold100 += _T('\\');
			fold100 += title100;
			if(_taccess(fold100,00)==-1)
				continue;
	
			Import(fold100,maxId);
		}
	}

	return TRUE;
}