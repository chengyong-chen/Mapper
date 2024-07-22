#include "stdafx.h"

#include "Line.h"
#include "LineNormal.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLineNormal, CLine, 0)

void CLineNormal::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CLine::Sha1(sha1);

	sha1.process_bytes(&m_wColorId, sizeof(WORD));
	sha1.process_bytes(&m_wDashId, sizeof(WORD));
}
BOOL CLineNormal::operator==(const CLine& line) const
{
	if(CLine::operator==(line)==FALSE)
		return FALSE;
	else if(line.IsKindOf(RUNTIME_CLASS(CLineNormal))==FALSE)
		return FALSE;
	else if(m_wColorId!=((CLineNormal*)&line)->m_wColorId)
		return FALSE;
	else if(m_wDashId!=((CLineNormal*)&line)->m_wDashId)
		return FALSE;
	else
		return TRUE;
}

void CLineNormal::CopyTo(CLine* line) const
{
	CLine::CopyTo(line);

	if(line->IsKindOf(RUNTIME_CLASS(CLineNormal))==TRUE)
	{
		((CLineNormal*)line)->m_wColorId = m_wColorId;
		((CLineNormal*)line)->m_wDashId = m_wDashId;
	}
}

void CLineNormal::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CLine::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_wColorId;
		ar<<m_wDashId;
	}
	else
	{
		ar>>m_wColorId;
		ar>>m_wDashId;
	}
}
