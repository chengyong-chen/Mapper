// Markup.h: interface for the CMarkup class.

// Markup Release 6.2
// Copyright (C) 1999-2001 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#pragma once

#include <afxtempl.h>

#ifdef _DEBUG
#define _DS(i) (i?&((LPCTSTR)m_csDoc)[m_aPos[i].nStartL]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif

class AFX_EXT_CLASS CMarkup
{
public:
	CMarkup() { SetDoc(nullptr); };
	CMarkup(LPCTSTR szDoc) { SetDoc(szDoc); };
	CMarkup(const CMarkup& markup) { *this = markup; };
	void operator=(const CMarkup& markup);

	virtual ~CMarkup()
	{
	};

	// Navigate
	bool Load(LPCTSTR szFileName);
	bool SetDoc(LPCTSTR szDoc);
	bool IsWellFormed();
	bool FindElem(LPCTSTR szName = nullptr);
	bool FindChildElem(LPCTSTR szName = nullptr);
	bool IntoElem();
	bool OutOfElem();
	void ResetChildPos() { m_iPosChild = 0; };
	void ResetPos();
	CString GetPosData(int nStart, int nEnd) const;
	CString GetTagName() const { return x_GetTagName(m_iPos); };
	CString GetChildTagName() const { return x_GetTagName(m_iPosChild); };
	CString GetData() const { return x_GetData(m_iPos); };
	CString GetChildData() const { return x_GetData(m_iPosChild); };
	CString GetAttrib(LPCTSTR szAttrib) const { return x_GetAttrib(m_iPos, szAttrib); };
	CString GetChildAttrib(LPCTSTR szAttrib) const { return x_GetAttrib(m_iPosChild, szAttrib); };
	CString GetAttribName(int n) const;
	bool SavePos(LPCTSTR szPosName = _T(""));
	bool RestorePos(LPCTSTR szPosName = _T(""));
	bool GetOffsets(int& nStart, int& nEnd) const;
	CString GetError() const { return m_csError; };

	// Create
	bool Save(LPCTSTR szFileName) const;
	CString GetDoc() const { return m_csDoc; };
	bool AddElem(LPCTSTR szName, LPCTSTR szData = nullptr);
	bool AddChildElem(LPCTSTR szName, LPCTSTR szData = nullptr);
	bool InsertChildElem(LPCTSTR szName, LPCTSTR szData = nullptr);
	bool AddAttrib(LPCTSTR szAttrib, LPCTSTR szValue);
	bool AddChildAttrib(LPCTSTR szAttrib, LPCTSTR szValue);
	bool AddAttrib(LPCTSTR szAttrib, int nValue);
	bool AddChildAttrib(LPCTSTR szAttrib, int nValue);
	bool AddChildSubDoc(LPCTSTR szSubDoc);
	bool InsertChildSubDoc(LPCTSTR szSubDoc);
	CString GetChildSubDoc() const;

	// Modify
	bool RemoveElem();
	bool RemoveChildElem();
	bool SetAttrib(LPCTSTR szAttrib, LPCTSTR szValue);
	bool SetChildAttrib(LPCTSTR szAttrib, LPCTSTR szValue);
	bool SetAttrib(LPCTSTR szAttrib, int nValue);
	bool SetChildAttrib(LPCTSTR szAttrib, int nValue);
	bool SetData(LPCTSTR szData, int nCDATA = 0) { return x_SetData(m_iPos, szData, nCDATA); };
	bool SetChildData(LPCTSTR szData, int nCDATA = 0) { return x_SetData(m_iPosChild, szData, nCDATA); };

protected:

#ifdef _DEBUG
	LPCTSTR m_pMainDS;
	LPCTSTR m_pChildDS;
#endif

	CString m_csDoc;
	int m_nLevel;
	CString m_csError;

	struct ElemPos
	{
		ElemPos() { Clear(); };
		ElemPos(const ElemPos& pos) { *this = pos; };
		bool IsEmptyElement() const { return (nStartR==nEndL+1); };

		void Clear()
		{
			nStartL = 0;
			nStartR = 0;
			nEndL = 0;
			nEndR = 0;
			nNext = 0;
			iElemParent = 0;
			iElemChild = 0;
			iElemNext = 0;
		};
		int nStartL;
		int nStartR;
		int nEndL;
		int nEndR;
		int nNext;
		int iElemParent;
		int iElemChild;
		int iElemNext;
	};

	CArray<ElemPos, ElemPos&> m_aPos;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;

	int x_GetFreePos();
	int x_ReleasePos();

	struct TokenPos
	{
		TokenPos() { Clear(); };
		bool IsValid() const { return (nL<=nR); };

		void Clear()
		{
			nL = 0;
			nR = -1;
			bIsString = false;
		};

		bool Match(LPCTSTR szDoc, LPCTSTR szName) const
		{
			const int nLen = nR-nL+1;
			// To ignore case, define MARKUP_IGNORECASE
#ifdef MARKUP_IGNORECASE
			return ((_tcsncicmp(&szDoc[nL], szName, nLen)==0)
#else
			return ((_tcsnccmp(&szDoc[nL], szName, nLen)==0)
#endif
				&&(szName[nLen]==_T('\0')||_tcschr(_T(" =/["), szName[nLen])));
		};
		int nL;
		int nR;
		int nNext;
		bool bIsString;
	};

	struct SavedPos
	{
		int iPos;
		int iPosChild;
		int iLevel;
	};

	CMap<CString, LPCTSTR, SavedPos, SavedPos> m_aSavedPos;

	int x_ParseElem(int iPos);
	int x_ParseError(LPCTSTR szError, LPCTSTR szTag = nullptr);
	bool x_FindChar(int& n, _TCHAR c) const;
	bool x_FindToken(TokenPos& token) const;
	CString x_GetToken(const TokenPos& token) const;
	int x_FindElem(int iPosParent, int iPos, LPCTSTR szPath);
	CString x_GetTagName(int iPos) const;
	CString x_GetData(int iPos) const;
	CString x_GetAttrib(int iPos, LPCTSTR szAttrib) const;
	int x_Add(int iPosParent, int iPosBefore, LPCTSTR szName, LPCTSTR szValue);
	int x_AddSubDoc(int iPosParent, int iPosBefore, LPCTSTR szSubDoc);
	bool x_FindAttrib(TokenPos& token, LPCTSTR szAttrib = nullptr) const;
	int x_AddAttrib(int iPos, LPCTSTR szAttrib, LPCTSTR szValue);
	int x_SetAttrib(int iPos, LPCTSTR szAttrib, LPCTSTR szValue);
	bool x_SetData(int iPos, LPCTSTR szData, int nCDATA);
	int x_Remove(int iPos);
	void x_DocChange(int nLeft, int nReplace, const CString& csInsert);
	void x_PosInsert(int iPos, int nInsertLength);
	void x_Adjust(int iPos, int nShift);
	CString x_TextToDoc(LPCTSTR szText, bool bAttrib = false) const;
	CString x_TextFromDoc(int nLeft, int nRight) const;
};
