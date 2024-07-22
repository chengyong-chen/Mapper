#pragma once

#include <Icm.h>

#include "Color.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf_pages.h"

class __declspec(dllexport) CColorSpot : public CColor
{
protected:
	DECLARE_SERIAL(CColorSpot);
	CColorSpot();
	CColorSpot(BYTE alpha);

public:
	CColorSpot(DWORD rgb);
	CColorSpot(DWORD rgb, BYTE alpha);
	CColorSpot(BYTE r, BYTE g, BYTE b, BYTE alpha);
	CColorSpot(const char* pszHex);

public:
	BYTE m_bR;
	BYTE m_bG;
	BYTE m_bB;

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	Gdiplus::Color GetColor() const override;
	COLORREF GetMonitorRGB() const override;
	DWORD ToCMYK() const override;
	COLORREF ToRGB() const override;
	virtual COLORREF ToHSV() const;
	BYTE Gettype() const override { return 2; };

	virtual void SetRGB(BYTE r, BYTE g, BYTE b);
	virtual void SetRGB(COLORREF color);
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;

	virtual void ExportPs3(FILE* file) const;
	void ExportPdf(HPDF_Page page, bool bFill) const override;

	BOOL operator ==(const CColor& colorSrc) const override;
	void CopyTo(CColor* pDen) const override;

public:
	static HTRANSFORM m_icmToCMYK;
	static HTRANSFORM m_icmToMonitor;
	static void LoadCMM();
	static void ReleaseCMM();
	
	static CColorSpot* White();
	static CColorSpot* Black();
	static CColorSpot _white;
	static CColorSpot _black;
};
