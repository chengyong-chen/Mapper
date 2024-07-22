#pragma once

#include "Layer.h"
#include "Layif.h"
class AFX_EXT_CLASS CLaylg : public CLayer
{
public:
	CLaylg(CTree<CLayer>& tree);
	~CLaylg() override;

public:
	BYTE Gettype() const override { return 9; };
private:
	CTypedPtrList<CObList, CGeom*> m_geoms;
	std::map<DWORD, BYTE> m_ifindex;

public:
	void Wipeout() override;
	void Rematch(CDatabase* pDatabase, CATTHeaderProfile& profile);
public:
	void Serializelist(CArchive& ar, const DWORD& dwVersion) override;
};
