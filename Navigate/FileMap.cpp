#include "stdafx.h"
#include <io.h>
#include "FileMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFileMap::CFileMap()
{
	m_hFile = nullptr;
	m_hFileMap = nullptr;
	m_hFileMapView = nullptr;
}

CFileMap::~CFileMap()
{
	if(m_hFileMapView!=nullptr)
	{
		UnmapViewOfFile(m_hFileMapView);
		m_hFileMapView = nullptr;
	}
	if(m_hFileMap!=nullptr)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = nullptr;
	}
	if(m_hFile!=nullptr)
	{
		CloseHandle(m_hFile);
		m_hFile = nullptr;
	}
}

bool CFileMap::Open(CString strFile)
{
	if(_taccess(strFile, 00)==-1)
		return false;

	m_hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
	if(m_hFile==INVALID_HANDLE_VALUE)
	{
		m_hFile = nullptr;
		return false;
	}

	m_hFileMap = CreateFileMapping(m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if(m_hFileMap==INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		return false;
	}
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_hFileMap);
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		return false;
	}

	// ”≥…‰÷¡ƒ⁄¥Ê
	m_hFileMapView = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
	if(m_hFileMapView==nullptr)
	{
		CloseHandle(m_hFileMap);
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		m_hFileMapView = nullptr;
		return false;
	}
	else
	{
		return true;
	}
}

void CFileMap::Close()
{
	if(m_hFileMapView!=nullptr)
	{
		UnmapViewOfFile(m_hFileMapView);
		m_hFileMapView = nullptr;
	}
	if(m_hFileMap!=nullptr)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = nullptr;
	}
	if(m_hFile!=nullptr)
	{
		CloseHandle(m_hFile);
		m_hFile = nullptr;
	}
}
