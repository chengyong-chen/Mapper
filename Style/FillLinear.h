#pragma once

#include "FillGradient.h"

class CColor;


class __declspec(dllexport) CFillLinear : public CFillGradient
{
public:
	enum VALUEMODE : BYTE
	{
		Angle,
		VALUEPercent,
		VALUEOffset,
		VALUEActual,
	};

protected:
	DECLARE_SERIAL(CFillLinear);

public:
	CFillLinear();
	CFillLinear(double x1, double y1, double x2, double y2, short angle, std::map<unsigned int, CColor*> stops);

	~CFillLinear() override;

public:
	VALUEMODE m_valuemode;
	short m_nAngle;
	double m_x1;
	double m_y1;
	double m_x2;
	double m_y2;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	Gdiplus::Brush* GetBrush(const float& fRatio, const CSize& dpi) const override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo, CRect& docrect) override;

public:
	BOOL operator ==(const CFill& fill) const override;
	void CopyTo(CFill* pDen) const override;
	FILLTYPE Gettype() const override { return FILLTYPE::Linear; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;

	void ExportPs3(FILE* file, const CLibrary& library) const override;

	void ExportPdf(HPDF_Page page) const override
	{
	};
};
class CLibrary;
class __declspec(dllexport) LinearBrush : public Gdiplus::LinearGradientBrush
{
public:
	LinearBrush(CFillLinear::VALUEMODE valuemode, short angle, double x1, double y1, double x2, double y2, std::map<unsigned int, CColor*>  stops)
		:LinearGradientBrush(Gdiplus::Point(0, 10), Gdiplus::Point(200, 10), Gdiplus::Color(255, 255, 0, 0), Gdiplus::Color(255, 0, 0, 255)), m_valuemode(valuemode), m_nAngle(angle), m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2), m_stops(stops)
	{
	};
public:
	CFillLinear::VALUEMODE m_valuemode;
	short m_nAngle;
	double m_x1;
	double m_y1;
	double m_x2;
	double m_y2;
	std::map<unsigned int, CColor*> m_stops;
};

namespace Gdiplus {
};
void AFXAPI operator <<(CArchive& ar, CFillLinear::VALUEMODE& mode);
void AFXAPI operator >>(CArchive& ar, CFillLinear::VALUEMODE& mode);
