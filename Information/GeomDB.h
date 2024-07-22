#pragma once

#include "ODBCDatabase.h"


class __declspec(dllexport) CGeomDB : public CODBCDatabase
{
protected:
	DECLARE_SERIAL(CGeomDB);
	CGeomDB();
   virtual ~CGeomDB();	

public:
	void   ShowRecord(CString strTable,DWORD dwId);
	void   UpdateRecord(CString strSQL);
};

typedef CTypedPtrList<CObList, CGeomDB*> CGeomDBList;
