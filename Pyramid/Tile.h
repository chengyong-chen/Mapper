#pragma once

#include "Render.h"



class __declspec(dllexport) CTile : public CRender
{
	DECLARE_SERIAL(CTile);
public:
	CTile();	
	virtual ~CTile();
};