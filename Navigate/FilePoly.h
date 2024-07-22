#pragma once

#include "FileMap.h"

class CPoly;

class __declspec(dllexport) CFilePoly : public CFileMap
{
	public:
	CFilePoly();

	public:
	~CFilePoly() override;

	public:
	CPoly* GetPoly(DWORD dwIndex) const;
};
