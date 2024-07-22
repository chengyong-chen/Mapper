#include "stdafx.h"

#include "FillAlone.h"
#include "FillEmpty.h"
#include "FillLinear.h"
#include "FillRadial.h"
#include "FillCompact.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillCompact::CFillCompact()
{
}

CFillCompact* CFillCompact::Apply(BYTE type)
{
	CFillCompact* fill = nullptr;
	switch(type)
	{
	case FILLTYPE::Blank:
		fill = new CFillEmpty();
		break;
	case FILLTYPE::Alone:
		fill = new CFillAlone();
		break;
	case FILLTYPE::Normal:
		break;
	case FILLTYPE::Linear:
		fill = new CFillLinear;
		break;
	case FILLTYPE::Radial:
		fill = new CFillRadial;
		break;
	}

	return fill;
}
