#pragma once

class CLink;

class __declspec(dllexport) CLinkMap : public CLink
{
	DECLARE_SERIAL(CLinkMap);
	CLinkMap();

public:
	CPoint m_Anchor;
	unsigned int m_nScale;

	BOOL m_bAutarchy;

public:
	CLink* Clone() override;
	BOOL Into(CWnd* pWnd) override;
	BYTE Gettype() const override { return 1; };

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseDCOM(CArchive& ar) override;
};
