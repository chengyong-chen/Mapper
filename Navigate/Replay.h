#pragma once

#include "../Dataview/Viewinfo.h"

class CPort;

class __declspec(dllexport) CReplay : public CPort
{
	DECLARE_DYNAMIC(CReplay);
	public:
	CReplay(CFrameWnd* pParents); // protected constructor used by dynamic creation

	public:
	bool Open() override;
	void Close() override;
	void ReceiveMsg() override;

	virtual void Erease(const DWORD& dwId)
	{
	};
	private:
	CFile m_Route;
	long m_Count;
	long m_Current;
	public:
	virtual CString PickTag(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point)
	{
		CString strTag;
		return strTag;
	};

	virtual void Draw(CWnd* pWnd)
	{
	};
};
