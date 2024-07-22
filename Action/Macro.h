#pragma once
		
#include <list>
#include <map>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Action.h"

using namespace std;

namespace Undoredo
{
	class CMacro : public CAction,	public std::list<CAction*>
	{
	public:
		virtual ~CMacro()
		{
		}
		//record new sub CAction. Note: all CAction pointer is created by new
		virtual void Record(CAction* pCmd)
		{
			this->push_back(pCmd);
		}

	public:
		virtual void Undo(CDocument& document) override
		{
			for(std::list<CAction*>::reverse_iterator it=this->rbegin();it!=this->rend();it++) 
			{
				(*it)->Undo(document);
			}
			//typedef std::mem_fun1_t<void,CAction,CDocument&> ft;
			//std::for_each(this->begin() ,this->end() ,std::bind2nd(ft(&CAction::Undo),document));
		}
		virtual void Redo(CDocument& document) override
		{
			for(std::list<CAction*>::iterator it=this->begin();it!=this->end();it++) 
			{
				(*it)->Redo(document);
			}
			//typedef std::mem_fun1_t<void,CAction,CDocument&> ft;
			//std::for_each(this->begin() ,this->end() ,std::bind2nd(ft(&CAction::Redo),document));
		}
		virtual void Free(CDocument& document,bool bInUndolist) override
		{
			for(std::list<CAction*>::iterator it=this->begin();it!=this->end();it++) 
			{
				(*it)->Free(document,bInUndolist);
				delete (*it);
			}
			this->clear();
		}
	};
}