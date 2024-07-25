#include "Stdafx.h"
#include "SVGHandler.h"

#include "../Style/ColorSpot.h"
#include "../Style/StandardColors.h"
#include "../Style/Type.h"

#include "../Geometry/Text.h"

#include "../../inkscape-0.48.2/src/unit-constants.h"

using namespace boost;
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void read_int(CSS_Int& data, const char* str)
{
	if(str==nullptr)
		return;

	char* end;
	const int value = strtol(str, &end, 10);
	if(end!=str)
	{
		data.status = Value_Status::WithValue;
		data.value = value;
	}
};

void read_int(const CSSDictKeyInt* dict, int& data, const char* str)
{
	if(str==nullptr)
		return;

	for(unsigned i = 0; dict[i].key; i++)
	{
		if(!strcmp(str, dict[i].key))
		{	
			data = dict[i].value;
			return;
		}
	}
}

void read_string(CSS_String& data, const char* str)
{
	if(str==nullptr)
		return;

	string stdstr(str);
	trim(stdstr);

	stdstr.erase(0, stdstr.find_first_not_of('\''));
	stdstr.erase(stdstr.find_last_not_of('\'')+1);

	data.status = Value_Status::WithValue;
	data.value = stdstr;
}

void read_double(CSS_Double& data, const char* str)
{
	if(str==nullptr)
		return;

	char* end;
	const double value = strtod(str, &end);
	if(end!=str)
	{
		data.status = Value_Status::WithValue;
		data.value = value;
		if(!strcmp(end, "%"))
			data.type = CSS_DOUBLEVALUETYPE::DOUBLE_PERCENT;
		else
			data.type = CSS_DOUBLEVALUETYPE::DOUBLE_VALAUE;
	}
};
void read_double(float& data, const char* str)
{
	if(str==nullptr)
		return;

	char* end;
	const double value = strtod(str, &end);
	if(end!=str)
	{
		data = value;
	}
};
void read_opacity(float& data, const char* str)
{
	if(str==nullptr)
		return;

	char* end;
	const double value = strtod(str, &end);
	if(end!=str)
	{
		data = value;
	}
};

void read_bool(CSS_Bool& data, const char* str)
{
	if(str==nullptr)
		return;

	if(!strcmp(str, "inherit"))
	{
	}
	else if(!strcmp(str, "none"))
	{
		data.status = Value_Status::WithValue;
		data.value = false;
	}
	else if(!strcmp(str, "0"))
	{
		data.status = Value_Status::WithValue;
		data.value = false;
	}
	else if(!strcmp(str, "1"))
	{
		data.status = Value_Status::WithValue;
		data.value = true;
	}
};

void read_double(const CSSDictKeyDouble* dict, CSS_Double& data, const char* str)
{
	if(str==nullptr)
		return;

	for(unsigned i = 0; dict[i].key!=nullptr; i++)
	{
		if(!strcmp(str, dict[i].key))
		{
			data.status = Value_Status::WithValue;
			data.value = dict[i].value;
			return;
		}
	}
};

void read_doublewithunit(CSS_Double& data, const char* str)
{
	if(str==nullptr)
		return;

	char* end;
	const double value = strtod(str, &end);
	if(end!=str)
	{
		if(!strcmp(end, "%")&&data.status==Value_Status::NoValue)
			return;

		if(!strcmp(end, "px"))
			data.value = value;
		else if(!strcmp(end, "pt"))
			data.value = value*PX_PER_PT;
		else if(!strcmp(end, "pc")) //12pt
			data.value = value*PX_PER_PT*12.0;
		else if(!strcmp(end, "mm"))
			data.value = value*PX_PER_MM;
		else if(!strcmp(end, "cm"))
			data.value = value*PX_PER_CM;
		else if(!strcmp(end, "in"))
			data.value = value*PX_PER_IN;
		else if(!strcmp(end, "%"))
			data.value = data.value*value/100.0;
		else
			data.value = value;

		data.status = Value_Status::WithValue;
	}
};



char* strstrip(const char* str)
{
	if(str==nullptr)// Exit if param is nullptr pointer
		return nullptr;

	const char* start = str;
	while((*start)&&isspace(*start))// Skip over leading whitespace
		start++;

	if(!(*start)) // Is string just whitespace?
		return nullptr;

	const char* end = start;
	while(*end)// Find end of string         
		end++;

	end--; // Step back from NUL

	while((end!=start)&&isspace(*end))// Step backward until first non-whitespace         
		end--;

	//	if (str != start)// If had leading whitespace, then move entire string back to beginning
	{
		char* ret = (char*)malloc(end-start+1+1);
		memcpy(ret, start, end-start+1);
		ret[end-start+1] = 0X00;
		return ret;
	}
	//	else
	//		return nullptr; 
};

int isnumeric(const char* str)
{
	if(str==nullptr)
		return false;

	char* end;
	strtod(str, &end);
	return !isspace(*str)&&strlen(end)==0;
}
std::map<string, string> tokenize(const char* str, const char* sep1, const char* sep2)
{
	string stdstr(str);
	trim(stdstr);
	std::map<string, string> pairs;
	tokenizer<char_separator<char>> tokens(stdstr, char_separator<char>(sep1));
	BOOST_FOREACH(string seg, tokens)
	{
		trim(seg);
		if(seg=="")
			continue;

		tokenizer<char_separator<char>> pair(seg, char_separator<char>(sep2));
		tokenizer<char_separator<char>>::iterator itKey = pair.begin();
		tokenizer<char_separator<char>>::iterator itValue = pair.begin();
		itValue++;
		if(itKey==pair.end()||itValue==pair.end())// like this: "font-family: Nimbus Sans L,'Arial Narrow',sans-serif;Sans L',sans-serif;"
			continue;
		string key(*itKey);
		string value(*itValue);
		trim(key);
		trim(value);
		if(key=="")
			continue;
		if(value=="")
			continue;

		pairs[key] = value;
	}

	return pairs;
}

void nextState(const char** str, char* state)
{
	if(**str=='\0')
	{
		*state = 'e';
		return;
	}
	while(isspace(**str))
		(*str)++;

	if(isalpha(**str))
	{
		*state = **str;
		(*str)++;
	}
	else if(**str=='\0')
		*state = 'e';
}

double d_string_to_double(const char* str, char** end)
{
	while(isspace(*str))
	{
		str++;
		(*end)++;
	}
	while(*str==',')
	{
		str++;
		(*end)++;
	}

	return strtod(str, end);
}
int d_string_to_int(const char* str, char** end)
{
	while(isspace(*str))
	{
		str++;
		(*end)++;
	}
	while(*str==',')
	{
		str++;
		(*end)++;
	}

	return strtol(str, end, 10);
}

CSVGHandler::CSVGHandler(DWORD& maxID)
	: IDataHandler(maxID)
{
}

void CSVGHandler::onPathEnd(bool line, bool fill)
{
	IDataHandler::onPathEnd(line, fill);

	if(properties.size()>0 && m_geomHolders.top()->IsEmpty() == false)
	{
		CGeom* pGeom = m_geomHolders.top()->GetTail();
		for(auto const& [key, val]:properties)
		{
			if(key=="id")
				pGeom->m_geoCode = val;
			else if(key=="name")
				pGeom->m_strName = val;
		}		
	}
}
void CSVGHandler::read_fill(const char* str)
{
	if(str==nullptr)
	{
	}
	else if(!strcmp(str, "none"))
	{
		CurrentState.filltype = FillType::None1;
	}
	else if(!strcmp(str, "inherit"))
	{
		CurrentState.filltype = FillType::Inherit;
	}
	else if(strncmp("url", str, 3)==0)//defined like fill:url(#pattern4943)
	{
		CurrentState.filltype = FillType::Gradient;
		strncpy(CurrentState.linkid, str+5, strlen(str)-6);
		CurrentState.linkid[strlen(str)-6] = 0;
	}
	else
	{
		CurrentState.filltype = FillType::Solid;
		CSS_Color color;
		read_color(color, str);
		if(CurrentState.fill_color!=nullptr)
		{
			delete	CurrentState.fill_color;
			CurrentState.fill_color = nullptr;
		}
		if(color.status==Value_Status::WithValue)
			CurrentState.fill_color = new CColorSpot((DWORD)color.value);
	}
}
void CSVGHandler::read_stroke(const char* str)
{
	CSS_Color color;
	read_color(color, str);
	if(color.status==Value_Status::WithValue)
	{
		if(CurrentState.line_color!=nullptr)
		{
			delete	CurrentState.line_color;
			CurrentState.line_color = nullptr;
		}
		if(color.status==Value_Status::WithValue)
			CurrentState.line_color = new CColorSpot((DWORD)color.value);
	}
}
void CSVGHandler::read_miterlimit(const char* str)
{
	CSS_Double data;
	read_double(data, str);
	if(data.status==Value_Status::WithValue)
		CurrentState.line_miterlimit = data.value*10;
}
void CSVGHandler::read_linewidth(const char* str)
{
	CSS_Double data;
	read_double(data, str);
	if(data.status==Value_Status::WithValue)
		CurrentState.line_width = data.value;
}
void CSVGHandler::read_linecap(const char* str)
{
	if(!strcmp(str, "butt"))
	{
		CurrentState.line_cap = 0;
	}
	else if(!strcmp(str, "round"))
	{
		CurrentState.line_cap = 1;
	}
	else if(!strcmp(str, "square"))
	{
		CurrentState.line_cap = 2;
	}
}
void CSVGHandler::read_linejoin(const char* str)
{
	if(!strcmp(str, "miter"))
	{
		CurrentState.line_join = 0;
	}
	else if(!strcmp(str, "round"))
	{
		CurrentState.line_join = 1;
	}
	else if(!strcmp(str, "bevel"))
	{
		CurrentState.line_join = 2;
	}
}
void CSVGHandler::read_lineheight(const char* str)
{
	CSS_Double value;
	read_double(value, str);
	if(value.status==Value_Status::WithValue)
		FontState.Leading = value.value;
}


void CSVGHandler::read_fontname(const char* str)
{
	CSS_String cstr;
	read_string(cstr, str);
	if(cstr.status==Value_Status::WithValue)
	{
		FontState.font = cstr.value;
	}
}


void CSVGHandler::read_fontstyle(const char* str)
{
	if(str==nullptr)
		return;

	for(unsigned i = 0; enum_font_style[i].key; i++)
	{
		if(!strcmp(str, enum_font_style[i].key))
		{
			FontState.style = (Gdiplus::FontStyle)enum_font_style[i].value;
			return;
		}
	}
}

void CSVGHandler::read_textalign(const char* str)
{
	if(str==nullptr)
		return;
	for(unsigned i = 0; enum_text_align[i].key; i++)
	{
		if(!strcmp(str, enum_text_align[i].key))
		{
			FontState.align = (TEXT_ALIGN)enum_text_align[i].value;
			return;
		}
	}
}

void CSVGHandler::read_textanchor(const char* str)
{
	if(str==nullptr)
		return;
	for(unsigned i = 0; enum_text_anchor[i].key; i++)
	{
		if(!strcmp(str, enum_text_anchor[i].key))
		{
			FontState.anchor = (TEXT_ANCHOR)enum_text_anchor[i].value;
			return;
		}
	}
}

void CSVGHandler::read_fontsize(const char* str)
{
	if(str==nullptr)
		return;

	if(!strcmp(str, "inherit"))
	{
	}
	else if((*str=='x')||(*str=='s')||(*str=='m')||(*str=='l')) // xx-small, x-small, etc.
	{
		for(int index = 0; enum_font_size[index].key!=nullptr; index++)
		{
			if(!strcmp(str, enum_font_size[index].key))
			{
				FontState.size = enum_font_size[index].value;
				break;
			}
		}
	}
	else
		return read_double(FontState.size, str);
};

void CSVGHandler::read_dash(const char* str)
{
	if(str==nullptr)
		return;

	if(strcmp(str, "inherit")==0)
		return;

	CurrentState.line_dash.Solid();
	if(strcmp(str, "none")==0)
		return;
	
	int index = 0;
	char* end = nullptr;
	while(end!=str&&index<7)
	{
		const double value = strtod(str, &end);
		if(value>0.00000001)
			CurrentState.line_dash.m_segment[index++] = value*10;

		if(end!=str)
		{
			str = end;
		}
		while(str&&*str&&!isalnum(*str))
			str += 1;
	}
}

void CSVGHandler::onSimpleText(const char* text,double x, double y)
{
	const CStringA strReal(FontState.font.c_str());
	const double wscale = sqrt(FontState.textmatrix.m11*FontState.textmatrix.m11+FontState.textmatrix.m12*FontState.textmatrix.m12);
	const double hscale = sqrt(FontState.textmatrix.m21*FontState.textmatrix.m21+FontState.textmatrix.m22*FontState.textmatrix.m22);
	const double mscale = max(wscale, hscale);

	CType* pType = new CType();
	pType->m_fSize = FontState.size*mscale;
	pType->m_fontdesc.SetRealName(strReal);
	pType->m_fontdesc.SetStyle(FontState.style);
	pType->m_pColor = CurrentState.fill_color==nullptr ? nullptr : CurrentState.fill_color->Clone();
	CText* pText = new CText(CPoint(0, 0));
	pText->m_Align = FontState.align==std::nullopt ? TEXT_ALIGN::TAL_START : *FontState.align;
	pText->m_Anchor = FontState.anchor==std::nullopt ? TEXT_ANCHOR::TAC_START : *FontState.anchor;
	pText->m_Rect = CRect(0, 0, 10000, 10000);

	PMatrix matrix = CurrentState.matrix*FontState.textmatrix;
	pText->Transform(matrix.m11, matrix.m21, matrix.dx, matrix.m12, matrix.m22, matrix.dy);

	const Gdiplus::PointF origin1 = matrix.TransformPoint(x, y);
	const CPoint orgin2 = Change(origin1.X, origin1.Y);
	pText->Move(orgin2.x-pText->m_Origin.x, orgin2.y-pText->m_Origin.y);

	pText->m_pType = pType;
	pText->m_strName = CA2W(text, CP_UTF8);

	pText->CalCorner(nullptr, 10000, 1.f);
	

	m_geomHolders.top()->AddTail(pText);
}
void read_color(CSS_Color& data, const char* str)
{
	if(str==nullptr)
		return;

	if(!strcmp(str, "inherit"))
	{
	}
	else if(!strcmp(str, "none"))
	{
	}
	else if(*str=='#')
	{
		unsigned int color = strtol(str+1, 0, 16);
		if(strlen(str)==7)
		{
			color |= 0xFF000000;
		}
		data.status = Value_Status::WithValue;
		data.value = color;
	}
	else if(strncmp("rgb", str, 3)==0)//rgb(r,g,b)
	{
		char* sub = (char*)(str+4);
		char* strs[3];
		int index = 0;
		char* pch = strtok(sub, ",");
		while(pch!=nullptr)
		{
			strs[index++] = pch;
			pch = strtok(nullptr, ",");
		}
		if(index==3)
		{
			const unsigned int r = strstr(strs[0], "%")!=nullptr ? (d_string_to_double(strs[0], &strs[0])*255/100.f) : d_string_to_int(strs[0], &strs[0]);
			const unsigned int g = strstr(strs[1], "%")!=nullptr ? (d_string_to_double(strs[1], &strs[1])*255/100.f) : d_string_to_int(strs[1], &strs[1]);
			const unsigned int b = strstr(strs[2], "%")!=nullptr ? (d_string_to_double(strs[2], &strs[2])*255/100.f) : d_string_to_int(strs[2], &strs[2]);

			data.status = Value_Status::WithValue;
			data.value = 0XFF000000;
			data.value |= (r<<16);
			data.value |= (g<<8);
			data.value |= (b<<0);
		}
	}
	else if(isnumeric(str)==false)
	{
		for(int index = 0; index<sizeof(standard_colors)/sizeof(StandardColor); index++)
		{
			if(!strcmp(str, standard_colors[index].key))
			{
				unsigned int color = 0XFF000000;
				color |= (standard_colors[index].R<<16);
				color |= (standard_colors[index].G<<8);
				color |= (standard_colors[index].B<<0);
				data.status = Value_Status::WithValue;
				data.value = color;
				break;
			}
		}
	}
	else
	{
		const unsigned int color = strtol(str+1, 0, 16);
		data.status = Value_Status::WithValue;
		data.value = color;
	}
};
