#pragma once

struct __declspec(dllexport) SMessage
{
	public:
	WORD m_wCommand; //  Command identity
	DWORD m_dwVehicle;

	public:
	BOOL m_bAvailable;
	long nTime;
	long nDate;
	double lat;
	double lng;
	double altitude;
	float nBearing;
	float fSpeed;

	public:
	long nQuality;
	unsigned char nSatelites;

	public:
	DWORD dwWarning;
	DWORD dwStatus;

	public:
	DATE m_varGPSTime;
	DATE m_varRevcTime;

	public:
	BOOL bRotateMap;
};
