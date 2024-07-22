#pragma once

#include "TokenString.h"
#include "Element.h"

#include <set>
using namespace std;

namespace StyleSheet
{
	class Style
	{
	private:
		std::set<Element> m_elements;

	public:
		static Style parse(const char* it)
		{
			Style style;
			char* p = (char*)it;
			while (*p != EOF)
			{
				std::string selector = StyleSheet::CTokenString::read_word(p, CHAR_DIGIT | CHAR_INTCHAR | CHAR_FLOATCHAR | CHAR_NAMECHAR);
				if (selector.empty())
					break;
				StyleSheet::SelectorType selectortype = StyleSheet::SelectorType::SELECTOR_CLASS;
				switch (selector.at(0))
				{
				case '.':
					selector = selector.substr(1);
					selectortype = SelectorType::SELECTOR_CLASS;
					break;
				case '#':
					selector = selector.substr(1);
					selectortype = SelectorType::SELECTOR_ID;
					break;
				}

				std::string body = StyleSheet::CTokenString::read_string(p, '{', '}');
				if (body.empty())
					break;
				Element element = Element::parse(body.c_str());
				if (element.getPropertyCount() > 0)
				{
					element.m_selectortype = selectortype;
					element.m_selectorname = selector;
					style.addElement(element);
				}
			}
			return style;
		}

		std::size_t getElementCount() const
		{
			return m_elements.size();
		}

		Element* getElement(StyleSheet::SelectorType type,const std::string& selector)
		{
			for (std::set<Element>::iterator it = m_elements.begin(); it != m_elements.end(); it++)
			{
				if (it->m_selectortype == type && selector == it->m_selectorname)
					return (Element*)&(*it);
			}
			return nullptr;
		}

		void addElement(const Element& element)
		{
			if (element.getPropertyCount() == 0)
				return;
			m_elements.insert(element);
		}
	};
}