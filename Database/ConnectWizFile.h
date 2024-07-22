#pragma once

using namespace std;
#include <map>

#ifndef		SAFE_FREE
#define SAFE_FREE( p )		{ if( p ){ free( (LPVOID)p ); p = nullptr; } }
#endif
#ifndef	SAFE_RELEASE
#if defined( __cplusplus )
#define SAFE_RELEASE( p ) \
			if( p ){ p->Release(); p = nullptr; }
#else
#define SAFE_RELEASE( p ) \
			if( p ){ p->lpVtbl->Release( p ); p = nullptr; }
#endif
#endif
#ifndef		SAFE_COTASKMEMFREE
#define SAFE_COTASKMEMFREE( p ) \
		if( p ){ CoTaskMemFree( (LPVOID)p ); p = nullptr; }
#endif

#include "Resource.h"


class IMyFolderFilter;
class CODBCDatabase;
class CConnectWizFile : public CPropertyPage
{
public:
	CConnectWizFile(std::map<CString, CODBCDatabase*>& databaselist, CODBCDatabase& ConnectInfo);

	~CConnectWizFile() override;

	// Dialog Data
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_PICKFILE
	};

public:
	IMyFolderFilter* m_pIMyFolderFilter;

	// Overrides
public:
	BOOL OnSetActive() override;

	LRESULT OnWizardBack() override;

	LRESULT OnWizardNext() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	// Implementation
protected:
	// Generated message map functions
	BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()

	void SetWizardButtons() const;

	CODBCDatabase& m_ConnectInfo;

	std::map<CString, CODBCDatabase*>& m_existinglist;

public:
	afx_msg void OnBnClickedPickfile();
};

#if defined	( __IFolderFilterSite_FWD_DEFINED__			)
#if defined ( __IFolderFilterSite_INTERFACE_DEFINED__	)
#if defined ( __IFolderFilter_FWD_DEFINED__				)
#if defined ( __IFolderFilter_INTERFACE_DEFINED__		)
#define USE_XP_FILTRATION
#endif
#endif
#endif
#endif

#ifndef		 _INC_SHLWAPI
#include < shlwapi.h >				// Required for the StrRet and Path functions
#endif
#pragma comment( lib, "shlwapi.lib" )

class IMyFolderFilter : public IFolderFilter
{
public:
	IMyFolderFilter(VOID);

public: // IUnknown implementation

	STDMETHOD(QueryInterface)(IN REFIID /*riid*/, IN OUT LPVOID* /*ppvObj*/) override;

	STDMETHOD_(ULONG, AddRef)(VOID) override;

	STDMETHOD_(ULONG, Release)(VOID) override;

public: // IFolderFilter implementation

	STDMETHOD(ShouldShow)(IN IShellFolder* /*pIShellFolder*/, IN LPCITEMIDLIST /*pidlFolder*/, IN LPCITEMIDLIST IN /*pidlItem*/) override;

	STDMETHOD(GetEnumFlags)(IN IShellFolder* /*pIShellFolder*/, IN LPCITEMIDLIST /*pidlFolder*/, IN HWND* /*phWnd*/, OUT LPDWORD /*pdwFlags*/) override;

protected:
	ULONG m_ulRef;

public:
	LPCTSTR m_pszFilter;
};
