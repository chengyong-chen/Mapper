#pragma once

#include "FontCombox.h"
#include "Hint.h"
#include "TextStyle.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CText;
class CColor;
class CPsboard;
class __declspec(dllexport) CType : public CHint
{
public:
	CType();

	~CType() override;

public:
	TextDeform m_deform;

public:
	bool Setup();

	CType* Clone() const override;

public:
	using CHint::Sha1;
	void Sha1(boost::uuids::detail::sha1& sha1) const override;

public:
	BOOL operator ==(const CHint& type) const override;
	void CopyTo(CHint* pHint) const override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;

	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void ExportPs3(FILE* file, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page) const override;

public:
	static CType m_default;
	static CType* Default(float size = 12, Gdiplus::FontStyle style = Gdiplus::FontStyle::FontStyleRegular);
};
