#pragma once
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

namespace boost {
	namespace 	json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CSymbol;
class CPsboard;
class BinaryStream;


class __declspec(dllexport) CSymbolSpot : public CSymbol
{
	DECLARE_SERIAL(CSymbolSpot);

public:
	CSymbolSpot();
	~CSymbolSpot() override;

public:
	CPoint m_Anchor;
	bool m_bDirection;

public:
	void CopyTo(CSymbol* pSymbol) const override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void ExportPs3(FILE* file) const override;
	virtual void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const;
	BYTE Gettype() const override { return 3; };
};
