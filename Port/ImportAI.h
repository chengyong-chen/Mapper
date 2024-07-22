#pragma once

#include "Import.h"
#include "PSTokenFile.h"
#include "Geomstack.h"
#include "PostscriptHandler.h"
#include "Stdstack.h"


using namespace std;

class CColor;
class CDash;
class CGeom;
class CText;
class CGeom;
class CLayer;
class CLayerTree;
class __declspec(dllexport) CImportAI : public CImport
{
	struct EPSHEADER
	{
		DWORD filetype;//Must be hex C5D0D3C6
		DWORD startPS; //position in file for start of PostScript language code section.
		DWORD sizePS; //Byte length of PostScript language section.
		DWORD startWMF; // Byte position in file for start of Metafile screen representation.
		DWORD sizeWMF; //Byte length of Metafile section.
		DWORD startTIFF;//Byte position of TIFF representation.
		DWORD sizeTIFF;// Byte length of TIFF section.
		DWORD Checksum;// Checksum of header (XOR of bytes 0-27). If Checksum is FFFF then ignore it.
	};

public:
	CImportAI(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId);

	~CImportAI() override;

public:
	CString CoverToString(CString source);
	CGeom* ReadPoly(CString first, CStdioFile& file);
	CText* ReadText(CString first, CStdioFile& file);
	bool ReadProlog(CPSTokenFile& file);
	bool ReadSetup(CPSTokenFile& file) const;

public:
	BOOL Import(LPCTSTR lpszFile) override;

public:
	void ExcuteCommand(std::string strCmd);

	float m_fVersion;
	CStdstack m_epsStack;
private:
	CLayer* m_pCurrentLayer;
private:
	CPostscriptHandler m_datahandler;
	bool gradientfill;
	bool in_gradient_instance;
	bool in_palette;
};
