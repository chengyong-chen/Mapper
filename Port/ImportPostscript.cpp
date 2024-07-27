#include "stdafx.h"
#include "ImportPostscript.h"
#include "Style.h"
#include <any>
#include <filesystem>

#include "../Geometry/Text.h"

#include "../Style/Line.h"
#include "../Layer/Layer.h"
#include "../Style/FontCombox.h"
#include "../Layer/LayerTree.h"

#include "../Image/Dib.h"
#include "../Geometry/Imager.h"
#include "../Geometry/Mask.h"

//#include "../SharpPlusProxy/ShapeParser.h"
#include "../../ThirdParty/poppler/poppler/GlobalParams.h"
#include "../../ThirdParty/poppler/poppler/ErrorCodes.h"
#include "../../ThirdParty/poppler/poppler/PDFDoc.h"
#include "../../ThirdParty/poppler/poppler/Parser.h"
#include "../../ThirdParty/poppler/poppler/Gfx.h"
#include "../../ThirdParty/poppler/poppler/GfxFont.h"
#include "../../ThirdParty/libpng//1.6.37/png.h"
#include "../Geometry/Clip.h"

class CDib;

//using namespace std;
using namespace StyleSheet;
extern unsigned long uDecimal;

extern __declspec(dllimport) std::list<std::pair<const char*, std::stack<std::any>>>* ParseFile(std::string ext, std::wstring file);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImportPostscript::CImportPostscript(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomID)
	: CImport(datainfo, layertree, dwMaxGeomID), m_datahandler(dwMaxGeomID)
{
	m_fVersion = 4.0;

	m_datahandler.SetCallBack(this/*,&CImport::OnGeomParsed,&CImport::Change*/);
}

CImportPostscript::~CImportPostscript()
{
}
void  xrefReconstructedCallback()
{

}
BOOL CImportPostscript::Import(LPCTSTR lpszFile)
{
	if(globalParams==nullptr)
	{
		char path[MAX_PATH];
		memset(path, 0, MAX_PATH);
		::GetModuleFileNameA(nullptr, path, MAX_PATH);
		char* pos = strrchr(path, '\\');
		memcpy(pos, "\\poppler\0", 9);
		globalParams = std::unique_ptr<GlobalParams>(new GlobalParams(path));
	}

	const LPCWSTR filename = lpszFile;
	const PDFDoc pdfDoc((wchar_t*)filename, _tcslen(lpszFile), nullptr, nullptr, nullptr, xrefReconstructedCallback);
	if(pdfDoc.isOk()==false) {
		const int errcode = pdfDoc.getErrorCode();
		CString msg;
		switch(errcode)
		{
			case errEncrypted:
				msg = _T("Document is encrypted.");
				break;
			case errOpenFile:
				msg = _T("couldn't open the PDF file.");
				break;
			case errBadCatalog:
				msg = _T("couldn't read the page catalog.");
				break;
			case errDamaged:
				msg = _T("PDF file was damaged and couldn't be repaired.");
				break;
			case errHighlightFile:
				msg = _T("nonexistent or invalid highlight file.");
				break;
			case errBadPrinter:
				msg = _T("invalid printer.");
				break;
			case errPrinting:
				msg = _T("Error during printing.");
				break;
			case errPermission:
				msg = _T("PDF file does not allow that operation.");
				break;
			case errBadPageNum:
				msg = _T("invalid page number.");
				break;
			case errFileIO:
				msg = _T("file IO error.");
				break;
			default:
				msg.Format(_T("Failed to load document from data(error%d)"), errcode);
				break;
		}
		AfxMessageBox(msg);
		return FALSE;
	}
	else
	{
		Catalog* const  catalog = pdfDoc.getCatalog();
		int const numOfpages = catalog->getNumPages();
		Page* const page = catalog->getPage(1);
		if(page==nullptr)
		{
			AfxMessageBox(_T("This file has no content"));
			return FALSE;
		}
		Object content = page->getContents();
		if(content.isNull()==true)
		{
			AfxMessageBox(_T("This file has no content"));
			return FALSE;
		}
		else if(content.isArray()==true)
		{
			bool good = true;
			for(int index = 0; index<content.arrayGetLength(); index++)
			{
				Object obj = content.arrayGet(index);
				if(obj.isStream()==false)
				{
					good = false;
					break;
				}
			}
			if(good==false)
			{
				AfxMessageBox(_T("Weird page contents"));
				return FALSE;
			}
		}
		else if(content.isStream()==false)
		{
			AfxMessageBox(_T("Weird page contents"));
			return FALSE;
		}


		CLayer* pLayer = new CLayer(m_layertree);
		pLayer->m_wId = m_layertree.ApplyId();
		m_layertree.m_root.AddChild(nullptr, pLayer);
		m_datahandler.m_geomHolders.push(&pLayer->m_geomlist);

		const auto xref = pdfDoc.getXRef();
		const auto dict = page->getResourceDict();
		GfxResources reses(xref, dict, nullptr);
		CImportPostscript::Parse(xref, reses, content);
		pLayer->RecalRect();
	}

	//if(filePath.extension() ==".pdf")
	//	cmds = ParseFile("pdf", filePath);
	//else if(filePath.extension() == ".eps")
	//	cmds = ParseFile("eps", filePath);
	//else if(filePath.extension()==".ai")
	//	cmds = ParseFile("ai", filePath);
	//else if(filePath.extension()==".svg")
	//	cmds = ParseFile("svg", filePath);
	//if(cmds!=nullptr)
	//{
	//	CImportPostscript::ExcuteCommand(cmds);
	//	delete cmds;
	//}
	CImport::Import(lpszFile);

	return TRUE;
}
void CImportPostscript::Parse(XRef* xref, GfxResources& reses, Object& content)
{
	Parser parser(xref, &content, true);
	Object obj = parser.getObj();
	std::stack<Object> stack;
	while(obj.isEOF()==false)
	{
		if(obj.isCmd())
		{
			const char* cmd = obj.getCmd();
			CImportPostscript::ExcuteCommand(xref, reses, cmd, stack);
			CStdstack::Clear(stack);
		}
		else
		{
			stack.push(std::move(obj));
		}

		obj = parser.getObj();
	}
}
void CImportPostscript::ExcuteCommand(XRef* xref, GfxResources& reses, const char* cmd, std::stack<Object>& args)
{
	if(cmd==nullptr)
		return;
	else if(strcmp(cmd, "")==0)
		return;
	else if(strcmp(cmd, "re")==0)
	{
		float cy = CStdstack::PopFloat(args);
		float cx = CStdstack::PopFloat(args);
		float y = CStdstack::PopFloat(args);
		float x = CStdstack::PopFloat(args);
		m_datahandler.onPathRect(x, y, cx, cy);
	}
	else if(strcmp(cmd, "m")==0)
	{
		float y = CStdstack::PopFloat(args);
		float x = CStdstack::PopFloat(args);
		m_datahandler.onPathMoveTo(x, y);
	}
	else if(strcmp(cmd, "l")==0)
	{
		float y = CStdstack::PopFloat(args);
		float x = CStdstack::PopFloat(args);
		m_datahandler.onPathLineTo(x, y);
	}
	else if(strcmp(cmd, "c")==0)
	{
		float y3 = CStdstack::PopFloat(args);
		float x3 = CStdstack::PopFloat(args);
		float y2 = CStdstack::PopFloat(args);
		float x2 = CStdstack::PopFloat(args);
		float y1 = CStdstack::PopFloat(args);
		float x1 = CStdstack::PopFloat(args);
		m_datahandler.onPathCurvetoc(x1, y1, x2, y2, x3, y3);
	}
	else if(strcmp(cmd, "v")==0)
	{
		float y3 = CStdstack::PopFloat(args);
		float x3 = CStdstack::PopFloat(args);
		float y2 = CStdstack::PopFloat(args);
		float x2 = CStdstack::PopFloat(args);
		m_datahandler.onPathCurvetov(x2, y2, x3, y3);
	}
	else if(strcmp(cmd, "y")==0)
	{
		float y3 = CStdstack::PopFloat(args);
		float x3 = CStdstack::PopFloat(args);
		float y1 = CStdstack::PopFloat(args);
		float x1 = CStdstack::PopFloat(args);
		m_datahandler.onPathCurvetoy(x1, y1, x3, y3);
	}
	else if(strcmp(cmd, "n")==0)
	{
		m_datahandler.onPathDiscard();
	}
	else if(strcmp(cmd, "W")==0)
	{
		m_datahandler.onClipStart(Gdiplus::FillMode::FillModeWinding);
	}
	else if(strcmp(cmd, "W*")==0)
	{
		m_datahandler.onClipStart(Gdiplus::FillMode::FillModeAlternate);
	}
	else if(strcmp(cmd, "Wx")==0)
	{
		m_datahandler.onClipStart(Gdiplus::FillMode::FillModeAlternate);
	}
	else if(strcmp(cmd, "h")==0)
	{
		m_datahandler.onPathClose(true);
	}
	else if(strcmp(cmd, "s")==0)
	{
		m_datahandler.onPathClose(true);
		m_datahandler.onPathEnd(true, false);
	}
	else if(strcmp(cmd, "S")==0)
	{
		m_datahandler.onPathEnd(true, false);
	}
	else if(strcmp(cmd, "f")==0)
	{//add fillrule NoneZero		
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeWinding;
		m_datahandler.onPathEnd(false, true);
	}
	else if(strcmp(cmd, "F")==0)
	{
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeWinding;
		m_datahandler.onPathEnd(false, ~m_datahandler.CurrentState.gradientfill);
	}
	else if(strcmp(cmd, "f*")==0)
	{
		m_datahandler.onPathEnd(false, true);
	}
	else if(strcmp(cmd, "fx")==0)
	{//add fillrule EvenOdd		
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeAlternate;
		m_datahandler.onPathEnd(false, true);
	}
	else if(strcmp(cmd, "b")==0)
	{
		m_datahandler.onPathClose(true);
		m_datahandler.onPathEnd(true, true);
	}
	else if(strcmp(cmd, "bx")==0)
	{
		m_datahandler.onPathClose(true);
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeAlternate;
		m_datahandler.onPathEnd(true, true);
	}
	else if(strcmp(cmd, "p")==0) //set_fill_pattern 
	{
		//name, px, py, sx, sy, angle, rf, r, k, ka,							matrix
		//if not in_palette:
		//add_message(_("Vector patterns not supported. Using black"))
		//fill_color = new CColorSpot(0,0,0);
	}
	else if(strcmp(cmd, "Bd")==0) //begin_gradient 
	{
		std::string name = CStdstack::PopString(args);
		std::string type = CStdstack::PopString(args);
		std::string ncolors = CStdstack::PopString(args);
		m_datahandler.onGradient(name, type, ncolors);
	}
	else if(strcmp(cmd, "Bs")==0) //gradient_stop 
	{
		int style = 0;
		int ramp_point = CStdstack::PopInt(args);
		CColor* color;
		if(style==0)// gray scale
		{
			int k = CStdstack::PopInt(args);
			//	color = new CColorSpot(k*100, k*100, k*100, 255);
		}
		else if(style==1)// CMYK
		{
			int tint = CStdstack::PopInt(args);
			int k = CStdstack::PopInt(args);
			int y = CStdstack::PopInt(args);
			int m = CStdstack::PopInt(args);
			int c = CStdstack::PopInt(args);
			//	color = new CColorGray(c*100, m*100, y*100, k*100, tint*100);
		}
		else if(style==2)// The cmyk and rgb values usually differ slightly because AI does some color correction. Which values should we choose here?
		{
			int b = CStdstack::PopInt(args);
			int g = CStdstack::PopInt(args);
			int r = CStdstack::PopInt(args);
			//	color = new CColorSpot(r*100, g*100, b*100, 255);
			int tint = CStdstack::PopInt(args);
			int k = CStdstack::PopInt(args);
			int y = CStdstack::PopInt(args);
			int m = CStdstack::PopInt(args);
			int c = CStdstack::PopInt(args);
			//	color = new CColorGray(c*100, m*100, y*100, k*100, tint*100);
		}
		else if(style==3)// CMYK Custom Color
		{
			//	CStdstack::Pop(args, 6);
			//	color = apply(CreateCMYKColor, tuple(args[:4]))
		}
		else if(style==4)
		{
			//	CStdstack::Pop(args, 10);
			//	color = new CColorSpot(0, 0, 0, 255);
		}
		else
		{
			//	color = new CColorSpot(0, 0, 0, 255); // XXX
				//add_message(_("Gradient ColorStyle %d not yet supported.substituted black")
		}

		//	m_epsStack.Push((ramp_point/100.0, color));
	}
	else if(strcmp(cmd, "BS")==0) //dummy_gradient_stop
	{
		// same as gradient_stop but ignore all arguments. Illustrator 8
		// seems to introduce this one for printing (i.e. Illustrator 8
		// files with printing info contain the gradient stops *twice* in
		// exactly the same format but once with the Bs operator and once
		// with BS. I guess this has something to do with support for
		// PostScript Level 3 and backwards compatibility with older
		// Illustrator versions.
		int style;
		if(style==0)// gray scale
		{
			int k = CStdstack::PopInt(args);
		}
		else if(style==1)// CMYK
		{
			//	CStdstack::Pop(args, 4);
		}
		else if(style==2)// RGB Color
		{
			//	CStdstack::Pop(args, 7);
		}
		else if(style==3)// CMYK Custom Color
		{
			//	CStdstack::Pop(args, 6);
		}
		else if(style==4)
		{
			//	CStdstack::Pop(args, 10);
		}
		//	else
		//		add_message(_("Unknown ColorStyle %d") % color_style)
	}
	else if(strcmp(cmd, "Br")==0) //gradient_ramps 
	{
		int i = 0; //// defines the ramp colors with a bunch of strings for printing.
		//// Here we just m_epsStack.Pop all the strings off the stack
		//num = (1, 4, 5, 6, 7, 8, 9)[ramp_type]
		//CStdstack::Pop(args, num);
	}
	else if(strcmp(cmd, "BD")==0) //end_gradient 
	{
		int i = 0;
		//CStringArray array = m_epsStack.PopTo("");
		//m_epsStack.Push(array)
		//array = CStdstack::Pop(args, );
		//if(array.GetCount() < 2)
		//	add_message(_("less than two color stops in gradient"));
		//else
		//	// sometimes the ramp_point values are increasing, sometimes
		//	// decreasing... what's going on here? The docs say they are
		//	// increasing.
		//	if(array[0][0] > array[-1][0])
		//		array.reverse();
		//	gradients[gradient_name] = (gradient_type, array);
		////del stack[:]
		//m_epsStack.RemoveAll();
		////m_epsStack.PopTo("")
	}
	else if(strcmp(cmd, "Bb")==0) //begin_gradient_instance 
	{
		//	in_gradient_instance = 1;
		//	gradientfill = true;
	}
	else if(strcmp(cmd, "Bg")==0) //gradient_geometry 
	{
		int i = 0;
		/*	float a, b, c, d, tx, ty;
		trafo = Trafo(a, b, c, d, tx, ty);
		trafo = artboard_trafo_inv(trafo(artboard_trafo));
		start = Gdiplus::PointF(xorig, yorig);
		end = start + Polar(length, (pi*angle)/180.0);
		gradient_geo = (name, trafo, start, end);*/
	}
	else if(strcmp(cmd, "BB")==0) //end_gradient_instance 
	{
		int i = 0;
		/*	int flag;
		gradientfill = false;
		if(flag == 2)
		fill_stroke_close();
		else if(flag == 1)
		fill_stroke();
		else
		fill();
		in_gradient_instance = 0;*/
	}
	else if(strcmp(cmd, "B")==0)
	{
		//NoneZero
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeWinding;
		m_datahandler.onPathEnd(true, true);
	}
	else if(strcmp(cmd, "Bx")==0)
	{//EvenOdd
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeAlternate;
		m_datahandler.onPathEnd(true, true);
	}
	else if(strcmp(cmd, "B*")==0)
	{
		//NoneZero
		m_datahandler.CurrentState.fillmode = Gdiplus::FillMode::FillModeWinding;
		m_datahandler.onPathEnd(true, true);
	}
	else if(strcmp(cmd, "BMC")==0)
	{
		auto marked = CStdstack::PopString(args);
	}
	else if(strcmp(cmd, "EMC")==0)//end 
	{
	}
	else if(strcmp(cmd, "BDC")==0)//begin marked content
	{
		auto marked = CStdstack::PopString(args);
	}
	else if(strcmp(cmd, "EMC")==0)//begin marked content
	{
	}
	else if(strcmp(cmd, "BI")==0)
	{
	}
	else if(strcmp(cmd, "ID")==0)
	{
	}
	else if(strcmp(cmd, "EI")==0)
	{
	}
	else if(strcmp(cmd, "g")==0)
	{
		float gray = CStdstack::PopFloat(args);
		m_datahandler.onFillColor((BYTE)(round(gray*255)));
	}
	else if(strcmp(cmd, "G")==0)
	{
		float gray = CStdstack::PopFloat(args);
		m_datahandler.onStrokeColor((BYTE)(round(gray*255)));
	}
	else if(strcmp(cmd, "rg")==0)
	{
		float b = CStdstack::PopFloat(args);
		float g = CStdstack::PopFloat(args);
		float r = CStdstack::PopFloat(args);
		m_datahandler.onFillColor(r, g, b);
	}
	else if(strcmp(cmd, "RG")==0)
	{
		float b = CStdstack::PopFloat(args);
		float g = CStdstack::PopFloat(args);
		float r = CStdstack::PopFloat(args);
		m_datahandler.onStrokeColor(r, g, b);
	}
	else if(strcmp(cmd, "k")==0)
	{
		float k = CStdstack::PopFloat(args);
		float y = CStdstack::PopFloat(args);
		float m = CStdstack::PopFloat(args);
		float c = CStdstack::PopFloat(args);
		m_datahandler.onFillColor(c, m, y, k);
	}
	else if(strcmp(cmd, "K")==0)
	{
		float k = CStdstack::PopFloat(args);
		float y = CStdstack::PopFloat(args);
		float m = CStdstack::PopFloat(args);
		float c = CStdstack::PopFloat(args);
		m_datahandler.onStrokeColor(c, m, y, k);
	}
	else if(strcmp(cmd, "cs")==0)
	{
		GfxColorSpace* colorSpace = lookupColorSpaceCopy(reses, args.top());
		if(colorSpace!=nullptr)
		{
			m_datahandler.CurrentState.fillColorSpace = colorSpace;
			GfxColor color;
			colorSpace->getDefaultColor(&color);
			auto dd = color.c;
		}
	}
	else if(strcmp(cmd, "CS")==0)
	{
		GfxColorSpace* colorSpace = lookupColorSpaceCopy(reses, args.top());
		if(colorSpace!=nullptr)
		{
			m_datahandler.CurrentState.strokeColorSpace = colorSpace;
			GfxColor color;
			colorSpace->getDefaultColor(&color);
			auto dd = color.c;

		}
	}
	else if(strcmp(cmd, "sc")==0)
	{
		if(args.size()==3)
		{
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onFillColor(c1, c2, c3);
		}
		else if(args.size()==4)
		{
			float c4 = CStdstack::PopFloat(args);
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onFillColor(c1, c2, c3, c4);
		}
	}
	else if(strcmp(cmd, "SC")==0)
	{
		if(args.size()==3)
		{
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onStrokeColor(c1, c2, c3);
		}
		else if(args.size()==4)
		{
			float c4 = CStdstack::PopFloat(args);
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onStrokeColor(c1, c2, c3, c4);
		}
	}
	else if(strcmp(cmd, "scn")==0)
	{
		if(args.size()==3)
		{
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onFillColor(c1, c2, c3);
		}
		else if(args.size()==4)
		{
			float c4 = CStdstack::PopFloat(args);
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onFillColor(c1, c2, c3, c4);
		}
	}
	else if(strcmp(cmd, "SCN")==0)
	{
		if(args.size()==3)
		{
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onStrokeColor(c1, c2, c3);
		}
		else if(args.size()==4)
		{
			float c4 = CStdstack::PopFloat(args);
			float c3 = CStdstack::PopFloat(args);
			float c2 = CStdstack::PopFloat(args);
			float c1 = CStdstack::PopFloat(args);
			m_datahandler.onStrokeColor(c1, c2, c3, c4);
		}
	}
	else if(strcmp(cmd, "sh")==0)//shading fill
	{
		auto name = CStdstack::PopString(args);
		PDFRectangle box(0, 0, 0, 0);
		GfxState state(72.0, 72.0, &box, 0, true);
		auto shading = reses.lookupShading(name.c_str(), nullptr, &state);
		if(shading!=nullptr)
		{
			m_datahandler.onShadingFill(shading);
			delete shading;
		}
	}
	else if(strcmp(cmd, "BT")==0)//text start
	{
		m_datahandler.FontState.textmatrix.Reset();
		m_datahandler.FontState.linestartx = 0;
		m_datahandler.FontState.linestarty = 0;
		m_datahandler.FontState.postostartx = 0;
		m_datahandler.FontState.postostarty = 0;
	}
	else if(strcmp(cmd, "Tj")==0)
	{
		auto gtext = args.top().getString();
		std::string str(gtext->c_str());
		m_datahandler.onSimpleText(str);
		args.pop();
	}
	else if(strcmp(cmd, "TJ")==0)
	{
		std::list<Glyph*> texts;
		auto array = args.top().getArray();
		float offset = 0;
		for(int i = 0; i<array->getLength(); ++i)
		{
			auto obj = array->get(i);
			if(obj.isNum())
			{
				offset = obj.getNum();
			}
			else if(obj.isString())
			{
				const GooString* gtext = obj.getString();
				std::string str(gtext->c_str());
				texts.push_back(new Glyph(str, offset));
			}
		}
		m_datahandler.onGapedText(texts);
	}
	else if(strcmp(cmd, "'")==0)
	{
		m_datahandler.FontState.linestarty -= m_datahandler.FontState.Leading;
		m_datahandler.FontState.postostartx = 0;

		auto gtext = args.top().getString();
		std::string str(gtext->c_str());
		m_datahandler.onSimpleText(str);
		args.pop();
	}
	else if(strcmp(cmd, "ET")==0)//end of text
	{
	}
	else if(strcmp(cmd, "Tf")==0)
	{
		if(args.size()==2)
		{
			m_datahandler.FontState.size = CStdstack::PopDouble(args);
		}
		if(args.size()>=1)
		{
			auto name = CStdstack::PopString(args);
			GfxFont* font = reses.lookupFont(name.c_str());
			if(font!=nullptr)
			{
				const char* fontfamily = nullptr;
				const char* fontname = nullptr;
				const GooString* gstring1 = font->getFamily();
				if(gstring1!=nullptr)
				{
					fontfamily = gstring1->c_str();
				}
				const GooString* gstring2 = font->getName();
				if(gstring2!=nullptr)
				{
					const char* specification = gstring2->c_str();
					const char* plussign = strstr(specification, "+");
					fontname = plussign!=nullptr ? plussign+1 : specification;
				}
				char* found = CFontDesc::GetBestMatched(fontfamily, fontname);
				m_datahandler.FontState.font = found;
				m_datahandler.FontState.style = Gdiplus::FontStyle::FontStyleRegular;
				DWORD dwFlg = font->getFlags();
				if((DWORD)(dwFlg&0X00000040)==0X00000040&&(DWORD)(dwFlg&00040000)==00040000)
					m_datahandler.FontState.style = Gdiplus::FontStyle::FontStyleBoldItalic;
				if((DWORD)(dwFlg&0X00000040)==0X00000040)
					m_datahandler.FontState.style = Gdiplus::FontStyle::FontStyleItalic;
				else if((DWORD)(dwFlg&00040000)==00040000)
					m_datahandler.FontState.style = Gdiplus::FontStyle::FontStyleBold;
				free(found);
				if(m_datahandler.FontState.gfxfont!=nullptr)
					m_datahandler.FontState.gfxfont->decRefCnt();
				font->incRefCnt();
				m_datahandler.FontState.gfxfont = font;
			}
			else
			{
				m_datahandler.FontState.font = name;
			}
		}
	}
	else if(strcmp(cmd, "Tr")==0)
	{
		m_datahandler.FontState.rendermode = CStdstack::PopInt(args);
	}
	else if(strcmp(cmd, "Tm")==0)
	{
		m_datahandler.FontState.textmatrix.dy = CStdstack::PopDouble(args);
		m_datahandler.FontState.textmatrix.dx = CStdstack::PopDouble(args);
		m_datahandler.FontState.textmatrix.m22 = CStdstack::PopDouble(args);
		m_datahandler.FontState.textmatrix.m21 = CStdstack::PopDouble(args);
		m_datahandler.FontState.textmatrix.m12 = CStdstack::PopDouble(args);
		m_datahandler.FontState.textmatrix.m11 = CStdstack::PopDouble(args);
		m_datahandler.FontState.linestartx = 0;
		m_datahandler.FontState.linestarty = 0;
		m_datahandler.FontState.postostartx = 0;
		m_datahandler.FontState.postostarty = 0;
	}
	else if(strcmp(cmd, "Td")==0)
	{
		float dy = CStdstack::PopFloat(args);
		float dx = CStdstack::PopFloat(args);
		m_datahandler.FontState.linestartx += dx;
		m_datahandler.FontState.linestarty += dy;
		m_datahandler.FontState.postostartx = 0;
		m_datahandler.FontState.postostarty = 0;
	}
	else if(strcmp(cmd, "TD")==0)
	{
		float dy = CStdstack::PopFloat(args);
		float dx = CStdstack::PopFloat(args);
		m_datahandler.FontState.linestartx += dx;
		m_datahandler.FontState.linestarty += dy;
		m_datahandler.FontState.postostartx = 0;
		m_datahandler.FontState.postostarty = 0;
		m_datahandler.FontState.Leading = -dy;
	}
	else if(strcmp(cmd, "TL")==0)
	{
		m_datahandler.FontState.Leading = CStdstack::PopFloat(args);
	}
	else if(strcmp(cmd, "T*")==0)
	{
		m_datahandler.FontState.linestarty -= m_datahandler.FontState.Leading;
		m_datahandler.FontState.postostartx = 0;
		m_datahandler.FontState.postostarty = 0;
	}
	else if(strcmp(cmd, "Tx")==0)
	{
		m_datahandler.FontState.linestartx = 0;
		m_datahandler.FontState.linestarty -= m_datahandler.FontState.Leading;
		m_datahandler.FontState.postostartx = 0;
		m_datahandler.FontState.postostarty = 0;
	}
	else if(strcmp(cmd, "q")==0)
	{
		m_datahandler.StateStack.push(m_datahandler.CurrentState);
	}
	else if(strcmp(cmd, "Q")==0)
	{
		CClip* oldClip = m_datahandler.CurrentState.clippingpath;
		m_datahandler.CurrentState = m_datahandler.StateStack.top();
		m_datahandler.StateStack.pop();
		CClip* newClip = m_datahandler.CurrentState.clippingpath;
		if(oldClip==newClip)
		{
		}
		else if(oldClip!=nullptr&&m_datahandler.m_geomHolders.size()>1)
		{
			m_datahandler.m_geomHolders.pop();
		}
		else if(newClip!=nullptr)
		{
			m_datahandler.m_geomHolders.push(&newClip->m_geomlist);
		}
	}
	else if(strcmp(cmd, "cm")==0)
	{
		PMatrix matrix;
		matrix.dy = CStdstack::PopDouble(args);
		matrix.dx = CStdstack::PopDouble(args);
		matrix.m22 = CStdstack::PopDouble(args);
		matrix.m21 = CStdstack::PopDouble(args);
		matrix.m12 = CStdstack::PopDouble(args);
		matrix.m11 = CStdstack::PopDouble(args);
		m_datahandler.CurrentState.matrix = m_datahandler.CurrentState.matrix*matrix;
	}
	else if(strcmp(cmd, "J")==0)
	{
		m_datahandler.onStrokeCap(CStdstack::PopInt(args));
	}
	else if(strcmp(cmd, "j")==0)
	{
		m_datahandler.onStrokeJoin(CStdstack::PopInt(args));
	}
	else if(strcmp(cmd, "M")==0)
	{
		m_datahandler.onStrokeMiterlimit(CStdstack::PopInt(args));
	}
	else if(strcmp(cmd, "d")==0)
	{
		int offset = CStdstack::PopFloat(args);
		Object dashes = CStdstack::Pop(args);
		if(dashes.isArray())
		{
			std::vector<float> data;
			for(int index = 0; index<dashes.arrayGetLength(); index++)
			{
				Object dash = dashes.arrayGet(index);
				float num = dash.getNum();
				data.push_back(num);
			}
			if(data.size()>0)
				m_datahandler.onStrokeDash(data, offset);
			else
				m_datahandler.onStrokeSolid();
		}
		else
		{
			m_datahandler.onStrokeSolid();
		}
	}
	else if(strcmp(cmd, "i")==0)//flatness
	{
	}
	else if(strcmp(cmd, "w")==0)
	{
		m_datahandler.onStrokeWidth(CStdstack::PopFloat(args));
	}
	else if(strcmp(cmd, "Tc")==0)
	{
		m_datahandler.FontState.charSpace = CStdstack::PopFloat(args);//to do
	}
	else if(strcmp(cmd, "Tw")==0)
	{
		m_datahandler.FontState.wordSpace = CStdstack::PopFloat(args);
	}
	else if(strcmp(cmd, "Ts")==0)
	{
		m_datahandler.FontState.rise = CStdstack::PopFloat(args);
	}
	else if(strcmp(cmd, "Ta")==0) //set_text_align 
	{
		//	m_datahandler.onTextAlign(m_epsStack.PopInt());
	}
	else if(strcmp(cmd, "TE")==0) //set_standard_encoding 
	{
		int i = 0;
		/*encoding = list(standard_encoding);
		pos = 0;
		defs = m_epsStack.PopTo("");
		for item in defs);
		if(type(item) == IntType)
		pos = item;
		else if(type(item) == StringType)
		encoding[pos] = item;
		pos = pos + 1;
		else
		add_message('unknown item %s in encoding' % `item`);
		standard_encoding = tuple(encoding);*/
	}
	else if(strcmp(cmd, "Tz")==0)
	{
		m_datahandler.FontState.hscale = CStdstack::PopFloat(args)/100.f;
	}
	else if(strcmp(cmd, "TZ")==0) //reencode_font 
	{
		int i = 0;
		/*args = m_epsStack.PopTo("");
		if(type(args[-1]) == ListType);
		add_message(_("Multiple Master fonts not supported. "
		"Using Times Roman"))
		newname = args[-6];
		define_font('Times Roman', newname);
		else
		newname, psname, direction, script, usedefault = args[-5:]
		if(len(args) > 5)
		add_message(_("Additional encoding ignored"));
		define_font(psname, newname);*/
	}
	else if(strcmp(cmd, "Do")==0)
	{
		auto name = CStdstack::PopString(args);
		Object object = reses.lookupXObject(name.c_str());
		if(object.isNull())
			return;
		else if(object.isStream()==false)
			return;
		else
		{
			auto dict = object.streamGetDict();
			auto subtype = dict->lookup("Subtype");
			if(subtype.isName("Image"))
			{
				auto xobj = reses.lookupXObjectNF(name.c_str());
				CGeom* pImager = CImportPostscript::ExtractImage(reses, object.getStream(), false);
				if(pImager!=nullptr)
					m_datahandler.onGeomParsed(pImager);
			}
			else if(subtype.isName("Form"))
			{
				CImportPostscript::ExtractForm(xref, reses, object);
			}
			else if(subtype.isName("PS"))
			{
			}
		}
	}
	else if(strcmp(cmd, "gs")==0)
	{
		auto ext = CStdstack::PopString(args);
		CImportPostscript::ApplyExState(xref, reses, ext.c_str());
	}
	else if(strcmp(cmd, "EX")==0)//End Ignore Undef
	{
	}
	else
	{
		OutputDebugStringA(cmd);
		OutputDebugStringA("\r\n");
	}
}
void CImportPostscript::ApplyExState(XRef* xref, GfxResources& reses, const char* ext)
{
	Object dict = reses.lookupGState(ext);
	if(dict.isNull())
		return;
	if(dict.isDict()==false)
		return;

	auto bm = dict.dictLookup("BM");
	if(bm.isNull()==false) {// transparency support: blend mode, fill/stroke opacity
		GfxBlendMode mode = gfxBlendNormal;
		//	if(GfxState::parseBlendMode(&bm, &mode))
		//		m_datahandler.CurrentState.blendMode = mode;
	}
	auto lwObj = dict.dictLookup("LW");
	if(lwObj.isNum())
	{
		m_datahandler.CurrentState.line_width = lwObj.getNum();
	}
	auto lcObj = dict.dictLookup("LC");
	if(lcObj.isInt())
		m_datahandler.CurrentState.line_cap = lcObj.getIntOrInt64();
	auto ljObj = dict.dictLookup("LJ");
	if(ljObj.isInt())
		m_datahandler.CurrentState.line_join = ljObj.getIntOrInt64();
	auto mlObj = dict.dictLookup("ML");
	//	if(mlObj.isNum()) 
	//		m_datahandler.CurrentState.line_miter = mlObj.getNum();
	auto dObj = dict.dictLookup("D");
	if(dObj.isArray()&&dObj.arrayGetLength()==2)
	{
		Object args2[2];
		args2[0] = dObj.arrayGet(0);
		args2[1] = dObj.arrayGet(1);
		//	if(args2[0].isArray()&&args2[1].isNum()) 
		//		opSetDash(args2, 2);		
	}
	Object dictFont = dict.dictLookup("Font");
	if(dictFont.isArray()&&dictFont.arrayGetLength()==2)
	{
		//const Object& fargs1 = dictFont.arrayGetNF(0);
		//auto fargs2 = dictFont.arrayGet(1);
		//if(fargs1.isRef() && fargs2.isNum())
		//{
		//	auto fobj = fargs1.fetch(xref);
		//	if(fobj.isDict()) {
		//		Ref r = fargs1.getRef();
		//		auto font = GfxFont::makeFont(xref, ext, r, fobj.getDict());
		//	//	state->setFont(font, fargs1.getNum());
		//	//	fontChanged = true;
		//	}
		//}
	}
	auto ca = dict.dictLookup("ca");
	if(ca.isNum())
	{
		auto  opacity = ca.getNum();
		m_datahandler.CurrentState.entryfillopacity = opacity;
	}
	auto CA = dict.dictLookup("CA");
	if(CA.isNum())
	{
		auto  opacity = CA.getNum();
		m_datahandler.CurrentState.entrystrokeopacity = opacity;
	}
	auto op = dict.dictLookup("op");// fill overprint
	auto OP = dict.dictLookup("OP");// stroke overprint
	auto SA = dict.dictLookup("SA");// fill overprint
	if(SA.isBool())
	{
		auto  adjust = SA.getBool();
		m_datahandler.CurrentState.strokeadjust = adjust;
	}
	auto TR = dict.dictLookup("TR2");// fill overprint
	if(TR.isNull())
		TR = dict.dictLookup("TR");
	if(TR.isNull())
	{
	}
	else if(TR.isName())
	{
		const char* name = TR.getName();
		if(strcmp(name, "Default")==0||strcmp(name, "Identity")==0)
		{
			//	funcs[0] = funcs[1] = funcs[2] = funcs[3] = nullptr;
			//	state->setTransfer(funcs);		
		}
	}
	else if(TR.isArray()&&TR.arrayGetLength()==4)
	{
		for(int i = 0; i<4; i++)
		{
			auto func = TR.arrayGet(i);
			//		funcs[i] = Function::parse(&obj3);
			//		if(func==nullptr)
			//			breal;
		}
		//		auto obj3, obj2.arrayGet(i);
	}
	else if(TR.isName()||TR.isDict()||TR.isStream()) {
		//	if((funcs[0] = Function::parse(&obj2))) {
		//		funcs[1] = funcs[2] = funcs[3] = nullptr;
		//		state->setTransfer(funcs);
		//	}
	}
	else if(TR.isNull()==false)
	{
		//	error(errSyntaxError, getPos(), "Invalid transfer function in ExtGState");
	}

	auto softmask = dict.dictLookup("SMask");
	if(softmask.isNull())
	{
	}
	else if(softmask.isName("None"))
	{
		//		builder->clearSoftMask(state);
	}
	else if(softmask.isDict()) {
		auto s = softmask.dictLookup("S");
		if(s.isName("Alpha"))
		{
			//alpha = true;
		}
		else  // "Luminosity"
		{
			//alpha = false;
		}
		auto TR = softmask.dictLookup("TR");
		if(TR.isNull()==false)
		{
			//			funcs[0] = Function::parse(&obj3);
			//			if(funcs[0]->getInputSize()!=1||
	//				funcs[0]->getOutputSize()!=1) {
	//				error(errSyntaxError, getPos(), "Invalid transfer function in soft mask in ExtGState");
	//				delete funcs[0];
	//				funcs[0] = nullptr;
	//			}
		}
		auto BC = softmask.dictLookup("BC");
		if(BC.isArray())
		{	//		if((haveBackdropColor = _POPPLER_CALL_ARGS_DEREF(obj3, obj2.dictLookup, "BC").isArray())) {
	//			for(int& i:backdropColor.c) {
	//				i = 0;
	//			}
	//			for(int i = 0; i<obj3.arrayGetLength()&&i<gfxColorMaxComps; ++i) {
	//				auto obj4, obj3.arrayGet(i);
	//				if(obj4.isNum()) {
	//					backdropColor.c[i] = dblToCol(obj4.getNum());
	//				}
	//			}
		}
		auto G = softmask.dictLookup("G");
		if(G.isStream())
		{
			auto group = G.streamGetDict()->lookup("Group");
			if(group.isDict())
			{
				//				GfxColorSpace* blendingColorSpace = nullptr;
				//				bool isolated = false;
				//				bool knockout = false;
				//				if(!_POPPLER_CALL_ARGS_DEREF(obj5, obj4.dictLookup, "CS").isNull()) {
				//					blendingColorSpace = GfxColorSpace::parse(nullptr, &obj5, nullptr, state);
				//				}

				//				if(_POPPLER_CALL_ARGS_DEREF(obj5, obj4.dictLookup, "I").isBool()) {
				//					isolated = obj5.getBool();
				//				}

				//				if(_POPPLER_CALL_ARGS_DEREF(obj5, obj4.dictLookup, "K").isBool()) {
				//					knockout = obj5.getBool();
				//				}

				//				if(!haveBackdropColor) {
				//					if(blendingColorSpace) {
				//						blendingColorSpace->getDefaultColor(&backdropColor);
				//					}
				//					else {
				//						//~ need to get the parent or default color space (?)
				//						for(int& i:backdropColor.c) {
				//							i = 0;
				//						}
				//					}
				//				}
				//				doSoftMask(&obj3, alpha, blendingColorSpace,
				//					isolated, knockout, funcs[0], &backdropColor);
				//				if(funcs[0]) {
				//					delete funcs[0];
				//				}
			}
			//			else {
			//				error(errSyntaxError, getPos(), "Invalid soft mask in ExtGState - missing group");
			//			}
		}
		//		else {
		//			error(errSyntaxError, getPos(), "Invalid soft mask in ExtGState - missing group");
		//		}
	}
	//	else if(!obj2.isNull()) {
	//		error(errSyntaxError, getPos(), "Invalid soft mask in ExtGState");
	//	}
}
CGeom* CImportPostscript::ExtractImage(GfxResources& reses, Stream* stream, bool inlineImg)
{
	StreamColorSpaceMode csMode = StreamColorSpaceMode::streamCSNone;
	int bits = 0;
	stream->getImageParams(&bits, &csMode);

	Dict* dict = stream->getDict();

	int imgWidth;
	auto iwObj = dict->lookup("Width");
	if(iwObj.isNull())
		iwObj = dict->lookup("W");
	if(iwObj.isInt())
		imgWidth = iwObj.getInt();
	else if(iwObj.isReal())
		imgWidth = (int)iwObj.getReal();
	else
		return nullptr;

	int imgHeight;
	auto ihObj = dict->lookup("Height");
	if(ihObj.isNull())
		ihObj = dict->lookup("H");
	if(ihObj.isInt())
		imgHeight = ihObj.getInt();
	else if(ihObj.isReal())
		imgHeight = static_cast<int>(ihObj.getReal());
	else
		return nullptr;

	bool imgInterpolate;
	auto ipObj = dict->lookup("Interpolate");
	if(ipObj.isNull())
		ipObj = dict->lookup("I");
	if(ipObj.isBool())
		imgInterpolate = ipObj.getBool();
	else
		imgInterpolate = false;

	bool mask = false;
	auto imObj = dict->lookup("ImageMask");// image or mask?
	if(imObj.isNull())
		imObj = dict->lookup("IM");
	if(imObj.isBool())
		mask = imObj.getBool();
	else if(imObj.isNull()==false)
		return nullptr;

	// bit depth
	if(bits==0) {
		auto bpcObj = dict->lookup("BitsPerComponent");
		if(bpcObj.isNull())
			bpcObj = dict->lookup("BPC");
		if(bpcObj.isInt())
			bits = bpcObj.getInt();
		else if(mask)
			bits = 1;
		else
			return nullptr;
	}
	if(mask&&bits!=1)
		return nullptr;

	if(mask) {// display a mask
		bool invert = false;
		auto ivtObj = dict->lookup("Decode");
		if(ivtObj.isNull())
			ivtObj = dict->lookup("D");
		if(ivtObj.isArray()) {
			auto obj = ivtObj.arrayGet(0);
			if(obj.isInt()&&obj.getInt()==1) {
				invert = true;
			}
		}
		else if(ivtObj.isNull()==false)
			return nullptr;

		// draw it
	//	builder->addImageMask(state, imgStream, imgWidth, imgHeight, invert, imgInterpolate);
	}
	else {// get color space and color map		
		auto csObj = dict->lookup("ColorSpace");
		if(csObj.isNull())
			csObj = dict->lookup("CS");

		GfxColorSpace* colorSpace = nullptr;
		if(csObj.isNull()==false)
			colorSpace = CImportPostscript::lookupColorSpaceCopy(reses, csObj);
		else if(csMode==StreamColorSpaceMode::streamCSDeviceGray)
			colorSpace = new GfxDeviceGrayColorSpace();
		else if(csMode==StreamColorSpaceMode::streamCSDeviceRGB)
			colorSpace = new GfxDeviceRGBColorSpace();
		else if(csMode==StreamColorSpaceMode::streamCSDeviceCMYK)
			colorSpace = new GfxDeviceCMYKColorSpace();
		if(colorSpace==nullptr)
			return nullptr;

		auto dcObj = dict->lookup("Decode");
		if(dcObj.isNull())
			dcObj = dict->lookup("D");

		GfxImageColorMap* imgColorMap = new GfxImageColorMap(bits, &dcObj, colorSpace);
		if(imgColorMap->isOk()==false)
		{
			delete imgColorMap;
			return  nullptr;
		}

		// get the mask
		auto maskObj = dict->lookup("Mask");
		auto smskObj = dict->lookup("SMask");
		if(smskObj.isStream()) {// soft mask			
			if(inlineImg)
				return nullptr;
			Stream* mskStream = smskObj.getStream();
			Dict* maskDict = smskObj.streamGetDict();
			auto mwObj = maskDict->lookup("Width");
			if(mwObj.isNull())
				mwObj = maskDict->lookup("W");
			if(mwObj.isInt()==false)
				return nullptr;
			int mskWidth = mwObj.getInt();

			auto mhObj = maskDict->lookup("Height");
			if(mhObj.isNull())
				mhObj = maskDict->lookup("H");
			if(mhObj.isInt()==false)
				return nullptr;
			int mskHeight = mhObj.getInt();

			auto mbpcObj = maskDict->lookup("BitsPerComponent");
			if(mbpcObj.isNull())
				mbpcObj = maskDict->lookup("BPC");
			if(!mbpcObj.isInt())
				return nullptr;
			int maskBits = mbpcObj.getInt();

			auto mitpObj = maskDict->lookup("Interpolate");
			if(mitpObj.isNull())
				mitpObj = maskDict->lookup("I");

			bool mskInterpolate = mitpObj.isBool() ? mitpObj.getBool() : false;

			auto csObj = maskDict->lookup("ColorSpace");
			if(csObj.isNull())
				csObj = maskDict->lookup("CS");
			GfxColorSpace* maskColorSpace = CImportPostscript::lookupColorSpaceCopy(reses, csObj);
			if(!maskColorSpace||maskColorSpace->getMode()!=csDeviceGray)
				return nullptr;

			auto mdcObj = maskDict->lookup("Decode");
			if(mdcObj.isNull())
				mdcObj = maskDict->lookup("D");

			auto mskColorMap = new GfxImageColorMap(maskBits, &mdcObj, maskColorSpace);
			if(mskColorMap->isOk())
			{
				CDib* pDib1 = CImportPostscript::CreateImage(mskStream, mskWidth, mskHeight, mskColorMap, mskInterpolate, nullptr, true, false);
				CDib* pDib2 = CImportPostscript::CreateImage(stream, imgWidth, imgHeight, imgColorMap, imgInterpolate, nullptr, false, false);
				delete mskColorMap;

				if(pDib1!=nullptr&&pDib2!=nullptr)
				{
					Gdiplus::PointF tl1 = m_datahandler.CurrentState.matrix.TransformPoint(0.f, 0.f);
					Gdiplus::PointF br1 = m_datahandler.CurrentState.matrix.TransformPoint(1.f, 1.f);
					auto tl2 = Change(tl1.X, tl1.Y);
					auto br2 = Change(br1.X, br1.Y);
					CRect  rect(tl2.x, tl2.y, br2.x, br2.y);
					rect.NormalizeRect();
					CImager* pImager = new CImager(rect, pDib2);

					CMask* pMask = new CMask(pDib1);
					pMask->AddMember(pImager);
					return pMask;
				}
				else if(pDib1!=nullptr)
				{
					delete pDib1;
					pDib1 = nullptr;
				}
				else if(pDib2!=nullptr)
				{
					Gdiplus::PointF tl1 = m_datahandler.CurrentState.matrix.TransformPoint(0.f, 0.f);
					Gdiplus::PointF br1 = m_datahandler.CurrentState.matrix.TransformPoint(1.f, 1.f);
					auto tl2 = Change(tl1.X, tl1.Y);
					auto br2 = Change(br1.X, br1.Y);
					CRect  rect(tl2.x, tl2.y, br2.x, br2.y);
					rect.NormalizeRect();
					return  new CImager(rect, pDib2);
				}
			}
			else
			{
				delete mskColorMap;
				return nullptr;
			}
		}
		else if(maskObj.isArray()) {// color key mask
			int maskColors[2*gfxColorMaxComps];
			for(int i = 0; i<maskObj.arrayGetLength()&&i<2*gfxColorMaxComps; ++i) {
				auto obj1 = maskObj.arrayGet(i);
				maskColors[i] = obj1.getInt();
			}
			//			builder->addImage(stream, imgWidth, imgHeight, imgColorMap, imgInterpolate, haveColorKeyMask ? maskColors : static_cast<int*>(nullptr));
		}
		else if(maskObj.isStream()) {// explicit mask			
			if(inlineImg)
				return nullptr;
			Stream* mskStream = maskObj.getStream();
			auto maskDict = maskObj.streamGetDict();
			auto mwObj = maskDict->lookup("Width");
			if(mwObj.isNull())
				mwObj = maskDict->lookup("W");
			if(mwObj.isInt()==false)
				return nullptr;
			int mskWidth = mwObj.getInt();

			auto mhObj = maskDict->lookup("Height");
			if(mhObj.isNull())
				mhObj = maskDict->lookup("H");
			if(mhObj.isInt()==false)
				return nullptr;
			int mskHeight = mhObj.getInt();

			auto mitpObj = maskDict->lookup("Interpolate");
			if(mitpObj.isNull())
				mitpObj = maskDict->lookup("I");
			bool mskInterpolate = mitpObj.isBool() ? mitpObj.getBool() : false;

			auto mdObj = maskDict->lookup("Decode");
			if(mdObj.isNull())
				mdObj = maskDict->lookup("D");

			bool maskInvert = false;
			if(mdObj.isArray()) {
				auto first = mdObj.arrayGet(0);
				if(first.isInt()&&first.getInt()==1) {
					maskInvert = true;
				}
			}
			else if(mdObj.isNull()==false)
				return nullptr;
			//			builder->addMaskedImage(stream, imgWidth, imgHeight, imgColorMap, imgInterpolate, mskStream, mskWidth, mskHeight, maskInvert, mskInterpolate);
		}
		else
		{
			CDib* pDib = CImportPostscript::CreateImage(stream, imgWidth, imgHeight, imgColorMap, imgInterpolate, nullptr, false, false);
			Gdiplus::PointF tl1 = m_datahandler.CurrentState.matrix.TransformPoint(0.f, 0.f);
			Gdiplus::PointF br1 = m_datahandler.CurrentState.matrix.TransformPoint(1.f, 1.f);
			auto tl2 = Change(tl1.X, tl1.Y);
			auto br2 = Change(br1.X, br1.Y);
			CRect  rect(tl2.x, tl2.y, br2.x, br2.y);
			rect.NormalizeRect();
			delete imgColorMap;
			return  new CImager(rect, pDib);
		}
	}
	return nullptr;
}

GfxColorSpace* CImportPostscript::lookupColorSpaceCopy(GfxResources& reses, Object& arg)
{
	const Object* argPtr = &arg;
	Object obj;
	char const* name = arg.isName() ? arg.getName() : nullptr;
	if(name!=nullptr)
	{
		if(GfxColorSpace* colorSpace = colorSpacesCache[name]; colorSpace!=nullptr)
			return colorSpace->copy();

		if(obj = reses.lookupColorSpace(name); obj.isNull()==false)
			argPtr = &obj;
	}

	const PDFRectangle box(0, 0, 0, 0);
	GfxState state(72.0, 72.0, &box, 0, true);
	GfxColorSpace* colorSpace = GfxColorSpace::parse(&reses, &arg, nullptr, &state, 0);
	if(name!=nullptr&&colorSpace!=nullptr)
	{
		colorSpacesCache[name] = colorSpace->copy();
	}
	return colorSpace;
}

void CImportPostscript::ExtractForm(XRef* xref, GfxResources& reses, Object& content)
{
	Dict* dict = content.streamGetDict();
	auto obj1 = dict->lookup("FormType");
	if(!(obj1.isNull()||(obj1.isInt()&&obj1.getInt()==1)))
		return;

	auto bboxObj = dict->lookup("BBox");
	if(bboxObj.isArray()==false)
		return;

	double bbox[4];
	bbox[0] = bboxObj.arrayGet(0).getNum();
	bbox[1] = bboxObj.arrayGet(1).getNum();
	bbox[2] = bboxObj.arrayGet(2).getNum();
	bbox[3] = bboxObj.arrayGet(3).getNum();

	PMatrix matrix;
	auto mObj = dict->lookup("Matrix");
	if(mObj.isArray())
	{
		matrix.m11 = mObj.arrayGet(0).getNum();
		matrix.m12 = mObj.arrayGet(1).getNum();
		matrix.m21 = mObj.arrayGet(2).getNum();
		matrix.m22 = mObj.arrayGet(3).getNum();
		matrix.dx = mObj.arrayGet(4).getNum();
		matrix.dy = mObj.arrayGet(5).getNum();
	}
	bool transpGroup = false;
	bool isolated = false;
	bool knockout = false;
	GfxColorSpace* blendingColorSpace = nullptr;

	auto gObj = dict->lookup("Group");
	if(gObj.isDict())
	{
		auto obj2 = gObj.dictLookup("S");
		if(obj2.isName("Transparency"))
		{
			transpGroup = true;
			{
				auto obj3 = gObj.dictLookup("CS");
				//	if(obj3.isNull())
				//		blendingColorSpace = GfxColorSpace::parse(nullptr, &obj3, nullptr, state);
			}
			{
				auto obj3 = gObj.dictLookup("I");
				if(obj3.isBool())
					isolated = obj3.getBool();
			}
			{
				auto obj3 = gObj.dictLookup("K");
				if(obj3.isBool())
					knockout = obj3.getBool();
			}
		}
	}


	auto resObj = dict->lookup("Resources");
	auto resDict = resObj.isDict() ? resObj.getDict() : (Dict*)nullptr;
	CImportPostscript::ExtractForm1(xref, reses, content, resDict, matrix, bbox, transpGroup, false, blendingColorSpace, isolated, knockout, false, nullptr, nullptr);

	if(blendingColorSpace!=nullptr) {
		delete blendingColorSpace;
	}
}

void CImportPostscript::ExtractForm1(XRef* xref, GfxResources& reses, Object& content, Dict* resDict, PMatrix& matrix, double* bbox, bool transpGroup, bool softMask, GfxColorSpace* blendingColorSpace, bool isolated, bool knockout, bool alpha, Function* transferFunc, GfxColor* backdropColor)
{
	m_datahandler.StateStack.push(m_datahandler.CurrentState);

	GfxResources newreses(xref, resDict, &reses);
	m_datahandler.onPathDiscard();

	//ccy	if(softMask||transpGroup) {
	//ccy		builder->clearSoftMask(state);
	//ccy		builder->pushTransparencyGroup(state, bbox, blendingColorSpace, isolated, knockout, softMask);
	//ccy	}
	m_datahandler.CurrentState.layerfillopacity = 1.0;
	m_datahandler.CurrentState.entryfillopacity = 1.0;
	m_datahandler.CurrentState.layerstrokeopacity = 1.0;
	m_datahandler.CurrentState.entrystrokeopacity = 1.0;
	m_datahandler.CurrentState.softmask = nullptr;

	m_datahandler.CurrentState.matrix = m_datahandler.CurrentState.matrix*matrix;
	m_datahandler.onPathMoveTo(bbox[0], bbox[1]);
	m_datahandler.onPathLineTo(bbox[2], bbox[1]);
	m_datahandler.onPathLineTo(bbox[2], bbox[3]);
	m_datahandler.onPathLineTo(bbox[0], bbox[3]);
	m_datahandler.onPathClose(true);
	m_datahandler.onClipStart(Gdiplus::FillMode::FillModeAlternate);
	//ccy	if(softMask||transpGroup) {
	//ccy		if(state->getBlendMode()!=gfxBlendNormal)
	//ccy			state->setBlendMode(gfxBlendNormal);
	//ccy
	//ccy		if(state->getFillOpacity()!=1) {
	//ccy			builder->setGroupOpacity(state->getFillOpacity());
	//ccy			state->setFillOpacity(1);
	//ccy		}
	//ccy		if(state->getStrokeOpacity()!=1) {
	//ccy			state->setStrokeOpacity(1);
	//ccy		}
	//ccy	}

		// draw the form
	CImportPostscript::Parse(xref, newreses, content);
	m_datahandler.onCollectionEnd();
	//ccy	if(softMask||transpGroup) {
	//ccy		builder->popTransparencyGroup(state);
	//ccy	}
	//ccy

	m_datahandler.CurrentState = m_datahandler.StateStack.top();
	m_datahandler.StateStack.pop();

	//ccy	if(softMask) {
	//ccy		builder->setSoftMask(state, bbox, alpha, transferFunc, backdropColor);
	//ccy	}
	//ccy	else if(transpGroup) {
	//ccy		builder->paintTransparencyGroup(state, bbox);
	//ccy	}
}

void CImportPostscript::ExtractSoftMask(Object* stream, bool alpha, GfxColorSpace* blendingColorSpace, bool isolated, bool knockout, Function* transferFunc, GfxColor* backdropColor)
{

}
void png_write_vector(png_structp png_ptr, png_bytep data, png_size_t length)
{
	auto* v_ptr = reinterpret_cast<std::vector<unsigned char> *>(png_get_io_ptr(png_ptr)); // Get pointer to stream
	for(unsigned i = 0; i<length; i++) {
		v_ptr->push_back(data[i]);
	}
}
CDib* CImportPostscript::CreateImage(Stream* stream, int width, int height, GfxImageColorMap* color_map, bool interpolation, int* mask_colors, bool alpha_only, bool invert_alpha)
{
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);// Create PNG write struct
	if(png_ptr==nullptr)
		return nullptr;

	png_infop info_ptr = png_create_info_struct(png_ptr);// Create PNG info struct
	if(info_ptr==nullptr)
	{
		png_destroy_write_struct(&png_ptr, nullptr);
		return nullptr;
	}
	if(setjmp(png_jmpbuf(png_ptr))) // Set error handler
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return nullptr;
	}

	std::vector<unsigned char> png_buffer;// Set read/write functions
	png_set_write_fn(png_ptr, &png_buffer, png_write_vector, nullptr);
	if(!invert_alpha&&!alpha_only) // Set header data
	{
		png_set_invert_alpha(png_ptr);
	}

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, alpha_only ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_color_8 sig_bit = {alpha_only ? 0 : 8, alpha_only ? 0 : 8, alpha_only ? 0 : 8, alpha_only ? 8 : 0, alpha_only ? 0 : 8};

	png_set_sBIT(png_ptr, info_ptr, &sig_bit);
	png_set_bgr(png_ptr);
	png_write_info(png_ptr, info_ptr);// Write the file header

	// Convert pixels
	ImageStream* image_stream;
	if(alpha_only) {
		image_stream = color_map ? new ImageStream(stream, width, color_map->getNumPixelComps(), color_map->getBits()) : new ImageStream(stream, width, 1, 1);
		image_stream->reset();

		// Convert grayscale values
		unsigned char* buffer = new unsigned char[width];
		const int invert_bit = invert_alpha ? 1 : 0;
		for(int y = 0; y<height; y++) {
			unsigned char* row = image_stream->getLine();
			if(color_map)
				color_map->getGrayLine(row, buffer, width);
			else {
				unsigned char* buf_ptr = buffer;
				for(int x = 0; x<width; x++)
				{
					*buf_ptr++ = (row[x]^invert_bit) ? 0X00 : 0XFF;
				}
			}
			png_write_row(png_ptr, (png_bytep)buffer);
		}
		delete[] buffer;
	}
	else if(color_map) {
		image_stream = new ImageStream(stream, width, color_map->getNumPixelComps(), color_map->getBits());
		image_stream->reset();

		// Convert RGB values
		unsigned int* buffer = new unsigned int[width];
		if(mask_colors) {
			for(int y = 0; y<height; y++) {
				unsigned char* row = image_stream->getLine();
				color_map->getRGBLine(row, buffer, width);

				unsigned int* dest = buffer;
				for(int x = 0; x<width; x++) {
					// Check each color component against the mask
					for(int i = 0; i<color_map->getNumPixelComps(); i++) {
						if(row[i] < mask_colors[2*i]*255||
							row[i] > mask_colors[2*i+1]*255) {
							*dest = *dest|0xff000000;
							break;
						}
					}
					// Advance to the next pixel
					row += color_map->getNumPixelComps();
					dest++;
				}
				// Write it to the PNG
				png_write_row(png_ptr, (png_bytep)buffer);
			}
		}
		else {
			for(int i = 0; i<height; i++) {
				unsigned char* row = image_stream->getLine();
				memset((void*)buffer, 0xff, sizeof(int)*width);
				color_map->getRGBLine(row, buffer, width);
				png_write_row(png_ptr, (png_bytep)buffer);
			}
		}
		delete[] buffer;
	}
	else {    // A colormap must be provided, so quit
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return nullptr;
	}
	delete image_stream;
	stream->close();

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return CDib::FromData(FREE_IMAGE_FORMAT::FIF_PNG, png_buffer.data(), png_buffer.size());
}