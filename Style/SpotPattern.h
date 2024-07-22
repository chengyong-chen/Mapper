#pragma once

#include "Spot.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
namespace boost {
	namespace 		json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CSpot;
class CGeom;
class CViewinfo;
class CPsboard;
class CLibrary;
class BinaryStream;

class __declspec(dllexport) CSpotPattern : public CSpot
{
protected:
	DECLARE_SERIAL(CSpotPattern);

public:
	CSpotPattern();

	~CSpotPattern() override;

public:
	unsigned short m_libId;
	unsigned short m_nIndex;
	unsigned short m_nScale;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;

public:
	BYTE Gettype() const override { return 3; };
	CRect GetWanRect(const CLibrary& library) const override;
	void Decrease(CLibrary& library) const override;

public:
	BOOL operator ==(const CSpot& spot) const override;
	void CopyTo(CSpot* pSpot) const override;
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
