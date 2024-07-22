#pragma once

class __declspec(dllexport) CLink abstract : public CObject
{
	DECLARE_DYNAMIC(CLink);
	CLink();

public:
	DWORD m_dwGeomId;
	CString m_strTarget;

	virtual BOOL Into(CWnd* pWnd);
public:
	virtual CLink* Clone() { return nullptr; };
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseDCOM(CArchive& ar);

	virtual BYTE Gettype() const { return 0; };

	static CLink* Apply(BYTE index);
	static void SerializeList(CArchive& ar, const DWORD dwVersion, CObList& oblist);
};
