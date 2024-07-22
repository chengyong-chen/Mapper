#pragma once

#include <map>
#include <list>
#include <queue>

using namespace std;

struct SVehicle;

class CVehicleVect : public vector<SVehicle*>
{
public:
	virtual ~CVehicleVect();

public:
	CVehicleVect::iterator find(const DWORD& dwId);
	
	SVehicle* GetVehicleInfo(const DWORD& dwId);
	SVehicle* GetVehicleInfo(LPCTSTR szCode);
	SVehicle* GetVehicleInfo(CString* pStrTel, BOOL bUseGPRS=FALSE);

	BOOL GetBroadCastVehicleInfos(ADOCG::_ConnectionPtr &pDBConnect);	
	BOOL GetVehicleInfo(const DWORD& dwId, SVehicle &vhcInfo, ADOCG::_ConnectionPtr &pDBConnect);

public:
	void DirtyFalseAllVhc();
	void ClearAllNotDirtyVhc();
	
	void clear();
};










