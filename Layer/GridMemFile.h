#pragma once

class CGeomMapFile;

class AFX_EXT_CLASS CGridMemFile : public CObject
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
	CGridMemFile();

public:
	~CGridMemFile() override;

public:
	HEAD m_Head;
	BYTE* m_pData;

	bool Open(CString strFile);
	void Close();
public:
	CMap<WORD, WORD&, WORD, WORD&> m_LayerIdMap;

public:
	int GetLayerIndex(WORD wLayerID) const;
	GRID* GetGrid(const WORD& wLayerIndex, const short& yGrid, const short& xGrid) const;
	DWORD* GetGridGeomList(CGridMemFile::GRID* pLayerGrid) const;
};
