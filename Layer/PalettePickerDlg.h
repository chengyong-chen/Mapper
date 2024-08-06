#pragma once
#include "afxdialogex.h"
#include <vector>
#include <string>

class CPalettePickerDlg : public CDialogEx
{
public:
    CPalettePickerDlg(CWnd* pParent = nullptr);
    CPalettePickerDlg(CWnd* pParent, const CStringA& filename);

    enum { IDD = IDD_PALETTEPICKER    };

private:
    std::vector<std::pair<std::string, std::vector<COLORREF>>> m_palettes;
public:
    std::vector<COLORREF> m_selectedColors;
public:
    CComboBox m_comboPalette;
    CStatic m_colorPreview;

private:
    std::vector<std::string> Split(const std::string& s, char delimiter);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    virtual BOOL OnInitDialog();
    void OnOK();
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnCbnSelchangeComboPalette();
};
