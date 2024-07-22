#pragma once

#include "FillGeneral.h"
class CGeom;
class CColor;
class CPoly;
class CLibrary;

class __declspec(dllexport) CFillComplex abstract : public CFillGeneral
{
protected:
	CFillComplex();

public:
	static CFillComplex* Apply(BYTE type);
};
