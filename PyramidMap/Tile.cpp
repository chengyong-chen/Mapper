#include "StdAfx.h"
#include "Tile.h"

CTile::CTile(void)
{
	m_waiting=false;
}

CTile::~CTile(void)
{
}


void CTile::SetOwner(CMap* pMap) 
{
	m_ownerMap = pMap;
}

int CTile::GetServerID() 
{
	return m_serverID;
}

bool CTile::GetAsyncActive(void) 
{
	return m_dle.GetAsyncActive();
}

void CTile::WaitFinished() 
{
	m_dle.WaitFinished();
}
	
void CTile::Abort() 
{
	m_dle.Abort();
}

CString CTile::GetURL() 
{
	return m_dle.GetURL();
}

CString CTile::GetFileName() 
{
	return m_dh.GetFilename();
}

void CTile::Load(CString strURL,CString strFile,int x,int y,int serverID)
{
	if(m_bAsyncloading == true)
	{
		return true;
	}
	else
	{
		m_serverID=serverID;
		m_Col=Col;
		m_Row=Row;
		
		CString strURL;
		if(m_Downloader.Open(strURL) == true)
		{
			m_Downloader.begin();
			m_bAsyncloading = true;
			return true;
		}
		else
		{
			m_bAsyncloading = false;
			return false;
		}
	}
}

void CTile::Loaded(int result)
{
	/* if loading error then add url to bad list and make draw code */
	/* draw a broken shape if tile is in the bad list */
	m_bAsyncloading = false;

	if(result != DOWNLOAD_OK)
	{
		m_owner->AddBadURL(m_dle.GetURL());
	}
	else
	{
		Dirty();
	}
}