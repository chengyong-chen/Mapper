#include "Stdafx.h"
#include "ImportSVG.h"
#include "Style.h"
#include "Element.h"

#include "../Image/Dib.h"
#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Group.h"
#include "../Geometry/Text.h"
#include "../Geometry/Imager.h"
#include "../Public/Base64.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"


#include <cstdlib>
#include <cstring>
#include <map>
#include <iostream>

#include "../../ThirdParty/boost/1.83.0/boost/foreach.hpp"
#include "../../ThirdParty/boost/1.83.0/boost/tokenizer.hpp"
#include "../../ThirdParty/boost/1.83.0/boost/algorithm/string/predicate.hpp"
#include "../../ThirdParty/boost/1.83.0/boost/archive/iterators/base64_from_binary.hpp"

using namespace boost;
using namespace std;
using namespace tinyxml2;
using namespace StyleSheet;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImportSVG::CImportSVG(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomID)
	: CImport(datainfo, layertree, dwMaxGeomID), m_datahandler(dwMaxGeomID)
{
	m_datahandler.SetCallBack(this/*,&CImport::OnGeomParsed,&CImport::Change*/);
}

CImportSVG::~CImportSVG()
{
}

BOOL CImportSVG::Import(LPCTSTR lpszFile)
{
	CT2CA caFile(lpszFile);
	tinyxml2::XMLDocument xmldoc;
	tinyxml2::XMLError eResult = xmldoc.LoadFile(caFile);
	if(eResult!=tinyxml2::XML_SUCCESS)
		return FALSE;

	tinyxml2::XMLElement* root = xmldoc.FirstChildElement("svg");
	if(root!=nullptr)
	{
		const char* str1 = root->Attribute("x");
		const char* str2 = root->Attribute("y");
		const char* str3 = root->Attribute("width");
		const char* str4 = root->Attribute("height");

		CSS_Double data1;
		CSS_Double data2;
		CSS_Double data3;
		CSS_Double data4;
		read_doublewithunit(data1, str1);
		read_doublewithunit(data2, str2);
		read_doublewithunit(data3, str3);
		read_doublewithunit(data4, str4);
		if(data1.status==data2.status==data3.status==data4.status==Value_Status::WithValue)
		{
			m_datainfo.m_mapOrigin.x;
			m_datainfo.m_mapOrigin.y;
			m_datainfo.m_mapCanvas.cx;
			m_datainfo.m_mapCanvas.cy;
		}
		m_fXScale = 10000;
		m_fYScale = -10000;
		m_fXOffset = 0;
		m_fYOffset = 0;

		CLayer* pLayer = new CLayer(m_layertree);
		pLayer->m_wId = m_layertree.ApplyId();
		m_layertree.m_root.AddChild(nullptr, pLayer);
		m_datahandler.m_geomHolders.push(&pLayer->m_geomlist);

		CImportSVG::parseSvg(root);
		pLayer->RecalRect();
		m_fYScale = 10000;
		return CImport::Import(lpszFile);
	}
	else
		return FALSE;
}
void CImportSVG::parseSvg(tinyxml2::XMLElement* xml)
{
	for(tinyxml2::XMLElement* node = xml->FirstChildElement(); node!=0; node = node->NextSiblingElement())
	{
		if(!strcmp(node->Value(), "symbol"))
			continue;
		else if(!strcmp(node->Value(), "defs"))
		{
			for(tinyxml2::XMLElement* def = node->FirstChildElement("style"); def!=0; def = def->NextSiblingElement("style"))
			{
				StyleSheet::Style style = StyleSheet::Style::parse(def->GetText());
				m_cssstyles.push_back(style);
			}
		}
		else if(!strcmp(node->Value(), "metadata"))
			continue;
		else if(!strcmp(node->Value(), "sodipodi:namedview"))
			continue;
		else if(!strcmp(node->Value(), "svg"))
			continue;
		else
			parseElement(node);
	}
}

void CImportSVG::parseElement(tinyxml2::XMLElement* element)
{
	if(element==nullptr)
		return;

	const char* display = element->Attribute("display");
	if(display!=nullptr&&strcmp(display, "none")==0)
		return;
	const char* style = element->Attribute("style");
	if(style!=nullptr&&strstr(style, "display:none")!=nullptr)
		return;
	const char* visibility = element->Attribute("visibility");
	if(visibility!=nullptr&&strcmp(visibility, "hidden")==0)
		return;
	if(visibility!=nullptr&&strcmp(visibility, "collapse")==0)
		return;

	parse_transform(element);
	parse_style(element);
	m_datahandler.properties = parse_attribute(element);

	const char* type = strstrip(element->Value());
	if(!strcmp(type, "g"))
		parse_group(element);
	else if(!strcmp(type, "a"))
		parse_group(element);
	else if(!strcmp(type, "path"))
		parse_path(element);
	else if(!strcmp(type, "polyline"))
		parse_poly(element, false);
	else if(!strcmp(type, "polygon"))
		parse_poly(element, true);
	else if(!strcmp(type, "line"))
		parse_line(element);
	else if(!strcmp(type, "circle"))
		parse_circle(element);
	else if(!strcmp(type, "ellipse"))
		parse_ellipse(element);
	else if(!strcmp(type, "rect"))
		parse_rect(element);
	else if(!strcmp(type, "text"))
		parse_text(element);
	else if(!strcmp(type, "image"))
		parse_image(element);
	else if(!strcmp(type, "radialGradient"))
		parse_gradient(element);
	else if(!strcmp(type, "linearGradient"))
		parse_gradient(element);
	else if(!strcmp(type, "use"))
		parse_use(element);
	else if(!strcmp(type, "clipPath"))
	{
	}
	else if(!strcmp(type, "mask"))
	{
	}
	//else if(!strcmp(type, "symbol"))
	//{
	/*if(m_layertree.m_library.m_libSpotSymbol.m_midlist.size() == 0)
	{
	m_layertree.m_library.m_libSpotSymbol.m_midlist[0]= new CSymbolTag<CSymbolSpot>();
	}
	CSymbolTag<CSymbolSpot>* tag = m_layertree.m_library.m_libSpotSymbol.m_midlist[0];
	tag->m_wId = 0;

	const char* id = element->Attribute("id");
	if(id != nullptr)
	{
	CLayer* oldLayer = m_pLayer;
	CLayer layer(m_layertree);
	m_pLayer = &layer;

	CSSStrokeStyle strokestyle;
	CSSFillStyle fillstyle(nullptr);
	CSSTextStyle textstyle;
	for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != 0; child = child->NextSiblingElement())
	{
	parseElement(child,strokestyle,fillstyle,textstyle);
	}

	CSymbolSpot* pSymbol = new CSymbolSpot();
	m_symbols.insert(std::make_pair(id,pSymbol));
	pSymbol->m_wId = m_symbols.size();
	tag->m_Symbollist.AddTail(pSymbol);

	POSITION pos = layer.m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
	CGeom* pGeom = layer.m_geomlist.GetNext(pos);
	pSymbol->m_geomlist.AddTail(pGeom);
	}
	pSymbol->RecalRect();
	layer.m_geomlist.RemoveAll();

	m_pLayer = oldLayer;
	}*/
	//} 
	//else if(!strcmp(type, "use"))
	//{
	//	bool bSymbol = false;
	//	tinyxml2::XMLNode* parent = element->Parent();
	//	while(parent != nullptr)
	//	{
	//		if(parent->Value() == nullptr)
	//			break;
	//		if(!strcmp(parent->Value(),"symbol"))
	//		{
	//			bSymbol = true;
	//			break;
	//		}

	//		parent = parent->Parent();
	//	}
	//	const char* href = element->Attribute("xlink:href");
	//	if(href != nullptr && bSymbol == false)
	//	{
	//		std::map<string, CSymbolSpot*>::iterator it = m_symbols.find(href+1);
	//		if(it != m_symbols.end())
	//		{
	//			CSymbolSpot* pSymbolSpot = it->second;
	//			if(pSymbolSpot != nullptr)
	//			{				
	//				CSpotSymbol* pSpot = new CSpotSymbol();
	//				pSpot->m_libId = 0;
	//				pSpot->m_symId = pSymbolSpot->m_wId;

	//				double x=0;
	//				double y=0;
	//				element->QueryDoubleAttribute("x", &x);
	//				element->QueryDoubleAttribute("y", &y);
	//				CPoint point = Change(x,y);
	//				CMark* pMark = new CMark(point);
	//				pMark->m_pSpot = pSpot;
	//				pMark->CalCorner(nullptr,m_datainfo,m_layertree.m_library);
	//				m_datahandler.onGeomBegin(pMark);	
	//				m_datahandler.onGeomEnd(nullptr,nullptr);
	//			}
	//		}
	//	}
	//}
	else
	{
		for(tinyxml2::XMLElement* child = element->FirstChildElement(); child!=0; child = child->NextSiblingElement())
		{
			parseElement(child);
		}
	}
}

void CImportSVG::parse_use(tinyxml2::XMLElement* element)
{
	const char* href = element->Attribute("xlink:href");
	if(href==nullptr)
		return;

	const char* id = href+1;
	CGroup* pGroup = new CGroup(nullptr, nullptr);
	tinyxml2::XMLElement* root = element->GetDocument()->FirstChildElement("svg");
	parse_xlink(root, id);

	double x = 0;
	double y = 0;
	element->QueryDoubleAttribute("x", &x);
	element->QueryDoubleAttribute("y", &y);
	const CPoint point = Change(x, y);
	const double m11 = 1;
	const double m21 = 0;
	const double m31 = point.x;
	const double m12 = 0;
	const double m22 = 1;
	const double m32 = point.y;

	pGroup->Transform(m11, m21, m31, m12, m22, m32);

	m_datahandler.m_geomHolders.top()->AddTail(pGroup);
}

void CImportSVG::parse_xlink(tinyxml2::XMLElement* root, const char* id)
{
	tinyxml2::XMLElement* linked = FindElement(root, id);
	if(linked==nullptr)
		return;

	parseElement(linked);

	const char* type = strstrip(linked->Value());
	if(!strcmp(type, "symbol"))
	{
		char* str = (char*)linked->Attribute("viewBox");
		if(str!=nullptr)
		{
			const double orgX = d_string_to_double(str, &str);
			const double orgY = d_string_to_double(str, &str);
			double width = d_string_to_double(str, &str);
			double height = d_string_to_double(str, &str);
			const double m11 = 1;
			const double m21 = 0;
			const double m31 = -orgX*m_fXScale;
			const double m12 = 0;
			const double m22 = 1;
			const double m32 = -orgY*m_fYScale;

			//		m_datahandler.onTransformMatrix(pGeom, m11, m21, m31, m12, m22, m32);
		}
	}
}

void CImportSVG::parse_group(tinyxml2::XMLElement* element)
{
	m_datahandler.StateStack.push(m_datahandler.CurrentState);

	m_datahandler.onGroupBegin();
	for(tinyxml2::XMLElement* child = element->FirstChildElement(); child!=0; child = child->NextSiblingElement())
	{
		parseElement(child);
	}
	m_datahandler.onCollectionEnd();

	m_datahandler.StateStack.pop();
}

void CImportSVG::parse_path(tinyxml2::XMLElement* element)
{
	const char* str = element->Attribute("d");
	if(str==nullptr)
		return;


	double startX = -1;
	double startY = -1;
	double prevX = 0;
	double prevY = 0;
	const double eps = std::numeric_limits<double>::epsilon();
	char state = 'e';
	nextState((const char**)&str, &state);
	while(state!='e')
	{
		switch(state)
		{
			case 'm':
			case 'M':
			{
				if(abs(prevX-startX)<eps&&abs(prevY-startY)<eps)
					m_datahandler.onPathClose(false);

				const double x = d_string_to_double(str, (char**)&str)+(state=='m' ? prevX : 0);
				const double y = d_string_to_double(str, (char**)&str)+(state=='m' ? prevY : 0);
				if(isnan(x)||isnan(y))
					break;

				if(abs(x-prevX)>eps||abs(y-prevY)>eps)
					m_datahandler.onPathMoveTo(x, y);

				state = state=='m' ? 'l' : 'L';
				prevX = x;
				prevY = y;
				startX = x;
				startY = y;
			}
			break;
			case 'l':
			case 'L':
			{
				const double x = d_string_to_double(str, (char**)&str)+(state=='l' ? prevX : 0);
				const double y = d_string_to_double(str, (char**)&str)+(state=='l' ? prevY : 0);
				if(isnan(x)||isnan(y))
					break;

				m_datahandler.onPathLineTo(x, y);
				prevX = x;
				prevY = y;
			}
			break;
			case 'h':
			case 'H':
			{
				const double x = d_string_to_double(str, (char**)&str)+(state=='h' ? prevX : 0);
				if(isnan(x))
					break;

				m_datahandler.onPathLineTo(x, prevY);
				prevX = x;
			}
			break;
			case 'v':
			case 'V':
			{
				const double y = d_string_to_double(str, (char**)&str)+(state=='v' ? prevY : 0);
				if(isnan(y))
					break;

				m_datahandler.onPathLineTo(prevX, y);
				prevY = y;
			}
			break;
			case 'c':
			case 'C':
			{
				const double x1 = d_string_to_double(str, (char**)&str)+(state=='c' ? prevX : 0);
				const double y1 = d_string_to_double(str, (char**)&str)+(state=='c' ? prevY : 0);
				const double x2 = d_string_to_double(str, (char**)&str)+(state=='c' ? prevX : 0);
				const double y2 = d_string_to_double(str, (char**)&str)+(state=='c' ? prevY : 0);
				const double x3 = d_string_to_double(str, (char**)&str)+(state=='c' ? prevX : 0);
				const double y3 = d_string_to_double(str, (char**)&str)+(state=='c' ? prevY : 0);
				m_datahandler.onPathCurvetoc(x1, y1, x2, y2, x3, y3);
				prevX = x3;
				prevY = y3;
			}
			break;
			case 's':
			case 'S':
			{
				const double x2 = d_string_to_double(str, (char**)&str)+(state=='s' ? prevX : 0);
				const double y2 = d_string_to_double(str, (char**)&str)+(state=='s' ? prevY : 0);
				const double x3 = d_string_to_double(str, (char**)&str)+(state=='s' ? prevX : 0);
				const double y3 = d_string_to_double(str, (char**)&str)+(state=='s' ? prevY : 0);
				m_datahandler.onPathCurvetov(x2, y2, x3, y3);
				prevX = x3;
				prevY = y3;
			}
			break;
			case 'a':
			case 'A':
			{
				const double rx = d_string_to_double(str, (char**)&str);
				const double ry = d_string_to_double(str, (char**)&str);
				const double x_axis_rotation = d_string_to_double(str, (char**)&str);
				const int large_arc_flag = d_string_to_int(str, (char**)&str);
				const int sweep_flag = d_string_to_int(str, (char**)&str);
				const double x = d_string_to_double(str, (char**)&str)+(state=='a' ? prevX : 0);
				const double y = d_string_to_double(str, (char**)&str)+(state=='a' ? prevY : 0);
				m_datahandler.onPathArc(prevX, prevY, x, y, rx, ry, x_axis_rotation, large_arc_flag==0 ? false : true, sweep_flag==0 ? false : true);
				prevX = x;
				prevY = y;
			}
			break;
			case 'z':
			case 'Z':
			{
				m_datahandler.onPathClose(false);
				prevX = startX;
				prevY = startY;
			}
			break;
			case 'q':
			case 'Q':
			{
				const double x1 = d_string_to_double(str, (char**)&str)+(state=='q' ? prevX : 0);
				const double y1 = d_string_to_double(str, (char**)&str)+(state=='q' ? prevY : 0);
				const double x2 = d_string_to_double(str, (char**)&str)+(state=='q' ? prevX : 0);
				const double y2 = d_string_to_double(str, (char**)&str)+(state=='q' ? prevY : 0);
				m_datahandler.onPathQuad(x1, y1, x2, y2);
				prevX = x2;
				prevY = y2;
			}
			break;
			default:
				// BUGBUG: can get stuck here!
				// TODO: figure out the next state if we don't handle a particular state or just dummy handle a state!
				str++;
				break;
		}

		nextState((const char**)&str, &state);
	}

	if(abs(prevX-startX)<eps&&abs(prevY-startY)<eps)
		m_datahandler.onPathClose(false);

	m_datahandler.onPathEnd(true, true);
}

void CImportSVG::parse_poly(tinyxml2::XMLElement* element, bool bClose)
{
	char* str = (char*)element->Attribute("points");
	if(str==nullptr)
		return;


	char state = 'l';
	nextState((const char**)&str, &state);
	while(state!='e')
	{
		const double x = d_string_to_double(str, &str);
		const double y = d_string_to_double(str, &str);
		m_datahandler.onPathLineTo(x, y);
		nextState((const char**)&str, &state);
	}

	if(bClose==true)
		m_datahandler.onPathClose(false);

	m_datahandler.onPathEnd(true, true);
}

void CImportSVG::parse_line(tinyxml2::XMLElement* element)
{
	double x1 = 0;
	double y1 = 0;
	double x2 = 0;
	double y2 = 0;
	element->QueryDoubleAttribute("x1", &x1);
	element->QueryDoubleAttribute("y1", &y1);
	element->QueryDoubleAttribute("x2", &x2);
	element->QueryDoubleAttribute("y2", &y2);
	m_datahandler.onPathMoveTo(x1, y1);
	m_datahandler.onPathLineTo(x2, y2);

	m_datahandler.onPathEnd(true, true);
}

void CImportSVG::parse_rect(tinyxml2::XMLElement* element)
{
	double x = 0;
	double y = 0;
	double cx = 0;
	double cy = 0;
	element->QueryDoubleAttribute("x", &x);
	element->QueryDoubleAttribute("y", &y);
	element->QueryDoubleAttribute("width", &cx);
	element->QueryDoubleAttribute("height", &cy);
	if(cx==0.0&&cy==0.0)
		return;

	m_datahandler.onPathRect(x, y, cx, cy);
	m_datahandler.onPathEnd(true, true);
}

void CImportSVG::parse_circle(tinyxml2::XMLElement* element)
{
	double r = 0;
	double cx = 0;
	double cy = 0;
	element->QueryDoubleAttribute("r", &r);
	element->QueryDoubleAttribute("cx", &cx);
	element->QueryDoubleAttribute("cy", &cy);

	//	m_datahandler.onPathEllipse()
	m_datahandler.onPathEnd(true, true);
}

void CImportSVG::parse_ellipse(tinyxml2::XMLElement* element)
{
	double rx = 0;
	double ry = 0;
	double cx = 0;
	double cy = 0;
	element->QueryDoubleAttribute("cx", &cx);
	element->QueryDoubleAttribute("cy", &cy);
	element->QueryDoubleAttribute("rx", &rx);
	element->QueryDoubleAttribute("ry", &ry);

	//	m_datahandler.onPathEllipse()
	m_datahandler.onPathEnd(true, true);
}

void CImportSVG::parse_text(tinyxml2::XMLElement* element)
{
	double x = 0;
	double y = 0;
	element->QueryDoubleAttribute("x", &x);
	element->QueryDoubleAttribute("y", &y);

	const char* str = element->GetText();
	if(str!=nullptr)
	{
		m_datahandler.onSimpleText(str, x, y);
	}
	else if(element->FirstChildElement("tspan")!=nullptr)
	{
		CTypedPtrList<CObList, CGeom*> texts;
		for(tinyxml2::XMLElement* child = element->FirstChildElement("tspan"); child!=0; child = child->NextSiblingElement("tspan"))
		{
			parse_style(child);
			m_datahandler.onSimpleText(child->GetText(),x,y);
		}
	}
}

void CImportSVG::parse_image(tinyxml2::XMLElement* element)
{
	const char* href = element->Attribute("xlink:href");
	if(href==nullptr)
		return;

	if(boost::algorithm::starts_with(href, "data:")==true)
	{
		href += strlen("data:");
		if(boost::algorithm::starts_with(href, "image/")==true)
		{
			href += strlen("image/");
			const char* comma = strchr(href, ';');
			const int len = strlen(href)-strlen(comma);
			char extension[255];
			memset(extension, 0, 255);
			strncpy(extension, href, len);
			href += len+1;
			if(boost::algorithm::starts_with(href, "base64,")==true)
			{
				href += strlen("base64,");

				BYTE* data = new BYTE[strlen(href)*3/4];
				memset(data, 0, strlen(href)*3/4);
				const int bytes = UnBase64((const unsigned char*)href, data, strlen(href));

				CDib* pDib = nullptr;
				if(strcmp(extension, "png")==0)
					pDib = CDib::FromData(FREE_IMAGE_FORMAT::FIF_PNG, data, bytes);
				if(strcmp(extension, "jpg")==0)
					pDib = CDib::FromData(FREE_IMAGE_FORMAT::FIF_JNG, data, bytes);
				else if(strcmp(extension, "jpeg")==0)
					pDib = CDib::FromData(FREE_IMAGE_FORMAT::FIF_JPEG, data, bytes);
				else if(strcmp(extension, "gif")==0)
					pDib = CDib::FromData(FREE_IMAGE_FORMAT::FIF_GIF, data, bytes);
				else if(strcmp(extension, "svg+xml")==0)
				{
				}

				delete[] data;

				if(pDib!=nullptr)
				{
					double x = 0;
					double y = 0;
					double cx = 0;
					double cy = 0;
					element->QueryDoubleAttribute("x", &x);
					element->QueryDoubleAttribute("y", &y);
					element->QueryDoubleAttribute("width", &cx);
					element->QueryDoubleAttribute("height", &cy);
					const CPoint origin = Change(x, y);
					CRect rect(origin, CSize(cx*m_fXScale, cy*m_fYScale));
					rect.NormalizeRect();
					CImager* pImager = new CImager(rect, pDib);
					m_datahandler.m_geomHolders.top()->AddTail(pImager);
				}
			}
		}
	}
	else if(boost::algorithm::starts_with(href, "http:")||boost::algorithm::starts_with(href, "ftp:"))
	{
	}
	else if(boost::algorithm::starts_with(href, "file:"))
	{
	}
}

void CImportSVG::parse_style(tinyxml2::XMLElement* element)
{
	const char* cssclass = element->Attribute("class");
	if(cssclass!=nullptr)
	{
		StyleSheet::Element* element = GetCssElement(StyleSheet::SelectorType::SELECTOR_CLASS, cssclass);
		if(element!=nullptr)
		{
			auto properties = element->getProperties();
			parse_class1(properties);
			parse_class2(properties);
			parse_class3(properties);
		}
	}
	{
		m_datahandler.read_fill(element->Attribute("color"));
		m_datahandler.read_fill(element->Attribute("fill"));

		read_opacity(m_datahandler.CurrentState.fill_opacity, element->Attribute("opacity"));
		read_opacity(m_datahandler.CurrentState.fill_opacity, element->Attribute("fill-opacity"));
	}
	{
		CSS_Color color;
		read_color(color, element->Attribute("color"));
		read_color(color, element->Attribute("stroke"));

		read_opacity(m_datahandler.CurrentState.line_opacity, element->Attribute("opacity"));
		read_opacity(m_datahandler.CurrentState.line_opacity, element->Attribute("stroke-opacity"));

		m_datahandler.read_linewidth(element->Attribute("stroke-width"));
		m_datahandler.read_miterlimit(element->Attribute("stroke-miterlimit"));
		m_datahandler.read_dash(element->Attribute("stroke-dasharray"));
	}
	{
		CSS_Color color1;
		read_color(color1, element->Attribute("color"));
		read_color(color1, element->Attribute("fill"));
		CSS_Color color2;
		read_color(color2, element->Attribute("stroke"));

		read_opacity(m_datahandler.FontState.opacity, element->Attribute("opacity"));
		CSS_String fontname;
		read_string(fontname, element->Attribute("font-family"));
		CSS_Double		fontsize;
		read_doublewithunit(fontsize, element->Attribute("font-size"));

		const char* style = element->Attribute("style");
		if(style!=nullptr)
		{
			std::map<string, string> properties = tokenize(style, ";", ":");
			parse_class1(properties);
			parse_class2(properties);
			parse_class3(properties);
		}
	}
}

void CImportSVG::parse_gradient(tinyxml2::XMLElement* element)
{
	const char* id = element->Attribute("id");

	CSS_Gradient* gradient = new CSS_Gradient();
	gradient->id = id;
	if(!strcmp(element->Name(), "radialGradient"))
	{
		gradient->type = CSS_GradientType::RADIAL;

		read_double(gradient->cx, element->Attribute("cx"));
		read_double(gradient->cy, element->Attribute("cy"));
		read_double(gradient->r, element->Attribute("r"));
		read_double(gradient->fx, element->Attribute("fx"));
		read_double(gradient->fy, element->Attribute("fy"));
	}
	else if(!strcmp(element->Name(), "linearGradient"))
	{
		gradient->type = CSS_GradientType::LINEAR;

		read_double(gradient->x1, element->Attribute("x1"));
		read_double(gradient->y1, element->Attribute("y1"));
		read_double(gradient->x2, element->Attribute("x2"));
		read_double(gradient->y2, element->Attribute("y2"));
	}
	else
	{
		delete gradient;
		return;
	}
	for(tinyxml2::XMLElement* child = element->FirstChildElement("stop"); child!=0; child = child->NextSiblingElement("stop"))
	{
		const char* str = child->Attribute("style");
		if(str==nullptr)
			continue;
		CSS_GradientStop* stop = new CSS_GradientStop();
		child->QueryDoubleAttribute("offset", &stop->offset);

		std::map<string, string> pairs = tokenize(str, ";", ":");
		for(std::map<string, string>::iterator it = pairs.begin(); it!=pairs.end(); it++)
		{
			//ccu	stop->opacity = fillstyle.opacity;
			string key = it->first;
			string value = it->second;
			if(it->first=="stop-color")
				read_color(stop->color, value.c_str());
			else if(key=="stop-opacity")
				read_double(stop->opacity, value.c_str());
		}
		gradient->stops.push_back(stop);
	}

	m_datahandler.gradients[id] = gradient;
}

void CImportSVG::parse_transform(tinyxml2::XMLElement* element)
{
	const char* transform = element->Attribute("transform");
	if(transform==nullptr)
		return;
	PMatrix matrix;
	std::map<string, string> pairs = tokenize(transform, ")", "(");
	for(std::map<string, string>::iterator it = pairs.begin(); it!=pairs.end(); it++)
	{
		double scale = 1.f;
		string key = it->first;
		string value = it->second;
		char* c = const_cast<char*>(value.c_str());
		if(key=="translate")
		{
			matrix.dx = d_string_to_double(c, &c);
			matrix.dy = d_string_to_double(c, &c);
		}
		else if(key=="scale")
		{
			const double sx = d_string_to_double(c, &c);
			double sy = d_string_to_double(c, &c);
			if(sy==0)
				sy = sx;
			matrix.m11 = sx;
			matrix.m22 = sy;
		}
		else if(key=="rotate")
		{
			const double angle = d_string_to_double(c, &c);
			matrix.m11 = cos(angle);
			matrix.m21 = -sin(angle);
			matrix.m12 = sin(angle);
			matrix.m22 = cos(angle);
		}
		else if(key=="matrix")
		{
			matrix.m11 = d_string_to_double(c, &c);
			matrix.m12 = d_string_to_double(c, &c)*-1;
			matrix.m21 = d_string_to_double(c, &c)*-1;
			matrix.m22 = d_string_to_double(c, &c);
			matrix.dx = d_string_to_double(c, &c);
			matrix.dy = d_string_to_double(c, &c);
		}
	}
	m_datahandler.CurrentState.matrix = matrix;
}
std::map<string, CString> CImportSVG::parse_attribute(tinyxml2::XMLElement* element)
{
	std::map<string, CString> properties;
	const char* id = element->Attribute("id");
	if(id!=nullptr)
	{
		properties["id"] = CString(id);
	}
	else if(id = element->Attribute("data-id"))
	{
		properties["id"] = CString(id);
	}

	if(const char* title = element->Attribute("title"))
	{
		properties["name"] = CString(title);
	}
	else if(const char* label = element->Attribute("inkscape:label"))
	{
		properties["name"] = CString(label);
	}
	else if(const char* name = element->Attribute("name"))
	{
		properties["name"] = CString(name);
	}
	else if(name = element->Attribute("data-name"))
	{
		properties["name"] = CString(name);
	}
	//if(properties.find("id")!=properties.end())
	//{
	//	CString strId = properties["id"];
	//	int pos = strId.Find('_');
	//	if(pos>0)
	//		properties["id"] = strId.Mid(pos+1);
	//}
	//if(properties.find("id")!=properties.end())
	//{
	//	CString strId = properties["id"];
	//	int pos = strId.Find('-');
	//	if(pos>0)
	//		properties["id"] = strId.Mid(pos+1);
	//}
	return properties;
}
tinyxml2::XMLElement* CImportSVG::FindElement(tinyxml2::XMLElement* parent, const char* id)
{
	if(parent==nullptr)
		return nullptr;
	if(id==nullptr)
		return nullptr;
	if(!strcmp(id, ""))
		return nullptr;

	for(tinyxml2::XMLElement* child = parent->FirstChildElement(); child!=0; child = child->NextSiblingElement())
	{
		const char* childid = child->Attribute("id");
		if(childid==nullptr)
			continue;
		if(!strcmp(childid, id))
			return child;
	}
	for(tinyxml2::XMLElement* child = parent->FirstChildElement(); child!=0; child = child->NextSiblingElement())
	{
		tinyxml2::XMLElement* element = CImportSVG::FindElement(child, id);
		if(element!=nullptr)
			return element;
	}

	return nullptr;
}

StyleSheet::Element* CImportSVG::GetCssElement(StyleSheet::SelectorType type, std::string selector)
{
	for(std::list<StyleSheet::Style>::iterator it = m_cssstyles.begin(); it!=m_cssstyles.end(); ++it)
	{
		StyleSheet::Element* element = (*it).getElement(type, selector);
		if(element!=nullptr)
			return element;
	}
	return nullptr;
}


void  CImportSVG::parse_class1(std::map<string, string>& properties)
{
	for(std::map<string, string>::iterator it = properties.begin(); it!=properties.end(); it++)
	{
		string key = it->first;
		string value = it->second;


		if(key=="display")
		{
		}
		else if(key=="visibility")
		{
		}
		else if(key=="color")
			m_datahandler.read_stroke(value.c_str());
		else if(key=="opacity")
			read_opacity(m_datahandler.CurrentState.line_opacity, value.c_str());
		else if(key=="stroke")
			m_datahandler.read_stroke(value.c_str());
		else if(key=="stroke-width")
			read_double(m_datahandler.CurrentState.line_width, value.c_str());
		else if(key=="stroke-linecap")
			m_datahandler.read_linecap(value.c_str());
		else if(key=="stroke-linejoin")
			m_datahandler.read_linejoin(value.c_str());
		else if(key=="stroke-miterlimit")
			m_datahandler.read_miterlimit(value.c_str());
		else if(key=="stroke-opacity")
			read_opacity(m_datahandler.CurrentState.line_opacity, value.c_str());
		else if(key=="stroke-dashoffset")
		{
			CSS_Double offset;
			read_double(offset, value.c_str());
			if(offset.status==Value_Status::WithValue)
				m_datahandler.CurrentState.line_dash.m_offset = offset.value*10;
		}
		else if(key=="stroke-dasharray")
			m_datahandler.read_dash(value.c_str());

	}
}

void  CImportSVG::parse_class2(std::map<string, string>& properties)
{
	for(std::map<string, string>::iterator it = properties.begin(); it!=properties.end(); it++)
	{
		string key = it->first;
		string value = it->second;

		if(key=="display")
		{
		}
		else if(key=="visibility")
		{
		}
		else if(key=="color")
			m_datahandler.read_fill(value.c_str());
		else if(key=="opacity")
			read_opacity(m_datahandler.CurrentState.fill_opacity, value.c_str());
		else if(key=="fill")
			m_datahandler.read_fill(value.c_str());
		else if(key=="fill-opacity")
			read_opacity(m_datahandler.CurrentState.fill_opacity, value.c_str());
		else if(key=="fill-rule")
		{
		}

		//"marker"
		//"marker-start"
		//"marker-mid"
		//"marker-end"
	}
}

void  CImportSVG::parse_class3(std::map<string, string>& properties)
{
	for(std::map<string, string>::iterator it = properties.begin(); it!=properties.end(); it++)
	{
		string key = it->first;
		string value = it->second;

		if(key=="display")
		{
		}
		else if(key=="visibility")
		{
		}
		else if(key=="overflow")
		{
		}
		else if(key=="font-family")
		{
			m_datahandler.read_fontname(value.c_str());
		}
		else if(key=="font-size")
		{
			m_datahandler.read_fontsize(value.c_str());
		}
		else if(key=="font-style")
			m_datahandler.read_fontstyle(value.c_str());
		else if(key=="font-variant")
		{
		}
		else if(key=="font-weight")
			read_int(enum_font_weight, m_datahandler.FontState.weight, value.c_str());
		else if(key=="font-stretch")
		{
		}
		else if(key=="text-indent")
		{
		}
		else if(key=="text-align")
			m_datahandler.read_textalign(value.c_str());
		else if(key=="text-decoration")
		{
		}
		else if(key=="line-height")
			m_datahandler.read_lineheight(value.c_str());
		else if(key=="letter-spacing")
		{
		}
		else if(key=="word-spacing")
		{
		}
		else if(key=="text-transform")
			m_datahandler.read_fontsize(value.c_str());
		else if(key=="direction")
		{
		}
		else if(key=="block_progression")
		{
		}
		else if(key=="writing-mode")
		{
		}
		else if(key=="text-anchor")
			m_datahandler.read_textanchor(value.c_str());
		else if(key=="baseline-shift")
			m_datahandler.read_fontsize(value.c_str());
		else
		{
		}
	}
}
