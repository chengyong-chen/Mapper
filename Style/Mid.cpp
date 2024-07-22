#include "stdafx.h"
#include "Mid.h"
#include  <stdio.h>

#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMid, CObject)

CMid::CMid()
{
	m_nUsers = 0;
	m_wId = -1;
}

CMid::~CMid()
{
}

BOOL CMid::Load(CString strFile)
{
	return TRUE;
}

CMid* CMid::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CMid* pClone = (CMid*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	return pClone;
}

void CMid::CopyTo(CMid* pTag) const
{
	pTag->m_wId = m_wId;
	pTag->m_nUsers = m_nUsers;
	pTag->m_strFile = m_strFile;
}

void CMid::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		CString strFile = m_strFile;
		AfxToRelatedPath(ar.m_strFileName, strFile);

		ar<<m_wId;
		ar<<m_nUsers;
		ar<<strFile;
	}
	else
	{
		ar>>m_wId;
		ar>>m_nUsers;

		CString strFile;
		ar>>strFile;
		if(strFile.IsEmpty()==FALSE)
		{
			AfxToFullPath(ar.m_strFileName, strFile);
			m_strFile = strFile;
			if(this->Load(strFile)==FALSE)
			{
				TCHAR CurrentDir[256];
				GetCurrentDirectory(255, CurrentDir);

				CString strExt = m_strFile.Right(m_strFile.GetLength()-m_strFile.ReverseFind(_T('.'))-1);
				CString strFilter;
				strFilter.Format(_T("file(*.%s)|*.%s||"), strExt, strExt);
				CString strTitle = _T("Find: ")+strFile;
				CFileDialog dlg(TRUE,
					strExt,
					m_strFile,
					OFN_HIDEREADONLY,
					strFilter,
					AfxGetMainWnd());
				dlg.m_ofn.lpstrTitle = strTitle;
				if(dlg.DoModal()==IDOK)
				{
					SetCurrentDirectory(CurrentDir);

					CString strFile = dlg.GetPathName();
					if(this->Load(strFile)==TRUE)
					{
						CDocument* pDocument = ar.m_pDocument;
						if(pDocument!=nullptr)
						{
							pDocument->SetModifiedFlag(TRUE);
						}

						m_strFile = strFile;
					}
				}
			}
		}
	}
}

bool CMid::Equals(CMid* pTag) const
{
	if(pTag==nullptr)
		return false;
	else if(pTag->m_strFile!=m_strFile)
		return false;
	else
		return true;
}

void CMid::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		CString strFile = m_strFile;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		strFile.MakeUpper();
		strFile.Replace(_T(".LYM"), _T(".Lce"));
		strFile.Replace(_T(".MYM"), _T(".Mce"));
		strFile.Replace(_T(".FYM"), _T(".Fce"));

		ar<<m_wId;
		SerializeStrCE(ar, strFile);
	}
}

void CMid::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar<<m_wId;
	}
	else
	{
		ar>>m_wId;
	}
}

void CMid::ReleaseWeb(CFile& file) const
{
	unsigned short wId = m_wId;
	ReverseOrder(wId);
	file.Write(&wId, sizeof(unsigned short));
}

void CMid::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_wId;
}
void CMid::ReleaseWeb(boost::json::object& json) const
{
	json["Id"] = m_wId;
}
void CMid::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_wId);
}
