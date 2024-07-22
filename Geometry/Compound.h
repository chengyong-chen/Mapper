#pragma once

#include "Cluster.h"

class AFX_EXT_CLASS CCompound sealed : public CCluster
{
protected:
	DECLARE_SERIAL(CCompound)
	CCompound();

public:
	bool IsArea() const  override { return true; };

public:
	void RecalCentroid() override;
public:
	~CCompound() override;
};
