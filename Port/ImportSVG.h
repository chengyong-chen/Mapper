#pragma once

#include "Import.h"
#include "SVGHandler.h"
#include "Style.h"
#include "../../Thirdparty/tinyxml/tinyxml2/tinyxml2.h"

#include <map>
#include <list>
class CGeom;
class CSymbolSpot;

namespace StyleSheet {
	class Element;
	class Style;
	enum SelectorType : BYTE;
};
using namespace tinyxml2;

class __declspec(dllexport) CImportSVG : public CImport
{
public:
	CImportSVG(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomID);

	~CImportSVG() override;

public:
	BOOL Import(LPCTSTR lpszFile) override;

private:
	std::map<string, CSymbolSpot*> m_symbols;
	std::list<StyleSheet::Style> m_cssstyles;
	std::map<string, CSymbolSpot*> m_clippathes;

private:
	static tinyxml2::XMLElement* FindElement(tinyxml2::XMLElement* parent, const char* id);
	void parseSvg(tinyxml2::XMLElement* xml);
	void parseElement(tinyxml2::XMLElement* element);
	void parse_group(tinyxml2::XMLElement* element);
	void parse_path(tinyxml2::XMLElement* element);
	void parse_poly(tinyxml2::XMLElement* element, bool bClose);
	void parse_line(tinyxml2::XMLElement* element);
	void parse_rect(tinyxml2::XMLElement* element);
	void parse_circle(tinyxml2::XMLElement* element);
	void parse_ellipse(tinyxml2::XMLElement* element);
	void parse_text(tinyxml2::XMLElement* element);
	void parse_image(tinyxml2::XMLElement* element);
	void parse_style(tinyxml2::XMLElement* element);
	void parse_gradient(tinyxml2::XMLElement* element);
	void parse_transform(tinyxml2::XMLElement* element);

	static std::map<string, CString> parse_attribute(tinyxml2::XMLElement* element);
	void parse_use(tinyxml2::XMLElement* element);
	void parse_xlink(tinyxml2::XMLElement* root, const char* id);
	void parse_class1(std::map<string, string>& properties);
	void parse_class2(std::map<string, string>& properties);
	void parse_class3(std::map<string, string>& properties);
private:
	CSVGHandler m_datahandler;
public:
	StyleSheet::Element* GetCssElement(StyleSheet::SelectorType type, std::string selector);
};
