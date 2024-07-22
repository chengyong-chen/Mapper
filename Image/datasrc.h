#pragma once

class IProgressNotification;

#pragma once

class CDataSource : public CObject
{
public:

	//!
	CDataSource(IProgressNotification* pNotification = nullptr);

	//!
	~CDataSource() override;

	//!
	virtual void Open(LPCTSTR pszName, int FileSize);

	//!
	virtual void Close();

	//!
	TCHAR* GetName() const;

	//! Read but don't advance file pointer.
	virtual BYTE* GetBufferPtr(int MinBytesInBuffer) = 0;

	//! This needs to be overridden in derived classes.
	virtual BYTE* ReadNBytes(int n);

	//!
	int GetFileSize() const;

	//! This is a legacy routine that interferes with progress notifications.
	//! Don't call it!
	virtual BYTE* ReadEverything() = 0;

	//!
	BYTE* Read1Byte();

	//!
	BYTE* Read2Bytes();

	//!
	BYTE* Read4Bytes();

	// JH 15.4.99:
	//! handles progress notification from other libs
	void OProgressNotification(double part) const;

	//!
	void AlignToWord();

	//!
	void Skip(int n);

	//! Test to see if we didn't go past the end of the file
	void CheckEOF() const;

protected:

private:
	TCHAR* m_pszName; // Name of the data source for diagnostic
	// purposes.
	int m_FileSize;
	int m_BytesRead;
	BOOL m_bSrcLSBFirst; // Source byte order: TRUE for intel order,
	// FALSE for Motorola et al. (MSB first).
	IProgressNotification* m_pNotification;
};

inline int CDataSource::GetFileSize() const
{
	return m_FileSize;
}

inline BYTE* CDataSource::Read1Byte()
{
	return ReadNBytes(1);
}

inline BYTE* CDataSource::Read2Bytes()
{
	return ReadNBytes(2);
}

inline BYTE* CDataSource::Read4Bytes()
{
	return ReadNBytes(4);
}

inline void CDataSource::AlignToWord()
{
	if(m_BytesRead&1)
		Read1Byte();
}

inline void CDataSource::Skip(int n)
{
	ReadNBytes(n);
}

inline void CDataSource::CheckEOF() const
{
	if(m_FileSize<m_BytesRead)
	{
		//   throw CTextException (ERR_END_OF_FILE,"End of file reached while decoding.\n");
		AfxMessageBox(_T("告诉陈成永，这里有个错误!"));
	}
}
