#pragma once

class CDib;

class CBme : public CDib
{
public:
	CBme();

	~CBme() override;

public:
	BITMAPFILEHEADER m_bmfHeader;

public:
	bool Open(LPCTSTR lpszPathName) override;

public:
	CDib* Clone() const override;
	virtual void Copy(CBme* BmeDen) const;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
