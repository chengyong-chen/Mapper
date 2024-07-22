#include "stdafx.h"

#include "TextGaped.h"
#include "Geom.h"

#include "../Style/color.h"
#include "../Style/Library.h"

#include "../Rectify/Tin.h"


#include <boost/locale.hpp>
#include <boost/json.hpp>
#include <cassert>

using namespace boost;
using namespace boost::locale;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CTextGaped, CText, 0)

CTextGaped::CTextGaped()
	: CText()
{
	m_bType = 18;
}

CTextGaped::CTextGaped(CPoint origin)
	: CText(origin)
{
	m_bType = 18;
}

void CTextGaped::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CText::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_charspace;
		ar<<m_wordspace;
		const int count = m_Gaps.size();
		ar << count;
		for(auto it = m_Gaps.begin(); it != m_Gaps.end(); it++)
		{
			ar << it->first;
			ar << it->second;
		}

	}
	else
	{
		ar >> m_charspace;
		ar >> m_wordspace;
		int count;
		ar >> count;
		for(int index=0;index<count;index++)
		{
			int pos;
			float gap;
			ar >> pos;
			ar >> gap;
			m_Gaps[pos] = gap;
		}
	}
}

void CTextGaped::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	CText::Draw(g, viewinfo, ratio);
}

void CTextGaped::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	CText::Draw(g, viewinfo, library, context);

	if(m_charspace!=0)
	{
		const HDC hdc = g.GetHDC();
		int dd = GetTextCharacterExtra(hdc);
		SetTextCharacterExtra(hdc, m_charspace);
		g.ReleaseHDC(hdc);
	}
}


void CTextGaped::ReleaseWeb(boost::json::object& json) const
{
	CText::ReleaseWeb(json);
	json["charspace"] = m_charspace;
	json["wordspace"] = m_wordspace;
	if(this->m_Gaps.size()>0){
		json["Gaps"] = boost::json::value_from(this->m_Gaps);
	}
}


void CTextGaped::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CText::ReleaseWeb(writer, offset);

	writer.add_float(m_charspace);
	writer.add_float(m_wordspace);
	const int count=m_Gaps.size();
	writer.add_variant_uint16(count);
	for(auto it=m_Gaps.begin(); it != m_Gaps.end(); it++)
	{
		writer.add_variant_uint32(it->first);
		writer.add_float(it->second);
	}
}
