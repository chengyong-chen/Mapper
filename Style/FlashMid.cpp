#include "stdafx.h"

#include  <io.h>
#include  <stdio.h>

#include "FlashMid.h"
#include "../Public/BinaryStream.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFlashMid, CMid, 0)

CFlashMid::CFlashMid()
{
	m_nCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
}

CFlashMid::CFlashMid(CString strFile)
{
	m_strFile = strFile;
}

CFlashMid::~CFlashMid()
{
}

void CFlashMid::CopyTo(CMid* pMid) const
{
	CMid::CopyTo(pMid);

	if(pMid->IsKindOf(RUNTIME_CLASS(CFlashMid))==TRUE)
	{
		((CFlashMid*)pMid)->m_nCount = m_nCount;
		((CFlashMid*)pMid)->m_nWidth = m_nWidth;
		((CFlashMid*)pMid)->m_nHeight = m_nHeight;
		((CFlashMid*)pMid)->m_strFile = m_strFile;
	}
}

CSize CFlashMid::GetSize() const
{
	return CSize(m_nWidth, m_nHeight);
}

void CFlashMid::Draw(CDC* pDC, Gdiplus::Point point) const
{
}

void CFlashMid::Draw(Gdiplus::Graphics& g, float fScale) const
{
}

BOOL CFlashMid::Load(CString strPathName)
{
	if(_taccess(strPathName, 00)==-1)
		return FALSE;

	return FALSE;
}

void CFlashMid::ReleaseCE(CArchive& ar) const
{
	CMid::ReleaseCE(ar);
}

void CFlashMid::ReleaseDCOM(CArchive& ar)
{
	CMid::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_nCount;
		ar<<m_nHeight;
	}
	else
	{
		ar>>m_nCount;
		ar>>m_nHeight;
	}
}

void CFlashMid::ReleaseWeb(CFile& file) const
{
	CMid::ReleaseWeb(file);
}

void CFlashMid::ReleaseWeb(BinaryStream& stream) const
{
	CMid::ReleaseWeb(stream);
}
void CFlashMid::ReleaseWeb(boost::json::object& json) const
{
	CMid::ReleaseWeb(json);
}
void CFlashMid::ReleaseWeb(pbf::writer& writer) const
{
	CMid::ReleaseWeb(writer);
}
