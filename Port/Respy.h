#pragma once

class CRespt;





class __declspec(dllexport) CRespy : public CRespt
{
	struct CPOPUPat : public PAT	
	{
		char RName[40  +1];	
		char PYName[60 +1];  
		char ADClass[2 +1];	
	}; 
public:
	CRespy(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CRespy();
		
public:
	PAT_LIST    patpopulist;
	PAL_LIST    palpopu_list;

	virtual long ReadPATPOPU() override;
	virtual long CRespy::ReadPAT();
	virtual BOOL Import(CLayerTree& layertree) override;
};
