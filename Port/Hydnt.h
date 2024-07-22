#pragma once

class CCoverage;



class __declspec(dllexport) CHydnt : public CCoverage
{
public:
	CHydnt(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CHydnt();
	
	struct RAT
	{
		DWORD   Id;
		char	Code[6    +1];
		char	Name[30  +1];  
	}; 
	typedef CTypedPtrMap<CMapWordToPtr,WORD,RAT*> CMapRat;


public:
	PAT_LIST patlakelist;
	PAL_LIST    pallake_list;
	
	CMapRat  rathydmap;
	SEC_LIST sechyd_list;
	
public:
	virtual long	ReadPAT();
	virtual long	ReadAAT();

	virtual long	ReadPATLAKE();

	virtual long	ReadRAT(CHydnt::CMapRat& ratmap, LPCTSTR lpszFile=nullptr);

	virtual BOOL Import(CLayerTree& layertree) override;
};