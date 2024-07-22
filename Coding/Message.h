#pragma once

struct __declspec(dllexport) SVehicle
{
	DWORD m_dwId; // ����ID
	CStringA m_strColor; // ������ɫ
	CStringA m_strType; // ��������
	CStringA m_strCode; // ���ƺ���
	CStringA m_strDriver; // ��������

	CStringA m_strIVUSIM; // ��������
	CStringA m_strIVUSN; // ������ʾ��
	CStringA m_strIVUType; // ��������
	CStringA m_strIVUPwd; // ��������

	CStringA m_strOwnerMobile; // �����ֻ�
	CStringA m_strOwnerPhone; // ��������

	BOOL m_bDirty; // ���ݿ��г����ļ�¼���޸Ĺ�

	SVehicle()
	{
		m_dwId = 0;
		m_strDriver = "";
		m_strCode = "";
		m_strColor = "";
		m_strType = "";

		m_strIVUType = "";
		m_strIVUSIM = "";
		m_strIVUPwd = "";

		m_strOwnerMobile = "";
		m_strOwnerPhone = "";

		m_bDirty = FALSE;
	}
};

enum __declspec(dllexport) MESSAGE_TYPE
{
	//	FROM_VEHICLE, 
	//	FROM_CLIENT, 
	//	FROM_SMSCOMMU/*��Ϣ����*/
};

class __declspec(dllexport) CMessageWillSend
{
public:
	CStringA m_strUser;
	DWORD m_dwUser;
	DWORD m_dwVehicle;
	WORD m_wCommand;
	CStringA m_strData;
	bool m_bSupportSMS;
	bool m_bSupportUDP;
	bool m_bSupportTCP;

	UINT m_uSendCnt; // for count this message been sent time

public:
	CMessageWillSend()
	{
		m_dwVehicle = 0;
		m_wCommand = 0;
		m_dwUser = 0;
		m_strUser = _T("");
		m_strData = _T("");
		m_uSendCnt = 0;

		m_bSupportSMS = true;
		m_bSupportUDP = false;
		m_bSupportTCP = false;
	}

	CMessageWillSend(const CMessageWillSend& other)
	{
		m_dwVehicle = other.m_dwVehicle;
		m_wCommand = other.m_wCommand;
		m_dwUser = other.m_dwUser;
		m_strData = other.m_strData;
		m_strUser = other.m_strUser;
		m_uSendCnt = other.m_uSendCnt;

		m_bSupportSMS = other.m_bSupportSMS;
		m_bSupportUDP = other.m_bSupportUDP;
		m_bSupportTCP = other.m_bSupportTCP;
	}

	// copy constructor
	CMessageWillSend(const CMessageWillSend* other)
	{
		m_dwVehicle = other->m_dwVehicle;
		m_wCommand = other->m_wCommand;
		m_dwUser = other->m_dwUser;
		m_strData = other->m_strData;
		m_strUser = other->m_strUser;
		m_uSendCnt = other->m_uSendCnt;

		m_bSupportSMS = other->m_bSupportSMS;
		m_bSupportUDP = other->m_bSupportUDP;
		m_bSupportTCP = other->m_bSupportTCP;
	}
};

class __declspec(dllexport) CMessageReceived
{
public:
	DWORD m_dwVehicle;
	DWORD m_dwUser;

	WORD m_wCommand;
	DWORD m_dwStatus;

	int m_nReserved;
	CString m_strMessage;
	CStringA m_strIVUSN;

	double m_Lon;
	double m_Lat;
	float m_Height;
	float m_Veo;
	WORD m_Direct;
	DATE m_varGPSTime;
	DATE m_varRevcTime;
	BOOL m_bAvailable;

public:
	CMessageReceived() :
		m_dwVehicle(0),
		m_dwUser(0),
		m_wCommand(0xff),
		m_strMessage(""),
		m_Lon(0),
		m_Lat(0),
		m_Height(0),
		m_Veo(0),
		m_Direct(0),
		m_bAvailable(FALSE),
		m_dwStatus(0)
	{
		m_varGPSTime = COleDateTime::GetCurrentTime();
		m_varRevcTime = m_varGPSTime;
	}

	~CMessageReceived()
	{
	}

	// copy constructor
	CMessageReceived(const CMessageReceived& other) :
		m_dwVehicle(other.m_dwVehicle),
		m_dwUser(other.m_dwUser),
		m_wCommand(other.m_wCommand), //  Command identity
		m_strMessage(other.m_strMessage), //  message string except head characters
		m_Lon(other.m_Lon), //����	Integer	4
		m_Lat(other.m_Lat), //γ��Integer	4
		m_Height(other.m_Height), //	�߶�	Integer	4
		m_Veo(other.m_Veo), //�ٶ�	SmallInt	2
		m_Direct(other.m_Direct),
		m_varGPSTime(other.m_varGPSTime),
		m_varRevcTime(other.m_varRevcTime),
		m_bAvailable(other.m_bAvailable),
		m_dwStatus(other.m_dwStatus)
	{
	}

	// copy constructor
	CMessageReceived(const CMessageReceived* other) :
		m_dwVehicle(other->m_dwVehicle),
		m_dwUser(other->m_dwUser),
		m_wCommand(other->m_wCommand), //  Command identity
		m_strMessage(other->m_strMessage), //  message string except head characters
		m_Lon(other->m_Lon), //����	Integer	4
		m_Lat(other->m_Lat), //γ��Integer	4
		m_Height(other->m_Height), //	�߶�	Integer	4
		m_Veo(other->m_Veo), //�ٶ�	SmallInt	2
		m_Direct(other->m_Direct),
		m_varGPSTime(other->m_varGPSTime),
		m_varRevcTime(other->m_varRevcTime),
		m_bAvailable(other->m_bAvailable),
		m_dwStatus(other->m_dwStatus)
	{
	}

	// assignment operator.
	CMessageReceived& operator=(const CMessageReceived& other)
	{
		if(&other==this)
		{
			return *this;
		}
		m_dwVehicle = other.m_dwVehicle;
		m_dwUser = other.m_dwUser;
		m_wCommand = other.m_wCommand; //  Command identity
		m_strMessage = other.m_strMessage; //  message string except head characters
		m_dwStatus = other.m_dwStatus;

		m_Lon = other.m_Lon; //����	Integer	4
		m_Lat = other.m_Lat; //γ��Integer	4
		m_Height = other.m_Height; //	�߶�	Integer	4
		m_Veo = other.m_Veo; //�ٶ�	SmallInt	2
		m_Direct = other.m_Direct;
		m_varGPSTime = other.m_varGPSTime;
		m_varRevcTime = other.m_varRevcTime;
		m_bAvailable = other.m_bAvailable;

		return *this;
	}

protected:
};
