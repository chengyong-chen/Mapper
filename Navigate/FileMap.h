#pragma once

class __declspec(dllexport) CFileMap : public CObject
{
	public:
	CFileMap();

	public:
	~CFileMap() override;

	public:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_hFileMapView;

	bool Open(CString strFile);
	void Close();
};
