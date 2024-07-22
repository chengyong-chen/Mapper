#pragma once

#include "Line.h"

class __declspec(dllexport) CLineEmpty : public CLine
{
protected:
	DECLARE_SERIAL(CLineEmpty);
	CLineEmpty();
public:

public:
	Gdiplus::Pen* GetPen(const float& fZoom, const CSize& dpi) const override;

	BOOL operator ==(const CLine& line) const override;
	LINETYPE Gettype() const override { return LINETYPE::Blank; };

	void Serialize(CArchive& ar, const DWORD& dwVersion) override
	{
	};

	void ReleaseWeb(CFile& file) const override
	{
	};

	void ReleaseWeb(BinaryStream& stream) const override
	{
	}
	void ReleaseWeb(boost::json::object& json) const override
	{
	}
	void ReleaseWeb(pbf::writer& writer) const override
	{
	}
};
