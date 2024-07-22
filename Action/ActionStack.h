#pragma once

#include <limits>
#include "Action.h"
#include "Macro.h"
#include <iterator>
#include <iterator>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
namespace Undoredo
{
	class CActionStack
	{
	public:
		CActionStack(CDocument& document)
			:m_document(document), m_limit(16)
		{
		}
		~CActionStack()
		{
			m_undoqueue.Free(m_document, true);//free Undo CAction queue
			m_redoqueue.Free(m_document, false);//free Redo CAction queue
		}

	private:
		CMacro m_undoqueue;
		CMacro m_redoqueue;

	private:
		CDocument& m_document;
		unsigned short m_limit;

	public:
		unsigned short limit()
		{
			return m_limit;
		}

		void Relimit(unsigned short limit)
		{
			while(m_undoqueue.size() > limit)
			{
				CAction* pCmd = static_cast<CAction*>(m_undoqueue.front());
				if(pCmd != nullptr)
				{
					pCmd->Free(m_document, true);
					delete pCmd;//then free memory of this CAction
				}
				m_undoqueue.pop_front();
			}

			m_limit = limit;
		}

	public:
		bool Undoable()
		{
			return !m_undoqueue.empty();
		}
		bool Redoable()
		{
			return !m_redoqueue.empty();
		}

		void Undo()
		{
			if(!m_undoqueue.empty())
			{
				CAction* pCmd = static_cast<CAction*>(m_undoqueue.back());
				if(pCmd != nullptr)
				{
					pCmd->Undo(m_document);
					m_redoqueue.push_back(pCmd);
				}
				m_undoqueue.pop_back();
			}
		}
		void Redo()
		{
			if(!m_redoqueue.empty())
			{
				CAction* pCmd = static_cast<CAction*>(m_redoqueue.back());
				if(pCmd != nullptr)
				{
					pCmd->Redo(m_document);
					m_undoqueue.push_back(pCmd);
				}
				m_redoqueue.pop_back();
			}
		}

		virtual void Record(CAction* pCmd = nullptr)//just only add a CAction pointer, won't be executed!!!
		{
			std::list<CAction*>::reverse_iterator rit = std::find(m_undoqueue.rbegin(), m_undoqueue.rend(), static_cast<CAction*>(nullptr));
			if(pCmd == nullptr)
				m_undoqueue.Record(pCmd); //add new CAction pointer to CAction history queue
			else if(rit != m_undoqueue.rend())
				m_undoqueue.Record(pCmd); //add new CAction pointer to CAction history queue
			else
			{
				while(m_undoqueue.size() >= m_limit)
				{
					CAction* pCmd = static_cast<CAction*>(m_undoqueue.front());
					if(pCmd != nullptr)
					{
						pCmd->Free(m_document, true);
						delete pCmd;
					}
					m_undoqueue.pop_front();
				}

				m_undoqueue.Record(pCmd); //add new CAction pointer to CAction history queue
				m_redoqueue.Free(m_document, false);	//clear all Redoable commands when record new CAction
			}
		}

		void Start()
		{
			m_undoqueue.Record(nullptr);
		}
		void Stop()
		{
			std::list<CAction*>::reverse_iterator rit = std::find(m_undoqueue.rbegin(), m_undoqueue.rend(), static_cast<CAction*>(nullptr));
			if(rit == m_undoqueue.rend())
				return;

			std::list<CAction*>::iterator it = m_undoqueue.begin();
			std::advance(it, std::distance(rit, m_undoqueue.rend()) - 1);
			if(it == m_undoqueue.end())
				return;

			std::list<CAction*>::difference_type diff = std::distance(it, m_undoqueue.end());
			if(diff == 1)	  //only nullmark
				m_undoqueue.erase(it);
			else if(diff == 2)  //only one action followed the null mark
			{
				CAction* pAction = m_undoqueue.back();
				m_undoqueue.pop_back();
				m_undoqueue.erase(it);
				CActionStack::Record(pAction);
			}
			else
			{
				CMacro* pMacro = new CMacro();
				pMacro->splice(pMacro->begin(), m_undoqueue, it, m_undoqueue.end());
				pMacro->remove(static_cast<CAction*>(nullptr));//list
				CActionStack::Record(pMacro);
			}
		}
		virtual void Clear()
		{
			m_undoqueue.Free(m_document, true);//free Undo CAction queue
			m_redoqueue.Free(m_document, false);//free Redo CAction queue
		}
	};
}