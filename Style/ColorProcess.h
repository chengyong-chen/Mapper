#pragma once

#include <Icm.h>

#include "Color.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf_pages.h"

class __declspec(dllexport) CColorProcess : public CColor
{
protected:
	DECLARE_SERIAL(CColorProcess);

public:
	CColorProcess();
	CColorProcess(BYTE alpha);
	CColorProcess(BYTE c, BYTE m, BYTE y, BYTE k, BYTE alpha);

public:
	BYTE m_bC;
	BYTE m_bM;
	BYTE m_bY;
	BYTE m_bK;

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:	
	Gdiplus::Color GetColor() const override;
	COLORREF GetMonitorRGB() const override;
	DWORD ToCMYK() const override;
	COLORREF ToRGB() const override;
	//	virtual COLORREF  ToHSV();
	BYTE Gettype() const override { return 1; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;

	virtual void ExportPs3(FILE* file) const;
	void ExportPdf(HPDF_Page page, bool bFill) const override;

	BOOL operator ==(const CColor& colorSrc) const override;
	void CopyTo(CColor* pDen) const override;

public:
	static HTRANSFORM m_icmToRGB;
	static HTRANSFORM m_icmToMonitor;
	static void LoadCMM();
	static void ReleaseCMM();
};
