#include "stdafx.h"
#include "Cursor.h"

int         g_cx; 
int         g_cy;     

SWORD CvtSqlToCType(SWORD fSqlType)
{
   switch (fSqlType) {
     case SQL_CHAR:
     case SQL_VARCHAR:
     case SQL_LONGVARCHAR:
     case SQL_DECIMAL:
     case SQL_NUMERIC:
     case SQL_BIGINT:           
		 return SQL_C_CHAR;
     case SQL_BIT:              return SQL_C_BIT;
     case SQL_TINYINT:
     case SQL_SMALLINT:         return SQL_C_SHORT;
     case SQL_INTEGER:          return SQL_C_LONG;
     case SQL_REAL:             return SQL_C_FLOAT;

     case SQL_FLOAT:
     case SQL_DOUBLE:           return SQL_C_DOUBLE;

     case SQL_BINARY:
     case SQL_VARBINARY:
     case SQL_LONGVARBINARY:    return SQL_C_BINARY;

     case SQL_TYPE_DATE:        return SQL_C_TYPE_DATE;
     case SQL_TYPE_TIME:        return SQL_C_TYPE_TIME;
     case SQL_TYPE_TIMESTAMP:   return SQL_C_TYPE_TIMESTAMP;

     case SQL_DATE:             return SQL_C_DATE;
     case SQL_TIME:             return SQL_C_TIME;
     case SQL_TIMESTAMP:        return SQL_C_TIMESTAMP;
   }

   return SQL_C_CHAR;
}


BOOL ProcessResults(SQLHSTMT& hstmt,COL*& lpcol,SWORD& nCol)
{		
	SQLRETURN   retcode;
   
	retcode = SQLNumResultCols(hstmt, &nCol);
	lpcol = new COL[nCol];

	for(int i=0;i < nCol; i++) 
	{  
		lpcol[i].lpb  = nullptr;
		lpcol[i].lpcb = nullptr;
	}
   
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_BIND_TYPE,(SQLPOINTER)SQL_BIND_BY_COLUMN,SQL_IS_INTEGER);
	for (i=0; i < nCol; i++) 
	{
		retcode = SQLColAttribute(hstmt,  i+1,SQL_DESC_NAME,         lpcol[i].szName, sizeof(lpcol[i].szName), nullptr, nullptr);
		retcode = SQLColAttributes(hstmt, i+1,SQL_COLUMN_LENGTH,     nullptr,                                  0, nullptr, &lpcol[i].cb);
		retcode = SQLColAttribute(hstmt,  i+1,SQL_DESC_DISPLAY_SIZE, nullptr,                                  0, nullptr, &lpcol[i].cbc);
		retcode = SQLColAttribute(hstmt,  i+1,SQL_DESC_CONCISE_TYPE, nullptr,                                  0, nullptr, (SDWORD FAR*) &lpcol[i].fSqlType);
		
		lpcol[i].cbc++;      
	  
		lpcol[i].fCType = CvtSqlToCType(lpcol[i].fSqlType);

      // For hard to handle C types, let the driver convert to character
		if(lpcol[i].fCType == SQL_C_BIT    ||
			lpcol[i].fCType == SQL_C_BINARY ||
			lpcol[i].fCType == SQL_C_DATE   ||
			lpcol[i].fCType == SQL_C_TIME   ||
			lpcol[i].fCType == SQL_C_TIMESTAMP ||
			lpcol[i].fCType == SQL_C_TYPE_DATE   ||
			lpcol[i].fCType == SQL_C_TYPE_TIME   ||
			lpcol[i].fCType == SQL_C_TYPE_TIMESTAMP) 
		{
			lpcol[i].fCType = SQL_C_CHAR;
			lpcol[i].cb     = lpcol[i].cbc;
		}

		if(lpcol[i].fCType == SQL_C_CHAR)
			lpcol[i].cb++;

		lpcol[i].lpb  = new BYTE[lpcol[i].cb];
		lpcol[i].lpcb = new SDWORD[1];
  
		retcode = SQLBindCol(hstmt, i+1, (SWORD)lpcol[i].fCType,(lpcol[i].lpb),lpcol[i].cb, (lpcol[i].lpcb));   
	}
      
	UDWORD    crow;
	UWORD  fStatus;
    
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_ROWS_FETCHED_PTR,&crow, SQL_IS_POINTER);
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_STATUS_PTR,&fStatus, SQL_IS_POINTER);
	return TRUE;
}

BOOL __stdcall IsUpdateable(SDWORD fSqlType)
{
   switch (fSqlType) {
     case SQL_CHAR:
     case SQL_VARCHAR:
     case SQL_SMALLINT:
     case SQL_INTEGER:
     case SQL_REAL:
     case SQL_FLOAT:
     case SQL_DOUBLE:
     case SQL_TINYINT:
      return TRUE;

     default:
      return FALSE;
   }
}


void __stdcall GetCurrentValue(LPSTR lpsz,COL* lpcol)
{
   LPBYTE      lpb;
   SDWORD FAR  *lpcb;
  
   // Get data and count field pointers based on binding type
    
   lpb  = lpcol->lpb;   
   lpcb = (LPSDWORD)(lpb + lpcol->cb);
   
   // Convert column data to character using the supplied buffer
   if(*lpcb == SQL_NULL_DATA) 
   {
      lstrcpy(LPTSTR(lpsz), _T("<null>"));
      return;
   }

   switch (lpcol->fSqlType) 
   {
     case SQL_CHAR:
     case SQL_VARCHAR:
      lstrcpy(LPTSTR(lpsz), (LPCTSTR)lpb);
      break;

     case SQL_INTEGER:
     case SQL_SMALLINT:
     case SQL_TINYINT: 
		 {
        long  l;

        l = (lpcol->fSqlType == SQL_INTEGER
             ? *((DWORD FAR *)lpb)
             : lpcol->fSqlType == SQL_SMALLINT
             ? *((WORD FAR *)lpb)
             : *((UCHAR FAR *)lpb));

        ltoa(l, lpsz, 10);
        break;
     }

     case SQL_REAL:
     case SQL_FLOAT:
     case SQL_DOUBLE: 
		 {
        double   d;

        d = (lpcol->fSqlType == SQL_REAL
             ? *((float FAR *)lpb)
             : *((double FAR *)lpb));

        _gcvt(d, 15, lpsz);
        break;
     }

     default:
      *lpsz = '\0';
      break;
   }

   return;
}

BOOL __stdcall SetCurrentValue(LPSTR lpsz,COL* lpcol)
{
   LPBYTE   lpb;
   LPSDWORD lpcb;
   BOOL     fNew;
//   UWORD    irowPos;

   // Get data and count field pointers based on binding type
      
   lpb  = lpcol->lpb;
   lpcb = (LPSDWORD)(lpb + lpcol->cb);

   // If the data is nullptr, just set the count field to SQL_NULL_DATA
   if(!lstrcmpi((LPCTSTR)lpsz, _T("<null>"))) 
   {
      if(*lpcb != SQL_NULL_DATA) 
	  {
         *lpcb = SQL_NULL_DATA;
         fNew  = TRUE;
      }
   }

   // Otherwise, convert the character data back to the appropriate type
   else switch (lpcol->fSqlType) 
   {
     case SQL_CHAR:
     case SQL_VARCHAR:
      if(lstrcmp((LPCTSTR)lpsz, (LPTSTR)lpb)) {
         lstrcpy((LPTSTR)lpb, (LPTSTR)lpsz);
         *lpcb = lstrlen((LPCTSTR)lpsz);
         fNew  = TRUE;
      }
      break;

     case SQL_INTEGER:
     case SQL_SMALLINT:
     case SQL_TINYINT: {
        long  lNew, lCur;
        char* EndPtr;

        lNew = strtol(lpsz, &EndPtr, 10);
        for (; *EndPtr && ISWHITE(*EndPtr); EndPtr = AnsiNext(EndPtr));
        if(*EndPtr)  
		{ 
           AfxMessageBox(_T("gfddfdf"));
           fNew = FALSE;
           break;
        }

        lCur = (lpcol->fSqlType == SQL_INTEGER
                ? *((DWORD FAR *)lpb)
                : lpcol->fSqlType == SQL_SMALLINT
                ? *((WORD FAR *)lpb)
                : *((UCHAR FAR *)lpb));

        if(lNew != lCur) {

           switch (lpcol->fSqlType) {
             case SQL_INTEGER:
              *((DWORD FAR *)lpb) = lNew;
              *lpcb = sizeof(DWORD);
              break;

             case SQL_SMALLINT:
              *((WORD FAR *)lpb) = (WORD)lNew;
              *lpcb = sizeof(WORD);
              break;

             case SQL_TINYINT:
              *((UCHAR FAR *)lpb) = (UCHAR)lNew;
              *lpcb = sizeof(UCHAR);
              break;
           }

           fNew = TRUE;
        }
        break;
     }

     case SQL_REAL:
     case SQL_FLOAT:
     case SQL_DOUBLE: {
        double   dNew, dCur;
        char* EndPtr;

        dNew = strtod(lpsz, &EndPtr);
        for (; *EndPtr && ISWHITE(*EndPtr); EndPtr = AnsiNext(EndPtr));
        if(*EndPtr)  { // check to see if there exists non-numeric chars
//           UCHAR szBuffer[128];

           AfxMessageBox(_T("gdfgdg"));
           fNew = FALSE;
           break;
        }

        dCur = (lpcol->fSqlType == SQL_REAL
                ? *((float FAR *)lpb)
                : *((double FAR *)lpb));

        if(dNew != dCur) {

           switch (lpcol->fSqlType) {
             case SQL_REAL:
              *((float FAR *)lpb) = (float)dNew;
              *lpcb = sizeof(float);
              break;

             case SQL_FLOAT:
             case SQL_DOUBLE:
              *((double FAR *)lpb) = dNew;
              *lpcb = sizeof(double);
              break;
           }

           fNew = TRUE;
        }
        break;
     }
   }
   return fNew;
}
