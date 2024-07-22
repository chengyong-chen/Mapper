#pragma once


#include "../../Thirdparty/libharu/2.4.4/include/hpdf_pages.h"
#include <boost/compute/detail/sha1.hpp>

namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf { 
	class writer; 
	using tag = uint8_t;
};

class CPsboard;
class BinaryStream;

class __declspec(dllexport) CColor abstract : public CObject
{
	DECLARE_DYNAMIC(CColor);
protected:
	CColor(BYTE alpha);
public:
	~CColor() override;

public:
	BYTE m_bId;
	BYTE m_bAlpha;

public:
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	virtual void SetAlpha(BYTE alpha) { m_bAlpha=alpha; };
	virtual Gdiplus::Color GetColor() const { return Gdiplus::Color(m_bAlpha, 0, 0, 0); };
	virtual COLORREF GetMonitorRGB() const { return RGB(0, 0, 0); };
	virtual DWORD ToCMYK() const { return 0; };
	virtual COLORREF ToRGB() const { return RGB(0, 0, 0); }
	virtual BYTE Gettype() const { return 0; };
	static CColor* Apply(BYTE index);

	virtual BOOL operator ==(const CColor& colorSrc) const;
	virtual BOOL operator !=(const CColor& colorSrc) const;
	virtual CColor* Clone() const;
	virtual void CopyTo(CColor* pDen) const;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
	virtual void ExportAIStroke(FILE* file, CPsboard& aiKey) const;
	virtual void ExportAIBrush(FILE* file, CPsboard& aiKey) const;

	virtual void ExportPdf(HPDF_Page page, bool bFill) const
	{
	};

public:
	static DWORD FindCMM(CString strCMM);
	static Gdiplus::ARGB RGBtoARGB(COLORREF rgb);
	static COLORREF ARGBtoRGB(Gdiplus::ARGB argb);
};

typedef CTypedPtrList<CObList, CColor*> CColorList;
