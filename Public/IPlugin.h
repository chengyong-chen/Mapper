#pragma once

struct Plugin
{
	UINT ID;
	CString title;
	CStringA file;
};

interface IPlugin
{
	public:
	virtual void Invoke(int count,...) = 0;
};
