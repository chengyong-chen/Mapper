#pragma once
#include <afxdialogex.h>
#include <list>

#include "../GridCtrl/GridCtrl.h"

class CFieldsPickerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFieldsPickerDlg)

public:
	CFieldsPickerDlg(CWnd* pParent, std::list<std::pair<CString, CString>> fileds, CString fieldAid, CString fieldName, CString fieldCode, CString fieldCentroidX, CString fieldCentroidY, std::list<std::list<CString>> samples, CString caption);   // standard constructor
	virtual ~CFieldsPickerDlg();

private:
	std::list<std::pair<CString, CString>> m_fileds;
	std::list<std::list<CString>> m_samples;
	CString m_caption;

public:
	CString m_strFieldAid;
	CString m_strFieldAnno;
	CString m_strFieldCode;
	CString m_strFieldCentroidX;
	CString m_strFieldCentroidY;
public:
	CGridCtrl m_DBGrid;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIELDPICKER };
#endif

protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	void OnOK() override;
	DECLARE_MESSAGE_MAP()
};
