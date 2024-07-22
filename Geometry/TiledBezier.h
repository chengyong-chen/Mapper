#pragma once
#include "../Geometry/Bezier.h"

namespace pbf {
	class writer;
	using tag = uint8_t;
};
struct TiledSegment;

class AFX_EXT_CLASS CTiledBezier sealed : public CBezier
{
protected:
	CTiledBezier();

public:
	CTiledBezier(const TiledSegment& segment, int anchors);

public:		
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const;

private:
	BYTE m_prevnext;
	WORD m_sequence;
};
