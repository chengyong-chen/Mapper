// JavascriptMinifierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JavascriptMinifierDlg.h"
#include "afxdialogex.h"

#include <string.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <regex>

#include "../../Basal/UrlCoder.hpp"
#include "../../Basal/Extension.hpp"
#include "../../Basal/StringUtilities.hpp"
#include "../../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"
#include "../../../Thirdparty/WinHttpClient/Common/Include/WinHttpClient.h"

using namespace std;

std::string ReadTextFile(CString strFile)
{
	const CT2CA pszFile(strFile);
	const std::string stdFile(pszFile);

	const std::ifstream stream(stdFile);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	std::string content = buffer.str();
	content = StringUtils::TrimStart(content, std::string("\xEF\xBB\xBF"));
	return content;
}

std::string ReadTextFile(const std::string stdFile)
{
	if(PathFileExistsA(CStringA(stdFile.c_str())))
	{
		const std::ifstream stream(stdFile);
		std::stringstream buffer;
		buffer << stream.rdbuf();
		return buffer.str();
	}
	else
		return std::string("");
}

// CJavascriptMinifierDlg dialog

IMPLEMENT_DYNAMIC(CJavascriptMinifierDlg, CDialogEx)

CJavascriptMinifierDlg::CJavascriptMinifierDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CJavascriptMinifierDlg::IDD, pParent)
{
}

CJavascriptMinifierDlg::~CJavascriptMinifierDlg()
{
}

void CJavascriptMinifierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CJavascriptMinifierDlg, CDialogEx)
	ON_BN_CLICKED(IDC_HTML, OnRadioButtonClicked)
	ON_BN_CLICKED(IDC_JAVASCRIPT, OnRadioButtonClicked)
	ON_BN_CLICKED(IDC_LOADPRESERVES, OnLoadpreserves)
END_MESSAGE_MAP()

// CJavascriptMinifierDlg message handlers
BOOL CJavascriptMinifierDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	static TCHAR BASED_CODE szFilter[] =
			_T("Html files  |*.htm;*.html|")
			_T("All files (*.*)|*.*");
	((CMFCEditBrowseCtrl*)GetDlgItem(IDC_HTMLFILE))->EnableFileBrowseButton(nullptr, szFilter);

	((CButton*)GetDlgItem(IDC_HTML))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_JAVASCRIPT))->SetCheck(FALSE);
	return TRUE;
}

void CJavascriptMinifierDlg::OnOK()
{
	CString strOutput;
	GetDlgItem(IDC_OUTPUTFILE)->GetWindowText(strOutput);
	if(strOutput.IsEmpty())
		return;

	std::string javascript;
	if(((CButton*)GetDlgItem(IDC_HTML))->GetCheck() == BST_CHECKED)
	{
		CString strHtml;
		GetDlgItem(IDC_HTMLFILE)->GetWindowText(strHtml);
		if(PathFileExists(strHtml))
		{
			std::string htmltext = ReadTextFile(strHtml);

			std::smatch matched;
			std::regex rgx("<script type=\"text/javascript\" src=\"(.*?)\"></script>");
			while(std::regex_search(htmltext, matched, rgx))
			{
				javascript += ReadTextFile(matched[1]);

				htmltext = matched.suffix().str();
			}
		}
	}
	else if(((CButton*)GetDlgItem(IDC_JAVASCRIPT))->GetCheck() == BST_CHECKED)
	{
		CString strFiles;
		((CEdit*)GetDlgItem(IDC_JAVASCRIPTFILES))->GetWindowText(strFiles);

		int nTokenPos = 0;
		CString strToken = strFiles.Tokenize(_T("\r\n"), nTokenPos);
		while(!strToken.IsEmpty())
		{
			if(PathFileExists(strToken))
			{
				javascript += ReadTextFile(strToken);
			}

			strToken = strFiles.Tokenize(_T("\r\n"), nTokenPos);
		}
	}
	javascript = UriEncode(javascript);

	try
	{
		WinHttpClient webclient(L"http://closure-compiler.appspot.com/compile");
		std::string data = "js_code=" + javascript;
		data += "&output_format=xml";
		data += "&output_info=compiled_code";
		data += "&output_info=errors";
		data += "&output_info=warnings";
		data += "&output_info=statistics";
		data += "&compilation_level=ADVANCED_OPTIMIZATIONS";
		CString strPreverses;
		((CEdit*)GetDlgItem(IDC_PRESERVES))->GetWindowText(strPreverses);
		if(strPreverses.IsEmpty() == FALSE)
		{
			CT2CA pszPreverses(strPreverses);
			std::string stdPreverses(pszPreverses);
			data += "&js_externs=" + stdPreverses;
		}
		webclient.SetAdditionalDataToSend((BYTE *)data.c_str(), data.size());

		// Set request headers.
		wchar_t szSize[50] = L"";
		swprintf_s(szSize, L"%d", data.size());
		wstring headers = L"Content-Length: ";
		headers += szSize;
		headers += L"\r\n";
		headers += L"Content-Type: application/x-www-form-urlencoded";
		webclient.SetAdditionalRequestHeaders(headers);

		// Send HTTP post request.
		AfxGetApp()->BeginWaitCursor();
		webclient.SendHttpRequest(L"POST");

		wstring httpResponseHeader = webclient.GetResponseHeader();
		wstring httpResponseContent = webclient.GetResponseContent();

		tinyxml2::XMLDocument xml;
		std::vector<char> output;
		WStringToCharArray(httpResponseContent, output);
		if(xml.Parse(&output[0]) == tinyxml2::XMLError::XML_SUCCESS)
		{
			tinyxml2::XMLElement* pElement = xml.FirstChildElement("compilationResult")->FirstChildElement("compiledCode");
			if(pElement != nullptr)
			{
				const char* compressed = pElement->GetText();
				if(compressed != nullptr)
				{
					std::ofstream ofs;
					ofs.open(strOutput, ios::binary);
					ofs.write(compressed, strlen(compressed));
					ofs.close();
				}
			}
		}
	}
	catch(...)
	{
	}
	AfxGetApp()->EndWaitCursor();
}

void CJavascriptMinifierDlg::OnRadioButtonClicked()
{
	if(((CButton*)GetDlgItem(IDC_HTML))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_JAVASCRIPTFILES)->EnableWindow(TRUE);
		GetDlgItem(IDC_HTMLFILE)->EnableWindow(FALSE);
	}
	else if(((CButton*)GetDlgItem(IDC_JAVASCRIPT))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_JAVASCRIPTFILES)->EnableWindow(TRUE);
		GetDlgItem(IDC_HTMLFILE)->EnableWindow(FALSE);
	}
}

//C:\Projects\Giscake\lib/third.js
//C:\Projects\Giscake\lib/leaflet-src.js
//C:\Projects\Giscake\lib/proj4js/lib/proj4js.js
//C:\Projects\Giscake\lib/proj4js/lib/projCode/merc.js
//C:\Projects\Giscake\lib/proj4js/lib/projCode/tmerc.js
//C:\Projects\Giscake\Giscake.js
//C:\Projects\Giscake\Basic/Basic.js
//C:\Projects\Giscake\Basic/Rect.js
//C:\Projects\Giscake\Basic/Coord.js
//C:\Projects\Giscake\Basic/Size.js
//C:\Projects\Giscake\Basic/Datatypes.js
//C:\Projects\Giscake\MapLayer\BlankLayer.js
//C:\Projects\Giscake\MapLayer\RasterLayer.js
//C:\Projects\Giscake\MapLayer\TileLayer.js
//C:\Projects\Giscake\MapLayer\RasterTileLayer.js
//C:\Projects\Giscake\Holder.js
//C:\Projects\Giscake\Map.js
//C:\Projects\Giscake\Provider/Provider.js
//C:\Projects\Giscake\Projection/Projection.js
//C:\Projects\Giscake\Projection/Geographic.js
//C:\Projects\Giscake\Projection/Geocentric.js
//C:\Projects\Giscake\Provider/TileProvider.js
//C:\Projects\Giscake\Provider/RasterTileProvider.js
//C:\Projects\Giscake\Tile/Queue.js
//C:\Projects\Giscake\Tile/Tile.js
//C:\Projects\Giscake\Tile/Pooling.js
//C:\Projects\Giscake\Tile/ImgLoading.js
//C:\Projects\Giscake\Tile/Waiting.js
//C:\Projects\Giscake\Tile/Caching.js

void CJavascriptMinifierDlg::OnLoadpreserves()
{
	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY, _T("All Files (*.*)|*.*|"), this);
	if(dlg.DoModal() == IDOK)
	{
		CFile file(dlg.GetPathName(), CFile::modeRead);
		CString text = _T("");

		char buffer[1024];
		UINT unRead = file.Read(buffer, sizeof(buffer) - 1);
		while(unRead)
		{
			buffer[unRead] = '\0';
			text += buffer;
			unRead = file.Read(buffer, sizeof(buffer) - 1);
		}
		GetDlgItem(IDC_PRESERVES)->SetWindowText(text);
	}
}
