#pragma once

class CCoverage;



class __declspec(dllexport) COthnt : public CCoverage
{
public:
	COthnt(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~COthnt();
	
public:
	virtual long ReadPAT() override;
	virtual long ReadAAT() override;

	virtual BOOL Import(CLayerTree& layertree) override;
};