#pragma once

#include <list>
using namespace std;

namespace Undoredo
{
	namespace Document
	{
		namespace Library
		{	
			template<class T>
			static void Undo_Append_Symbol(CDocument& document,void* pVoid,std::list<T*>& library,std::pair<int,T*>& tag)
			{
				//pLibrary->find
				T* pTag = tag.second;
				for(std::list<T*>::iterator it=library.begin();it != library.end(); ++it)
				{
					T* tag = *it;
					if(pTag == tag)
					{
						library.erase(it);
					}
				}
			}
			template<class T>
			static void Redo_Append_Symbol(CDocument& document,void* pVoid,std::list<T*>& library,std::pair<int,T*>& tag)
			{
				T* pTag = tag.second;
				pTag->m_nUsers = 1;
				library.push_back(pTag);
			}
			template<class T>
			static void Free_Append_Symbol(CDocument& document,void* pVoid,std::list<T*>& library,std::pair<int,T*>& tag)
			{
				T* pTag = tag.second;
				delete pTag;
			}
		}
	}
}