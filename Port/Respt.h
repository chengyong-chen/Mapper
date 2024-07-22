#pragma once

class CCoverage;




class __declspec(dllexport) CRespt : public CCoverage
{
public:
	CRespt(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CRespt();
	
public:
	virtual long ReadPAT() override;

	virtual BOOL Import(CLayerTree& layertree) override;
};