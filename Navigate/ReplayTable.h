#pragma once

#include "../Dataview/Viewinfo.h"

#include "Replay.h"

class __declspec(dllexport) CReplayTable : public CReplay
{
	DECLARE_DYNAMIC(CReplayTable);

	class CRS : public CObject
	{
	public:
		CRS()
		{
			rs = nullptr;
		}

		~CRS() override
		{
			if(rs != nullptr)
			{
				if(rs->IsOpen() == TRUE)
				{
					rs->Close();
				}
				delete rs;
				rs = nullptr;
			}
		}

	public:
		CRecordset* rs;
		DWORD dwId;
	};

	CObList RSlist;
	CPtrList Hashinglist;
public:
	CReplayTable(CFrameWnd* pParents); // protected constructor used by dynamic creation

public:
	bool Open() override;
	void Close() override;
	void ReceiveMsg() override;
	void Erease(const DWORD& dwId) override;

private:
	CImageList m_imagelist1;
	CImageList m_imagelist2;

public:
	CString PickTag(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point) override;
	virtual void Draw(CWnd* pWnd, const CViewinfo& viewinfo);

	CString m_strCode;
};
