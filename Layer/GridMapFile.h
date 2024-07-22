#pragma once

class CGeomMapFile;

class AFX_EXT_CLASS CGridMapFile : public CObject
{
public:
	struct HEAD
	{
		BYTE bFlag[8];
		DWORD wLayerCount;
		DWORD dwWidth;
		DWORD dwHeight;
		DWORD xCount;
		DWORD yCount;
	};

	struct LAYER
	{
		WORD wId;
		DWORD dwGridOffset;
	};

	struct GRID
	{
		WORD wGeomCount;
		DWORD dwOffset;
	};

public:
	CGridMapFile();

public:
	~CGridMapFile() override;

public:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_hFileMapView;

	bool Open(CString strFile);
	void Close();
public:
	CMap<WORD, WORD&, WORD, WORD&> m_LayerIdMap;

public:
	HEAD m_Head;
public:
	WORD GetLayerCount() const;
	int GetLayerIndex(WORD wLayerID) const;
	GRID* GetGrid(const WORD& wLayerIndex, const short& yGrid, const short& xGrid) const;
	DWORD* GetGridGeomList(CGridMapFile::GRID* pLayerGrid) const;
};
