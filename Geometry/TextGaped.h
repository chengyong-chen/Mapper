#pragma once

#include "Text.h"
#include "Global.h"

class CLibrary;
class CFillGeneral;
class CDatainfo;
class CViewinfo;

class AFX_EXT_CLASS CTextGaped sealed : public CText
{
protected:
	DECLARE_SERIAL(CTextGaped);
	CTextGaped();
	CTextGaped(CPoint origin);

public:

public:
	float m_charspace = 0.f;
	float m_wordspace = 0.f;
	std::map<int, float> m_Gaps;

public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
};