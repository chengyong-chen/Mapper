#include "stdafx.h"
#include "TiledBezier.h"
#include "TiledPoly.h"

#include "../Pbf/writer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTiledBezier::CTiledBezier()
	: CBezier()
{
	m_bType = 102;
	m_prevnext = 0;
	m_sequence = 0;
}

CTiledBezier::CTiledBezier(const TiledSegment& segment, int anchors)
	: CBezier(segment.points, anchors, false)
{
	m_bType = 102;
	m_prevnext = segment.prevnext;
	m_sequence = segment.sequence;
}

void CTiledBezier::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CBezier::ReleaseWeb(writer, offset);

	writer.add_byte(m_prevnext);
	writer.add_variant_uint16(m_sequence);
}
