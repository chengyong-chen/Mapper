#pragma once

#define MSG_MAX_LENGTH      1024
#define STRING_MAX_LENGTH   512

#define INVALID_MESSAGE     -1

//�ͻ��˵���������ָ��
static const WORD CLIENT_LOGIN                  = 1;
static const WORD CLIENT_LOGOFF                 = 2;
static const WORD CLIENT_PULSE                  = 3;

//��ʾ��ͼ�ͻ���
static const WORD MAPCLIENT_OPENATLAS           = 10;
static const WORD MAPCLIENT_OPENMAP             = 11;
static const WORD MAPCLIENT_MAPDATA             = 12;
static const WORD MAPCLIENT_GETMAPID            = 13;
static const WORD MAPCLIENT_GETPARENTMAP        = 14;
static const WORD MAPCLIENT_GETSUBMAP           = 15;
static const WORD MAPCLIENT_MATCHMAP            = 16;

static const WORD MAPCLIENT_ATLASLOCATION       = 201;
static const WORD MAPCLIENT_ATLAS               = 202;
static const WORD MAPCLIENT_JPGIMAGE            = 203;

//GPS��ؿͻ���
static const WORD GPSCLIENT_VEHICLE_EREASE         = 51; 
static const WORD GPSCLIENT_VEHICLE_TRACK          = 52; 
static const WORD GPSCLIENT_VEHICLE_UNTRACK        = 53; 
static const WORD GPSCLIENT_VEHICLE_CALL           = 54; 
static const WORD GPSCLIENT_VEHICLE_REPLAY         = 55; 
static const WORD GPSCLIENT_VEHICLE_REPLAY_STOP    = 56; 
static const WORD GPSCLIENT_VEHICLE_SORT           = 57;         // infoclent logon
static const WORD GPSCLIENT_VEHICLE_MESSAGE        = 58;
static const WORD GPSCLIENT_VEHICLE_LOCATION       = 59;
static const WORD GPSCLIENT_VEHICLE_DISPATCH       = 60;
static const WORD GPSCLIENT_CLEAR_WARNING          = 63;
static const WORD GPSCLIENT_VEHICLE_COMMAND        = 64;


//���������ͻ��˵�ָ��
static const WORD SERVER_LOGIN_SUCCEED           = 1000 + 1;
static const WORD SERVER_LOGIN_FAILED            = 1000 + 2;
static const WORD SERVER_DOWN                    = 1000 + 3;
static const WORD SERVER_LOGOFF                  = 1000 + 4;
static const WORD SERVER_PULSE                   = 1000 + 5;
static const WORD SERVER_SECURITY                = 1000 + 6;
static const WORD SERVER_BALLOON                 = 1000 + 7;

static const WORD MAPSERVER_OPENATLAS_SUCCEED    = 1000 + 8;
static const WORD MAPSERVER_OPENATLAS_FAILED     = 1000 + 9;
static const WORD MAPSERVER_OPENMAP_SUCCEED      = 1000 + 10;
static const WORD MAPSERVER_OPENMAP_FAILED       = 1000 + 11;
static const WORD MAPSERVER_MAPDATA_SUCCEED      = 1000 + 12;
static const WORD MAPSERVER_MAPDATA_FAILED       = 1000 + 13;

static const WORD MAPSERVER_GETMAPID_SUCCEED     = 1000 + 14;
static const WORD MAPSERVER_GETMAPID_FAILED      = 1000 + 15;
static const WORD MAPSERVER_GETPARENTMAP_SUCCEED = 1000 + 16;
static const WORD MAPSERVER_GETPARENTMAP_FAILED  = 1000 + 17;
static const WORD MAPSERVER_GETSUBMAP_SUCCEED    = 1000 + 18;
static const WORD MAPSERVER_GETSUBMAP_FAILED     = 1000 + 19;
static const WORD MAPSERVER_MATCHMAP_SUCCEED     = 1000 + 20;
static const WORD MAPSERVER_MATCHMAP_FAILED      = 1000 + 21;


static const WORD MAPSERVER_ATLASLOCATION        = 1000 + 201;
static const WORD MAPSERVER_ATLAS                = 1000 + 202;
static const WORD MAPSERVER_BALLOON              = 1000 + 203;
static const WORD MAPSERVER_JPGIMAGE             = 1000 + 256;

static const WORD GPSSERVER_VEHICLE_NEW          = 2000 + 51; 
static const WORD GPSSERVER_VEHICLE_EREASE       = 2000 + 52; 
static const WORD GPSSERVER_VEHICLE_TRACK        = 2000 + 54; 
static const WORD GPSSERVER_VEHICLE_UNTRACK      = 2000 + 55; 
static const WORD GPSSERVER_TRACE_DRAW           = 2000 + 56; 
static const WORD GPSSERVER_TRACE_UNDRAW         = 2000 + 57; 
static const WORD GPSSERVER_TRACE_SAVE           = 2000 + 58; 
static const WORD GPSSERVER_TRACE_UNSAVE         = 2000 + 59; 
static const WORD GPSSERVER_VEHICLE_MOVE         = 2000 + 60; 
static const WORD GPSSERVER_VEHICLE_ICON         = 2000 + 61; 
static const WORD GPSSERVER_VEHICLE_MESSAGE      = 2000 + 64; 
static const WORD GPSSERVER_VEHICLE_LOCATION     = 2000 + 65; 
static const WORD GPSSERVER_VEHICLE_DISPATCH     = 2000 + 67;
static const WORD GPSSERVER_DEALWITH_REPORT      = 2000 + 70;
static const WORD GPSSERVER_SEND_FAILED          = 2000 + 71;

static const WORD VEHICLE_PWD_CHANGED            = 3000 + 72;
static const WORD VEHICLE_MODE_CHANGED           = 3000 + 73;


struct STRUCT_MSG_HEADER
{
	DWORD dwTotalLen;	//�ܳ�
	DWORD wCmdId;  		//����ID
	DWORD dwVehicle;	//����ID
	unsigned char  btHdrCRC;		//��Ϣͷ���У����
	unsigned char  btHdyCRC;		//��Ϣ�����У����


	STRUCT_MSG_HEADER()
	{
		ZeroMemory(this, sizeof(STRUCT_MSG_HEADER));
		dwTotalLen = sizeof(STRUCT_MSG_HEADER);
	}
	STRUCT_MSG_HEADER(DWORD dwVehicleID, WORD wControl)
	{
		ZeroMemory(this, sizeof(STRUCT_MSG_HEADER));
		dwTotalLen = sizeof(STRUCT_MSG_HEADER);

		wCmdId = wControl;
		dwVehicle = dwVehicleID;
	}
};

/// client message
struct STRUCT_CLIENT_PULSE : public STRUCT_MSG_HEADER
{
	STRUCT_CLIENT_PULSE() : STRUCT_MSG_HEADER()
	{
		wCmdId = CLIENT_PULSE;
	}
};

struct STRUCT_CLIENT_LOGIN : public STRUCT_MSG_HEADER
{
	char   cName[20];
	char   cPass[20];

	STRUCT_CLIENT_LOGIN()
	{
		ZeroMemory(this, sizeof(STRUCT_CLIENT_LOGIN));
		dwTotalLen = sizeof(STRUCT_CLIENT_LOGIN);
		wCmdId = CLIENT_LOGIN;
	}

	STRUCT_CLIENT_LOGIN(char* name, char* pass) 
	{
		ZeroMemory(this, sizeof(STRUCT_CLIENT_LOGIN));
		dwTotalLen = sizeof(STRUCT_CLIENT_LOGIN);
		wCmdId = CLIENT_LOGIN;

		if(strlen(name)<sizeof(cName))
			strcpy_s(cName, name);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(pass)<sizeof(cPass)) 
			strcpy_s(cPass, pass);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_CLIENT_LOGOFF : public STRUCT_MSG_HEADER
{
	STRUCT_CLIENT_LOGOFF() : STRUCT_MSG_HEADER()
	{
		wCmdId = CLIENT_LOGOFF;
	}
};

struct STRUCT_MAPCLIENT_OPENATLAS : public STRUCT_MSG_HEADER
{
	char cAtlas[20];

	STRUCT_MAPCLIENT_OPENATLAS()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_OPENATLAS));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_OPENATLAS);
		wCmdId = MAPCLIENT_OPENATLAS;
	}

	STRUCT_MAPCLIENT_OPENATLAS(char* atlas) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_OPENATLAS));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_OPENATLAS);
		wCmdId = MAPCLIENT_OPENATLAS;

		if(strlen(atlas)<sizeof(cAtlas))
			strcpy_s(cAtlas, atlas);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_MAPCLIENT_OPENMAP : public STRUCT_MSG_HEADER
{
	WORD wMapId;

	STRUCT_MAPCLIENT_OPENMAP()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_OPENMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_OPENMAP);
		wCmdId = MAPCLIENT_OPENMAP;
	}

	STRUCT_MAPCLIENT_OPENMAP(WORD wId) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_OPENMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_OPENMAP);
		wCmdId = MAPCLIENT_OPENMAP;

		wMapId = wId;
	}
};
struct STRUCT_MAPCLIENT_MAPDATA : public STRUCT_MSG_HEADER
{
	WORD wMapId;
	WORD wZoom;
	LONG nLeft;
	LONG nTop;
	LONG nRight;
	LONG nBottom;

	STRUCT_MAPCLIENT_MAPDATA()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_MAPDATA));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_MAPDATA);
		wCmdId = MAPCLIENT_MAPDATA;
	}

	STRUCT_MAPCLIENT_MAPDATA(WORD wId, WORD zoom, LONG left, LONG top, LONG right, LONG bottom) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_MAPDATA));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_MAPDATA);
		wCmdId = MAPCLIENT_MAPDATA;

		wMapId = wId;
		wZoom = zoom;
		nLeft = left;
		nTop = top;
		nRight = right;
		nBottom = bottom;
	}
};

struct STRUCT_MAPCLIENT_GETMAPID : public STRUCT_MSG_HEADER
{
	char cMap[20];

	STRUCT_MAPCLIENT_GETMAPID()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_GETMAPID));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_GETMAPID);
		wCmdId = MAPCLIENT_GETMAPID;
	}

	STRUCT_MAPCLIENT_GETMAPID(char* map) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_GETMAPID));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_GETMAPID);
		wCmdId = MAPCLIENT_GETMAPID;

		if(strlen(map)<sizeof(cMap))
			strcpy_s(cMap, map);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_MAPCLIENT_GETPARENTMAP : public STRUCT_MSG_HEADER
{
	WORD wMapId;

	STRUCT_MAPCLIENT_GETPARENTMAP()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_GETPARENTMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_GETPARENTMAP);
		wCmdId = MAPCLIENT_GETPARENTMAP;
	}

	STRUCT_MAPCLIENT_GETPARENTMAP(WORD wId) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_GETPARENTMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_GETPARENTMAP);
		wCmdId = MAPCLIENT_GETPARENTMAP;

		wMapId = wId;
	}
};

struct STRUCT_MAPCLIENT_GETSUBMAP : public STRUCT_MSG_HEADER
{
	WORD wMapId;
	DOUBLE fLng;
	DOUBLE fLat;

	STRUCT_MAPCLIENT_GETSUBMAP()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_GETSUBMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_GETSUBMAP);
		wCmdId = MAPCLIENT_GETSUBMAP;
	}

	STRUCT_MAPCLIENT_GETSUBMAP(WORD wId,DOUBLE lng,DOUBLE lat) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_GETSUBMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_GETSUBMAP);
		wCmdId = MAPCLIENT_GETSUBMAP;

		wMapId = wId;
		fLng = lng;
		fLat = lat; 
	}
};

struct STRUCT_MAPCLIENT_MATCHMAP : public STRUCT_MSG_HEADER
{
	DOUBLE fLng;
	DOUBLE fLat;

	STRUCT_MAPCLIENT_MATCHMAP()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_MATCHMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_MATCHMAP);
		wCmdId = MAPCLIENT_MATCHMAP;
	}

	STRUCT_MAPCLIENT_MATCHMAP(DOUBLE lng,DOUBLE lat) 
	{
		ZeroMemory(this, sizeof(STRUCT_MAPCLIENT_MATCHMAP));
		dwTotalLen = sizeof(STRUCT_MAPCLIENT_MATCHMAP);
		wCmdId = MAPCLIENT_MATCHMAP;

		
		fLng = lng;
		fLat = lat; 
	}
};

struct STRUCT_GPSCLIENT_REPLAY : public STRUCT_MSG_HEADER
{
	char startTime[20];
	char endTime[20];
	UINT  uDelay;

	STRUCT_GPSCLIENT_REPLAY()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_REPLAY));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_REPLAY);
		wCmdId = GPSCLIENT_VEHICLE_REPLAY;
	}
	STRUCT_GPSCLIENT_REPLAY(DWORD vhcId, char* start, char* end, UINT delay)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_REPLAY));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_REPLAY);
		wCmdId = GPSCLIENT_VEHICLE_REPLAY;

		dwVehicle = vhcId;

		if(strlen(start)<sizeof(startTime)) 
			strcpy_s(startTime, start);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(end)<sizeof(endTime)) 
			strcpy_s(endTime, end);
		else 
			wCmdId = INVALID_MESSAGE;

		uDelay = delay;
	}
};

struct STRUCT_GPSCLIENT_VEHICLE_MESSAGE : public STRUCT_MSG_HEADER
{
	char  msg[STRING_MAX_LENGTH];
	char  cIVUSendto[15];
	
	STRUCT_GPSCLIENT_VEHICLE_MESSAGE()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_MESSAGE));

		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_MESSAGE);
		wCmdId = GPSCLIENT_VEHICLE_MESSAGE;
	}
	STRUCT_GPSCLIENT_VEHICLE_MESSAGE(DWORD vhcId, char* str)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_MESSAGE));

		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_MESSAGE);
		wCmdId = GPSCLIENT_VEHICLE_MESSAGE;

		dwVehicle = vhcId;

		if(strlen(str)<sizeof(msg)) 
			strcpy_s(msg, str);
		else 
			wCmdId = INVALID_MESSAGE;
	}
	STRUCT_GPSCLIENT_VEHICLE_MESSAGE(DWORD vhcId, char* str, char* IVU)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_MESSAGE));

		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_MESSAGE);
		wCmdId = GPSCLIENT_VEHICLE_MESSAGE;

		dwVehicle = vhcId;
	
		if(strlen(str)<sizeof(msg)) 
			strcpy_s(msg, str);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(IVU)<sizeof(cIVUSendto)) 
			strcpy_s(cIVUSendto, IVU);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_GPSCLIENT_VEHICLE_COMMAND : public STRUCT_MSG_HEADER
{
	WORD  wCmd;
	char cCmd[STRING_MAX_LENGTH];

	STRUCT_GPSCLIENT_VEHICLE_COMMAND()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_COMMAND));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_COMMAND);
		wCmdId = GPSCLIENT_VEHICLE_COMMAND;
	}
	STRUCT_GPSCLIENT_VEHICLE_COMMAND(DWORD vhcId, WORD cmdId, char* cmd)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_COMMAND));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_COMMAND);
		wCmdId = GPSCLIENT_VEHICLE_COMMAND;

		dwVehicle = vhcId;
		wCmd = cmdId;
		if(strlen(cmd)<sizeof(cCmd)) 
			strcpy_s(cCmd, cmd);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_GPSCLIENT_VEHICLE_LOCATION : public STRUCT_MSG_HEADER
{
	TCHAR cProvince[60];
	TCHAR cCountry[60];
	TCHAR cLocation[STRING_MAX_LENGTH];

	STRUCT_GPSCLIENT_VEHICLE_LOCATION()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION);
		wCmdId = GPSCLIENT_VEHICLE_LOCATION;
	}
	STRUCT_GPSCLIENT_VEHICLE_LOCATION(DWORD vhcId, TCHAR* province, TCHAR* country, TCHAR* location)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION);
		wCmdId = GPSCLIENT_VEHICLE_LOCATION;

		dwVehicle = vhcId;
		if(_tcslen(province)<sizeof(cProvince)) 
			_tcscpy(cProvince, province);
		else 
			wCmdId = INVALID_MESSAGE;

		if(_tcslen(country)<sizeof(cCountry))
			_tcscpy(cCountry, country);
		else 
			wCmdId = INVALID_MESSAGE;

		if(_tcslen(location)<sizeof(cLocation))
			_tcscpy_s(cLocation, location);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};


/// server message
struct STRUCT_SERVER_LOGOFF : public STRUCT_MSG_HEADER
{
	STRUCT_SERVER_LOGOFF() : STRUCT_MSG_HEADER()
	{
		wCmdId = SERVER_LOGOFF;
	}
};

struct STRUCT_SERVER_DOWN : public STRUCT_MSG_HEADER
{
	STRUCT_SERVER_DOWN() : STRUCT_MSG_HEADER()
	{
		wCmdId = SERVER_DOWN;
	}
};

struct STRUCT_SERVER_BALLOON : public STRUCT_MSG_HEADER
{
	char cContent[STRING_MAX_LENGTH];
	STRUCT_SERVER_BALLOON() : STRUCT_MSG_HEADER()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION);
		wCmdId = SERVER_BALLOON;
	}
	STRUCT_SERVER_BALLOON(char* content)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION));
		dwTotalLen = sizeof(STRUCT_GPSCLIENT_VEHICLE_LOCATION);
		wCmdId = SERVER_BALLOON;

		if(strlen(content)<sizeof(cContent))
			strcpy_s(cContent, content);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_SERVER_REPLY_LOGIN : public STRUCT_MSG_HEADER
{
	DWORD dwUserId;
	DWORD dwErr;
	STRUCT_SERVER_REPLY_LOGIN() : STRUCT_MSG_HEADER()
	{
		ZeroMemory(this, sizeof(STRUCT_SERVER_REPLY_LOGIN));
		dwTotalLen = sizeof(STRUCT_SERVER_REPLY_LOGIN);

		wCmdId = SERVER_LOGIN_SUCCEED;
	}

	STRUCT_SERVER_REPLY_LOGIN(DWORD err)
	{
		ZeroMemory(this, sizeof(STRUCT_SERVER_REPLY_LOGIN));
		dwTotalLen = sizeof(STRUCT_SERVER_REPLY_LOGIN);

		wCmdId = SERVER_LOGIN_SUCCEED;

		dwErr = err;
	}
};

struct STRUCT_MAPSERVER_REPLY_OPENATLAS : public STRUCT_MSG_HEADER
{
	STRUCT_MAPSERVER_REPLY_OPENATLAS() : STRUCT_MSG_HEADER()
	{
		wCmdId = MAPSERVER_OPENATLAS_SUCCEED;
	}
};


struct STRUCT_MAPSERVER_REPLY_OPENMAP : public STRUCT_MSG_HEADER
{
//	void* pData;
	STRUCT_MAPSERVER_REPLY_OPENMAP() : STRUCT_MSG_HEADER()
	{
		wCmdId = MAPSERVER_OPENMAP_SUCCEED;
	}
};

struct STRUCT_MAPSERVER_REPLY_MAPDATA : public STRUCT_MSG_HEADER
{
	STRUCT_MAPSERVER_REPLY_MAPDATA() : STRUCT_MSG_HEADER()
	{
		wCmdId = MAPSERVER_MAPDATA_SUCCEED;
	}
};

struct STRUCT_MAPSERVER_REPLY_MAPID : public STRUCT_MSG_HEADER
{
	WORD wMapId;
	STRUCT_MAPSERVER_REPLY_MAPID() : STRUCT_MSG_HEADER()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPSERVER_REPLY_MAPID));
		dwTotalLen = sizeof(STRUCT_MAPSERVER_REPLY_MAPID);

		wCmdId = MAPSERVER_GETMAPID_SUCCEED;
	}
	STRUCT_MAPSERVER_REPLY_MAPID(WORD wId) : STRUCT_MSG_HEADER()
	{
		ZeroMemory(this, sizeof(STRUCT_MAPSERVER_REPLY_MAPID));
		dwTotalLen = sizeof(STRUCT_MAPSERVER_REPLY_MAPID);

		wCmdId = MAPSERVER_GETMAPID_SUCCEED;

		wMapId = wId; 
	}
};

struct STRUCT_GPSSERVER_VEHICLE_TRACK : public STRUCT_MSG_HEADER
{
	STRUCT_GPSSERVER_VEHICLE_TRACK() : STRUCT_MSG_HEADER()
	{
		wCmdId = GPSSERVER_VEHICLE_TRACK;
	}
	STRUCT_GPSSERVER_VEHICLE_TRACK(DWORD vhcId) : STRUCT_MSG_HEADER()
	{
		wCmdId = GPSSERVER_VEHICLE_TRACK;
		dwVehicle = vhcId;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_ICON : public STRUCT_MSG_HEADER
{
	unsigned char  btIcon;
	STRUCT_GPSSERVER_VEHICLE_ICON()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_ICON));
		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_ICON);
		wCmdId = GPSSERVER_VEHICLE_ICON;
	}
	STRUCT_GPSSERVER_VEHICLE_ICON(DWORD vhcId, unsigned char btIcon)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_ICON));
		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_ICON);
		wCmdId = GPSSERVER_VEHICLE_ICON;

		dwVehicle = vhcId;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_EREASE : public STRUCT_MSG_HEADER
{
	STRUCT_GPSSERVER_VEHICLE_EREASE() :  STRUCT_MSG_HEADER()
	{
		wCmdId = GPSSERVER_VEHICLE_EREASE;
	}
	STRUCT_GPSSERVER_VEHICLE_EREASE(DWORD vhcId) :  STRUCT_MSG_HEADER()
	{
		wCmdId = GPSSERVER_VEHICLE_EREASE;
		dwVehicle = vhcId;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_LOCATION : public STRUCT_MSG_HEADER
{
	double  dbLat;
	double  dbLng;

	STRUCT_GPSSERVER_VEHICLE_LOCATION()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_LOCATION));
		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_LOCATION);
		wCmdId = GPSSERVER_VEHICLE_LOCATION;
	}
	STRUCT_GPSSERVER_VEHICLE_LOCATION(DWORD vhcId, double lat, double log)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_LOCATION));
		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_LOCATION);
		wCmdId = GPSSERVER_VEHICLE_LOCATION;

		dwVehicle = vhcId;
		dbLat  = lat;
		dbLng = log;
	}
};

struct STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION : public STRUCT_MSG_HEADER
{
	char cProvince[60];
	char cCountry[60];
	char cLocation[STRING_MAX_LENGTH];
	char cIVU[15];
	char cCode[16];

	STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION));
		
		dwTotalLen = sizeof(STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION);
		wCmdId = GPSSERVER_VEHICLE_LOCATION;
	}
	STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION(DWORD vhcId, char* IVU, char* code, char* province, char* country, char* location)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION));
		
		dwTotalLen = sizeof(STRUCT_GPSSERVER_TOINFOCLIENT_LOCATION);
		wCmdId = GPSSERVER_VEHICLE_LOCATION;

		dwVehicle = vhcId;

		if(strlen(province)<sizeof(cProvince)) 
			strcpy_s(cProvince, province);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(country)<sizeof(cCountry))
			strcpy_s(cCountry, country);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(location)<sizeof(cLocation)) 
			strcpy_s(cLocation, location);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(IVU)<sizeof(cIVU)) 
			strcpy_s(cIVU, IVU);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(code)<sizeof(cCode)) 
			strcpy_s(cCode, code);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_MOVE : public STRUCT_MSG_HEADER
{
	double  dbLat;
	double  dbLng;
	float   flHeight;
	float   flVeo;
	WORD    wDirect;
	double  gpsTime;
	double  locTime;
	DWORD   dwStatus;
	BOOL    bAV;
	
	STRUCT_GPSSERVER_VEHICLE_MOVE()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_MOVE));
		
		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_MOVE);
		wCmdId = GPSSERVER_VEHICLE_MOVE;
	}
	STRUCT_GPSSERVER_VEHICLE_MOVE(DWORD vhcId, double lat, double log, float height, float veo, WORD direct, double gtime, double ltime, DWORD status, BOOL av)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_MOVE));
		
		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_MOVE);
		wCmdId = GPSSERVER_VEHICLE_MOVE;

		dwVehicle = vhcId;
		dbLat  = lat;
		dbLng = log;
		flHeight    = height;
		flVeo       = veo;
		wDirect     = direct;
		gpsTime     = gtime;
		locTime     = ltime;
		dwStatus    = status;
		bAV         = av;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_MESSAGE : public STRUCT_MSG_HEADER
{
	TCHAR  msg[STRING_MAX_LENGTH];
	char  cIVUSendto[15];
	
	STRUCT_GPSSERVER_VEHICLE_MESSAGE()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE);
		wCmdId = GPSSERVER_VEHICLE_MESSAGE;
	}
	STRUCT_GPSSERVER_VEHICLE_MESSAGE(DWORD vhcId, TCHAR* str)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE);
		wCmdId = GPSSERVER_VEHICLE_MESSAGE;

		dwVehicle = vhcId;

		if(_tcslen(str)<sizeof(msg)) 
			_tcscpy(msg, str);
		else 
			wCmdId = INVALID_MESSAGE;
	}
	STRUCT_GPSSERVER_VEHICLE_MESSAGE(DWORD vhcId, TCHAR* str, char* IVU)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE);
		wCmdId = GPSSERVER_VEHICLE_MESSAGE;

		dwVehicle = vhcId;
	
		if(_tcslen(str)<sizeof(msg)) 
			_tcscpy(msg, str);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(IVU)<sizeof(cIVUSendto)) 
			strcpy_s(cIVUSendto, IVU);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_DISPATCH : public STRUCT_MSG_HEADER
{
	UINT uNum;
	
	STRUCT_GPSSERVER_VEHICLE_DISPATCH()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_DISPATCH));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_DISPATCH);
		wCmdId = GPSSERVER_VEHICLE_DISPATCH;
	}
	STRUCT_GPSSERVER_VEHICLE_DISPATCH(DWORD vhcId, UINT num)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_DISPATCH));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_DISPATCH);
		wCmdId = GPSSERVER_VEHICLE_DISPATCH;

		dwVehicle = vhcId;
		uNum = num;
	}
};

struct STRUCT_GPSSERVER_VEHICLE_NEW : public STRUCT_MSG_HEADER
{
	char  vhcCode[16];
	char  vhcColor[8];
	char  vhcIVU[15];
	char  vhcType[20];
	char  vhcPass[10];
	char  strIVU[20];
	char  ownIVU[15];
	char  ownName[10];
	
	STRUCT_GPSSERVER_VEHICLE_NEW()
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_NEW));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_NEW);
		wCmdId = GPSSERVER_VEHICLE_NEW;
	}
	STRUCT_GPSSERVER_VEHICLE_NEW(DWORD vhcId,char* code, char* color, char* vIVU, char* vtype,char* pass, char* mtype, char* oIVU, char* name)
	{
		ZeroMemory(this, sizeof(STRUCT_GPSSERVER_VEHICLE_NEW));

		dwTotalLen = sizeof(STRUCT_GPSSERVER_VEHICLE_NEW);
		wCmdId = GPSSERVER_VEHICLE_NEW;

		dwVehicle = vhcId;

		if(strlen(code)<sizeof(vhcCode)) 
			strcpy_s(vhcCode, code);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(color)<sizeof(vhcColor)) 
			strcpy_s(vhcColor, color);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(vIVU)<sizeof(vhcIVU)) 
			strcpy_s(vhcIVU, vIVU);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(vtype)<sizeof(vhcType)) 
			strcpy_s(vhcType, vtype);
		else
			wCmdId = INVALID_MESSAGE;

		if(strlen(pass)<sizeof(vhcPass)) 
			strcpy_s(vhcPass, pass);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(mtype)<sizeof(strIVU)) 
			strcpy_s(strIVU, mtype);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(oIVU)<sizeof(ownIVU)) 
			strcpy_s(ownIVU, oIVU);
		else 
			wCmdId = INVALID_MESSAGE;

		if(strlen(name)<sizeof(ownName)) 
			strcpy_s(ownName, name);
		else 
			wCmdId = INVALID_MESSAGE;
	}
};