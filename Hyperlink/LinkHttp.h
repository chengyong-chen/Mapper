#pragma once

class CLink;

class __declspec(dllexport) CLinkHttp : public CLink
{
	DECLARE_SERIAL(CLinkHttp);
	CLinkHttp();

public:
	CLink* Clone() override;
	BOOL Into(CWnd* pWnd) override;
	BYTE Gettype() const override { return 2; };

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseDCOM(CArchive& ar) override;
};
