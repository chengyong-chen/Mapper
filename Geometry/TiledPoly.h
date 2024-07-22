#pragma once

#include "Poly.h"
namespace pbf {
	class writer;
	using tag = uint8_t;
};

struct TiledSegment
{
	int row;
	int col;
	BYTE prevnext;
	std::vector<CPoint> points;
	WORD sequence;
};

struct TileIntersect
{
	int x;
	int y;
	double distance;
};

class AFX_EXT_CLASS CTiledPoly sealed : public CPoly
{
protected:
	CTiledPoly();

public:
	CTiledPoly(const TiledSegment& segment);

public:	
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;

private:
	BYTE m_prevnext;
	WORD m_sequence;
};
