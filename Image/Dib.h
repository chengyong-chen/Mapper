#pragma once

#include <string>
#include "../Public/Global.h"
#include "../../Thirdparty/FreeImage/Dist/FreeImage.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf_pages.h"
#include <boost/compute/detail/sha1.hpp>

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace json {
		class object;
	}
};

typedef struct tagNEWSTRING
{
	int Prefix;
	BYTE Character;
} NEW_STRING;

typedef struct tagDICTIONARY
{
	int Prefix;
	BYTE Character;
} DICTIONARY;

#define EMPTY -1
typedef LONG RGBAPIXEL;

struct Blob
{
	void* bytes = nullptr;
	unsigned  int length = 0;
	FREE_IMAGE_FORMAT format = FIF_UNKNOWN;
	~Blob()
	{
		if(bytes!=nullptr)
		{
			::free(bytes);
			bytes = nullptr;
		}
	}

	Blob* Clone()
	{
		Blob* newone = new Blob();
		if(bytes!=nullptr&&length>0)
		{
			newone->bytes = ::malloc(length);
			newone->length = length;
		}
		newone->format = format;
		return newone;
	};
};

class __declspec(dllexport) CDib : public CObject
{
public:
	CDib();

	~CDib() override;

public:
	HGLOBAL m_hData;
	HGLOBAL m_hInfo;
protected:
	SIZE m_Size;
	CSizeF m_DPI;
	int m_nBpp;

protected:
	Blob* m_Source;

	BYTE** m_pLineArray; // Table of the starting addresses of
	bool m_bAlphaChannel;

public:
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	SIZE GetSize() const { return m_Size; }
	CSizeF GetDPI() const { return m_DPI; }
	Gdiplus::Bitmap* GetBitmap(bool asMask) const;

	// Operations
public:
	virtual CDib* GetSubDib(const CRect& rect) const;

	virtual void ExportToBmp(CFile& file)
	{
	};
	virtual bool Open(LPCTSTR lpszPathName);
	virtual void Draw(Gdiplus::Graphics& g, const Gdiplus::Rect& fitinRect, Gdiplus::Rect drawRect,bool asMask) const;
	virtual void Draw(Gdiplus::Graphics& g, const CRect& rect, CRect drawRect, int nAlpha);

	static void CalcDestBPP(int SrcBPP, int* DestBPP);
	void Create(LONG Width, LONG Height, WORD Bpp, bool bAlphaChannel);

	//virtual bool ReadOutBlockDate(CDC* pDC,int Idlecount) override{return TRUE;}

#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseDCOM(CArchive& ar);
	virtual void ReleaseWeb(boost::json::object& json, std::string key) const;
	virtual void ReleaseWeb(pbf::writer& writer, CSize offset = CSize(0, 0)) const;
	virtual void ExportPs3(FILE* file, double fZoom) const;
	virtual void ExportPdf(HPDF_Page page) const;
	virtual bool SaveAs(const char* pathfile) const;

public:

	virtual CDib* Clone() const;
	virtual void Copy(CDib* denDib) const;

public:
	virtual void Or(CDib* pDib);
	virtual void And(CDib* pDib);

public:
	static CDib* FromFile(LPCTSTR lpszPathName);
	static CDib* FromData(FREE_IMAGE_FORMAT m_FIF, BYTE* data, int len);
	static CDib* Load(FREE_IMAGE_FORMAT fif, FIBITMAP* dib);
};

typedef CTypedPtrList<CObList, CDib*> CDibList;
