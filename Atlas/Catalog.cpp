#include "stdafx.h"
#include "Catalog.h"
#include "CatalogTree.h"
#include "../Information/POUListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_SERIAL(CCatalog, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CCatalog

CCatalog::CCatalog()
{
}

CCatalog::~CCatalog()
{
	Release();
}

void CCatalog::Create(CPage* pParent, CString strFold)
{
	CPage* pFold = new CPage();
	pFold->m_wId = this->GetMaxItemId()+1;
	pFold->m_wType = CPage::typeFolder;
	pFold->m_wParent = pParent==nullptr ? 0 : pParent->m_wId;

	m_PageList.AddTail(pFold);

	strFold.TrimRight('\\');
	pFold->m_strName = strFold.Mid(strFold.ReverseFind(_T('\\'))+1);

	WIN32_FIND_DATA FindDate1;
	const HANDLE hFile1 = FindFirstFile(strFold+_T("\\*.*"), &FindDate1);
	do
	{
		if((FindDate1.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)
			continue;

		CString strFile = FindDate1.cFileName;
		strFile.MakeUpper();
		if(strFile.Right(4)==_T(".GIS")||strFile.Right(4)==_T(".Grf"))
		{
			CPage* pPage = new CPage();
			pPage->m_wId = this->GetMaxItemId()+1;
			pPage->m_wType = CPage::typeMap;
			pPage->m_wParent = pFold->m_wId;

			m_PageList.AddTail(pPage);

			CString strName = strFile.Mid(strFile.ReverseFind(_T('\\'))+1);
			strName = strName.Left(strName.GetLength()-4);
			pPage->m_strName = strName;
			pPage->m_strTarget = strFold+_T('\\')+FindDate1.cFileName;;
		}
	} while(FindNextFile(hFile1, &FindDate1)==TRUE);

	WIN32_FIND_DATA FindDate2;
	const HANDLE hFile2 = FindFirstFile(strFold+_T("\\*.*"), &FindDate2);
	do
	{
		if((FindDate2.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
			continue;

		if(_tcscmp(FindDate2.cFileName, _T("."))==0||_tcscmp(FindDate2.cFileName, _T(".."))==0)
			continue;

		const CString subFold = strFold+_T('\\')+FindDate2.cFileName;
		Create(pFold, subFold);
	} while(FindNextFile(hFile2, &FindDate2)==TRUE);
}

void CCatalog::Filter()
{
	POSITION pos1 = m_PageList.GetHeadPosition();
	while(pos1!=nullptr)
	{
		const CPage* pPage1 = m_PageList.GetAt(pos1);
		if(pPage1->m_wType==CPage::typeFolder)
		{
			int childMap = 0;

			POSITION pos2 = pos1;
			m_PageList.GetNext(pos2);
			while(pos2!=nullptr)
			{
				CPage* pPage2 = m_PageList.GetNext(pos2);
				if(pPage2->m_wParent==pPage1->m_wId&&pPage2->m_wType!=CPage::typeFolder)
					childMap++;
			}

			if(childMap==0)
			{
				POSITION pos3 = pos1;
				m_PageList.GetNext(pos3);
				while(pos3!=nullptr)
				{
					const CPage* pPage3 = m_PageList.GetAt(pos3);
					if(pPage3->m_wParent==pPage1->m_wId&&pPage3->m_wType==CPage::typeFolder)
					{
						POSITION pos4 = pos3;
						m_PageList.GetNext(pos4);
						while(pos4!=nullptr)
						{
							CPage* pPage4 = m_PageList.GetNext(pos4);
							if(pPage4->m_wParent==pPage3->m_wId)
								pPage4->m_wParent = pPage1->m_wId;
						}
						const POSITION del = pos3;
						m_PageList.GetNext(pos3);

						delete pPage3;
						m_PageList.RemoveAt(del);
					}
					else
					{
						m_PageList.GetNext(pos3);
					}
				}
			}
		}

		m_PageList.GetNext(pos1);
	}
}

DWORD CCatalog::GetMaxItemId()
{
	WORD wMaxId = 0;
	POSITION pos = m_PageList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CTreeNode1* node = m_PageList.GetNext(pos);
		if(node->m_wId>wMaxId)
		{
			wMaxId = node->m_wId;
		}
	}

	return wMaxId;
}

void CCatalog::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
	}
	else
	{
	}

	m_PageList.Serialize(ar);
}

void CCatalog::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
	}
	const unsigned short nCount = m_PageList.GetCount();

	ar<<nCount;

	POSITION pos = m_PageList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPage* pPage = m_PageList.GetNext(pos);
		pPage->ReleaseCE(ar);
	}
}

void CCatalog::SerializeWEB(CArchive& ar) const
{
}

void CCatalog::PublishPC()
{
}

void CCatalog::PublishCE()
{
}

void CCatalog::PublishWEB()
{
}

void CCatalog::Release()
{
	POSITION pos = m_PageList.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CPage* pPage = m_PageList.GetNext(pos);
		delete pPage;
	}
	m_PageList.RemoveAll();
}

CPage* CCatalog::GetPageByMapName(CString strMapName)
{
	POSITION pos = m_PageList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPage* pPage = m_PageList.GetNext(pos);
		CString strName = pPage->m_strName;
		if(strName.IsEmpty()==TRUE)
			continue;

		if(strMapName.CompareNoCase(strName)==0)
		{
			return pPage;
		}
		else if(strMapName==strName)
		{
			return pPage;
		}
	}

	return nullptr;
}

CPage* CCatalog::GetPage(WORD wMapId)
{
	POSITION pos = m_PageList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPage* pPage = m_PageList.GetNext(pos);
		if(pPage->m_wId==wMapId)
		{
			return pPage;
		}
	}

	return nullptr;
};

CPage* CCatalog::GetPageByMapFile(CString strMap)
{
	POSITION pos = m_PageList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPage* pPage = m_PageList.GetNext(pos);
		CString strFile = pPage->m_strTarget;
		if(strFile.IsEmpty()==TRUE)
			continue;

		if(strMap.CompareNoCase(strFile)==0)
		{
			return pPage;
		}
	}

	return nullptr;
}
