#include "stdafx.h"
#include "Resource.h"
#include "Global.h"

#include "ExportMapInfo.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ExportMapInfoTab(CDatainfo& datainfo,CLayerList* playerlist,LPCTSTR lpszPath,const unsigned int& tolerance)
{
	POSITION pos; 
	pos = playerlist->GetTailposition();
	while(pos != nullptr)
	{
		CLayer* layer = playerlist->GetPrev(pos);
		
		layer->ExportMapInfoTab(Datainfo,lpszPath,true,tolerance);
	} 		

	return TRUE;
}	
BOOL ExportMapInfoMif(const CDatainfo& datainfo,CLayerList* playerlist,LPCTSTR lpszPath,const unsigned int& tolerance)
{
	POSITION pos; 
	pos = playerlist->GetTailposition();
	while(pos != nullptr)
	{
		CLayer* layer = playerlist->GetPrev(pos);
		
		layer->ExportMapInfoMif(Datainfo,lpszPath,true,tolerance);
	} 		

	return TRUE;
}	