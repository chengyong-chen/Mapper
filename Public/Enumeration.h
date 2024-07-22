#pragma once

typedef enum
{
	Left = 0,
	HCenter,
	Right,
	Top,
	VCenter,
	Bottom,
	Center,
	YLeft,
	YCenter,
	YRight,
	XOver,
	XCenter,
	XUnder,
	Origin
} ALIGN;

typedef enum
{
	Tofront,
	Toback,
	Backward,
	Forward
} ARRANGE;

typedef enum
{
	Inverse,
	All,
	Mark,
	Line,
	Area,
	Text,
	PolyText,
	Image,
	Group,
	Doughnut,
	Clip,
	Mask,
	HasName,
	NoName,
	WithName,
	Rebel,
	Bezier,
	Stroke,
	Fill,
	Style,
	Spot,
	Type
} ACTIVEALL;
