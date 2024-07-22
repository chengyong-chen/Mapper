#pragma once

#include "Layer.h"
class CLaylg;
class AFX_EXT_CLASS CLayif : public CLayer
{
public:
	CLayif(CTree<CLayer>& tree);
	CLayif(CTree<CLayer>& tree, CLaylg* pParent, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);
	~CLayif() override;

public:
	BYTE Gettype() const override { return 10; };
	bool IsControllable() const override { return false; }
	bool IsInfertile() const override { return true; }
public:
	CStringA m_strIf;
public:
	void Wipeout() override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms = false) override;
};
