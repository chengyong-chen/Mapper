#pragma once

#include <boost/compute/detail/sha1.hpp>
template<> UINT AFXAPI HashKey(const CString& key);

enum FontStyle
{
	Regular = 0X00,
	Italic = 0X01,
	Bold = 0X02,
};
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
class __declspec(dllexport) CFontDesc
{
public:
	CFontDesc();
	CFontDesc(LPCTSTR lpszFaceName);

private:
	CString m_strFamily;
	CString m_strFaceName;
	CStringA m_strRealName;
public:
	Gdiplus::FontStyle m_style;
	BYTE m_Weight;	
public:
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	void SetFaceName(CString strFaceName);
	void SetRealName(CStringA strRealName);
	CString GetFaceName() const
	{
		return m_strFaceName;
	};
	CStringA GetRealName() const
	{
		return m_strRealName;
	};
	CString GetFamilyName() const
	{
		return m_strFamily;
	};

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
	static CMap<CStringA, const CStringA&, CString, const CString&> m_RealToFace;
	static CMap<CStringA, const CStringA&, CString, const CString&> m_RealToFamily;
	
	static void LoadFontNames();
	static bool FontRealNameExists(CStringA strReal);
	static CString GetFaceByReal(CStringA strReal);
	static CStringA GetRealByFace(CString strFace);
	static CStringA GetRealByFamily(CString strFamily);
	static CString GetFileByReal(CStringA strReal);
	static CString GetFamilyByReal(CStringA strReal);
	

	static char* GetBestMatchedByFace(const char* family, const char* name);
};

class __declspec(dllexport) CFontComboBox : public CComboBox
{
public:
	CFontComboBox();

public:
	CTypedPtrMap<CMapStringToPtr, CString, CFontDesc*> m_mapFonts;

	//Operations
	void LoadFontList(BYTE nCharSet = DEFAULT_CHARSET);
	void BuildFontList();
	bool MatchFont(CFontDesc* fontdesac);
	bool MatchFont(CString strFont);

	CFontDesc* GetCurSelDesc() const;

	static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* lpntm, DWORD FontType, LPVOID pThis);

	// Implementation
public:
	~CFontComboBox() override;

protected:
	// Generated message map functions
	//{{AFX_MSG(CFontComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//enum FontCharSet : BYTE
//{
//	ANSI_CHARSET		= 0X00,// ANSI charset (Windows-1252)
//	DEFAULT_CHARSET		= 0X01,
//	SYMBOL_CHARSET		= 0X02,
//	MAC_CHARSET			= 0X4D,
//	SHIFTJIS_CHARSET	= 0X80,// Shift JIS charset (Windows-932)
//	HANGEUL_CHARSET		= 0X81,// Hangeul charset (Windows-949)
//	JOHAB_CHARSET		= 0X82,// Johab charset (Windows-1361)
//	GB2312_CHARSET		= 0X86,// GB2312 charset (Windows-936)
//	CHINESEBIG5_CHARSET = 0X88,// Chinese Big5 charset (Windows-950)
//	GREEK_CHARSET		= 0XA1,// Greek charset (Windows-1253)
//	TURKISH_CHARSET		= 0XA2,// Turkish charset (Windows-1254)
//	VIETNAMESE_CHARSET	= 0XA3,// Vietnamese charset (Windows-1258)
//	HEBREW_CHARSET		= 0XB1,// Hebrew charset (Windows-1255)
//	ARABIC_CHARSET		= 0XB2,// Arabic charset (Windows-1256)
//	BALTIC_CHARSET		= 0XBA,// Baltic charset (Windows-1257)
//	RUSSIAN_CHARSET		= 0XCC,// Cyrillic charset (Windows-1251)
//	THAI_CHARSET		= 0XDE,// Thai charset (Windows-874)
//	EASTEUROPE_CHARSET	= 0XEE,// Eastern european charset (Windows-1250)
//	OEM_CHARSET			= 0XFF
//};
