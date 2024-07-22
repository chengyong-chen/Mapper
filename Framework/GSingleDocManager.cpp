#include "StdAfx.h"
#include "GSingleDocManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(GSingleDocManager, CDocManager)

class CNewTypeDlg : public CDialog
{
protected:
	CPtrList* m_pList; // actually a list of doc templates
public:
	CDocTemplate* m_pSelectedTemplate;

public:
	//{{AFX_DATA(CNewTypeDlg)
	enum
	{
		IDD = AFX_IDD_NEWTYPEDLG
	};

	//}}AFX_DATA
	CNewTypeDlg(CPtrList* pList) : CDialog(CNewTypeDlg::IDD)
	{
		m_pList = pList;
		m_pSelectedTemplate = nullptr;
	}

	~CNewTypeDlg() override
	{
	}

protected:
	BOOL OnInitDialog() override;
	void OnOK() override;
	//{{AFX_MSG(CNewTypeDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CNewTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CNewTypeDlg)
	ON_LBN_DBLCLK(AFX_IDC_LISTBOX, &CNewTypeDlg::OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CNewTypeDlg::OnInitDialog()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);
	ENSURE(pListBox!=nullptr);

	// fill with document templates in list
	pListBox->ResetContent();

	POSITION pos = m_pList->GetHeadPosition();
	// add all the CDocTemplates in the list by name
	while(pos!=nullptr)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)m_pList->GetNext(pos);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CString strTypeName;
		if(pTemplate->GetDocString(strTypeName, CDocTemplate::fileNewName)&&!strTypeName.IsEmpty())
		{
			// add it to the listbox
			const int nIndex = pListBox->AddString(strTypeName);
			if(nIndex==-1)
			{
				EndDialog(-1);
				return FALSE;
			}
			pListBox->SetItemDataPtr(nIndex, pTemplate);
		}
	}

	const int nTemplates = pListBox->GetCount();
	if(nTemplates==0)
	{
		TRACE(traceAppMsg, 0, "Error: no document templates to select from!\n");
		EndDialog(-1); // abort
	}
	else if(nTemplates==1)
	{
		// get the first/only item
		m_pSelectedTemplate = (CDocTemplate*)pListBox->GetItemDataPtr(0);
		ASSERT_VALID(m_pSelectedTemplate);
		ASSERT_KINDOF(CDocTemplate, m_pSelectedTemplate);
		EndDialog(IDOK); // done
	}
	else
	{
		// set selection to the first one (NOT SORTED)
		pListBox->SetCurSel(0);
	}

	return CDialog::OnInitDialog();
}

void CNewTypeDlg::OnOK()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);
	ENSURE(pListBox!=nullptr);
	// if listbox has selection, set the selected template
	int nIndex;
	if((nIndex = pListBox->GetCurSel())==-1)
	{
		// no selection
		m_pSelectedTemplate = nullptr;
	}
	else
	{
		m_pSelectedTemplate = (CDocTemplate*)pListBox->GetItemDataPtr(nIndex);
		ASSERT_VALID(m_pSelectedTemplate);
		ASSERT_KINDOF(CDocTemplate, m_pSelectedTemplate);
	}
	CDialog::OnOK();
}

void GSingleDocManager::OnFileNew()
{
	if(m_templateList.IsEmpty())
	{
		TRACE(traceAppMsg, 0, "Error: no document templates registered with CWinApp.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return;
	}

	CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead();
	if(m_templateList.GetCount()>1)
	{
		// more than one document template to choose from
		// bring up dialog prompting user
		CNewTypeDlg dlg(&m_templateList);
		const INT_PTR nId = dlg.DoModal();
		if(nId==IDOK)
			pTemplate = dlg.m_pSelectedTemplate;
		else
			return; // none - cancel operation
	}

	ASSERT(pTemplate!=nullptr);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CDocManager::SaveAllModified();
	CDocManager::CloseAllDocuments(FALSE);
	pTemplate->OpenDocumentFile(nullptr);
	// if returns nullptr, the user has already been alerted
}

CDocument* GSingleDocManager::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CDocManager::SaveAllModified();
	CDocManager::CloseAllDocuments(FALSE);
	return CDocManager::OpenDocumentFile(lpszFileName, TRUE);
}
