#pragma once

#include "Spot.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

namespace boost {
	namespace json {
		class object;
	}
};
class CSpot;
class CLibrary;
class CPsboard;
class BinaryStream;

class __declspec(dllexport) CSpotSymbol : public CSpot
{
protected:
	DECLARE_SERIAL(CSpotSymbol);

public:
	CSpotSymbol();

	~CSpotSymbol() override;

public:
	WORD m_libId;
	WORD m_symId;
	unsigned short m_nScale;
	unsigned short m_nAngle;

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;

public:
	BYTE Gettype() const override { return 1; };
	CRect GetWanRect(const CLibrary& library) const override;
	void Decrease(CLibrary& library) const override;

public:
	BOOL operator ==(const CSpot& spot) const override;
	void CopyTo(CSpot* pSpot) const override;
	std::tuple<int, int, const Gdiplus::Bitmap*, bool> GetBitmap1(const CLibrary& library, const float& fSin, const float& fCos, float fRatio, const CSize& dpi) const override;
	std::tuple<int, int, HBITMAP, bool> GetBitmap2(const CLibrary& library, const float& fSin, const float& fCos, float ratio, const CSize& dpi) const override;

public:
	void Draw(Gdiplus::Graphics& g, const CLibrary& library, const float& fSin, const float& fCos, const float& fZoom, const CSize& dpi) const override;
	
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void ExportPs3(FILE* file, const CLibrary& library) const override;
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CLibrary& library, const Gdiplus::Matrix& matrix) const override;
};
