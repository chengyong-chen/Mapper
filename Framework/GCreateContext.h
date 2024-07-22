#pragma once

#include "Afxext.h"
class GRuntimeClass;
struct GCreateContext : CCreateContext
{
	GRuntimeClass* m_pFirstViewClass;
	GRuntimeClass* m_pSecondViewClass;
};
