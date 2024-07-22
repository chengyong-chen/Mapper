#pragma once

class CCoverage;



class __declspec(dllexport) CHydlk : public CCoverage
{
public:
	CHydlk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CHydlk();
	

public:
	virtual long	ReadPAT();
	virtual long	ReadAAT();

	virtual BOOL Import(CLayerTree& layertree) override;
};