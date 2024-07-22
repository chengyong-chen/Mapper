#pragma once

#include "../../Thirdparty/libharu/2.4.4/include/hpdf_pages.h"

class CColor;

class __declspec(dllexport) CColorGray : public CColor
{
protected:
	DECLARE_SERIAL(CColorGray);
	CColorGray();
public:
	CColorGray(BYTE gray,BYTE alpha);

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	BYTE m_gray;
	Gdiplus::Color GetColor() const override;
	COLORREF ToRGB() const override;
	DWORD ToCMYK() const override;
	//	virtual COLORREF  ToHSV();
	BYTE Gettype() const override { return 3; }
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;

	virtual void ExportPs3(FILE* file) const;
	void ExportPdf(HPDF_Page page, bool bFill) const override;

	BOOL operator ==(const CColor& colorSrc) const override;
	void CopyTo(CColor* pDen) const override;
};
