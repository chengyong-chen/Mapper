#pragma once

#include "../../Public/IPlugin.h"

class AFX_EXT_CLASS CTiler : public IPlugin
{
public:
	void Invoke(int count, ...) override;
};
