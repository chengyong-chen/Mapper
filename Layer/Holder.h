#pragma once

#include "Layer.h"

class AFX_EXT_CLASS CHolder : public CLayer
{
public:
	CHolder(CTree<CLayer>& tree) :CLayer(tree) 
	{
	};
};
