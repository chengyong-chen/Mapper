// SingleInstanceApp.cpp : implementation file
//
// CWinApp with single-instance support
//
// Copyright (C) 1997, 1998 Giancarlo Iovino (giancarlo@saria.com)
// All rights reserved. May not be sold for profit.
//
// Thanks to Kevin Lussier for the base idea.
//
// This code was developed for MFC Programmers SourceBook
// (http://www.codeguru.com)
//

#include "stdafx.h"
#include "SingleInstanceApp.h"

CSingleInstanceApp::CSingleInstanceApp()
{
	// Set our default values
	m_hMutex = nullptr;
}

CSingleInstanceApp::~CSingleInstanceApp()
{
	// Release the mutex
	if(m_hMutex != nullptr) {
		ReleaseMutex(m_hMutex);
	}
}

BOOL CSingleInstanceApp::CheckSingleInstance(UINT nId) 
{
	CString strFullString;

	// Generate a synthetic name for this class using the
	// AfxRegisterWndClass convention
	HINSTANCE hInst = AfxGetInstanceHandle();
	UINT nClassStyle = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;	
	HCURSOR hCursor = LoadCursor(IDC_ARROW);
	HICON	hIcon = LoadIcon(MAKEINTRESOURCE(nId));
	HBRUSH hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

	LPCTSTR lpstrAppName = AfxGetAppName();
	ASSERT(lpstrAppName != nullptr);

	if(hCursor == nullptr && hbrBackground == nullptr && hIcon == nullptr)
		m_strClassName.Format(_T("%s:%x:%x"), lpstrAppName,
			(UINT)hInst, nClassStyle);
	else
		m_strClassName.Format(_T("%s:%x:%x:%x:%x"), lpstrAppName,
			(UINT)hInst, nClassStyle, (UINT)hCursor, (UINT)hbrBackground);
	
	// Create the mutex with the class name
	m_hMutex = CreateMutex(nullptr, FALSE, m_strClassName);
	// Check for errors
	if(GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		// Reset our mutex handle (just in case)
		m_hMutex = nullptr;
		// The mutex already exists: an instance is already
		// running. Find the app window and bring it to foreground
		HWND hWnd = FindWindowEx(nullptr, nullptr, m_strClassName, nullptr);
		if(hWnd != nullptr) 
		{
			WINDOWPLACEMENT wndpl;
			ZeroMemory(&wndpl, sizeof(WINDOWPLACEMENT));
			wndpl.length = sizeof(WINDOWPLACEMENT);
			if(GetWindowPlacement(hWnd, &wndpl))
			{
				if(wndpl.showCmd == SW_SHOWMINIMIZED)
					ShowWindow(hWnd, SW_RESTORE);
				else
					ShowWindow(hWnd, wndpl.showCmd);
			}
			else
				ShowWindow(hWnd, SW_RESTORE);

			BringWindowToTop(hWnd);
			SetForegroundWindow(hWnd);

			HandleExistedWindow(hWnd);
		}
	
		// Return failure
		return FALSE;
	}

	// Register the unique window class name
	WNDCLASS wndclass;	
	ZeroMemory(&wndclass, sizeof(WNDCLASS));

	/*
	wndclass.style = nClassStyle;
	wndclass.lpfnWndProc = AfxWndProc;
	wndclass.hInstance = hInst;
	wndclass.hIcon = hIcon; 
	wndclass.hCursor = hCursor;
	wndclass.hbrBackground = hbrBackground;
	wndclass.lpszMenuName = nullptr;
	*/
	wndclass.style = nClassStyle;
	wndclass.lpfnWndProc = DefDlgProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;
	wndclass.hInstance = hInst;
	wndclass.hIcon = hIcon;
	wndclass.hCursor = hCursor;
	wndclass.hbrBackground = hbrBackground;
	wndclass.lpszMenuName = nullptr;
	wndclass.lpszClassName = m_strClassName; // The class name
	
	// Use AfxRegisterClass to register the class, exit if it fails
	if(!AfxRegisterClass(&wndclass)) {
		AfxMessageBox(_T("Failed to register window class!"), MB_ICONSTOP | MB_OK);
		// Return failure
		return FALSE;
	}

	// Return success
	return TRUE;
}

CString CSingleInstanceApp::GetClassName() const
{
	return m_strClassName;
}

void CSingleInstanceApp::HandleExistedWindow(HWND)
{
}

