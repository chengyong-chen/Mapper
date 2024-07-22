#include "StdAfx.h"
#include "GGLTile.h"

CGGLTile::CGGLTile()
{
}

CGGLTile::~CGGLTile()
{
}


void CGGLTile::Load(kGUIString *url,kGUIString *fn,int x,int y,int serverid)
{
	m_serverid=serverid;
	m_x=x;
	m_y=y;
	m_dle.SetReferer("http://maps.google.com");
	m_dh.SetFilename(fn);
	DebugPrint("LoadTile start,x=%d,y=%d,url='%s'\n",m_x,m_y,url->GetString());

	m_dle.SetAllowCookies(false);
	if(gpx->GetMapAsync()==true)
		m_dle.AsyncDownLoad(&m_dh,url,this,CALLBACKNAME(TileLoaded));
	else
		m_dle.DownLoad(&m_dh,url);
}

void CGGLTile::TileLoaded(int result)
{
	/* if loading error then add url to bad list and make draw code */
	/* draw a broken shape if tile is in the bad list */

	if(result!=DOWNLOAD_OK)
		m_owner->AddBadURL(m_dle.GetURL());
	gpx->GridDirty(m_x,m_y);
}
