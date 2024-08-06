#include "stdafx.h"
#include "Resource.h"

#include "PalettePickerDlg.h"
#include "../Public/Global.h"
#include <fstream>
#include <sstream>


CPalettePickerDlg::CPalettePickerDlg(CWnd* pParent)
	: CDialogEx(IDD_PALETTEPICKER, pParent)
{

}
CPalettePickerDlg::CPalettePickerDlg(CWnd* pParent, const CStringA& filename)
    : CDialogEx(IDD_PALETTEPICKER, pParent)
{
    std::ifstream file(GetExeDirectory() + "palettes\\" + filename);
    std::string line;
    while(std::getline(file, line))
    {
        size_t colonPos = line.find(':');
        if(colonPos != std::string::npos)
        {
            std::string name = line.substr(0, colonPos);
            std::string colors = line.substr(colonPos + 1);

            std::vector<std::string> colorStrings = this->Split(colors, ',');
            std::vector<COLORREF> colorValues;
            for(const auto& color : colorStrings)
            {
                colorValues.push_back(strtol(color.c_str() + 1, nullptr, 16));
            }

            m_palettes.push_back({name, colorValues});
        }
    }
}


void CPalettePickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PALETTE, m_comboPalette);
	DDX_Control(pDX, IDC_COLOR_PREVIEW, m_colorPreview);
}


BEGIN_MESSAGE_MAP(CPalettePickerDlg, CDialogEx)
    ON_CBN_SELCHANGE(IDC_COMBO_PALETTE, &CPalettePickerDlg::OnCbnSelchangeComboPalette)
END_MESSAGE_MAP()
   
BOOL CPalettePickerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    for(const auto& palette : m_palettes)
    {
        m_comboPalette.AddString(CString(palette.first.c_str()));
    }

    return TRUE; 
}


void CPalettePickerDlg::OnCbnSelchangeComboPalette()
{
    int selectedIndex = m_comboPalette.GetCurSel();
    if(selectedIndex != CB_ERR)
    {
        CClientDC dc(this);
        CRect rect;
        m_colorPreview.GetWindowRect(&rect);
        ScreenToClient(&rect);

        const std::vector<COLORREF>& colors = m_palettes[selectedIndex].second;
        int width = rect.Width() / colors.size();
        for(size_t i = 0; i < colors.size(); ++i)
        {
            CBrush brush(colors[i]);
            CRect colorRect(rect.left + i * width, rect.top, rect.left + (i + 1) * width, rect.bottom);
            dc.FillRect(&colorRect, &brush);
        }
        m_selectedColors = colors;
    }
}


std::vector<std::string> CPalettePickerDlg::Split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while(std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void CPalettePickerDlg::OnOK()
{
    int selectedIndex = m_comboPalette.GetCurSel();
    if(selectedIndex != CB_ERR)
    {
    }

    CDialogEx::OnOK();
}