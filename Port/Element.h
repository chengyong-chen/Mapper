#pragma once

#include <set>
#include <string>
#include "Style.h"

namespace StyleSheet
{
	enum SelectorType :BYTE
	{
		SELECTOR_TYPE,
		SELECTOR_CLASS,
		SELECTOR_ID
	};
	class Element
	{
	public:
		StyleSheet::SelectorType m_selectortype;
		std::string  m_selectorname;

	private:
		std::map<std::string, std::string> m_properties;

	public:
		static Element parse(const char* it)
		{
			Element element;
			char* p = (char*)it;
			while(p != nullptr)
			{
				std::string name = StyleSheet::CTokenString::read_string(p, ':');
				if(name.empty())
					break;
				std::string value = StyleSheet::CTokenString::read_string(p, ';');
				if(value.empty())
					break;
				element.addProperty(name, value);
			}
			return element;
		}

		std::size_t getPropertyCount() const
		{
			return m_properties.size();
		}
		std::map<std::string, std::string>& getProperties()
		{
			return m_properties;
		}
		void addProperty(std::string name, string value)
		{
			m_properties[name] = value;
		}

	public:
		bool operator<(const Element& p) const
		{
			return m_selectorname < p.m_selectorname;
		}
	};
}
