#pragma once

#include <Icm.h>

class CIcmm : public CObject
{
public:
	CIcmm();

	~CIcmm() override;

public:
	CString m_strEngine;
	CString m_profileCMYK;
	CString m_profileRGB;
	CString m_profileMonitor;

	HTRANSFORM m_icmCMYKToRGB;
	HTRANSFORM m_icmRGBToCMYK;

	HTRANSFORM m_icmCMYKToMonitor;
	HTRANSFORM m_icmRGBToMonitor;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);

	void Load();
};
