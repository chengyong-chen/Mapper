#include "stdafx.h"
#include "TreeNode.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CTreeNode1, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CTreeNode1

CTreeNode1::CTreeNode1()
{
	m_wId = 0;
	m_wParent = 0;
	m_wType = 0;
}

CTreeNode1::~CTreeNode1()
{
}

void CTreeNode1::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_wId;
		ar << m_wParent;
		ar << m_strName;
		ar << m_wType;
	}
	else
	{
		ar >> m_wId;
		ar >> m_wParent;
		ar >> m_strName;
		ar >> m_wType;
	}
}

void CTreeNode1::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar<<m_wType;
		ar<<m_wId;
		ar<<m_wParent;

		SerializeStrCE(ar, m_strName);
	}
}

void CTreeNode1::SerializeWEB(CArchive& ar) const
{
}
