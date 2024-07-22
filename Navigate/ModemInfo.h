/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1999 by Seain B. Conover, Tarasoft Corporation
// All rights reserved

// Distribute freely. Absolutely do not remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.

// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.

// Read *Atlas Shrugged*, http://www.aynrand.org

/////////////////////////////////////////////////////////////////////////////

#pragma once

// ModemInfo.h : header file

/////////////////////////////////////////////////////////////////////////////
// CModemInfo class

class CModemInfo
{
public:
	CModemInfo();
	virtual ~CModemInfo();

	// Operations
public:
	int GetModemCount() const;
	int GetModemBps(CString strName) const;
	int GetModemComPort(int nIndex) const;

	CString GetFirstModemName() const;

	// init string, with optional desired volume level specified
	// error control on, compression on, hardward flow control, modulation CCITT, blind off, call setup fail timer, inactivity timer + if requested, includes  volume init
	CString GetModemInitString(CString strName, int nSpeakerVolume=-1) const;

	// fills CComboBoxBox with list of modem names
	void FillComboBoxWithModemNames(CComboBox* pComboBox) const;

	// modem name based on index
	CString GetModemName(int nIndex) const;

	// does modem have speaker
	BOOL GetModemHasSpeaker(CString strName) const;

	// how many volume levels? 
	int GetModemVolumeLevelsCount(CString strName) const;

protected:
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault, HKEY hKey) const;
	CString GetModemRegistrySection(int nIndex) const;
	int GetModemIndex(CString strName) const;

	static CString GetPaddedIndex(int nIndex);

	// Attributes
protected:
	CStringArray m_aModems;
	CString m_strModemClassGuid;
};

/////////////////////////////////////////////////////////////////////////////
