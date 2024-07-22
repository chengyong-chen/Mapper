#pragma once

class CRoalk;



class __declspec(dllexport) CRailk : public CRoalk
{
public:
	CRailk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CRailk();
	
public:
	CMapRat  ratraimap;
	SEC_LIST secrai_list;
	
public:
	virtual long	ReadPAT();
	virtual long	ReadAAT();

	virtual BOOL Import(CLayerTree& layertree) override;
};