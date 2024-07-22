#pragma once
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include <tuple>

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace 	json {
		class object;
	}
};

class CGeom;
class BinaryStream;

class __declspec(dllexport) CSymbol abstract : public CObject
{
protected:
	DECLARE_DYNAMIC(CSymbol);

public:
	CSymbol();
	~CSymbol() override;

public:
	CRect m_Rect;

	WORD m_wId;
	CString m_strName;
	CTypedPtrList<CObList, CGeom*> m_geomlist;
public:
	virtual void RecalRect();
	virtual CRect GetRect() const { return m_Rect; };
public:
	virtual CSymbol* Clone() const;
	virtual void CopyTo(CSymbol* pDen) const;

public:
	mutable std::tuple<unsigned int[5], int, int, Gdiplus::Bitmap*> m_cache1;
	mutable std::tuple<unsigned int[5], int, int, HBITMAP> m_cache2;
	virtual std::tuple<int, int, const Gdiplus::Bitmap*> GetBitmap1(float fRatio, const CSize& dpi) const;
	virtual std::tuple<int, int, HBITMAP> GetBitmap2(float ratio, const CSize& dpi) const;
public:
	virtual void Draw(Gdiplus::Graphics& g, float fSin, float fCos, float fZoom, const CSize& dpi) const;
	virtual void Mono(Gdiplus::Graphics& g, float fSin, float fCos, float fZoom, const CSize& dpi) const;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
	virtual void ExportPs3(FILE* file) const
	{
	};

	virtual void ExportPdf(HPDF_Page page) const
	{
	};

	virtual BYTE Gettype() const { return 0; };
	static CSymbol* Apply(BYTE index);
};

typedef CTypedPtrList<CObList, CSymbol*> CSymbolList;
