#include "stdafx.h"

#include "Color.h"

#include "FillAlone.h"
#include "Psboard.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillAlone, CFillCompact, 0)

CFillAlone::CFillAlone()
{
	m_bOverprint = false;
	m_pColor = nullptr;
}
CFillAlone::CFillAlone(CColor* pColor)
	:m_pColor(pColor)
{

}
CFillAlone::~CFillAlone()
{
	if(m_pColor!=nullptr)
	{
		delete m_pColor;
		m_pColor = nullptr;
	}
}
void CFillAlone::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	if(m_pColor!=nullptr)
	{
		m_pColor->Sha1(sha1);
	}
}
BOOL CFillAlone::operator==(const CFill& fill) const
{
	if(CFillCompact::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillAlone))==FALSE)
		return FALSE;
	else if(m_pColor!=nullptr&&((CFillAlone&)fill).m_pColor!=nullptr)
		return (*m_pColor==*((CFillAlone&)fill).m_pColor);
	else if(m_pColor==nullptr&&((CFillAlone&)fill).m_pColor==nullptr)
		return TRUE;
	else
		return FALSE;
}

void CFillAlone::CopyTo(CFill* fill) const
{
	CFillCompact::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillAlone))==TRUE)
	{
		delete ((CFillAlone*)fill)->m_pColor;
		((CFillAlone*)fill)->m_pColor = nullptr;
		if(m_pColor!=nullptr)
		{
			((CFillAlone*)fill)->m_pColor = m_pColor->Clone();
		}
	}
}

Gdiplus::Brush* CFillAlone::GetBrush(const float& fRatio, const CSize& dpi) const
{
	if(m_pColor!=nullptr)
	{
		const Gdiplus::Color color = m_pColor->GetColor();
		return ::new Gdiplus::SolidBrush(color);
	}
	else
	{
		return ::new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255));
	}
}

Gdiplus::Brush* CFillAlone::GetBrush(CLibrary& library, const float& fRatio, const CSize& dpi) const
{
	return GetBrush(fRatio, dpi);
}

void CFillAlone::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillCompact::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}
	else
	{
		BYTE colortype = -1;
		ar>>colortype;

		delete m_pColor;
		m_pColor = CColor::Apply(colortype);
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->Serialize(ar, dwVersion);
	}
}

void CFillAlone::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseCE(ar);
	}
}

void CFillAlone::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}
	else
	{
		BYTE colortype = -1;
		ar>>colortype;

		delete m_pColor;
		m_pColor = CColor::Apply(colortype);
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseDCOM(ar);
	}
}

DWORD CFillAlone::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile!=nullptr)
	{
		DWORD size = 0;

		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		pFile->Write(&colortype, sizeof(BYTE));

		size += sizeof(BYTE);

		if(m_pColor!=nullptr)
		{
			size += m_pColor->PackPC(pFile, bytes);
		}
		return size;
	}
	else
	{
		const BYTE colortype = *(BYTE*)bytes;
		bytes += sizeof(BYTE);

		delete m_pColor;
		m_pColor = CColor::Apply(colortype);

		if(m_pColor!=nullptr)
		{
			m_pColor->PackPC(nullptr, bytes);
		}
		return 0;
	}
}

void CFillAlone::ReleaseWeb(CFile& file) const
{
	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseWeb(file);
	}
	else
	{
		const BYTE alpha = 255;
		file.Write(&alpha, sizeof(BYTE));

		const DWORD color = 0XFFFEFEFE;
		file.Write(&color, sizeof(DWORD));
	}
}

void CFillAlone::ReleaseWeb(BinaryStream& stream) const
{
	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseWeb(stream);
	}
	else
	{
		constexpr BYTE alpha = 255;
		stream<<alpha;
		constexpr DWORD color = 0XFFFEFEFE;
		stream<<color;
	}
}
void CFillAlone::ReleaseWeb(boost::json::object& json) const
{
	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(json);
	else
		json["Color"] = "rgba(255,255,255,1)";
}
void CFillAlone::ReleaseWeb(pbf::writer& writer) const
{
	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(writer);
	else
		writer.add_string("rgba(255,255,255,1)");
}
void CFillAlone::ExportPs3(FILE* file, CPsboard& aiKey) const
{
	if(m_bOverprint!=aiKey.O)
	{
		_ftprintf(file, _T("%d O\n"), m_bOverprint);
		aiKey.O = m_bOverprint;
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ExportAIBrush(file, aiKey);
	}
	else if(aiKey.k!=100)
	{
		_ftprintf(file, _T("0 0 0 0 k\n"));
		aiKey.k = 0;
	}
}

void CFillAlone::ExportPdf(HPDF_Page page) const
{
	if(m_pColor!=nullptr)
	{
		m_pColor->ExportPdf(page, true);
	}
}
