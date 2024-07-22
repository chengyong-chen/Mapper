#pragma once
#include "ClientSocket.h"

class AFX_EXT_CLASS CMapSinkSocket : public CClientSocket
{
public:
	class CLayerVestige : public CObject
	{
	public:
		WORD wLayerId;
		CMap<DWORD, DWORD&, DWORD, DWORD&>* lasttimeId;
		CDWordArray existentId;
		CMap<DWORD, DWORD&, DWORD, DWORD&>* currentId;
	public:
		CLayerVestige()
		{
			lasttimeId = new CMap<DWORD, DWORD&, DWORD, DWORD&>;
			currentId = new CMap<DWORD, DWORD&, DWORD, DWORD&>;
		}

		~CLayerVestige() override
		{
			if(lasttimeId!=nullptr)
			{
				lasttimeId->RemoveAll();
				delete lasttimeId;
				lasttimeId = nullptr;
			}

			existentId.RemoveAll();

			if(currentId!=nullptr)
			{
				currentId->RemoveAll();
				delete currentId;
				currentId = nullptr;
			}
		}
	};

public:
	CMapSinkSocket();

	~CMapSinkSocket() override;
public:
	WORD m_wMapId;
	CObList vestigelist;
};
