#pragma once

using namespace std;
#include <string>
#include <map>
#include <list>
#include <format>
namespace Number
{
	struct Abbrv
	{
		unsigned int value;
		std::string symbol;
	};
	const std::map<const char*, std::list<Abbrv>> abbrvs=
	{
		{"en", {{1, ""}, {(unsigned int)(1E3), "k"}, {(unsigned int)(1E6), "M"}, {(unsigned int)(1E9), "B"}, {(unsigned int)(1E12), "T"}, {(unsigned int)(1E15), "P"}, {(unsigned int)(1E18), "E"}}},
		{"zh", {{1, ""}, {(unsigned int)(1E4), "万"}, {(unsigned int)(1E8), "亿"}, {(unsigned int)(1E12), "万亿"}, {(unsigned int)(1E13), "兆"}}}
	};

	std::string abbrnum(float num, const char* locale)
	{
		auto it1=abbrvs.find(locale) == abbrvs.end() ? abbrvs.find("en") : abbrvs.find(locale);
		std::list<Abbrv> abbrv=it1->second;

		char sign=num < 0 ? '-' : ' ';
		num=std::abs(num);
		Abbrv* maximum=nullptr;
		for(std::list<Abbrv>::iterator it2=abbrv.begin(); it2 != abbrv.end(); ++it2)
		{
			if(num >= it2->value)
			{
				maximum=&(*it2);
			}
		}
		return  maximum == nullptr ? std::format("{:.2f}", num) : (sign + std::format("{:.2f}", num/maximum->value) + maximum->symbol);
	}
}