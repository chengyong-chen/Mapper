#pragma once

#include "Tool.h"

class __declspec(dllexport) CBufferTool : public CTool
{
public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;

public:
	static long Extent;
};
