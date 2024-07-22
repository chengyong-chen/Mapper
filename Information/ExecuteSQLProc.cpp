#include "stdafx.h"
#include "ExecuteSQLProc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

const TCHAR* g_szFunctionSequenceError = _T("State:S1010");

UINT ExecuteSQLProc(LPVOID lpVoid)
{
	ThreadParam* pTP = (ThreadParam*)lpVoid;
	ASSERT(pTP);
	ASSERT(!pTP->m_strSQL.IsEmpty());

	ASSERT(pTP->m_pSet);

	bool bCaughtException = FALSE;

	try
	{
		if(pTP->m_pSet->m_hstmt&&pTP->m_pSet->IsOpen())
			pTP->m_pSet->Close();
		//		pTP->m_pDatabase.SetQueryTimeout(0);
		pTP->m_pSet->Open(CRecordset::forwardOnly, pTP->m_strSQL, CRecordset::executeDirect);
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		bCaughtException = TRUE;

		CString sMsg;
		if(ex->m_strStateNativeOrigin.Find(g_szFunctionSequenceError)!=-1)
		{
			//				pTP->m_pFrame->m_bCanceled = TRUE;
			//				sMsg = "Query canceled by user.";
			//				pTP->m_pFrame->m_strStatusText = _T("Canceled");
		}
		else
		{
			if(!ex->m_strError.IsEmpty())
				sMsg.Format(_T("%s%s"), (LPCTSTR)ex->m_strError, (LPCTSTR)ex->m_strStateNativeOrigin);
			else
				sMsg = ex->m_strStateNativeOrigin;

			//				pTP->m_pFrame->m_strStatusText.Format("CDBException::m_nRetCode == %d", e->m_nRetCode);
		}

		//			pTP->m_pView->SetWindowText(sMsg);

		ex->Delete();
	}
	catch(CMemoryException* ex)
	{
		bCaughtException = TRUE;
		ex->Delete();
		//		pTP->m_pFrame->m_strStatusText = "Out-of-memory";
		//		pTP->m_pView->SetWindowText(pTP->m_pFrame->m_strStatusText);
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	if(bCaughtException)
		if(pTP->m_pSet->IsOpen())
			pTP->m_pSet->Close();

	//	::PostMessage(pTP->m_pFrame->m_hWnd, WM_EXECUTION_COMPLETE, bCaughtException, 0L);

	return 0; // Gracefully exit
}
