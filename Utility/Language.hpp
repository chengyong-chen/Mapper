#pragma once

using namespace std;


namespace Language
{
	void GetLanguageCode(char* code)
	{
		LCID   lid=GetUserDefaultLCID();
		GetLocaleInfoA(lid, LOCALE_SISO639LANGNAME, code, 3);
	}
	void GetCountryCode(char* code)
	{
		LCID   lid=GetUserDefaultLCID();
		GetLocaleInfoA(lid, LOCALE_SISO3166CTRYNAME, code, 4);		
	}
}