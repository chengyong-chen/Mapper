#include "stdafx.h"
#include "MapSinksocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSinkSocket::CMapSinkSocket()
{
	m_wMapId = -1;
}

CMapSinkSocket::~CMapSinkSocket()
{
	POSITION pos = vestigelist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CObject* vestige = vestigelist.GetNext(pos);
		delete vestige;
	}
	vestigelist.RemoveAll();
}
