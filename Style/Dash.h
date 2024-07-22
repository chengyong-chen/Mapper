#pragma once


using namespace std;
#include <vector>

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
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
class BinaryStream;
class CPsboard;

class __declspec(dllexport) CDash : public CObject
{
protected:
	DECLARE_SERIAL(CDash);
public:
	CDash();
	CDash(float data[8], int count);

	~CDash() override;

public:
	WORD m_wId;

	DWORD m_offset;
	DWORD m_segment[8];
	DWORD* GetDate();
	unsigned short GetCount() const;

public:
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	virtual void Scale(float ratio);
public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
	virtual void ExportPs3(FILE* file, CPsboard& aiKey) const;
	virtual void ExportPdf(HPDF_Page page) const;

	const CDash& operator =(const CDash& dashSrc);
	BOOL operator ==(const CDash& dashSrc) const;
	BOOL operator !=(const CDash& dashSrc) const;

	void Solid();
	bool Reset(std::vector<float> data, float offset);
	void Copy(CDash& dashDen) const;
	CDash* Clone() const;
};

typedef CTypedPtrList<CObList, CDash*> CDashList;
