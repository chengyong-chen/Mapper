#pragma once

class CGeom;
class CMark;
class CPoly;
class CBezier;
class CEllipse;
class CImager;
class CDoughnut;
class CPRect;
class CRRect;
class CText;
class CTextPoly;

class AFX_EXT_CLASS CGeomMapFile : public CObject
{
public:
	struct LAYER
	{
		WORD wId;
		DWORD dwGeomCount;
		DWORD dwGeomHeadOffset;
	};

	struct GEOMHEAD
	{
		BYTE bType;
		DWORD dwId;
		DWORD dwGeomOffset;
		CRect rect;
	};

public:
	CGeomMapFile();

public:
	~CGeomMapFile() override;

public:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_hFileMapView;

	bool Open(CString strFile);
	void Close();
public:
	GEOMHEAD* GetGeomHeadByIndex(const WORD& wLayerIndex, const DWORD& dwGeomIndex) const;
	GEOMHEAD* GetGeomHeadByID(const WORD& wLayerIndex, const DWORD& dwGeomID) const;
	GEOMHEAD* GetGeomHeadByID(const DWORD& dwGeomID, WORD& layerCount) const;
	CGeom* GetGeom(CGeomMapFile::GEOMHEAD* pOBJHEAD) const;

public:
	void Export(const WORD& wLayerIndex, CObList& geomlist);

public:
	CGeom* m_pFlash;
	CGeom* m_pGet;
};
