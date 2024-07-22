#include "StdAfx.h"
#include "ViewExporter.h"
#include "Datainfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewExporter::CViewExporter(const CDatainfo& foreground)
	: CViewinfo(foreground)
{
}

CViewExporter::~CViewExporter(void)
{
}
