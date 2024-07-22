#include "stdafx.h"
#include "TiledPoly.h"

#include "../Pbf/writer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTiledPoly::CTiledPoly()
	: CPoly()
{
	m_bType = 101;
	m_prevnext = 0;
	m_sequence = 0;
}

CTiledPoly::CTiledPoly(const TiledSegment& segment)
	: CPoly(segment.points, false)
{
	m_bType = 101;
	m_prevnext = segment.prevnext;
	m_sequence = segment.sequence;
}

void CTiledPoly::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPoly::ReleaseWeb(writer, offset);

	writer.add_byte(m_prevnext);
	writer.add_variant_uint16(m_sequence);
}
