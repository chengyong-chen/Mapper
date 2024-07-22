#pragma once

#include "FillGradient.h"

class CColor;

class __declspec(dllexport) CFillRadial : public CFillGradient
{
public:
	enum CENTERMODE : BYTE
	{
		Geomcenter = 0,
		Centroid,
		CenterPercent,
		CenterOffset,
		CenterCoordinate,
	};

	enum RADIUSMODE : BYTE
	{
		OUTERELLIPSE = 0,
		INNERELLIPSE,
		OUTERCIRCLE,
		INNERCIRCLE,
		OUTPATH,
		RadiusPercent,
		RadiusValue,
	};

	enum FOCUSMODE : BYTE
	{
		CENTER = 0,
		FocusPercent,
		FocusOffset,
		FocusCoordinate,
	};

protected:
	DECLARE_SERIAL(CFillRadial);

public:
	CFillRadial();
	CFillRadial(float cx, float cy, float cr, float fx, float fy, float fr, std::map<unsigned int, CColor*> stops);

	~CFillRadial() override;

public:
	CENTERMODE m_centermode;
	RADIUSMODE m_radiusmode;
	FOCUSMODE m_focusmode;
	float m_cx;
	float m_cy;
	float m_cr;
	float m_fr;
	float m_fx;
	float m_fy;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	Gdiplus::Brush* GetBrush(const float& fRatio, const CSize& dpi) const override;

public:
	BOOL operator ==(const CFill& fill) const override;
	void CopyTo(CFill* pDen) const override;
	FILLTYPE Gettype() const override { return FILLTYPE::Radial; };
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo, CRect& docrect) override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;

	void ExportPs3(FILE* file, const CLibrary& library) const override;

	void ExportPdf(HPDF_Page page) const override
	{
	};
};
class __declspec(dllexport) RadialBrush : public Gdiplus::PathGradientBrush
{
public:
	RadialBrush(CFillRadial::CENTERMODE centermode, CFillRadial::RADIUSMODE radiusmode, CFillRadial::FOCUSMODE focusmode, float cx, float cy, float fx, float fy, float cr, float fr, std::map<unsigned int, CColor*>  stops)
		:PathGradientBrush(new Gdiplus::Point[4]{Gdiplus::Point(0,0), Gdiplus::Point(1, 0), Gdiplus::Point(1, 1), Gdiplus::Point(0, 1)},4),		m_centermode(centermode),  m_radiusmode(radiusmode), m_focusmode(focusmode), m_cx(cx), m_cy(cy), m_fx(fx), m_fy(fy), m_cr(cr), m_fr(fr), m_stops(stops)
	{
	};
public:
	CFillRadial::CENTERMODE m_centermode;
	CFillRadial::RADIUSMODE m_radiusmode;
	CFillRadial::FOCUSMODE m_focusmode;
	float m_cx;
	float m_cy;
	float m_cr;
	float m_fr;
	float m_fx;
	float m_fy;
	std::map<unsigned int, CColor*> m_stops;
};

namespace Gdiplus
{
};
void AFXAPI operator <<(CArchive& ar, CFillRadial::CENTERMODE& center);
void AFXAPI operator >>(CArchive& ar, CFillRadial::CENTERMODE& center);
void AFXAPI operator <<(CArchive& ar, CFillRadial::RADIUSMODE& radial);
void AFXAPI operator >>(CArchive& ar, CFillRadial::RADIUSMODE& radial);
void AFXAPI operator <<(CArchive& ar, CFillRadial::FOCUSMODE& focus);
void AFXAPI operator >>(CArchive& ar, CFillRadial::FOCUSMODE& focus);
