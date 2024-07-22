#include "Stdafx.h"
#include "PostscriptHandler.h"

#include <poppler/GfxFont.h>

#include "../Style/Line.h"

#include "../Style/Type.h"
#include "../Style/Dash.h"
#include "../Style/Color.h"
#include "../Style/ColorSpot.h"
#include "../Style/ColorProcess.h"
#include "../Style/ColorGray.h"
#include "../Style/FillRadial.h"
#include "../Style/FillLinear.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Text.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/Clip.h"
#include "../Geometry/TextGaped.h"

#include <poppler/PDFDocEncoding.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPostscriptHandler::CPostscriptHandler(DWORD& maxID)
	: IDataHandler(maxID)
{
}
CPostscriptHandler::~CPostscriptHandler()
{
}

void CPostscriptHandler::onDonutBegin()
{
	CDoughnut* pDonut = new CDoughnut();
	pDonut->m_geoId = ++m_maxId;
	m_geomHolders.push(&(CTypedPtrList<CObList, CGeom*>&)pDonut->m_geomlist);
}
void CPostscriptHandler::onGapedText(std::list<Glyph*> texts)
{
	if(FontState.gfxfont==nullptr)
		return;

	CString str;
	std::map<int, float> offset;
	int pos = 0;
	double dx = 0;
	double dy = 0;
	for(const Glyph* text:texts)
	{	
		dx += (FontState.horizontal ? -1 : 1)*text->gap*abs(FontState.size)/1000.f;

		offset[pos] = dx*FontState.textmatrix.m11;
		double cx = 0;
		double cy = 0;
		wchar_t* result = predict(text->gtext, cx, cy);
		dx += cx;
		dy += cy;
		str += CString(result);
		pos += wcslen(result);
		free(result);		
	}
	if(str=="Spratly Islands")
	{
		int i = 0;
	}
	const CStringA strReal(FontState.font.c_str());
	const double wscale = sqrt(FontState.textmatrix.m11*FontState.textmatrix.m11+FontState.textmatrix.m12*FontState.textmatrix.m12);
	const double hscale = sqrt(FontState.textmatrix.m21*FontState.textmatrix.m21+FontState.textmatrix.m22*FontState.textmatrix.m22);
	const double mscale = max(wscale, hscale);

	CType* pType = new CType();
	pType->m_fSize = FontState.size*mscale;
	pType->m_fontdesc.SetRealName(strReal);
	pType->m_fontdesc.m_style = FontState.style;
	switch(FontState.rendermode)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
	}
	pType->m_pColor = CurrentState.fill_color==nullptr ? nullptr : CurrentState.fill_color->Clone();
	CTextGaped* pText = new CTextGaped(CPoint(0, 0));
	pText->m_Rect = CRect(0, 0, dx*10000, dy*10000);
	pText->m_charspace = FontState.charSpace*FontState.textmatrix.m11;
	pText->m_wordspace = FontState.wordSpace*FontState.textmatrix.m11;
	PMatrix matrix = CurrentState.matrix*FontState.textmatrix;
	pText->Transform(matrix.m11, matrix.m21, matrix.dx, matrix.m12, matrix.m22, matrix.dy);

	const Gdiplus::PointF origin1 = matrix.TransformPoint(FontState.linestartx+FontState.postostartx, FontState.linestarty+FontState.postostarty);
	const CPoint orgin2 = Change(origin1.X, origin1.Y);
	pText->Move(orgin2.x-pText->m_Origin.x, orgin2.y-pText->m_Origin.y);

	pText->m_pType = pType;
	pText->m_strName = str;
	pText->m_Gaps = offset;

	pText->CalCorner(nullptr, 10000, 1.f);
	m_geomHolders.top()->AddTail(pText);

	if(FontState.horizontal)
		FontState.postostartx += dx;
	else
		FontState.postostarty += dy;
}

void CPostscriptHandler::onSimpleText(std::string text)
{
	if(FontState.gfxfont==nullptr)
		return;

	double cx = 0;
	double cy = 0;
	wchar_t* result = predict(text, cx, cy);

	const CStringA strReal(FontState.font.c_str());
	const double wscale = sqrt(FontState.textmatrix.m11*FontState.textmatrix.m11+FontState.textmatrix.m12*FontState.textmatrix.m12);
	const double hscale = sqrt(FontState.textmatrix.m21*FontState.textmatrix.m21+FontState.textmatrix.m22*FontState.textmatrix.m22);
	const double mscale = max(wscale, hscale);
	
	CType* pType = new CType();
	pType->m_fSize = FontState.size*mscale;
	pType->m_fontdesc.SetRealName(strReal);
	pType->m_fontdesc.m_style = FontState.style;
	switch(FontState.rendermode)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
	}
	pType->m_pColor = CurrentState.fill_color==nullptr ? nullptr : CurrentState.fill_color->Clone();
	CText* pText = new CText(CPoint(0, 0));
	pText->m_Rect = CRect(0, 0, cx*10000, cy*10000);

	PMatrix matrix = CurrentState.matrix*FontState.textmatrix;
	pText->Transform(matrix.m11, matrix.m21, matrix.dx, matrix.m12, matrix.m22, matrix.dy);

	const Gdiplus::PointF origin1 = matrix.TransformPoint(FontState.linestartx+FontState.postostartx, FontState.linestarty+FontState.postostarty);
	const CPoint orgin2 = Change(origin1.X, origin1.Y);
	pText->Move(orgin2.x-pText->m_Origin.x, orgin2.y-pText->m_Origin.y);

	pText->m_pType = pType;
	pText->m_strName = CString(result);

	pText->CalCorner(nullptr, 10000, 1.f);
	free(result);

	m_geomHolders.top()->AddTail(pText);
	if(FontState.horizontal)
		FontState.postostartx += cx;
	else
		FontState.postostarty += cy;
}
wchar_t* CPostscriptHandler::predict(std::string text, double& cx, double& cy)
{
	const char* pointer1 = text.c_str();
	auto len = text.size();
	wchar_t* result = (wchar_t*)malloc((len+1)*sizeof(wchar_t));
	memset(result, 0, (len+1)*sizeof(wchar_t));
	wchar_t* pointer2 = result;
	while(len>0)
	{
		CharCode code;
		const Unicode* unicode = nullptr;
		int uLen;

		double dx, dy;
		double oriX, oriY;
		const auto size = FontState.gfxfont->getNextChar(pointer1, len, &code, &unicode, &uLen, &dx, &dy, &oriX, &oriY);
		if(FontState.horizontal)
		{
			dx = dx*FontState.size+FontState.charSpace;			
			if(size==1&&*pointer1==' ')
			{
				dx += FontState.wordSpace;
			}
			dx *= FontState.hscale;
		}
		else
		{
			dx = dx*FontState.size-FontState.charSpace;
			dy = -dx;
			if(size==1&&*pointer1==' ')
			{
				dy -= FontState.wordSpace;
			}			
		}
		if(unicode == nullptr) 
		{
			break;
		}
		else if(uLen>0&&unicode[0]<0x80&&iscntrl(unicode[0])&&isspace(unicode[0])==false)
		{
		}
		else
		{
			*pointer2 = (wchar_t)*unicode;
			pointer2++;
		}

		cx += dx;
		cy += dy;

		len -= size;
		pointer1 += size;
	}

	return result;
}
void CPostscriptHandler::onShadingFill(GfxShading* shading)
{
	//std::stack<CPath*> savedPaths;
	// PMatrix originMatrix;
	//switch(shading->getType())
	//{
	//	case 2:
	//	case 3:
	//		CClip*pClip = CurrentState.clippingpath;
	//		originMatrix = CurrentState.matrix;
	//		CurrentState.matrix.Reset();
	//		break;
	//	default:
	//		savedPath = m_activePaths;
	//		break;
	//}
	if(shading->getHasBBox())
	{
		double left, top, right, bottom;
		shading->getBBox(&left, &top, &right, &bottom);
		Gdiplus::PointF point1 = CurrentState.matrix.TransformPoint(left, top);
		Gdiplus::PointF point2 = CurrentState.matrix.TransformPoint(right, top);
		Gdiplus::PointF point3 = CurrentState.matrix.TransformPoint(right, bottom);
		Gdiplus::PointF point4 = CurrentState.matrix.TransformPoint(left, bottom);
	}
	switch(shading->getType())
	{
		case 1:
		{
			GfxFunctionShading* function = (GfxFunctionShading*)shading;
			FunctionShadingFill0(function);
		}
		break;
		case 2:// Axial shading
		{
			const GfxAxialShading* axial = (GfxAxialShading*)shading;
			double x1, y1, x2, y2;
			axial->getCoords(&x1, &y1, &x2, &y2);
			bool extend0 = axial->getExtend0();
			bool extend1 = axial->getExtend1();
			const int num_funcs = axial->getNFuncs();
			if(num_funcs>1)
				break;

			const Function* func = axial->getFunc(0);
			std::map<unsigned int, CColor*> stops;
			AddStops(stops, shading, func);
			if(CurrentState.clippingpath!=nullptr)
			{
				if(CurrentState.clippingpath->m_pFill!=nullptr)
				{
					delete CurrentState.clippingpath->m_pFill;
				}

				CurrentState.clippingpath->m_pFill = new CFillLinear(x1, y1, x2, y2, 0, stops);
			}
		}
		break;
		case 3:// Radial shading
		{
			const GfxRadialShading* radial = (GfxRadialShading*)shading;
			double cx, cy, cr, fx, fy, fr;
			radial->getCoords(&cx, &cy, &cr, &fx, &fy, &fr);
			bool extend0 = radial->getExtend0();
			bool extend1 = radial->getExtend1();
			const int num_funcs = radial->getNFuncs();
			if(num_funcs>1)
				break;

			const Function* func = radial->getFunc(0);
			std::map<unsigned int, CColor*> stops;
			AddStops(stops, shading, func);
			if(CurrentState.clippingpath!=nullptr)
			{
				if(CurrentState.clippingpath->m_pFill!=nullptr)
				{
					delete CurrentState.clippingpath->m_pFill;
				}
				CurrentState.clippingpath->m_pFill = new CFillRadial(cx, cy, cr, fx, fy,fr,  stops);
			}
		}
		break;
		case 4:
		case 5:
		{
			GfxGouraudTriangleShading* triangle = (GfxGouraudTriangleShading*)shading;
			double x0, y0, x1, y1, x2, y2;
			GfxColor color0, color1, color2;
			const int nComps = triangle->getColorSpace()->getNComps();
			for(int i = 0; i<triangle->getNTriangles(); ++i)
			{
				triangle->getTriangle(i, &x0, &y0, &color0, &x1, &y1, &color1, &x2, &y2, &color2);
				GouraudFillTriangle(x0, y0, &color0, x1, y1, &color1, x2, y2, &color2, nComps, 0);
			}
		}
		break;
		case 6:
		case 7:
		{
			GfxPatchMeshShading* patch = (GfxPatchMeshShading*)shading;
			int start;
			if(patch->getNPatches()>128)
				start = 3;
			else if(patch->getNPatches()>64)
				start = 2;
			else if(patch->getNPatches()>16)
				start = 1;
			else
				start = 0;

			const int nComps = patch->getColorSpace()->getNComps();
			for(int i = 0; i<patch->getNPatches(); ++i)
			{
				FillPatch(patch->getPatch(i), nComps, start);
			}
		}
		break;
	}
}

static bool GetShadingColorRGB(GfxShading* shading, double offset, GfxRGB* result) {
	const GfxColorSpace* color_space = shading->getColorSpace();
	GfxColor temp;
	if(shading->getType()==2)   // Axial shading
		((GfxAxialShading*)(shading))->getColor(offset, &temp);
	else if(shading->getType()==3)  // Radial shading
		((GfxRadialShading*)(shading))->getColor(offset, &temp);

	else
		return false;

	// Convert it to RGB
	color_space->getRGB(&temp, result);

	return true;
}

void AddStop(std::map<unsigned int, CColor*>& stops, float offset, GfxRGB& color)
{
	const BYTE r = floor(255*(double)color.r/65535.0);
	const BYTE g = floor(255*(double)color.g/65535.0);
	const BYTE b = floor(255*(double)color.b/65535.0);
	stops.insert(std::make_pair(floor(offset*100), new CColorSpot(r, g, b, 255)));
}

bool CPostscriptHandler::AddStops(std::map<unsigned int, CColor*>& stops, GfxShading* shading, const Function* func)
{
	switch(func->getType())
	{
		case 0:
		case 2:
		{  // Sampled or exponential function
			GfxRGB stop1, stop2;
			if(!GetShadingColorRGB(shading, 0.0, &stop1))
				return false;
			else if(!GetShadingColorRGB(shading, 1.0, &stop2))
				return false;
			else
			{
				AddStop(stops, 0, stop1);
				AddStop(stops, 1, stop2);
			}
		}
		break;
		case 3:
		{ // Stitching
			const auto stitchingFunc = (const StitchingFunction*)func;
			const double* bounds = stitchingFunc->getBounds();
			const double* encode = stitchingFunc->getEncode();
			const int num_funcs = stitchingFunc->getNumFuncs();

			// Add stops from all the stitched functions
			GfxRGB prev;
			GetShadingColorRGB(shading, bounds[0], &prev);
			AddStop(stops, bounds[0], prev);

			for(int i = 0; i<num_funcs; i++)
			{
				GfxRGB color;
				GetShadingColorRGB(shading, bounds[i+1], &color);
				if(stitchingFunc->getFunc(i)->getType()==2) // process exponential fxn
				{
					double expE = ((const ExponentialFunction*)(stitchingFunc->getFunc(i)))->getE();
					if(expE>1.0)
					{
						expE = (bounds[i+1]-bounds[i])/expE;    // approximate exponential as a single straight line at x=1
						if(encode[2*i]==0)     // normal sequence
							AddStop(stops, bounds[i+1]-expE, prev);
						else                    // reflected sequence
							AddStop(stops, bounds[i]+expE, color);

					}
				}

				AddStop(stops, bounds[i+1], color);
				prev = color;
			}
		}
		break;
		default:
			break;
	}

	return true;
}
GfxPatch CPostscriptHandler::BlankPatch()
{
	GfxPatch patch;
	memset(&patch, 0, sizeof(patch));
	return patch;
}
void CPostscriptHandler::FillPatch(const GfxPatch* patch, int nComps, int depth)
{
	GfxPatch patch00 = BlankPatch();
	GfxPatch patch01 = BlankPatch();
	GfxPatch patch10 = BlankPatch();
	GfxPatch patch11 = BlankPatch();
	GfxColor color = {{0}};
	double xx[4][8];
	double yy[4][8];
	double xxm;
	double yym;

	int i;
	for(i = 0; i<nComps; ++i)
	{
		if(std::abs(patch->color[0][0].c[i]-patch->color[0][1].c[i])>colorDelta)
			break;
		if(std::abs(patch->color[0][1].c[i]-patch->color[1][1].c[i])>colorDelta)
			break;
		if(std::abs(patch->color[1][1].c[i]-patch->color[1][0].c[i])>colorDelta)
			break;
		if(std::abs(patch->color[1][0].c[i]-patch->color[0][0].c[i])>colorDelta)
			break;

		color.c[i] = GfxColorComp(patch->color[0][0].c[i]);
	}
	if(i==nComps||depth==6) {
		//ccu		state->moveTo(patch->x[0][0], patch->y[0][0]);
		//ccu		state->curveTo(patch->x[0][1], patch->y[0][1], patch->x[0][2], patch->y[0][2], patch->x[0][3], patch->y[0][3]);
		//ccu		state->curveTo(patch->x[1][3], patch->y[1][3], patch->x[2][3], patch->y[2][3], patch->x[3][3], patch->y[3][3]);
		//ccu		state->curveTo(patch->x[3][2], patch->y[3][2], patch->x[3][1], patch->y[3][1], patch->x[3][0], patch->y[3][0]);
		//ccu		state->curveTo(patch->x[2][0], patch->y[2][0], patch->x[1][0], patch->y[1][0], patch->x[0][0], patch->y[0][0]);
		//ccu		state->closePath();
		//ccu		state->setFillColor(&color);
		//ccu		builder->addPath(state, true, false);
		//ccu		state->clearPath();
	}
	else
	{
		for(i = 0; i<4; ++i) {
			xx[i][0] = patch->x[i][0];
			yy[i][0] = patch->y[i][0];
			xx[i][1] = 0.5*(patch->x[i][0]+patch->x[i][1]);
			yy[i][1] = 0.5*(patch->y[i][0]+patch->y[i][1]);
			xxm = 0.5*(patch->x[i][1]+patch->x[i][2]);
			yym = 0.5*(patch->y[i][1]+patch->y[i][2]);
			xx[i][6] = 0.5*(patch->x[i][2]+patch->x[i][3]);
			yy[i][6] = 0.5*(patch->y[i][2]+patch->y[i][3]);
			xx[i][2] = 0.5*(xx[i][1]+xxm);
			yy[i][2] = 0.5*(yy[i][1]+yym);
			xx[i][5] = 0.5*(xxm+xx[i][6]);
			yy[i][5] = 0.5*(yym+yy[i][6]);
			xx[i][3] = xx[i][4] = 0.5*(xx[i][2]+xx[i][5]);
			yy[i][3] = yy[i][4] = 0.5*(yy[i][2]+yy[i][5]);
			xx[i][7] = patch->x[i][3];
			yy[i][7] = patch->y[i][3];
		}
		for(i = 0; i<4; ++i) {
			patch00.x[0][i] = xx[0][i];
			patch00.y[0][i] = yy[0][i];
			patch00.x[1][i] = 0.5*(xx[0][i]+xx[1][i]);
			patch00.y[1][i] = 0.5*(yy[0][i]+yy[1][i]);
			xxm = 0.5*(xx[1][i]+xx[2][i]);
			yym = 0.5*(yy[1][i]+yy[2][i]);
			patch10.x[2][i] = 0.5*(xx[2][i]+xx[3][i]);
			patch10.y[2][i] = 0.5*(yy[2][i]+yy[3][i]);
			patch00.x[2][i] = 0.5*(patch00.x[1][i]+xxm);
			patch00.y[2][i] = 0.5*(patch00.y[1][i]+yym);
			patch10.x[1][i] = 0.5*(xxm+patch10.x[2][i]);
			patch10.y[1][i] = 0.5*(yym+patch10.y[2][i]);
			patch00.x[3][i] = 0.5*(patch00.x[2][i]+patch10.x[1][i]);
			patch00.y[3][i] = 0.5*(patch00.y[2][i]+patch10.y[1][i]);
			patch10.x[0][i] = patch00.x[3][i];
			patch10.y[0][i] = patch00.y[3][i];
			patch10.x[3][i] = xx[3][i];
			patch10.y[3][i] = yy[3][i];
		}
		for(i = 4; i<8; ++i) {
			patch01.x[0][i-4] = xx[0][i];
			patch01.y[0][i-4] = yy[0][i];
			patch01.x[1][i-4] = 0.5*(xx[0][i]+xx[1][i]);
			patch01.y[1][i-4] = 0.5*(yy[0][i]+yy[1][i]);
			xxm = 0.5*(xx[1][i]+xx[2][i]);
			yym = 0.5*(yy[1][i]+yy[2][i]);
			patch11.x[2][i-4] = 0.5*(xx[2][i]+xx[3][i]);
			patch11.y[2][i-4] = 0.5*(yy[2][i]+yy[3][i]);
			patch01.x[2][i-4] = 0.5*(patch01.x[1][i-4]+xxm);
			patch01.y[2][i-4] = 0.5*(patch01.y[1][i-4]+yym);
			patch11.x[1][i-4] = 0.5*(xxm+patch11.x[2][i-4]);
			patch11.y[1][i-4] = 0.5*(yym+patch11.y[2][i-4]);
			patch01.x[3][i-4] = 0.5*(patch01.x[2][i-4]+patch11.x[1][i-4]);
			patch01.y[3][i-4] = 0.5*(patch01.y[2][i-4]+patch11.y[1][i-4]);
			patch11.x[0][i-4] = patch01.x[3][i-4];
			patch11.y[0][i-4] = patch01.y[3][i-4];
			patch11.x[3][i-4] = xx[3][i];
			patch11.y[3][i-4] = yy[3][i];
		}
		//~ if the shading has a Function, this should interpolate on the
		//~ function parameter, not on the color components
		for(i = 0; i<nComps; ++i) {
			patch00.color[0][0].c[i] = patch->color[0][0].c[i];
			patch00.color[0][1].c[i] = (patch->color[0][0].c[i]+patch->color[0][1].c[i])/2;
			patch01.color[0][0].c[i] = patch00.color[0][1].c[i];
			patch01.color[0][1].c[i] = patch->color[0][1].c[i];
			patch01.color[1][1].c[i] = (patch->color[0][1].c[i]+patch->color[1][1].c[i])/2;
			patch11.color[0][1].c[i] = patch01.color[1][1].c[i];
			patch11.color[1][1].c[i] = patch->color[1][1].c[i];
			patch11.color[1][0].c[i] = (patch->color[1][1].c[i]+patch->color[1][0].c[i])/2;
			patch10.color[1][1].c[i] = patch11.color[1][0].c[i];
			patch10.color[1][0].c[i] = patch->color[1][0].c[i];
			patch10.color[0][0].c[i] = (patch->color[1][0].c[i]+patch->color[0][0].c[i])/2;
			patch00.color[1][0].c[i] = patch10.color[0][0].c[i];
			patch00.color[1][1].c[i] = (patch00.color[1][0].c[i]+patch01.color[1][1].c[i])/2;
			patch01.color[1][0].c[i] = patch00.color[1][1].c[i];
			patch11.color[0][0].c[i] = patch00.color[1][1].c[i];
			patch10.color[0][1].c[i] = patch00.color[1][1].c[i];
		}
		FillPatch(&patch00, nComps, depth+1);
		FillPatch(&patch10, nComps, depth+1);
		FillPatch(&patch01, nComps, depth+1);
		FillPatch(&patch11, nComps, depth+1);
	}
}


void CPostscriptHandler::GouraudFillTriangle(double x0, double y0, GfxColor* color0, double x1, double y1, GfxColor* color1, double x2, double y2, GfxColor* color2, int nComps, int depth)
{
	double x01, y01, x12, y12, x20, y20;
	GfxColor color01, color12, color20;

	int i;
	for(i = 0; i<nComps; ++i)
	{
		if(abs(color0->c[i]-color1->c[i])>colorDelta||abs(color1->c[i]-color2->c[i])>colorDelta) {
			break;
		}
	}
	if(i==nComps||depth==6)
	{
		//ccu state->setFillColor(color0);
		//ccu state->moveTo(x0, y0);
		//ccu state->lineTo(x1, y1);
		//ccu state->lineTo(x2, y2);
		//ccu state->closePath();
		//ccu builder->addPath(state, true, false);
		//ccu state->clearPath();
	}
	else {
		x01 = 0.5*(x0+x1);
		y01 = 0.5*(y0+y1);
		x12 = 0.5*(x1+x2);
		y12 = 0.5*(y1+y2);
		x20 = 0.5*(x2+x0);
		y20 = 0.5*(y2+y0);
		//~ if the shading has a Function, this should interpolate on the
		//~ function parameter, not on the color components
		for(i = 0; i<nComps; ++i)
		{
			color01.c[i] = (color0->c[i]+color1->c[i])/2;
			color12.c[i] = (color1->c[i]+color2->c[i])/2;
			color20.c[i] = (color2->c[i]+color0->c[i])/2;
		}
		GouraudFillTriangle(x0, y0, color0, x01, y01, &color01, x20, y20, &color20, nComps, depth+1);
		GouraudFillTriangle(x01, y01, &color01, x1, y1, color1, x12, y12, &color12, nComps, depth+1);
		GouraudFillTriangle(x01, y01, &color01, x12, y12, &color12, x20, y20, &color20, nComps, depth+1);
		GouraudFillTriangle(x20, y20, &color20, x12, y12, &color12, x2, y2, color2, nComps, depth+1);
	}
}

void CPostscriptHandler::FunctionShadingFill1(GfxFunctionShading* shading, double x0, double y0, double x1, double y1, GfxColor* colors, int depth)
{
	const int nComps = shading->getColorSpace()->getNComps();
	const double* matrix = shading->getMatrix();

	// compare the four corner colors
	int i;
	int j;
	for(i = 0; i<4; ++i)
	{
		for(j = 0; j<nComps; ++j)
		{
			if(abs(colors[i].c[j]-colors[(i+1)&3].c[j])>colorDelta)
				break;
		}
		if(j<nComps)
			break;
	}

	// center of the rectangle
	const double xM = 0.5*(x0+x1);
	const double yM = 0.5*(y0+y1);

	// the four corner colors are close (or we hit the recursive limit)
	// -- fill the rectangle; but require at least one subdivision
	// (depth==0) to avoid problems when the four outer corners of the
	// shaded region are the same color
	if((i==4&&depth>0)||depth==6) {

		// use the center color
		GfxColor fillColor;
		shading->getColor(xM, yM, &fillColor);
		//ccu state->setFillColor(&fillColor);

		// fill the rectangle
		//ccu state->moveTo(x0*matrix[0]+y0*matrix[2]+matrix[4], x0*matrix[1]+y0*matrix[3]+matrix[5]);
		//ccu state->lineTo(x1*matrix[0]+y0*matrix[2]+matrix[4], x1*matrix[1]+y0*matrix[3]+matrix[5]);
		//ccu state->lineTo(x1*matrix[0]+y1*matrix[2]+matrix[4], x1*matrix[1]+y1*matrix[3]+matrix[5]);
		//ccu state->lineTo(x0*matrix[0]+y1*matrix[2]+matrix[4], x0*matrix[1]+y1*matrix[3]+matrix[5]);
		//ccu state->closePath();
		//ccu builder->addPath(state, true, false);
		//ccu state->clearPath();
		// the four corner colors are not close enough -- subdivide the
		// rectangle
	}
	else {

		// colors[0]       colorM0       colors[2]
		//   (x0,y0)       (xM,y0)       (x1,y0)
		//         +----------+----------+
		//         |          |          |
		//         |    UL    |    UR    |
		// color0M |       colorMM       | color1M
		// (x0,yM) +----------+----------+ (x1,yM)
		//         |       (xM,yM)       |
		//         |    LL    |    LR    |
		//         |          |          |
		//         +----------+----------+
		// colors[1]       colorM1       colors[3]
		//   (x0,y1)       (xM,y1)       (x1,y1)
		GfxColor color0M, color1M, colorM0, colorM1, colorMM;
		shading->getColor(x0, yM, &color0M);
		shading->getColor(x1, yM, &color1M);
		shading->getColor(xM, y0, &colorM0);
		shading->getColor(xM, y1, &colorM1);
		shading->getColor(xM, yM, &colorMM);

		GfxColor colors2[4];
		// upper-left sub-rectangle
		colors2[0] = colors[0];
		colors2[1] = color0M;
		colors2[2] = colorM0;
		colors2[3] = colorMM;
		FunctionShadingFill1(shading, x0, y0, xM, yM, colors2, depth+1);

		// lower-left sub-rectangle
		colors2[0] = color0M;
		colors2[1] = colors[1];
		colors2[2] = colorMM;
		colors2[3] = colorM1;
		FunctionShadingFill1(shading, x0, yM, xM, y1, colors2, depth+1);

		// upper-right sub-rectangle
		colors2[0] = colorM0;
		colors2[1] = colorMM;
		colors2[2] = colors[2];
		colors2[3] = color1M;
		FunctionShadingFill1(shading, xM, y0, x1, yM, colors2, depth+1);

		// lower-right sub-rectangle
		colors2[0] = colorMM;
		colors2[1] = colorM1;
		colors2[2] = color1M;
		colors2[3] = colors[3];
		FunctionShadingFill1(shading, xM, yM, x1, y1, colors2, depth+1);
	}
}
void CPostscriptHandler::FunctionShadingFill0(GfxFunctionShading* shading) {
	double x0, y0, x1, y1;
	shading->getDomain(&x0, &y0, &x1, &y1);

	GfxColor colors[4];
	shading->getColor(x0, y0, &colors[0]);
	shading->getColor(x0, y1, &colors[1]);
	shading->getColor(x1, y0, &colors[2]);
	shading->getColor(x1, y1, &colors[3]);
	FunctionShadingFill1(shading, x0, y0, x1, y1, colors, 0);
}
void CPostscriptHandler::onStrokeWidth(float width)
{
	const double x = CurrentState.matrix.m11+CurrentState.matrix.m12;
	const double y = CurrentState.matrix.m21+CurrentState.matrix.m22;
	CurrentState.line_width = width*sqrt((x*x+y*y)/2);
}

void CPostscriptHandler::onStrokeColor(BYTE gray)
{
	if(CurrentState.line_color!=nullptr)
		delete CurrentState.line_color;
	CurrentState.line_color = new CColorGray(gray, round(CurrentState.layerstrokeopacity*CurrentState.entrystrokeopacity*255));
}
void CPostscriptHandler::onStrokeColor(float r, float g, float b)
{
	if(CurrentState.line_color!=nullptr)
		delete CurrentState.line_color;
	CurrentState.line_color = new CColorSpot(r*255, g*255, b*255, round(CurrentState.layerstrokeopacity*CurrentState.entrystrokeopacity*255));
}

void CPostscriptHandler::onStrokeColor(float c, float m, float y, float k)
{
	if(CurrentState.line_color!=nullptr)
		delete CurrentState.line_color;
	CurrentState.line_color = new CColorProcess(c*100, m*100, y*100, k*100, round(CurrentState.layerstrokeopacity*CurrentState.entrystrokeopacity*255));
}

void CPostscriptHandler::onStrokeOpacity(float opacity)
{
	CurrentState.entrystrokeopacity = opacity;
}

void CPostscriptHandler::onStrokeSolid()
{
	CurrentState.line_dash.Solid();
}

void CPostscriptHandler::onStrokeDash(std::vector<float> data,float offset)
{
	CurrentState.line_dash.Reset(data, offset);
}

void CPostscriptHandler::onStrokeJoin(int join)
{
	CurrentState.line_join = join;
}

void CPostscriptHandler::onStrokeCap(int cap)
{
	CurrentState.line_cap = cap;
}

void CPostscriptHandler::onStrokeMiterlimit(int miterlimit)
{
	CurrentState.line_miterlimit = miterlimit;
}

void CPostscriptHandler::onFillColor(BYTE gray)
{
	if(CurrentState.fill_color!=nullptr)
		delete CurrentState.fill_color;
	CurrentState.fill_color = new CColorGray(gray, round(CurrentState.layerfillopacity*CurrentState.entryfillopacity*255));
}
void CPostscriptHandler::onFillColor(float r, float g, float b)
{
	if(CurrentState.fill_color!=nullptr)
		delete CurrentState.fill_color;
	CurrentState.fill_color = new CColorSpot(r*255, g*255, b*255, round(CurrentState.layerfillopacity*CurrentState.entryfillopacity*255));
}

void CPostscriptHandler::onFillColor(float c, float m, float y, float k)
{
	if(CurrentState.fill_color!=nullptr)
		delete CurrentState.fill_color;
	CurrentState.fill_color = new CColorProcess(c*100, m*100, y*100, k*100, round(CurrentState.layerfillopacity*CurrentState.entryfillopacity*255));
}

void CPostscriptHandler::onFillOpacity(float opacity)
{
	CurrentState.entryfillopacity = opacity;
}

void CPostscriptHandler::onGradient(std::string name, std::string type, std::string ncolors)
{
	CurrentState.gradient_name = name;
	CurrentState.gradient_type = type;
	CurrentState.gradient_ncolors = ncolors;
}

void CPostscriptHandler::onTextAlign(TEXT_ALIGN align)
{
	FontState.align = align;
}
