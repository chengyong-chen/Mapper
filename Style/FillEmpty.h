#pragma once

#include "FillCompact.h"
class CColor;

class __declspec(dllexport) CFillEmpty : public CFillCompact
{
protected:
	DECLARE_SERIAL(CFillEmpty);
	CFillEmpty();

public:
	BOOL operator ==(const CFill& fill) const override;

public:	
	Gdiplus::Brush* GetBrush(const float& fRatio, const CSize& dpi) const override;
	FILLTYPE Gettype() const override { return FILLTYPE::Blank; };
	CColor* GetColor() const override
	{ 
		return nullptr; 
	};
	void SetColor(CColor* pColor) override {};

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
