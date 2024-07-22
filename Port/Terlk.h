#pragma once

class CCoverage;



class __declspec(dllexport) CTerlk : public CCoverage
{
public:
	CTerlk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CTerlk();
	
public:
	virtual long ReadPAT() override;
	virtual long ReadAAT() override;

	virtual BOOL Import(CLayerTree& layertree) override;
};