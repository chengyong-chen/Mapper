#include "stdafx.h"
#include "Dash.h"

#include <any>
#include "Psboard.h"
#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>

using namespace std;
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CDash, CObject, 0)

CDash::CDash()
{
	m_wId = 0;
	m_offset = 0;
	for(BYTE i = 0; i < 8; i++)
		m_segment[i] = 0;
}

CDash::CDash(float data[8], int count)
{
	CDash::Solid();

	m_offset = 0;
	for(BYTE i = 0; i < min(8, count*2); i++)
		m_segment[i] = (data[i]*10);
}

CDash::~CDash()
{
}
void CDash::Sha1(boost::uuids::detail::sha1& sha1) const
{
	unsigned short count = GetCount();
	for(int index = 0; index < count; index++)
	{
		sha1.process_bytes(&m_segment[index],sizeof(int));
	}
}
void CDash::Scale(float ratio)
{
	m_offset *= ratio;
	for(BYTE i = 0; i < 8; i++)
	{
		m_segment[i] *= ratio;
	}
}

DWORD* CDash::GetDate()
{
	return m_segment;
}

void CDash::Solid()
{
	m_offset = 0;
	for(int index = 0; index < 8; index++)
	{
		m_segment[index] = 0;
	}
}

bool CDash::Reset(std::vector<float> data, float offset)
{
	int count = data.size()/2;
	if(count == 0)
		return false;
	else if(count > 4)
		count = 4;

	try
	{
		for(int index = 0; index < 8; index++)
		{
			m_segment[index] = 0;
		}
		for(int index = 0; index < count; index++)
		{
			m_segment[index*2 + 0] = data[index*2 + 0]*10;
			m_segment[index*2 + 1] = data[index*2 + 1]*10;
		}
		return true;
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return false;
}

unsigned short CDash::GetCount() const
{
	int index = 7;
	while(index >= 0 && m_segment[index] == 0)
	{
		index--;
	}
	return (index + 1) % 2 == 0 ? index + 1 : (int)((index + 1)/2)*2;
}

const CDash& CDash::operator =(const CDash& dashSrc)
{
	if(&dashSrc != this)
		memcpy(m_segment, dashSrc.m_segment, 32);
	return *this;
}

BOOL CDash::operator==(const CDash& dashSrc) const
{
	if(m_offset != dashSrc.m_offset)
		return FALSE;

	for(BYTE i = 0; i < 7; i++)
	{
		if(m_segment[i] != dashSrc.m_segment[i])
			return FALSE;
	}
	return TRUE;
}

BOOL CDash::operator !=(const CDash& dashSrc) const
{
	return !(*this == dashSrc);
}

void CDash::Copy(CDash& dashDen) const
{
	dashDen.m_wId = m_wId;
	dashDen.m_offset = m_offset;

	memcpy(dashDen.m_segment, m_segment, sizeof(DWORD)*8);
}

CDash* CDash::Clone() const
{
	CDash* pClone = new CDash;

	pClone->m_wId = m_wId;
	pClone->m_offset = m_offset;

	memcpy(pClone->m_segment, m_segment, sizeof(DWORD)*8);

	return pClone;
}

void CDash::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_offset;
		ar << m_wId;
		ar.Write(&m_segment, 32);
	}
	else
	{
		ar >> m_offset;
		ar >> m_wId;
		ar.Read(&m_segment, 32);
	}
}

void CDash::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar << m_wId;
		ar.Write(&m_segment, 8*sizeof(DWORD));
	}
}

void CDash::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_wId;
		ar.Write(&m_segment, 8*sizeof(DWORD));
	}
	else
	{
		ar >> m_wId;
		ar.Read(&m_segment, 8*sizeof(DWORD));
	}
}

void CDash::ReleaseWeb(CFile& file) const
{
	for(int index = 0; index < 8; index++)
	{
		WORD wLen = m_segment[index];
		ReverseOrder(wLen);
		file.Write(&wLen, sizeof(WORD));
	}
}

void CDash::ReleaseWeb(BinaryStream& stream) const
{
	for(int index = 0; index < 8; index++)
	{
		stream << m_segment[index];
	}
}
void CDash::ReleaseWeb(boost::json::object& json) const
{
	const unsigned short count = CDash::GetCount();
	if(count > 0)
	{
		std::vector<int> child;
		for(int index = 0; index < count; index++)
		{
			child.push_back(m_segment[index]);
		}
		json["Dash"] = boost::json::value_from(child);
	}
}
void CDash::ReleaseWeb(pbf::writer& writer) const
{
	const unsigned short count = CDash::GetCount();
	writer.add_variant_uint16(count);
	for(int index = 0; index < count; index++)
	{
		writer.add_variant_uint32(m_segment[index]);
	}
}
void CDash::ExportPs3(FILE* file, CPsboard& aiKey) const
{
	for(unsigned short i = 0; i < 8; i++)
	{
		if(aiKey.d[i] != m_segment[i])
		{
			const unsigned short count = GetCount();

			_ftprintf(file, _T("["));
			for(unsigned short i = 0; i < count; i++)
			{
				_ftprintf(file, _T("%g "), m_segment[i]/10.f);
			}
			_ftprintf(file, _T("]%d d"), 0);

			memcpy(aiKey.d, m_segment, 8*sizeof(DWORD));
			break;
		}
	}
}

void CDash::ExportPdf(HPDF_Page page) const
{
	const unsigned short count = GetCount();
	if(count > 0)
	{
		float seg[8];
		for(int index = 0; index < count; index++)
		{
			seg[index] = max(m_segment[index]/10.f, 1);
		}
		HPDF_Page_SetDash(page, seg, count, 0);
	}
	else
	{
		HPDF_Page_SetDash(page, nullptr, 0, 0);
	}
}
