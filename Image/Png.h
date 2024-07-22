#pragma once

#include "Dib.h"

class __declspec(dllexport) CPng : public CDib
{
public:
	CPng(const unsigned char* data,int len);
};