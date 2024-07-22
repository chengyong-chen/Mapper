#pragma once

#include <poppler/Object.h>

#include "Import.h"
#include "PostscriptHandler.h"
#include "Stdstack.h"

class CColor;
class CDash;
class CGeom;
class CText;
class CGeom;
class CDib;
class GfxColorSpace;
class CLayerTree;

class __declspec(dllexport) CImportPostscript : public CImport
{
public:
	CImportPostscript(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomID);

	~CImportPostscript() override;

public:
	BOOL Import(LPCTSTR lpszFile) override;

private:
	void Parse(XRef* xref, GfxResources& reses, Object& content);
	void ExcuteCommand(XRef* xref, GfxResources& reses, const char* cmd, std::stack<Object>& args);
	void ApplyExState(XRef* xref, GfxResources& reses, const char* ext);
	GfxColorSpace* lookupColorSpaceCopy(GfxResources& reses, Object& arg);
	CGeom* ExtractImage(GfxResources& reses, Stream* stream, bool inlineImg);

	static void ExtractSoftMask(Object* stream, bool alpha, GfxColorSpace* blendingColorSpace, bool isolated, bool knockout, Function* transferFunc, GfxColor* backdropColor);
	void ExtractForm(XRef* xref, GfxResources& reses, Object& content);
	void ExtractForm1(XRef* xref, GfxResources& reses, Object& content, Dict* resDict, PMatrix& matrix, double* bbox, bool transpGroup, bool softMask, GfxColorSpace* blendingColorSpace, bool isolated, bool knockout, bool alpha, Function* transferFunc, GfxColor* backdropColor);
public:
	CPostscriptHandler m_datahandler;
//
private:
	float m_fVersion;
//	bool gradientfill;
//	bool in_gradient_instance;
//	bool in_palette;

	std::map<std::string, GfxColorSpace*> colorSpacesCache;
public:
	static CDib* CreateImage(Stream* str, int width, int height, GfxImageColorMap* color_map, bool interpolation, int* mask_colors, bool alpha_only, bool invert_alpha);
};
