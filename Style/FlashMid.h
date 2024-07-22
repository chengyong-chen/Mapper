#pragma once

#include "Mid.h"
class BinaryStream;
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class __declspec(dllexport) CFlashMid : public CMid
{
protected:
	DECLARE_SERIAL(CFlashMid);
	CFlashMid();
	CFlashMid(CString strFile);

public:
	~CFlashMid() override;

	unsigned short m_nCount;
	unsigned short m_nHeight;
	unsigned short m_nWidth;

public:
	CSize GetSize() const;
	CSize GetDPI();
	BOOL Load(CString strPathName) override;

public:
	void CopyTo(CMid* pMid) const override;

public:
	virtual void Draw(CDC* pDC, Gdiplus::Point point) const;
	virtual void Draw(Gdiplus::Graphics& g, float fScale) const;

public:
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
};
