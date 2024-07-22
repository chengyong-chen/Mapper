#pragma once

#include "Viewinfo.h"

class __declspec(dllexport) CViewExporter : public CViewinfo
{
public:
	CViewExporter(const CDatainfo& foreground);
	~CViewExporter(void);
};
