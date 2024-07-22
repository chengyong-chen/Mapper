#include "stdafx.h"
#include "Color.h"
#include "ColorSpot.h"

#include "Fill.h"
#include "FillGradient.h"
#include "Library.h"
#include "Psboard.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>
using namespace boost;

using namespace std;
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CFillGradient::CFillGradient()
	: CFillCompact()
{
	m_spread = SPREADMODE::pad;
	m_stops.insert(std::make_pair(0, new CColorSpot(0XFFFFFFFF,255)));
	m_stops.insert(std::make_pair(100, new CColorSpot(0XFF000000,255)));
}
CFillGradient::CFillGradient(std::map<unsigned int, CColor*> stops)
{
	m_spread = SPREADMODE::pad;
	m_stops = stops;
	stops.clear();
}
CFillGradient::~CFillGradient()
{
	for(std::map<unsigned int, CColor*>::const_iterator it = m_stops.begin(); it!=m_stops.end(); it++)
	{
		CColor* pColor = it->second;
		if(pColor!=nullptr)
		{
			delete pColor;
			pColor = nullptr;
		}
	}
	m_stops.clear();
}
void CFillGradient::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	for(std::map<unsigned int, CColor*>::const_iterator it = m_stops.cbegin(); it!=m_stops.cend(); it++)
	{
		sha1.process_bytes(&it->first, sizeof(unsigned int));		
		it->second->Sha1(sha1);
	}
}
void CFillGradient::ClearStops()
{
	for(std::map<unsigned int, CColor*>::const_iterator it = m_stops.begin(); it!=m_stops.end(); it++)
	{
		const CColor* pColor = it->second;
		delete pColor;
	}
	m_stops.clear();
}

BOOL CFillGradient::operator==(const CFill& fill) const
{
	if(CFillCompact::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillGradient))==FALSE)
		return FALSE;
	else if(m_spread!=((CFillGradient&)fill).m_spread)
		return FALSE;
	else if(m_stops.size()!=((CFillGradient*)&fill)->m_stops.size())
		return FALSE;
	else
	{
		std::map<unsigned int, CColor*>::const_iterator it1 = m_stops.begin();
		std::map<unsigned int, CColor*>::const_iterator it2 = ((CFillGradient*)&fill)->m_stops.begin();
		for(int index = 0; index<m_stops.size(); index++)
		{
			if(it1->first!=it2->first)
				return FALSE;
			else if(it1->second==nullptr||it2->second==nullptr)
				return FALSE;
			else if(*(it1->second)!=*(it2->second))
				return FALSE;

			it1++;
			it2++;
		}
		return TRUE;
	}
}

void CFillGradient::CopyTo(CFill* fill) const
{
	CFillCompact::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillGradient))==TRUE)
	{
		((CFillGradient*)fill)->m_spread = m_spread;

		for(std::map<unsigned int, CColor*>::const_iterator it = ((CFillGradient*)fill)->m_stops.begin(); it!=((CFillGradient*)fill)->m_stops.end(); it++)
		{
			const CColor* pColor = it->second;
			delete pColor;
		}
		((CFillGradient*)fill)->m_stops.clear();

		for(std::map<unsigned int, CColor*>::const_iterator it = m_stops.begin(); it!=m_stops.end(); it++)
		{
			CColor* pColor = it->second;
			if(pColor!=nullptr)
			{
				((CFillGradient*)fill)->m_stops.insert(std::make_pair(it->first, pColor->Clone()));
			}
		}
	}
}

void CFillGradient::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillCompact::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_spread;
		const int size = m_stops.size();
		ar<<size;
		for(std::map<unsigned int, CColor*>::iterator it = m_stops.begin(); it!=m_stops.end(); it++)
		{
			ar<<it->first;
			const BYTE colortype = it->second->Gettype();
			ar<<colortype;
			it->second->Serialize(ar, dwVersion);
		}
	}
	else
	{
		ClearStops();
		ar>>m_spread;
		int size;
		ar>>size;
		for(int index = 0; index<size; index++)
		{
			unsigned int position;
			ar>>position;
			BYTE colortype;
			ar>>colortype;
			CColor* pColor = CColor::Apply(colortype);
			if(pColor!=nullptr)
			{
				pColor->Serialize(ar, dwVersion);
				m_stops.insert(std::make_pair(position, pColor));
			}
		}
	}
}

void CFillGradient::ReleaseCE(CArchive& ar) const
{
	CFillCompact::ReleaseCE(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CFillGradient::ReleaseDCOM(CArchive& ar)
{
	CFillCompact::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

DWORD CFillGradient::PackPC(CFile* pFile, BYTE*& bytes)
{
	return CFillCompact::PackPC(pFile, bytes);
}

void CFillGradient::ReleaseWeb(CFile& file) const
{
	CFillCompact::ReleaseWeb(file);
}
void CFillGradient::ReleaseWeb(BinaryStream& stream) const
{
	CFillCompact::ReleaseWeb(stream);
}
void CFillGradient::ReleaseWeb(boost::json::object& json) const
{
	CFillCompact::ReleaseWeb(json);

	boost::json::array child1;
	for(auto it = m_stops.cbegin(); it != m_stops.cend(); it++)
	{
		boost::json::object child2;
		child2["pos"] = it->first;
		it->second->ReleaseWeb(child2);
		child1.push_back(child2);
	}
	json["Stops"] = child1;
}
void CFillGradient::ReleaseWeb(pbf::writer& writer) const
{
	CFillCompact::ReleaseWeb(writer);

	writer.add_variant_uint32(m_stops.size());
	for(auto it=m_stops.cbegin(); it != m_stops.cend(); it++)
	{
		writer.add_variant_uint32(it->first);
		it->second->ReleaseWeb(writer);
	}
}
void CFillGradient::ExportPs3(FILE* file, const CLibrary& library) const
{
	CPsboard psboard(library);
}

void AFXAPI operator <<(CArchive& ar, CFillGradient::SPREADMODE& spread)
{
	ar<<(BYTE)spread;
};

void AFXAPI operator >>(CArchive& ar, CFillGradient::SPREADMODE& spread)
{
	BYTE byte;
	ar>>byte;
	spread = (CFillGradient::SPREADMODE)byte;
};
