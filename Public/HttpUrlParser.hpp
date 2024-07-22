using namespace std;

#include <regex>
#include <string>

class CHttpUrlParser
{
public:
	CString scheme;
	CString domain;
	int port;
	CString path;
	CString query;
	bool valid = false;
	//	UrlError ErrorCode;

	CHttpUrlParser(std::string url)
	{
		const std::regex regex(R"(^(([^:\/?#]+):)?//([^:\/?#]*)(:([0-9]+))?(/?([^?#]*))(\?([^#]*))?(#(.*))?)", std::regex::extended);
		std::smatch match;
		if(std::regex_match(url, match, regex)) {
			scheme = match[2].matched ? CString(match[2].str().c_str()) : CString("http");
			if(match[3].matched)
			{
				domain = CString(match[3].str().c_str());
				port = match[5].matched ? std::stoi(match[5].str()) : (scheme.CompareNoCase(_T("http"))==0 ? 80 : 430);
				path = match[7].matched ? CString(match[7].str().c_str()) : CString("");
				valid = true;
			}
			else
				valid = false;
		}
		else {
			valid = false;
		}
	}
};