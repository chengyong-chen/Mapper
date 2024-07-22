#pragma once

#include "../../Public/IPlugin.h"

class AFX_EXT_CLASS CJavascriptMinifier : public IPlugin
{
public:
	void Invoke(int count, ...) override;
};
