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

class AFX_EXT_CLASS CGeomMemFile : public CObject
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
	CGeomMemFile();

public:
	~CGeomMemFile() override;

public:
	BYTE* m_pData;

	bool Open(CString strFile);
	void Close();
public:
	GEOMHEAD* GetGeomHeadByIndex(const WORD& wLayerIndex, const DWORD& dwGeomIndex) const;
	GEOMHEAD* GetGeomHeadByID(const WORD& wLayerIndex, const DWORD& dwGeomID) const;
	CGeom* GetGeom(CGeomMemFile::GEOMHEAD* pOBJHEAD) const;
};
