#pragma once

#include "DataHandler.h"

class CColor;

class __declspec(dllexport) CPostscriptHandler : public IDataHandler
{
public:
	CPostscriptHandler(DWORD& maxId);
	~CPostscriptHandler();

	virtual void onDonutBegin();
	
	virtual void onGapedText(std::list<Glyph*> texts);
	virtual void onSimpleText(std::string text);
public:
	virtual void onShadingFill(GfxShading* shading);

public: //style	
// fill
	virtual void onFillColor(BYTE gray);
	virtual void onFillColor(float r, float g, float b);
	virtual void onFillColor(float c, float m, float y, float k);
	virtual void onFillOpacity(float opacity);

	virtual void onFillRule(const string& rule)
	{
	};
	virtual void onGradient(std::string name, std::string type, std::string ncolors);

	// stroke
	virtual void onStrokeWidth(float width);

	virtual void onStrokeColor(BYTE gray);
	virtual void onStrokeColor(float r, float g, float b);
	virtual void onStrokeColor(float c, float m, float y, float k);

	virtual void onStrokeOpacity(float opacity);
	virtual void onStrokeJoin(int join);
	virtual void onStrokeCap(int cap);
	virtual void onStrokeMiterlimit(int miterlimit);
	virtual void onStrokeSolid();
	virtual void onStrokeDash(std::vector<float> data, float offset);

	//text	
	virtual void onTextAlign(TEXT_ALIGN align);
private:
	wchar_t* predict(std::string gtext, double& cx, double& cy);
	void FunctionShadingFill0(GfxFunctionShading* shading);		
	void FunctionShadingFill1(GfxFunctionShading* shading, double x0, double y0, double x1, double y1, GfxColor* colors, int depth);
	void GouraudFillTriangle(double x0, double y0, GfxColor* color0, double x1, double y1, GfxColor* color1, double x2, double y2, GfxColor* color2, int nComps, int depth);
	void FillPatch(const GfxPatch* patch, int nComps, int depth);

	static GfxPatch BlankPatch();

	static bool AddStops(std::map<unsigned int, CColor*>& stops,  GfxShading* shading, const Function* func);
};
