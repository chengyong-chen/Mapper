#pragma once

class CCoverage;
class PAT;
class AAT;

	



class __declspec(dllexport) CBount : public CCoverage
{
	struct CBountPat : public PAT
	{
		TCHAR Pac[7];	

		CBountPat()
			:PAT()
		{
			_tcsnset(Pac,0X00,7);
		}
	}; 
public:
	CBount(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CBount();
	
	PAL_LIST palcnty_list;
	PAT_LIST patcntylist;
	
	PAL_LIST paldist_list;
	PAT_LIST patdistlist;
	
	PAL_LIST palprov_list;
	PAT_LIST patprovlist;
	
public:
	virtual long	ReadPAT();
	virtual long	ReadAAT();


	virtual long	ReadPATCNTY();
	virtual long	ReadPATDIST();
	virtual long	ReadPATPROV();


	virtual BOOL Import(CLayerTree& layertree) override;
};