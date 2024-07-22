#pragma once

class CLink;

class __declspec(dllexport) CLinkHtml : public CLink
{
	DECLARE_SERIAL(CLinkHtml);
	CLinkHtml();

public:
	CLink* Clone() override;
	BOOL Into(CWnd* pWnd) override;
	BYTE Gettype() const override { return 3; };

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseDCOM(CArchive& ar) override;
};
