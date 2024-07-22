#include "stdafx.h"
#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSecurity::CSecurity()
{
	m_dwRights = 0B00000000000000000000000000000000;

	m_dwRights |= SECURITY_FREE;
	m_dwRights |= SECURITY_EDIT;
	m_dwRights |= SECURITY_QUERY;
	m_dwRights |= SECURITY_REGIONQUERY;
	m_dwRights |= SECURITY_DATABASE;
	m_dwRights |= SECURITY_TRAFFIC;
	m_dwRights |= SECURITY_HTML;
	m_dwRights |= SECURITY_GPS;
	m_dwRights |= SECURITY_NAVIGATE;

	m_bViewLevel = 7;

	DWORD dwSerial;
	GetVolumeInformation(_T("c:\\"), nullptr, 0, &dwSerial, nullptr, nullptr, nullptr, 0);
	m_sysKey1 = (dwSerial>>16);
	m_sysKey2 = (dwSerial<<16)>>16;
}

CSecurity::~CSecurity()
{
}

void CSecurity::SetRights(DWORD dwRights)
{
	m_dwRights = dwRights;
}

void CSecurity::SetFreeOpenRight(bool bFreeOpenRight)
{
	m_dwRights = bFreeOpenRight==true ? (m_dwRights|SECURITY_FREE) : (m_dwRights&~SECURITY_FREE);
}

void CSecurity::SetEditRight(bool bEditRight)
{
	m_dwRights = bEditRight==true ? (m_dwRights|SECURITY_EDIT) : (m_dwRights&~SECURITY_EDIT);
}

void CSecurity::SetRegionQueryRight(bool bRegionQueryRight)
{
	m_dwRights = bRegionQueryRight==true ? (m_dwRights|SECURITY_REGIONQUERY) : (m_dwRights&~SECURITY_REGIONQUERY);
}

void CSecurity::SetDatabaseRight(bool bDatabaseRight)
{
	m_dwRights = bDatabaseRight==true ? (m_dwRights|SECURITY_DATABASE) : (m_dwRights&~SECURITY_DATABASE);
}

void CSecurity::SetTableEditRight(bool bTableEditRight)
{
	m_dwRights = bTableEditRight==true ? (m_dwRights|SECURITY_TABLEEDIT) : (m_dwRights&~SECURITY_TABLEEDIT);
}

void CSecurity::SetBroadcastRight(bool bBroadcastRight)
{
	m_dwRights = bBroadcastRight==true ? (m_dwRights|SECURITY_BROADCAST) : (m_dwRights&~SECURITY_BROADCAST);
}

void CSecurity::SetTrafficRight(bool bTrafficRight)
{
	m_dwRights = bTrafficRight==true ? (m_dwRights|SECURITY_TRAFFIC) : (m_dwRights&~SECURITY_TRAFFIC);
}

void CSecurity::SetNavigateRight(bool bNavigateRight)
{
	m_dwRights = bNavigateRight==true ? (m_dwRights|SECURITY_NAVIGATE) : (m_dwRights&~SECURITY_NAVIGATE);
}

void CSecurity::SetHtmlRight(bool bHtmlRight)
{
	m_dwRights = bHtmlRight==true ? (m_dwRights|SECURITY_HTML) : (m_dwRights&~SECURITY_HTML);
}

void CSecurity::SetTracarRight(bool bTracarRight)
{
	m_dwRights = bTracarRight==true ? (m_dwRights|SECURITY_TRACAR) : (m_dwRights&~SECURITY_TRACAR);
}

void CSecurity::SetViewCoordinateRight(bool bViewCoordinateRight)
{
	m_dwRights = bViewCoordinateRight==true ? (m_dwRights|SECURITY_VIEWCOORDINTE) : (m_dwRights&~SECURITY_VIEWCOORDINTE);
}

bool CSecurity::HasFreeOpenRight() const
{
	return (m_dwRights&SECURITY_FREE)==SECURITY_FREE ? true : false;
}

bool CSecurity::HasEditRight() const
{
	return (m_dwRights&SECURITY_EDIT)==SECURITY_EDIT ? true : false;
}

bool CSecurity::HasRegionQueryRight() const
{
	return (m_dwRights&SECURITY_REGIONQUERY)==SECURITY_REGIONQUERY ? true : false;
}

bool CSecurity::HasDatabaseRight() const
{
	return (m_dwRights&SECURITY_DATABASE)==SECURITY_DATABASE ? true : false;
}

bool CSecurity::HasTableEditRight() const
{
	return (m_dwRights&SECURITY_TABLEEDIT)==SECURITY_TABLEEDIT ? true : false;
}

bool CSecurity::HasTrafficRight() const
{
	return (m_dwRights&SECURITY_TRAFFIC)==SECURITY_TRAFFIC ? true : false;
}

bool CSecurity::HasBroadcastRight() const
{
	return (m_dwRights&SECURITY_BROADCAST)==SECURITY_BROADCAST ? true : false;
}

bool CSecurity::HasNavigateRight() const
{
	return (m_dwRights&SECURITY_NAVIGATE)==SECURITY_NAVIGATE ? true : false;
}

bool CSecurity::HasHtmlRight() const
{
	return (m_dwRights&SECURITY_HTML)==SECURITY_HTML ? true : false;
}

bool CSecurity::HasTracarRight() const
{
	return (m_dwRights&SECURITY_TRACAR)==SECURITY_TRACAR ? true : false;
}

bool CSecurity::HasViewCoordinateRight() const
{
	return (m_dwRights&SECURITY_VIEWCOORDINTE)==SECURITY_VIEWCOORDINTE ? true : false;
}

WORD CSecurity::GetSysKey1() const
{
	return m_sysKey1;
}

WORD CSecurity::GetSysKey2() const
{
	return m_sysKey2;
}

// CSecurity ��Ա����
