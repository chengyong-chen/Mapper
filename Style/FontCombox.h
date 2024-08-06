#pragma once
class CFontDesc;
class CFontComboBox : public CComboBox
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
	bool MatchFont(CStringA strFont);
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
