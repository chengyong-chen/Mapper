#include "stdafx.h"

#include "Line.h"
#include "LineEmpty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLineEmpty, CLine, 0)

CLineEmpty::CLineEmpty()
{
	m_nWidth = 0;
}

BOOL CLineEmpty::operator==(const CLine& line) const
{
	if(CLine::operator==(line)==FALSE)
		return FALSE;
	else
		return line.IsKindOf(RUNTIME_CLASS(CLineEmpty));
}

Gdiplus::Pen* CLineEmpty::GetPen(const float& fScale, const CSize& dpi) const
{
	return nullptr;
}