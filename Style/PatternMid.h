#pragma once

#include "Mid.h"
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};

class __declspec(dllexport) CPatternMid : public CMid
{
protected:
	DECLARE_SERIAL(CPatternMid);
	CPatternMid();
	CPatternMid(CString strFile);
public:
	~CPatternMid() override;

	unsigned short m_nCount;
	unsigned short m_nHeight;
	mutable CImageList m_imagelist;
	Gdiplus::Bitmap* m_Bitmap;

public:
	Gdiplus::Bitmap* GetBitmap(const int& index) const;
	CSize GetSize() const;

	static CSize GetDPI();
	BOOL Load(CString strPathName) override;

public:
	void CopyTo(CMid* pMid) const override;

public:
	virtual void Draw(CDC* pDC, Gdiplus::Point point, const int& index) const;
	virtual void Draw(Gdiplus::Graphics& g, const int& index, float fScale) const;

public:
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
};
