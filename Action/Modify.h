#pragma once

#include "Action.h"

namespace Undoredo
{
	template<class OwnerType,class ObjectType,class ContentType> 
	class CModify : public CAction
	{
	private:
		OwnerType m_Container ;//Container variable
		ObjectType m_Object ;//Container variable
		ContentType m_Original;//target object information 
		ContentType m_Modified;//target object information 
	public:
		void (*undo)(CDocument&,OwnerType,ObjectType,ContentType,ContentType); 
		void (*redo)(CDocument&,OwnerType,ObjectType,ContentType,ContentType); 
		void (*free)(CDocument&,OwnerType,ObjectType,ContentType); 

	public:
		CModify(OwnerType container,ObjectType object,const ContentType original,const ContentType modified)
			:m_Container(container),m_Object(object),m_Original(original),m_Modified(modified)
		{	
			undo = nullptr;
			redo = nullptr;
			free = nullptr;
		}
		virtual void Undo(CDocument& document) override
		{
			if(undo != nullptr)
			{
				undo(document,m_Container,m_Object,m_Original,m_Modified);
			}
		}
		virtual void Redo(CDocument& document) override
		{
			if(redo != nullptr)
			{
				redo(document,m_Container,m_Object,m_Original,m_Modified);
			}
		}

		virtual void Free(CDocument& document,bool bInUndolist) override
		{
			if(free != nullptr)
			{
				if(bInUndolist == true)
					free(document,m_Container,m_Object,m_Original);
				else
					free(document,m_Container,m_Object,m_Modified);

			}	
		}
	};
}