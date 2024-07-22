#include "stdafx.h"
#include "SmartERDoc.h"

#include "../Public/Function.h"
#include <propkey.h>
#include <gdiplus.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSmartERDoc

IMPLEMENT_DYNCREATE(CSmartERDoc, CDocument)

	BEGIN_MESSAGE_MAP(CSmartERDoc, CDocument)
		//{{AFX_MSG_MAP(CSmartERDoc)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	// CSmartERDoc construction/destruction

	CSmartERDoc::CSmartERDoc()
	{
		// TODO: add one-time construction code here
		m_Datainfo.m_mapCanvas = CSizeF(2000,2000);
		m_Datainfo.m_mapOrigin = CPointF(-1000,-1000);

		m_Datainfo.m_scaleMaximum = 0.1;
		m_Datainfo.m_scaleSource = 1.f;
		m_Datainfo.m_scaleMinimum = 10;
		m_Datainfo.m_pProjection = nullptr;
	}

	CSmartERDoc::~CSmartERDoc()
	{
	}

	BOOL CSmartERDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: add reinitialization code here
		// (SDI documents will reuse this document)

		return TRUE;
	}




	// CSmartERDoc serialization

	void CSmartERDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
	{
		if(ar.IsStoring())
		{
			m_Datainfo.Serialize(ar, dwVersion);
		}
		else
		{
			if(m_Datainfo.Serialize(ar) == false)
			{
				AfxMessageBox(_T("Wrong file format!"));
				return;
			}
			else if(AfxGetArVersion() > AfxGetSoftVersion())
			{
				OutputDebugString(this->GetPathName());
				OutputDebugStringA("     ");
				CStringA string;
				string.Format("File version:%0X, Software version:%0X ",AfxGetArVersion(),AfxGetSoftVersion());
				OutputDebugStringA(string);
				AfxMessageBox(_T("Your software is too old!\nPlease install the new version."));
				this->SetModifiedFlag(TRUE);
				return;
			}
		}
	}

	// CSmartERDoc diagnostics

#ifdef _DEBUG
	void CSmartERDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}

	void CSmartERDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
#endif //_DEBUG

void CSmartERDoc::Draw(CView* pView,CViewinfo& viewinfo,Graphics& g,CRect& inRect)
{
	
}