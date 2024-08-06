#include "stdafx.h"

#include "FontCombox.h"
#include "GetFontFile.h"
#include "GetNameValue.h"
#include "FontDesc.h"

#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include "../Utility/cstring.hpp"
#include <filesystem> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontComboBox 

CFontComboBox::CFontComboBox()
{
}

CFontComboBox::~CFontComboBox()
{
	POSITION pos = m_mapFonts.GetStartPosition();
	while(pos != nullptr)
	{
		CString strKey;
		CFontDesc* pFontDesc;

		m_mapFonts.GetNextAssoc(pos, strKey, pFontDesc);
		delete pFontDesc;
		pFontDesc = nullptr;
	}
}

BEGIN_MESSAGE_MAP(CFontComboBox, CComboBox)
	//{{AFX_MSG_MAP(CFontComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFontComboBox::LoadFontList(BYTE nCharSet)
{
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	lf.lfCharSet = nCharSet;
	lf.lfPitchAndFamily = 0;
	*lf.lfFaceName = 0;
	const HDC hDC = ::GetDC(m_hWnd);
	HRESULT hr = ::EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)CFontComboBox::EnumFontFamExProc, (LPARAM)this, (DWORD)0);
	::ReleaseDC(m_hWnd, hDC);

	BuildFontList();
}
void CFontComboBox::BuildFontList()
{
	POSITION pos = m_mapFonts.GetStartPosition();
	while(pos != nullptr)
	{
		CString strKey;
		CFontDesc* pFontDesc;

		m_mapFonts.GetNextAssoc(pos, strKey, pFontDesc);
		const long nIndex = AddString(strKey);
		ASSERT(nIndex >= 0);
		if(nIndex >= 0) //no error
			SetItemData(nIndex, (DWORD)pFontDesc);
	}
}
int CALLBACK CFontComboBox::EnumFontFamExProc(ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* lpntm, DWORD  FontType, LPVOID pThis)
{
	if(FontType & RASTER_FONTTYPE)
		return 1;

	DWORD dwType = FontType;
	if(FontType & TRUETYPE_FONTTYPE == TRUETYPE_FONTTYPE)
	{
		const DWORD dwFontFlags = lpntm->ntmTm.ntmFlags;
		dwType |= (dwFontFlags & NTM_PS_OPENTYPE ? PS_OPENTYPE_FONTTYPE : 0);
		dwType |= (dwFontFlags & NTM_TT_OPENTYPE ? TT_OPENTYPE_FONTTYPE : 0);
		dwType |= (dwFontFlags & NTM_TYPE1 ? TYPE1_FONTTYPE : 0);
	}
	const LOGFONT& lf = pelf->elfLogFont;
	if(lf.lfCharSet == MAC_CHARSET) // don't put in MAC fonts, commdlg doesn't either
		return 1;

	// Don't display vertical font for FE platform
	if(lf.lfFaceName[0] == '@')//GetSystemMetrics(SM_DBCSENABLED) && 
		return 1;
	const CString strFaceName = pelf->elfLogFont.lfFaceName;
	CFontDesc* pDesc = nullptr;
	if(!((CFontComboBox*)pThis)->m_mapFonts.Lookup(strFaceName, pDesc))
	{
		CFontDesc* pDesc = new CFontDesc(lf.lfFaceName);
		((CFontComboBox*)pThis)->m_mapFonts.SetAt(strFaceName, pDesc);
	}
	return 1;
}

// find a font with the face name and charset
bool CFontComboBox::MatchFont(CFontDesc* fontdesac)
{
	CString face = fontdesac->GetFace();
	const int nIndex = FindStringExact(-1, face);
	if(nIndex != -1)
	{
		SetCurSel(nIndex);
		return true;
	}
	else
	{
		SetCueBanner(face);
		return false;
	}
}

bool CFontComboBox::MatchFont(CString strFont)
{
	const int nIndex = FindStringExact(-1, strFont);
	if(nIndex != -1)
	{
		SetCurSel(nIndex);
		return true;
	}
	else
	{
		SetCueBanner(strFont);
		return false;
	}
}

CFontDesc* CFontComboBox::GetCurSelDesc() const
{
	const long nIndex = GetCurSel();
	return  nIndex != -1 ? (CFontDesc*)GetItemData(nIndex) : nullptr;
}
