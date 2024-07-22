#pragma once

#include "Downloader.h"

class CTile
{
public:
	CTile();
	~CTile();

private:
	CMap* m_ownerMap;
	DWORD m_serverID;
	CDownloader m_Downloader;
private:
	int m_Row;
	int m_Col;

	static int m_Width
	static int m_Height;
	
	bool m_bAsyncActive;
	
public:
	 SetOwner(CMap* pMap);

public:
	void Load(CString strURL,CString strfn,int x,int y,int serverid);
	CString GetURL();
	CString GetFileName();

	bool GetAsyncActive();
	void WaitFinished();
	int GetServerID();
	void Abort();

	void Loaded(int result);	
};
