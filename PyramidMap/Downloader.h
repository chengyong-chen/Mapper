#pragma once

class CDownloader : public CAsyncMonikerFile
{
public:
	CDownloader();
	virtual ~CDownloader();

// Overrides
protected:


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloader)
	virtual void OnDataAvailable(DWORD dwSize, DWORD bscfFlag) override;
	virtual void OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR szStatusText) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDownloader)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
};


