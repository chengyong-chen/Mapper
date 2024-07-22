#pragma once

#include "../Style/Dash.h"
#include "../Style/Color.h"

#include "../Geometry/Global.h"
#include <stack>
#include <poppler/GfxState.h>
using namespace std;
#include <optional>
class CGeom;
class CType;
class CColor;
class CFillCompact;
class CImport;
class CLine;
class CFillCompact;
class CClip;
class GooString;

typedef void (CImport::* CallbackOnGeomParsed)(CGeom*);
typedef CPoint(CImport::* CallbackOnChange)(double, double);
enum FillType : BYTE
{
	None1,
	Default,
	Inherit,
	Solid,
	Gradient
};
class CPath;
struct PMatrix
{
	double m11 = 1.0f;
	double m12 = 0.0f;
	float dx = 0.0f;

	double m21 = 0.0f;
	double m22 = 1.0f;
	float dy = 0.0f;
	PMatrix()
	{
		m11 = 1.0f;
		m12 = 0.0f;
		dx = 0.0f;

		m21 = 0.0f;
		m22 = 1.0f;
		dy = 0.0f;
	}
	PMatrix(double xm11, double xm12, double xm21, double xm22, float xdx, float xdy)
	{
		m11 = xm11;
		m12 = xm12;
		dx = xdx;

		m21 = xm21;
		m22 = xm22;
		dy = xdy;
	}
	PMatrix(const PMatrix& source)
	{
		m11 = source.m11;
		m12 = source.m12;
		dx = source.dx;

		m21 = source.m21;
		m22 = source.m22;
		dy = source.dy;
	}
	PMatrix& operator=(const PMatrix& source)
	{
		m11 = source.m11;
		m12 = source.m12;
		dx = source.dx;

		m21 = source.m21;
		m22 = source.m22;
		dy = source.dy;
		return *this;
	}

	friend PMatrix operator*(const PMatrix& matrix1, const PMatrix& matrix2)
	{
		const double m11 = matrix1.m11*matrix2.m11+matrix1.m12*matrix2.m21;
		const double m12 = matrix1.m11*matrix2.m12+matrix1.m12*matrix2.m22;
		const double m21 = matrix1.m21*matrix2.m11+matrix1.m22*matrix2.m21;
		const double m22 = matrix1.m21*matrix2.m12+matrix1.m22*matrix2.m22;
		const float dx = matrix1.m11*matrix2.dx+matrix1.m12*matrix2.dy+matrix1.dx;
		const float dy = matrix1.m21*matrix2.dx+matrix1.m22*matrix2.dy+matrix1.dy;

		return PMatrix(m11, m12, m21, m22, dx, dy);
	}

	void Reset()
	{
		m11 = 1.0f;
		m12 = 0.0f;
		dx = 0.0f;

		m21 = 0.0f;
		m22 = 1.0f;
		dy = 0.0f;
	}
	Gdiplus::PointF TransformPoint(double x, double y)
	{
		return Gdiplus::PointF(m11*x+m21*y+dx, m12*x+m22*y+dy);
	}
	void Translate(double cx, double cy)
	{
		const double offsetX = m11*cx+m12*cy+dx;
		const double offsetY = m21*cx+m22*cy+dy;
		dx = offsetX;
		dy = offsetY;
	}
};
struct State
{
	PMatrix matrix;
	CColor* line_color = nullptr;
	CDash line_dash;

	float line_opacity;
	float line_width;
	int line_join;
	int line_cap;
	int line_miterlimit;
	float entrystrokeopacity = 1;
	float layerstrokeopacity = 1;
	GfxColorSpace* strokeColorSpace;

	bool strokeadjust;

	FillType filltype = FillType::Solid;
	CColor* fill_color = nullptr;
	float fill_opacity;
	bool gradientfill = false;
	bool in_gradient_instance = false;
	bool in_palette = false;
	float entryfillopacity = 1;
	float layerfillopacity = 1;
	Gdiplus::FillMode fillmode;
	GfxColorSpace* fillColorSpace;

	char linkid[100];

	CClip* clippingpath = nullptr;

	bool gradient_geo = false;
	std::string gradient_name;
	std::string gradient_type;
	std::string gradient_ncolors;
	std::string gradient_matrix;
	int gradient_start;
	int gradient_end;

	void* softmask = nullptr;

	GfxBlendMode blendMode;
	State()
	{
	}
	State(const State& source)
	{
		matrix = source.matrix;
		line_dash = source.line_dash;
		line_width = source.line_width;
		line_join = source.line_join;
		line_cap = source.line_cap;
		line_miterlimit = source.line_miterlimit;
		layerstrokeopacity = source.layerstrokeopacity;
		entrystrokeopacity = source.entrystrokeopacity;

		gradientfill = source.gradientfill;
		in_gradient_instance = source.in_gradient_instance;
		in_palette = source.in_palette;
		layerstrokeopacity = source.layerstrokeopacity;
		entryfillopacity = source.entryfillopacity;

		clippingpath = source.clippingpath;

		gradient_geo = source.gradient_geo;
		gradient_name = source.gradient_name;
		gradient_type = source.gradient_type;
		gradient_ncolors = source.gradient_ncolors;
		gradient_matrix = source.gradient_matrix;
		gradient_start = source.gradient_start;
		gradient_end = source.gradient_end;

		if(source.line_color!=nullptr)
		{
			line_color = source.line_color->Clone();
		}
		if(source.fill_color!=nullptr)
		{
			fill_color = source.fill_color->Clone();
		}
	}

	State& operator=(const State& source)
	{
		matrix = source.matrix;
		line_dash = source.line_dash;
		line_width = source.line_width;
		line_join = source.line_join;
		line_cap = source.line_cap;
		line_miterlimit = source.line_miterlimit;
		layerstrokeopacity = source.layerstrokeopacity;
		entrystrokeopacity = source.entrystrokeopacity;

		gradientfill = source.gradientfill;
		in_gradient_instance = source.in_gradient_instance;
		in_palette = source.in_palette;
		layerfillopacity = source.layerfillopacity;
		entryfillopacity = source.entryfillopacity;

		clippingpath = source.clippingpath;

		gradient_geo = source.gradient_geo;
		gradient_name = source.gradient_name;
		gradient_type = source.gradient_type;
		gradient_ncolors = source.gradient_ncolors;
		gradient_matrix = source.gradient_matrix;
		gradient_start = source.gradient_start;
		gradient_end = source.gradient_end;
		if(line_color!=nullptr)
		{
			delete line_color;
			line_color = nullptr;
		}
		if(source.line_color!=nullptr)
		{
			line_color = source.line_color->Clone();
		}		
		if(fill_color!=nullptr)
		{
			delete fill_color;
			fill_color = nullptr;
		}
		if(source.fill_color!=nullptr)
		{
			fill_color = source.fill_color->Clone();
		}
		return *this;
	}
	~State()
	{
		if(line_color!=nullptr)
		{
			delete line_color;
			line_color = nullptr;
		}
		if(fill_color!=nullptr)
		{
			delete fill_color;
			fill_color = nullptr;
		}
	}
};
struct Glyph
{
	std::string gtext;
	float gap;
	Glyph(std::string text, float dis)
		:gtext(text), gap(dis)
	{
	};
};

struct FontState
{
	PMatrix textmatrix;
	double  linestartx;
	float  linestarty;
	float  postostartx;
	float  postostarty;

	std::string font;
	Gdiplus::FontStyle  style = Gdiplus::FontStyle::FontStyleRegular;
	std::optional<TEXT_ALIGN> align;
	std::optional<TEXT_ANCHOR> anchor;
	int weight;
	float size = 10;
	float Leading = 0;
	bool horizontal = true;
	int rendermode;


	float charSpace = 0;
	float wordSpace = 0;
	float rise = 0;
	float hscale = 1.f;
	float opacity;
	GfxFont* gfxfont = nullptr;
};

class __declspec(dllexport) IDataHandler abstract
{
public:
	IDataHandler(DWORD& maxId);
	~IDataHandler();

public:
	DWORD& m_maxId;
	bool bBezier = false;

public:
	virtual void onPathMoveTo(float x, float y);
	virtual void onPathLineTo(float x, float y);
	virtual void onPathCurvetoc(float x1, float y1, float x2, float y2, float x3, float y3);
	virtual void onPathCurvetov(float x2, float y2, float x3, float y3);
	virtual void onPathCurvetoy(float x1, float y1, float x3, float y3);
	virtual void onPathArc(float x1, float y1, float x2, float y2, float rx, float ry, float angle, bool bLarge, bool bSweep);
	virtual void onPathRect(float x, float y, float cx, float cy);
	virtual void onPathQuad(float x1, float y1, float x2, float y2)
	{
	};
	virtual void onPathDiscard();
	virtual void onPathClose(bool ignore);
	virtual void onPathEnd(bool stroke, bool fill);

	virtual void onClipStart(Gdiplus::FillMode mode);

	virtual void onGroupBegin();
	virtual void onCollectionEnd();
	virtual void onGeomParsed(CGeom* pGeom);

public:
	State CurrentState;
	FontState FontState;
	std::stack<State> StateStack;
	std::stack<CTypedPtrList<CObList, CGeom*>*> m_geomHolders;
	double colorDelta = 0.5;

protected:
	std::stack<CPath*> m_activePaths;

	CPoint Change(const double& x, const double& y) const;
	CPoint Change(const Gdiplus::PointF& point) const;
	CImport* m_instance;

public:
	CLine* GetLine();
	CFillCompact* GetFill() const;
public:
	void SetCallBack(CImport* instance/*,CallbackOnGeomParsed callback1,CallbackOnChange callback2*/)
	{
		m_instance = instance;/*OnGeomParsed=callback1;OnChange=callback2;*/
	};

public:
	static float CalculateVectorAngle(double ux, double uy, double vx, double vy);
};
