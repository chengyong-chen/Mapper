#include "stdafx.h"

#include "FillGeneral.h"
#include "FillCompact.h"
#include "FillComplex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillGeneral::CFillGeneral()
{
	m_bOverprint = false;
}

BOOL CFillGeneral::operator==(const CFill& fill) const
{
	if(CFill::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillGeneral))==FALSE)
		return FALSE;
	else if(m_bOverprint!=((CFillGeneral*)&fill)->m_bOverprint)
		return FALSE;
	else
		return TRUE;
}

void CFillGeneral::CopyTo(CFill* fill) const
{
	CFill::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillGeneral))==TRUE)
	{
		((CFillGeneral*)fill)->m_bOverprint = m_bOverprint;
	}
}

void CFillGeneral::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFill::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_bOverprint;
	}
	else
	{
		ar>>m_bOverprint;
	}
}

CFillGeneral* CFillGeneral::Apply(BYTE type)
{
	CFillGeneral* fill = CFillCompact::Apply(type);
	fill = fill==nullptr ? CFillComplex::Apply(type) : fill;
	return fill;
}
