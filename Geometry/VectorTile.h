#pragma once

#include <list>
#include <string>
using namespace std;
class CGeom;

class AFX_EXT_CLASS CVectorTile : public CObject
{
protected:
	DECLARE_SERIAL(CVectorTile);

public:
	CVectorTile();

	~CVectorTile() override;

public:
	std::list<std::pair<WORD, std::list<CGeom*>>> m_layergeomlist;
public:
	std::list<CGeom*>& GetGeomList(WORD wId);
public:
	std::string Extract(CSize& offset, DWORD dwVersion) const;
};