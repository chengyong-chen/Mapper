#pragma once
#include "../Geometry/PRect.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
class CGrfDoc;
class CViewinfo;
class CLibrary;
struct Context;
namespace Holder
{
	class Submap;
}
class CSubmap : public CPRect
{
public:
	CSubmap(Holder::Submap* holder);
	CSubmap(CString strFile, CGrfDoc* pDocument);
protected:	
	~CSubmap();

public:
	CString m_strFile;
	CGrfDoc* m_pDocument;
public:
	bool IsArea() const  override { return true; };

public:
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override { return 0.f; };
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;

public:
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
};
