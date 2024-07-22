#include "stdafx.h"

#include "FillComplex.h"
#include "FillSymbol.h"
#include "FillPattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillComplex::CFillComplex()
{
}

CFillComplex* CFillComplex::Apply(BYTE type)
{
	CFillComplex* fill = nullptr;
	switch(type)
	{
	case FILLTYPE::Symbol:
		fill = new CFillSymbol;
		break;
	case FILLTYPE::Pattern:
		fill = new CFillPattern;
		break;
	}

	return fill;
}
