#pragma once

class __declspec(dllexport) EndianSwap
{
public:
	EndianSwap();
	~EndianSwap();

	union unionUshort
	{
		USHORT usNum;
		BYTE bytes[2];
	};

	union unionShort
	{
		SHORT nNum;
		BYTE bytes[2];
	};

	union unionUint
	{
		UINT nNum;
		BYTE bytes[4];
	};

	union unionInt
	{
		INT nNum;
		BYTE bytes[4];
	};

	union unionUlong
	{
		ULONG nNum;
		BYTE bytes[4];
	};

	union unionLong
	{
		LONG nNum;
		BYTE bytes[4];
	};

	union unionString
	{
		CHAR charr[1024];
		WCHAR wcharr[512];
	};

	static void Switch(USHORT& SrcDest);
	static void Switch(SHORT& SrcDest);
	static void Switch(UINT& SrcDest);
	static void Switch(INT& SrcDest);
	static void Switch(ULONG& SrcDest);
	static void Switch(LONG& SrcDest);
	static void Switch(WCHAR* SrcDest, size_t Size);
};
