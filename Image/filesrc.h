#ifndef INCL_FILESRC
#define INCL_FILESRC

#ifndef INCL_DATASRC
#include "datasrc.h"
#endif

#ifdef _WINDOWS
#define FILE_MAPPING
#endif

class IProgressNotification;

//! This is a class which takes a file as a source of picture data.
class CFileSource : public CDataSource
{
public:
	//!
	CFileSource(IProgressNotification* pNotification = nullptr);

	//!
	~CFileSource() override;

	//!
	virtual int Open(LPCTSTR pszFName);

	//!
	void Close() override;

	BYTE* ReadNBytes(int n) override;

	//! Read but don't advance file pointer.
	BYTE* GetBufferPtr(int MinBytesInBuffer) override;

	BYTE* ReadEverything() override;

private:
#ifdef FILE_MAPPING
	HANDLE m_hf; // File handle.
	HANDLE m_hm; // Handle to file-mapping object.

#else
	BOOL bytesAvailable(int n);

	void fillBuffer();

	FILE* m_pFile;
	BYTE* m_pBuffer;

	BYTE* m_pReadPos;
	int m_BytesReadFromFile;
#endif
	BYTE* m_pStartData;
	BYTE* m_pCurPos;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log$
|
--------------------------------------------------------------------
*/
