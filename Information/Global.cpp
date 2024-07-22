#include "stdafx.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ParseWhere(CString strWhere, CString& strFields, CString& strValues)
{
	if(strWhere.IsEmpty()==TRUE)
		return;

	strWhere.MakeUpper();
	while(strWhere.IsEmpty()==FALSE)
	{
		strWhere.TrimLeft();

		CString Token;
		const int pos = strWhere.Find(_T(" AND "));
		if(pos!=-1)
		{
			Token = strWhere.Left(pos);
			Token.TrimLeft();
			Token.TrimRight();

			strWhere = strWhere.Mid(pos+5);
		}
		else
		{
			Token = strWhere;
			strWhere.Empty();
		}

		if(Token.IsEmpty()==FALSE)
		{
			int len = 1;
			int pos = Token.Find(_T("="));
			if(pos==-1)
			{
				pos = Token.Find(_T("LIKE"));
				len = 4;
			}
			if(pos!=-1)
			{
				CString strField = Token.Left(pos);
				CString strValue = Token.Mid(pos+len);
				strField.TrimRight();
				strValue.TrimLeft();
				if(strField.IsEmpty()==FALSE&&strValue.IsEmpty()==FALSE)
				{
					strFields += _T(',')+strField;
					strValues += _T(',')+strValue;
				}
			}
		}
	}
}
