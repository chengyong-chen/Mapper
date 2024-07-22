#pragma once

#define cxBORDER    6                      // Painting border
#define cyBORDER    2                      // Painting border
typedef UWORD  FAR  *LPUWORD;
typedef SDWORD FAR  *LPSDWORD;
#define AllocPtr(x) GlobalAllocPtr(GHND, (x))
typedef struct tagCOL {                    // Column structure
    char        szName[256];          //   Column name
    SDWORD      cb;                        //   Column width (transfer width)
    SDWORD      cbc;                       //   Column width (display  width)
    SWORD       fCType;                    //   C type
    SWORD       fSqlType;                  //   ODBC Sql type
    LPSDWORD    lpcb;                      //   Pointer to returned width
    LPBYTE      lpb;                       //   Pointer to returned data
} COL, FAR *LPCOL;
 
SWORD CvtSqlToCType(SWORD fSqlType);
BOOL ProcessResults(SQLHSTMT& hstmt,COL*& lpcol,SWORD& nCol);
BOOL __stdcall IsUpdateable(SDWORD fSqlType);
void __stdcall GetCurrentValue(LPSTR lpsz,COL* lpcol);
BOOL __stdcall SetCurrentValue(LPSTR lpsz,COL* lpcol);


#define ISBLANK(x)      ((x) == ' ')
#define ISCOMMA(x)      ((x) == ',')
#define ISNUM(x)        (((x) >= '0') && ((x) <= '9'))
#define ISLPAREN(x)     ((x) == '(')
#define ISRPAREN(x)     ((x) == ')')
#define ISPERIOD(x)     ((x) == '.')
#define ISRETURN(x)     (((x) == '\n') || ((x) == '\r'))
#define ISTAB(x)        ((x) == '\t')
#define ISWHITE(x)      (ISBLANK(x) || ISTAB(x) || ISRETURN(x))
