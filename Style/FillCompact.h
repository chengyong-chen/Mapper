#pragma once

#include "FillGeneral.h"

class CGeom;
class CViewinfo;
class CColor;

class __declspec(dllexport) CFillCompact abstract : public CFillGeneral
{
protected:
	CFillCompact();
public:
	virtual void SetColor(CColor* pColor) = 0;
	virtual CColor* GetColor() const = 0;
public:
	static CFillCompact* Apply(BYTE type);
};
