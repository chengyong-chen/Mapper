#include "stdafx.h"
#include "Psboard.h"

#include "Line.h"
#include "FillGeneral.h"
#include "Type.h"
#include "Hint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPsboard::CPsboard(const CLibrary& library)
	: m_library(library)
{
}

CPsboard::~CPsboard()
{
}

void CPsboard::ExportStroke(FILE* file, const CLine* line)
{
	if(line != nullptr)
	{
		line->ExportPs3(file, *this);
	}
}

void CPsboard::ExportBrush(FILE* file, const CFillGeneral* fill)
{
	if(fill != nullptr)
	{
		fill->ExportPs3(file, *this);
	}
}

void CPsboard::ExportType(FILE* file, const CType* type)
{
	if(type != nullptr)
	{
		type->ExportPs3(file, *this);
	}
}

void CPsboard::ExportHint(FILE* file, const CHint* hint)
{
	if(hint != nullptr)
	{
		hint->ExportPs3(file, *this);
	}
}
