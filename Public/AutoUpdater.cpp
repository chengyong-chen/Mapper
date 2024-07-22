#include "stdafx.h"
#include "AutoUpdater.h"
#include "WebService.h"

#include "../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"

using namespace std;
#include <variant>
#include <vector>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoUpdater::CAutoUpdater()
{
}

CAutoUpdater::~CAutoUpdater()
{
}

CString CAutoUpdater::GetNewVersion(std::string strSoftware, std::string strVersion)
{
	CString version = _T("");
	const auto failed = [](CString message)->void
	{
		AfxMessageBox(message);
	};
	const auto success = [&version](std::string response)->void
	{
		tinyxml2::XMLDocument xml;
		xml.Parse(response.c_str());
		//	TiXmlElement* node = xml.FirstChildElement("soap:Envelope")->FirstChildElement("soap:Body")->FirstChildElement("GetVersionResponse")->FirstChildElement("GetVersionResult");
		tinyxml2::XMLElement* node = xml.FirstChildElement("string");
		if(node!=nullptr&&node->FirstChild()!=nullptr)
		{
			version = CString(node->FirstChild()->Value());
		}
	};

	std::map<CString, std::variant<std::string, int>> parameters =
	{
		{_T("software"), strSoftware},
		{_T("version"), strVersion}
	};
	std::map<CString, CString> files;
	CWebService::CallService("http://www.diwatu.com/webservice/softwareversion.asmx/GetVersion", _T(""), _T(""), _T(""), parameters, files, failed, success);
	return version;
}