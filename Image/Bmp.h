#pragma once

class CDib;

class CBmp : public CDib
{
public:
	CBmp();

	~CBmp() override;

public:
	BITMAPFILEHEADER m_bmfHeader;

public:
	bool Open(LPCTSTR lpszPathName) override;

public:
	CDib* Clone() const override;
	virtual void Copy(CBmp* BmpDen) const;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
