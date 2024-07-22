#pragma once

#include <functional>
#include "Action.h"

namespace Undoredo
{
	template<class GUIType, class OwnerType, class ObjectType>
	class CAppend : public CAction
	{
	public:
		GUIType m_GUI;
		OwnerType m_Owner;//Container variable
		ObjectType m_Object;

	public:
		void (*undo)(CDocument&, GUIType, OwnerType, ObjectType&);
		void (*redo)(CDocument&, GUIType, OwnerType, ObjectType&);
		void (*free)(CDocument&, GUIType, OwnerType, ObjectType&);

	public:
		//backup any needed infomation in construct function
		CAppend(GUIType gui, OwnerType owner, const ObjectType object)
			:m_GUI(gui), m_Owner(owner), m_Object(object)
		{
			undo = nullptr;
			redo = nullptr;
			free = nullptr;
		}

		virtual void Undo(CDocument& document) override
		{
			if(undo != nullptr)
			{
				undo(document, m_GUI, m_Owner, m_Object);
			}
		}
		virtual void Redo(CDocument& document) override
		{
			if(redo != nullptr)
			{
				redo(document, m_GUI, m_Owner, m_Object);
			}
		}
		virtual void Free(CDocument& document, bool bInUndolist) override
		{
			if(free == nullptr)
				return;

			if(bInUndolist == false)
			{
				free(document, m_GUI, m_Owner, m_Object);
			}
		}
	};
}