#pragma once
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace 	json {
		class object;
	}
};
class CPath;
class CSymbol;
class CSymbolSpot;
class CViewinfo;
class CPsboard;
class BinaryStream;
struct Context;

class __declspec(dllexport) CSymbolLine : public CSymbol
{
	DECLARE_SERIAL(CSymbolLine);

public:
	CSymbolLine();

	~CSymbolLine() override;

public:
	unsigned short m_nWidth;
	unsigned short m_nStep;

public:
	void RecalRect() override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CPath* pPath, Context& context, float fZoom) const;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CPath* pPath, Context& context, float fZoom) const;

public:
	void CopyTo(CSymbol* pDen) const override;
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	virtual void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CPath* pPath) const;
	BYTE Gettype() const override { return 1; };
};
