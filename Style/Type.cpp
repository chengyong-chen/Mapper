#include "stdafx.h"

#include "Type.h"
#include "TypeDlg.h"
#include "Color.h"
#include "Psboard.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CType CType::m_default;

CType::CType()
{
	m_fSize = 12;
}

CType::~CType()
{
}
void CType::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CHint::Sha1(sha1);

	m_deform.Sha1(sha1);
}
BOOL CType::operator==(const CHint& hint) const
{
	if(CHint::operator==(hint)==FALSE)
		return FALSE;
	else if(hint.IsKindOf(RUNTIME_CLASS(CType))==FALSE)
		return FALSE;
	else if(m_deform!=((CType&)hint).m_deform)
		return FALSE;
	else
		return TRUE;
}

CType* CType::Clone() const
{
	CType* pClone = new CType();
	this->CopyTo(pClone);
	return pClone;
}

void CType::CopyTo(CHint* pHint) const
{
	CHint::CopyTo(pHint);

	if(pHint->IsKindOf(RUNTIME_CLASS(CType))==TRUE)
	{
		CType* pType = (CType*)pHint;
		pType->m_deform = m_deform;
	}
}

bool CType::Setup()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CType* pNew = this->Clone();
	CTypeDlg dlg(nullptr, *pNew);
	if(dlg.DoModal()==IDOK)
	{
		*this = *pNew;
		delete pNew;
		return true;
	}
	else
	{
		delete pNew;
		return false;
	}
}

void CType::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CHint::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_deform;
	}
	else
	{
		ar>>m_deform;
	}
}

void CType::ReleaseCE(CArchive& ar) const
{
	SerializeStrCE(ar, m_fontdesc.GetRealName());
	ar<<m_fSize;
	ar<<m_filter;
	ar<<m_frame;
}

void CType::ReleaseDCOM(CArchive& ar)
{
	m_fontdesc.ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_fSize;
		ar<<m_deform;
		ar<<m_filter;
		ar<<m_frame;
	}
	else
	{
		ar>>m_fSize;
		ar>>m_deform;
		ar>>m_filter;
		ar>>m_frame;
	}
}

DWORD CType::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile!=nullptr)
	{
		DWORD size = m_fontdesc.PackPC(pFile, bytes);

		pFile->Write(&m_fSize, sizeof(float));
		pFile->Write(&m_deform, sizeof(TextDeform));

		size += sizeof(WORD);
		size += sizeof(TextDeform);

		pFile->Write(&m_filter, sizeof(BYTE));
		size += sizeof(BYTE);
		pFile->Write(&m_frame, sizeof(Frame));
		size += sizeof(Frame);
		return size;
	}
	else
	{
		m_fontdesc.PackPC(nullptr, bytes);

		bytes += sizeof(WORD);

		m_fSize = *(float*)bytes;
		bytes += sizeof(unsigned short);

		m_deform = *(TextDeform*)bytes;
		bytes += sizeof(TextDeform);
		return 0;
	}
}

void CType::ReleaseWeb(CFile& file) const
{
	CHint::ReleaseWeb(file);

	m_deform.ReleaseWeb(file);
}

void CType::ReleaseWeb(BinaryStream& stream) const
{
	CHint::ReleaseWeb(stream);

	m_deform.ReleaseWeb(stream);
}
void CType::ReleaseWeb(boost::json::object& json) const
{
	CHint::ReleaseWeb(json);

	m_deform.ReleaseWeb(json);
}
void CType::ReleaseWeb(pbf::writer& writer) const
{
	CHint::ReleaseWeb(writer);

	m_deform.ReleaseWeb(writer);
}
void CType::ExportPs3(FILE* file, CPsboard& aiKey) const
{
	const CStringA strFont = m_fontdesc.GetRealName();
	if(m_pColor!=nullptr)
	{
		m_pColor->ExportAIBrush(file, aiKey);
	}
	else if(aiKey.K!=100)
	{
		_ftprintf(file, _T("0 0 0 1 k\n"));
		aiKey.k = 100;
	}

	if(aiKey.strFont!=strFont||aiKey.sizeFont!=m_fSize)
	{
		const float nDescent = this->GetDescent(1.0f, 72);
		const float nAscent = this->GetAscent(1.0f, 72);

		_ftprintf(file, _T("0 To\n"));
		_ftprintf(file, _T("1 0 0 1 34.4756 256.5342 0 Tp\n"));
		_ftprintf(file, _T("0 Tv\n"));
		_ftprintf(file, _T("TP\n"));
		_ftprintf(file, _T("0 Tr\n"));

		_ftprintf(file, _T("/_%s %f %g -%g Tf\n"), strFont, m_fSize, nAscent, nDescent);
		_ftprintf(file, _T("%d 100 Tz\n"), m_deform.m_nHorzscale);
		_ftprintf(file, _T("0 Tt\n"));
		_ftprintf(file, _T("%%AI55J_GlyphSubst: GlyphSubstNone \n"));
		_ftprintf(file, _T("%%_ 0 XL\n"));
		_ftprintf(file, _T("0 0 Tl\n"));
		_ftprintf(file, _T("0 Tc\n"));
		_ftprintf(file, _T("() Tx 1 0 Tk\n"));
		_ftprintf(file, _T("TO\n"));

		aiKey.strFont = strFont;
		aiKey.sizeFont = m_fSize;
	}
}

void CType::ExportPdf(HPDF_Doc pdf, HPDF_Page page) const
{
	CHint::ExportPdf(pdf, page);
}

CType* CType::Default(float size, Gdiplus::FontStyle style)
{
	m_default.m_fSize = size;
	m_default.m_fontdesc.SetStyle(style);
	return &m_default;
}
