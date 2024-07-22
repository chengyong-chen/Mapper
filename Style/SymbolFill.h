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
class CSymbol;
class BinaryStream;

class __declspec(dllexport) CSymbolFill : public CSymbol
{
protected:
	DECLARE_SERIAL(CSymbolFill);

public:
	CSymbolFill();

	~CSymbolFill() override;

public:
	void RecalRect() override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void ExportPs3(FILE* file) const override;

	void ExportPdf(HPDF_Page page) const override
	{
	};
	BYTE Gettype() const override { return 2; };
};
