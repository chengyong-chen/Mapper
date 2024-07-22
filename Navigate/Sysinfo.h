// Sysinfo.h: interface for the Sysinfo class.
// Written by Tran Ngoc Tien 1999-2K.
// email: Cats@hn.fpt.vn, M_blue_bird@yahoo.com
//////////////////////////////////////////////////////////////////////



#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class __declspec(dllexport) Sysinfo  
{
public:	
	Sysinfo();
	CString TNTGetInfo() ;	
	virtual ~Sysinfo();
};


