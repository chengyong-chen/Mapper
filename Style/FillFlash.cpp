#include "stdafx.h"
#include "Fill.h"
#include "FlashMid.h"
#include "FillFlash.h"
#include "Library.h"
#include "Psboard.h"
#include "../Public/BinaryStream.h"
#include "../Geometry/Geom.h"

#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillFlash, CFillCompact, 0)

CFillFlash::CFillFlash()
{
	m_libId = -1;
	m_symId = -1;
	m_nScale = 100;
	m_nAngle = 0;
}

CFillFlash::~CFillFlash()
{
}
void CFillFlash::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	sha1.process_bytes(&m_libId, sizeof(unsigned short));
	sha1.process_bytes(&m_symId, sizeof(unsigned short));
	sha1.process_bytes(&m_nScale,sizeof(unsigned short));
	sha1.process_bytes(&m_nAngle,sizeof(unsigned short));
}
BOOL CFillFlash::operator==(const CFill& fill) const
{
	if(CFillCompact::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillFlash))==FALSE)
		return FALSE;
	else if(m_libId!=((CFillFlash*)&fill)->m_libId)
		return FALSE;
	else if(m_symId!=((CFillFlash*)&fill)->m_symId)
		return FALSE;
	else if(m_nScale!=((CFillFlash*)&fill)->m_nScale)
		return FALSE;
	else if(m_nAngle!=((CFillFlash*)&fill)->m_nAngle)
		return FALSE;
	else
		return TRUE;
}

void CFillFlash::CopyTo(CFill* fill) const
{
	CFillCompact::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillFlash))==TRUE)
	{
		((CFillFlash*)fill)->m_libId = m_libId;
		((CFillFlash*)fill)->m_symId = m_symId;
		((CFillFlash*)fill)->m_nScale = m_nScale;
		((CFillFlash*)fill)->m_nAngle = m_nAngle;
	}
}

Gdiplus::Brush* CFillFlash::GetBrush(const float& fRatio, const CSize& dpi) const
{
	return nullptr;
}

void CFillFlash::ExportPs3(FILE* file, const CLibrary& library) const
{
}

void CFillFlash::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const
{
	if(pGeom->m_bClosed==false)
		return;

	CFlashMid* pFlash = (CFlashMid*)aiKey.m_library.m_libFillFlash.GetMid(m_libId);
	if(pFlash==nullptr)
		return;
}

void CFillFlash::Decrease(CLibrary& library) const
{
	library.m_libFillFlash.Decrease(m_libId);
}

void CFillFlash::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillCompact::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
	else
	{
		ar>>m_libId;
		ar>>m_symId;
		ar>>m_nScale;
		ar>>m_nAngle;
	}
}

void CFillFlash::ReleaseDCOM(CArchive& ar)
{
	CFillCompact::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
	else
	{
		ar>>m_libId;
		ar>>m_symId;
		ar>>m_nScale;
		ar>>m_nAngle;
	}
}

void CFillFlash::ReleaseCE(CArchive& ar) const
{
	CFillCompact::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
}

DWORD CFillFlash::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CFillCompact::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_libId, sizeof(BYTE));
		pFile->Write(&m_symId, sizeof(BYTE));
		pFile->Write(&m_nScale, sizeof(unsigned short));
		pFile->Write(&m_nAngle, sizeof(unsigned short));

		size += sizeof(BYTE);
		size += sizeof(BYTE);
		size += sizeof(BYTE);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);

		return size;
	}
	else
	{
		m_libId = *(BYTE*)bytes;
		bytes += sizeof(BYTE);
		m_symId = *(BYTE*)bytes;
		bytes += sizeof(BYTE);

		m_nScale = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		m_nAngle = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CFillFlash::ReleaseWeb(CFile& file) const
{
	//	CFillCompact::ReleaseWeb(file);

	file.Write(&m_libId, sizeof(BYTE));
	file.Write(&m_symId, sizeof(BYTE));

	unsigned short nScale = m_nScale;
	ReverseOrder(nScale);
	file.Write(&nScale, sizeof(unsigned short));

	unsigned short nAngle = m_nAngle;
	ReverseOrder(nAngle);
	file.Write(&m_nAngle, sizeof(unsigned short));
}

void CFillFlash::ReleaseWeb(BinaryStream& stream) const
{
	//	CFillCompact::ReleaseWeb(file);

	stream<<m_libId;
	stream<<m_symId;
	const unsigned short nScale = m_nScale;
	stream<<nScale;
	const unsigned short nAngle = m_nAngle;
	stream<<nAngle;
}
void CFillFlash::ReleaseWeb(boost::json::object& json) const
{
	//	CFillCompact::ReleaseWeb(file);
	json["Mid"] = m_libId;
	json["Sym"] = m_symId;
	json["Scale"] = m_nScale;
	json["Angle"] = m_nAngle;
}
void CFillFlash::ReleaseWeb(pbf::writer& writer) const
{
	//	CFillCompact::ReleaseWeb(file);
	writer.add_byte(m_libId);
	writer.add_byte(m_symId);
	writer.add_variant_int16(m_nScale);
	writer.add_variant_int16(m_nAngle);
}