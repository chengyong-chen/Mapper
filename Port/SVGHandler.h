#pragma once

#include "DataHandler.h"

//#include "CSSStyle.h"

enum Value_Status : BYTE
{
	NoValue = 0,
	WithValue
};

struct CSSDictKeyByte
{
	const char* key;
	BYTE value;
};

struct CSSDictKeyInt
{
	const char* key;
	int value;
};

struct CSSDictKeyDouble
{
	const char* key;
	BYTE value;
};

enum CSS_DOUBLEVALUETYPE : BYTE
{
	DOUBLE_VALAUE,
	DOUBLE_PERCENT
};

enum CSS_UNIT : BYTE
{
	UNIT_NONE,
	UNIT_PX,
	UNIT_PT,
	UNIT_PC,
	UNIT_MM,
	UNIT_CM,
	UNIT_IN,
	UNIT_EM,
	UNIT_EX,
	UNIT_PERCENT
};

enum SPStrokeJoinType : BYTE
{
	LINEJOIN_MITER,
	LINEJOIN_ROUND,
	LINEJOIN_BEVEL
};

enum SPStrokeCapType : BYTE
{
	LINECAP_BUTT,
	LINECAP_ROUND,
	LINECAP_SQUARE
};

enum CSS_FontVariant : BYTE
{
	FONT_VARIANT_NORMAL,
	FONT_VARIANT_SMALL_CAPS
};

enum CSS_GradientType : BYTE
{
	LINEAR = 0,
	RADIAL
};

enum CSS_FontWeight : BYTE
{
	FONT_WEIGHT_100,
	FONT_WEIGHT_200,
	FONT_WEIGHT_300,
	FONT_WEIGHT_400,
	FONT_WEIGHT_500,
	FONT_WEIGHT_600,
	FONT_WEIGHT_700,
	FONT_WEIGHT_800,
	FONT_WEIGHT_900,
	FONT_WEIGHT_NORMAL,
	FONT_WEIGHT_BOLD,
	FONT_WEIGHT_LIGHTER,
	FONT_WEIGHT_BOLDER
};

enum CSS_FontStretch : BYTE
{
	FONT_STRETCH_ULTRA_CONDENSED,
	FONT_STRETCH_EXTRA_CONDENSED,
	FONT_STRETCH_CONDENSED,
	FONT_STRETCH_SEMI_CONDENSED,
	FONT_STRETCH_NORMAL,
	FONT_STRETCH_SEMI_EXPANDED,
	FONT_STRETCH_EXPANDED,
	FONT_STRETCH_EXTRA_EXPANDED,
	FONT_STRETCH_ULTRA_EXPANDED,
	FONT_STRETCH_NARROWER,
	FONT_STRETCH_WIDER
};

enum CSS_TextTransform : BYTE
{
	TEXT_TRANSFORM_CAPITALIZE,
	TEXT_TRANSFORM_UPPERCASE,
	TEXT_TRANSFORM_LOWERCASE,
	TEXT_TRANSFORM_NONE
};

enum CSS_Direction : BYTE
{
	DIRECTION_LTR,
	DIRECTION_RTL
};

enum CSS_WritingMode : BYTE
{
	WRITING_MODE_LR_TB,
	WRITING_MODE_RL_TB,
	WRITING_MODE_TB_RL,
	WRITING_MODE_TB_LR
};

enum CSS_TextAnchor : BYTE
{
	ANCHOR_START = 0,
	ANCHOR_MIDDLE,
	ANCHOR_END
};

enum CSS_BaselineShift : BYTE
{
	TEXT_ANCHOR_START,
	TEXT_ANCHOR_MIDDLE,
	TEXT_ANCHOR_END
};

struct CSS_String
{
	Value_Status status;
	string value;

	CSS_String()
		: status(Value_Status::NoValue), value("")
	{
	}
};

struct CSS_Double
{
	Value_Status status;
	CSS_DOUBLEVALUETYPE type;
	double value;

	CSS_Double()
		: status(Value_Status::NoValue), value(0), type(CSS_DOUBLEVALUETYPE::DOUBLE_VALAUE)
	{
	}
};

struct CSS_Bool
{
	Value_Status status;
	bool value;

	CSS_Bool()
		: status(Value_Status::NoValue), value(false)
	{
	}
};

struct CSS_Int
{
	Value_Status status;
	int value;

	CSS_Int()
		: status(Value_Status::NoValue), value(0)
	{
	}
};

struct CSS_Short
{
	Value_Status status;
	short value;

	CSS_Short()
		: status(Value_Status::NoValue), value(0)
	{
	}
};

struct CSS_Enum
{
	Value_Status status;
	BYTE value;

	CSS_Enum()
		: status(Value_Status::NoValue), value(255)
	{
	}
};

struct CSS_Color
{
	Value_Status status;
	unsigned int value;

	CSS_Color()
		: status(Value_Status::NoValue), value(0XFF000000)
	{
	}
};

struct CSS_TextDecoration
{
	unsigned set : 1;
	unsigned inherit : 1;
	unsigned underline : 1;
	unsigned overline : 1;
	unsigned line_through : 1;
	unsigned blink : 1; // "Conforming user agents are not required to support this value." yay!
};

/// Stroke dash details.
struct CSS_Dash
{
	Value_Status status;
	CSS_Double offset;
	int n_dash;
	float dash[8];

	CSS_Dash()
		: status(Value_Status::NoValue), n_dash(0)
	{
	}
};

struct CSS_GradientStop
{
	double offset;
	CSS_Color color;
	CSS_Double opacity;
};

struct CSS_Gradient
{
	const char* id;
	std::list<CSS_GradientStop*> stops;
	CSS_GradientType type;
	CSS_Double cx;
	CSS_Double cy;
	CSS_Double r;
	CSS_Double fx;
	CSS_Double fy;

	CSS_Double x1;
	CSS_Double y1;
	CSS_Double x2;
	CSS_Double y2;
};

static const CSSDictKeyDouble enum_font_size[] =
{
	{"xx-small", 12*(3.0/5.0)},
	{"x-small", 12*(3.0/4.0)},
	{"small", 12*(8.0/9.0)},
	{"smaller", 12*0.84},
	{"medium", 12},
	{"large", 12*(6.0/5.0)},
	{"larger", 12*1.26},
	{"x-large", 12*(3.0/2.0)},
	{"xx-large", 12*2.0},
	{nullptr, -1}
};

static const CSSDictKeyByte enum_stroke_linecap[] =
{
	{"butt", LINECAP_BUTT},
	{"round", LINECAP_ROUND},
	{"square", LINECAP_SQUARE},
	{nullptr, -1}
};
static const CSSDictKeyByte enum_stroke_linejoin[] =
{
	{"miter", LINEJOIN_MITER},
	{"round", LINEJOIN_ROUND},
	{"bevel", LINEJOIN_BEVEL},
	{nullptr, -1}
};
static const CSSDictKeyByte enum_font_style[] =
{
	{"normal", Gdiplus::FontStyle::FontStyleRegular},
	{"italic", Gdiplus::FontStyle::FontStyleItalic},
	{"oblique", Gdiplus::FontStyle::FontStyleBold},
	{nullptr, -1}
};

static const CSSDictKeyInt enum_font_weight[] =
{
	{"100", FW_THIN},
	{"200", FW_EXTRALIGHT},
	{"300", FW_LIGHT},
	{"400", FW_NORMAL},
	{"500", FW_MEDIUM},
	{"600", FW_SEMIBOLD},
	{"700", FW_BOLD},
	{"800", FW_EXTRABOLD},
	{"900", FW_HEAVY},
	{"normal", FW_NORMAL},
	{"bold", FW_BOLD},
	{"lighter", FW_EXTRALIGHT},
	{"bolder", FW_EXTRABOLD},
	{nullptr, -1}
};

static const CSSDictKeyByte const enum_text_anchor[] =
{
	{"start", TEXT_ANCHOR::TAC_START},
	{"middle", TEXT_ANCHOR::TAC_MIDDLE},
	{"end", TEXT_ANCHOR::TAC_END},
	{nullptr, -1}
};
static const CSSDictKeyByte const enum_text_align[] =
{
	{"start", TEXT_ALIGN::TAL_START},
	{"end", TEXT_ALIGN::TAL_END},
	{"left", TEXT_ALIGN::TAL_LEFT},
	{"right", TEXT_ALIGN::TAL_RIGHT},
	{"center", TEXT_ALIGN::TAL_CENTER},
	{"justify", TEXT_ALIGN::TAL_JUSTIFY},
	{nullptr, -1}
};
class __declspec(dllexport) CSVGHandler : public IDataHandler
{
public:
	CSVGHandler(DWORD& maxID);
public:
	std::map<std::string, CSS_Gradient*> gradients;
	std::map<string, CString> properties;
		  
public:
	void onPathEnd(bool line, bool fill) override;
	void onSimpleText(const char* text, double x, double y);

public:
	void read_fill(const char* str);
	void read_stroke(const char* str);
	void read_miterlimit(const char* str);
	void read_linecap(const char* str);
	void read_linejoin(const char* str);
	void read_linewidth(const char* str);
	void read_dash(const char* str);
	
	void read_fontstyle(const char* str);
	void read_textalign(const char* str);
	void read_textanchor(const char* str);

	void read_fontsize(const char* str);
	void read_fontname(const char* str);
	void read_textcolor(const char* str);
	void read_lineheight(const char* str);
};

int isnumeric(const char* str);
char* strstrip(const char* str);
void read_color(CSS_Color& data, const char* str);
void read_bool(CSS_Bool& data, const char* str);
void read_double(float& data, const char* str);
void read_double(CSS_Double& data, const char* str);
void read_opacity(float& opacity, const char* str);
void read_string(CSS_String& data, const char* str);
void read_doublewithunit(CSS_Double& data, const char* str);
void read_double(const CSSDictKeyDouble* dict, CSS_Double& data, const char* str);
void read_int(const CSSDictKeyInt* dict, int& data, const char* str);


double d_string_to_double(const char* str, char** end);
int d_string_to_int(const char* str, char** end);
void nextState(const char** str, char* state);
std::map<string, string> tokenize(const char* str, const char* sep1, const char* sep);