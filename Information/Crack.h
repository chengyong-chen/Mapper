#pragma once

class __declspec(dllexport) CCrack
{
public:
	static LPCTSTR strFieldType(short sType);
	static LPCTSTR strFieldType0(short sType);
	static LPCTSTR strQueryDefType(short sType);
	static LPCTSTR strBOOL(BOOL bFlag);
	static CString strVARIANT(const COleVariant& var);
	static CString strFieldAttribute(long lAttribute);
	static void SetDaoFieldInfoEmpty(CDaoFieldInfo& fieldInfo);
	static BOOL AnalysisDateTime(COleDateTime& dt, LPCSTR lpsztext);
};
