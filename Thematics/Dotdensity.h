#pragma once

#include "Theme.h"

class CDotdensity : public CTheme
{
public:
	CDotdensity();

public:
	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 4; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
