#pragma once

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include <boost/compute/detail/sha1.hpp>

namespace boost {
	namespace 		json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CLibrary;
class CViewinfo;
class CPsboard;
class CDatainfo;
class BinaryStream;

class __declspec(dllexport) CSpot abstract : public CObject
{
public:
	CSpot();
	~CSpot() override;
public:
	virtual void Sha1(boost::uuids::detail::sha1::digest_type& hash) const;
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	virtual BYTE Gettype() const { return 0; };
	virtual CRect GetWanRect(const CLibrary& library) const;
	virtual CRect GetDocRect(const CDatainfo& datainfo, const CLibrary& library) const;
	virtual void Decrease(CLibrary& library) const {};

public:
	virtual CSpot* Clone() const;
	virtual BOOL operator ==(const CSpot& spot) const;
	virtual BOOL operator !=(const CSpot& spot) const;
	virtual void CopyTo(CSpot* pSpot) const;
	virtual std::tuple<int, int, const Gdiplus::Bitmap*, bool> GetBitmap1(const CLibrary& library, const float& fSin, const float& fCos, float fRatio, const CSize& dpi) const { return {0, 0, nullptr, true}; };
	virtual std::tuple<int, int, HBITMAP, bool> GetBitmap2(const CLibrary& library, const float& fSin, const float& fCos, float ratio, const CSize& dpi) const { return {0, 0, nullptr, true}; };

public:
	virtual void Draw(Gdiplus::Graphics& g, const CLibrary& library, const float& fSin, const float& fCos, const float& fZoom, const CSize& dpi) const=0;
public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion){};
	virtual void ReleaseCE(CArchive& ar) const{};
	virtual void ReleaseDCOM(CArchive& ar){};
	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	virtual void ReleaseWeb(CFile& file) const=0;
	virtual void ReleaseWeb(BinaryStream& stream) const = 0;
	virtual void ReleaseWeb(boost::json::object& json) const = 0;
	virtual void ReleaseWeb(pbf::writer& writer) const = 0;
	virtual void ExportPs3(FILE* file, const CLibrary& library) const = 0;
	virtual void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const = 0;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CLibrary& library, const Gdiplus::Matrix& matrix) const = 0;
	static CSpot* Apply(BYTE type);


protected:
	mutable	bool m_bInitialized;
	mutable	CRect m_tmpRect;
};

typedef CTypedPtrList<CObList, CSpot*> CSpotList;
