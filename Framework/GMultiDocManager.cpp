#include "StdAfx.h"
#include "GMultiDocManager.h"
#include "GMultiDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AFX_STATIC void AFXAPI AfxAppendFilterSuffix(CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ENSURE_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;
	if(pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)&&
		!strFilterExt.IsEmpty()&&
		pTemplate->GetDocString(strFilterName, CDocTemplate::filterName)&&
		!strFilterName.IsEmpty())
	{
		if(pstrDefaultExt!=nullptr)
			pstrDefaultExt->Empty();

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty()); // must have a file type name
		filter += (TCHAR)'\0'; // next string please

		int iStart = 0;
		do
		{
			CString strExtension = strFilterExt.Tokenize(_T(";"), iStart);

			if(iStart!=-1)
			{
				// a file based document template - add to filter list

				// If you hit the following ASSERT, your document template 
				// string is formatted incorrectly.  The section of your 
				// document template string that specifies the allowable file
				// extensions should be formatted as follows:
				//    .<ext1>;.<ext2>;.<ext3>
				// Extensions may contain wildcards (e.g. '?', '*'), but must
				// begin with a '.' and be separated from one another by a ';'.
				// Example:
				//    .jpg;.jpeg
				ASSERT(strExtension[0]=='.');
				if((pstrDefaultExt!=nullptr)&&pstrDefaultExt->IsEmpty())
				{
					// set the default extension
					*pstrDefaultExt = strExtension.Mid(1); // skip the '.'
					ofn.lpstrDefExt = const_cast<LPTSTR>((LPCTSTR)(*pstrDefaultExt));
					ofn.nFilterIndex = ofn.nMaxCustFilter+1; // 1 based number
				}

				filter += (TCHAR)'*';
				filter += strExtension;
				filter += (TCHAR)';'; // Always append a ';'.  The last ';' will get replaced with a '\0' later.
			}
		} while(iStart!=-1);

		filter.SetAt(filter.GetLength()-1, '\0');; // Replace the last ';' with a '\0'
		ofn.nMaxCustFilter++;
	}
}

IMPLEMENT_DYNAMIC(GMultiDocManager, CDocManager)

GMultiDocManager::GMultiDocManager(void)
{
}

GMultiDocManager::~GMultiDocManager(void)
{
}

BOOL GMultiDocManager::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
	CFileDialog dlgFile(bOpenFileDialog, nullptr, nullptr, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, nullptr, nullptr, 0);

	CString title;
	ENSURE(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0'; // next string please
	strFilter += _T("*.Grf;*.Gis;*.Geo");
	strFilter += (TCHAR)'\0'; // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	CString strDefault;

	if(pTemplate!=nullptr)
	{
		ASSERT_VALID(pTemplate);
		AfxAppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
	}
	else// do for all doc template
	{
		POSITION pos = m_templateList.GetHeadPosition();
		BOOL bFirst = FALSE;
		while(pos!=nullptr)
		{
			pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
			AfxAppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, bFirst ? &strDefault : nullptr);
			bFirst = FALSE;
		}
	}

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	return nResult==IDOK;
}

void GMultiDocManager::CloseAllDocumentsWithSaveModified(BOOL bEndSession)
{
	POSITION pos = m_templateList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
		ASSERT_KINDOF(CDocTemplate, pTemplate);
		if(pTemplate->IsKindOf(RUNTIME_CLASS(GMultiDocTemplate))==TRUE)
		{
			((GMultiDocTemplate*)pTemplate)->CloseAllDocumentsWithSaveModified(bEndSession);
		}
		else
			pTemplate->CloseAllDocuments(TRUE);
	}
}