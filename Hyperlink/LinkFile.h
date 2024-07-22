#pragma once

class CLink;

class __declspec(dllexport) CLinkFile : public CLink
{
	DECLARE_SERIAL(CLinkFile);
	CLinkFile();

public:
	CLink* Clone() override;
	BOOL Into(CWnd* pWnd) override;
	BYTE Gettype() const override { return 5; };

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseDCOM(CArchive& ar) override;
};
