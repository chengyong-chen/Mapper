#pragma once

class CLink;

class __declspec(dllexport) CLinkEMail : public CLink
{
	DECLARE_SERIAL(CLinkEMail);
	CLinkEMail();

public:
	CLink* Clone() override;
	BOOL Into(CWnd* pWnd) override;
	BYTE Gettype() const override { return 4; };

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseDCOM(CArchive& ar) override;
};
