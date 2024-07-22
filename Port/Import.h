#pragma once

#include "../Style/FillCompact.h"
#include "../Style/Spot.h"
#include "../Style/Line.h"
#include "../Style/Type.h"
class CDaoDatabase;
class CDatainfo;
class CLayerTree;
class CHint;
class CGeom;
class CLine;
class CPointF;

class __declspec(dllexport) CImport
{
public:
	CImport(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId);
	virtual ~CImport();
public:
	double m_fXScale;
	double m_fYScale;
	double m_fXOffset;
	double m_fYOffset;

public:
	DWORD& m_dwMaxGeomId;
	CDatainfo& m_datainfo;
	CLayerTree& m_layertree;

public:
	CPoint Change(const CPointF& Scr) const;
	CPoint Change(const CPoint& Scr) const;
	virtual CPoint Change(const double& x, const double& y) const;

public:
	virtual BOOL Import(LPCTSTR lpszFile);

public:
	virtual void ReadPen(CStdioFile& file, const CGeom* pGeom)
	{
	}

	virtual void ReadBrush(CStdioFile& file, const CGeom* pGeom)
	{
	}

	virtual void ReadSpot(CStdioFile& file, const CGeom* pGeom)
	{
	}

	virtual void ReadType(CStdioFile& file, const CGeom* pGeom)
	{
	}

protected:
	static void CountInStyles(CGeom* pGeom, CValueCounter<CLine>& lines, CValueCounter<CFillCompact>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, CValueCounter<CHint>& hints);

	static void RemoveStyles(CGeom* pGeom, const CLine* pLine, const CFillCompact* pFill, const CSpot* pSpot, const CType* pType, const CHint* pHint);
};
