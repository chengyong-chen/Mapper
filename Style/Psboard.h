#pragma once

class CLine;
class CFillGeneral;
class CSpot;
class CType;
class CHint;
class CLibrary;

class __declspec(dllexport) CPsboard : public CObject
{
public:
	DWORD k; //Stroke Color 
	int R; //Stroke Overprint;
	int J; //Capindex;
	int j; //Joinindex;
	float w; //Width;
	int M; //Miterlimit;
	DWORD d[8]; //dash;

	DWORD K; //Fill Color
	int O; //Fill Overprint;

	CStringA strFont;
	float sizeFont;

public:
	CPsboard(const CLibrary& library);

	~CPsboard() override;

public:
	const CLibrary& m_library;
	CLine* pubLine;
	CFillGeneral* pubFill;
	CSpot* pubSpot;
	CType* pubType;
	CHint* pubHint;

public:
	void ExportStroke(FILE* file, const CLine* line);
	void ExportBrush(FILE* file, const CFillGeneral* fill);
	void ExportType(FILE* file, const CType* type);
	void ExportHint(FILE* file, const CHint* hint);
};