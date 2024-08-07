#pragma once

#include <map>
#ifdef STYLE_EXPORTS
#define EXIMPORT __declspec(dllexport)
#else
#define EXIMPORT __declspec(dllimport)
#endif

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
class EXIMPORT CFontDesc
{
public:
	CFontDesc();
	CFontDesc(LPCTSTR lpszFace);

private:
	CStringA m_strReal;
	CString m_strFamily;
	Gdiplus::FontStyle m_style;

public:
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	void SetByFace(CString strFace);
	void SetReal(CStringA strReal);
	void SetStyle(Gdiplus::FontStyle style);
	CString GetFace() const;
	CStringA GetReal() const
	{
		return m_strReal;
	};
	CString GetFamily() const
	{
		return m_strFamily;
	};
	Gdiplus::FontStyle GetStyle() const
	{
		return m_style;
	}
	static CStringA GetEncoding();

	const CFontDesc& operator =(const CFontDesc& descSrc);
	const BOOL operator ==(const CFontDesc& descSrc) const;
	const BOOL operator !=(const CFontDesc& descSrc) const;

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);

	virtual void ReleaseDCOM(CArchive& ar)
	{
	};

	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
public:
	static CMap<CStringA, const CStringA&, CString, const CString&> m_RealToFace;  //one to one
	static CMap<CStringA, const CStringA&, CString, const CString&> m_RealToFamily;//multiple to one

public:
	static void LoadFonts();
	static bool FontRealNameExists(CStringA strReal);
	static CString GetFaceByReal(CStringA strReal);
	static CStringA GetRealByFace(CString strFace);
	static CStringA GetRealByFamily(CString strFamily, Gdiplus::FontStyle style);
	static CString GetFileByReal(CStringA strReal);
	static CString GetFamilyByReal(CStringA strReal);

	static char* GetBestMatched(const char* family, const char* name);
};
