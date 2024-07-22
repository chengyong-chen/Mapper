#pragma once

#include "Line.h"

class __declspec(dllexport) CLineNormal : public CLine
{
protected:
	DECLARE_SERIAL(CLineNormal);
public:
	WORD m_wColorId;
	WORD m_wDashId;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;

public:
	BOOL operator ==(const CLine& line) const override;
	void CopyTo(CLine* pDen) const override;
	LINETYPE Gettype() const override { return LINETYPE::Normal; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
