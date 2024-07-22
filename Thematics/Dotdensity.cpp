// Dotdensity.cpp : implementation file
//

#include "stdafx.h"
#include "Dotdensity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDotdensity::CDotdensity()
{
	Category = CTheme::CATEGORY::Area;
}

void CDotdensity::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTheme::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

BOOL CDotdensity::operator==(const CTheme& theme) const
{
	if(CTheme::operator==(theme)==FALSE)
		return FALSE;

	if(theme.Gettype()!=this->Gettype())
		return FALSE;
	else
		return TRUE;
}

void CDotdensity::CopyTo(CTheme* pTheme) const
{
	CTheme::CopyTo(pTheme);

	if(pTheme->Gettype()==this->Gettype())
	{
	}
}
