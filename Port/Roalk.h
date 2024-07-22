#pragma once

class CCoverage;



class __declspec(dllexport) CRoalk : public CCoverage
{
public:
	CRoalk(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CRoalk();
	
	struct RAT
	{
		DWORD   ID;
		DWORD   UserID;
		char	Code[4 +1];  
		char	Name[20+1];  	
		char	FName[20+1];		
		char	TName[20+1];	
	}; 
	typedef CTypedPtrMap<CMapWordToPtr,WORD,RAT*> CMapRat;

public:
	CMapRat  ratnatmap;
	CMapRat  rathigmap;

	SEC_LIST secnat_list;
	SEC_LIST sechig_list;

public:
	virtual long	ReadPAT();
	virtual long	ReadAAT();
	virtual long	ReadRAT(CRoalk::CMapRat& ratmap, LPCTSTR lpszFile=nullptr);

	virtual BOOL Import(CLayerTree& layertree) override;
};
