#pragma once

class CCoverage;



class __declspec(dllexport) CTernt : public CCoverage
{
public:
	CTernt(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CTernt();
	
public:
	virtual long ReadPAT() override;
	virtual long ReadAAT() override;

	virtual BOOL Import(CLayerTree& layertree) override;
};