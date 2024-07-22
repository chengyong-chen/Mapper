#pragma once

typedef struct tagFILEENTRY
{
	TCHAR  szFileName[0x18];
    DWORD  Plate;
	DWORD  dwPosition;
	DWORD  dwLength;
} FILEENTRY;

typedef struct tagPACKHEADER
{
	DWORD     dwFileMark;
	DWORD     dwTotalFile;
	DWORD     Reservel;
	DWORD     Reserve2;
	FILEENTRY feFileList[1];
} PACKHEADER;	
	


class AFX_EXT_CLASS  CResFile : public CFile
{   
public:       
	CResFile();          

public: 
	PACKHEADER* m_PackHeader;
	long    m_nIndex;
    DWORD  CurrentPosition;

public: 
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = nullptr) override;
	virtual void Close() override;

public:
	 bool OpenFile(CString strFile,UINT nFlags,CFileException* pError=nullptr);
	 void CloseFile();

	 virtual UINT  Read(void* lpBuf,UINT nCount);
};