#include "stdafx.h"

#include "Fill.h"
#include "FillEmpty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillEmpty, CFillCompact, 0)

CFillEmpty::CFillEmpty()
{
}

BOOL CFillEmpty::operator==(const CFill& fill) const
{
	if(CFill::operator==(fill)==FALSE)
		return FALSE;
	else
		return fill.IsKindOf(RUNTIME_CLASS(CFillEmpty));
}

Gdiplus::Brush* CFillEmpty::GetBrush(const float& fRatio, const CSize& dpi) const
{
	return nullptr;
}

void CFillEmpty::Serialize(CArchive& ar, const DWORD& dwVersion)
{
}
