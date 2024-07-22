#pragma once

struct ThreadParam
{
	CString m_strSQL;
	CDatabase* m_pDatabase;
	CRecordset* m_pSet;
};

UINT ExecuteSQLProc(LPVOID lpVoid);
