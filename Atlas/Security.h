#pragma once

// CSecurity ÃüÁîÄ¿±ê

static const DWORD SECURITY_FREE = 0X00000004;
static const DWORD SECURITY_EDIT = 0X00000040;
static const DWORD SECURITY_QUERY = 0X00000400;
static const DWORD SECURITY_REGIONQUERY = 0X00000800;
static const DWORD SECURITY_DATABASE = 0X00004000;
static const DWORD SECURITY_TABLEEDIT = 0X00008000;
static const DWORD SECURITY_BROADCAST = 0X00020000;
static const DWORD SECURITY_TRAFFIC = 0X00040000;
static const DWORD SECURITY_HTML = 0X00080000;
static const DWORD SECURITY_GPS = 0X00100000;
static const DWORD SECURITY_NAVIGATE = 0X00200000;
static const DWORD SECURITY_TRACAR = 0X00400000;
static const DWORD SECURITY_VIEWCOORDINTE = 0X00800000;

class __declspec(dllexport) CSecurity : public CObject
{
public:
	CSecurity();

	~CSecurity() override;

	DWORD m_dwRights;
	BYTE m_bViewLevel;

	bool HasFreeOpenRight() const;
	bool HasEditRight() const;
	bool HasDatabaseRight() const;
	bool HasTableEditRight() const;
	bool HasRegionQueryRight() const;
	bool HasTrafficRight() const;
	bool HasNavigateRight() const;
	bool HasViewCoordinateRight() const;
	bool HasHtmlRight() const;
	bool HasTracarRight() const;
	bool HasBroadcastRight() const;

	void SetRights(DWORD dwRights);
	void SetFreeOpenRight(bool bFreeOpenRight);
	void SetEditRight(bool bEditRight);
	void SetDatabaseRight(bool bEditRight);
	void SetTableEditRight(bool bTableEditRight);
	void SetRegionQueryRight(bool bRegionQueryRight);
	void SetTrafficRight(bool bEditRight);
	void SetNavigateRight(bool bEditRight);
	void SetViewCoordinateRight(bool bViewCoordinateRight);
	void SetHtmlRight(bool bEditRight);
	void SetTracarRight(bool bMonitorRight);
	void SetBroadcastRight(bool bBroadcastRight);

	WORD m_sysKey1;
	WORD m_sysKey2;

	WORD GetSysKey1() const;
	WORD GetSysKey2() const;
};
