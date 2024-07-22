#pragma once

#include "Append.h"

namespace Undoredo
{
	template<class GUIType, class OwnerType, class ObjectType>
	class CRemove : public CAppend<GUIType, OwnerType, ObjectType>
	{
	public:
		//backup any needed infomation in construct function
		CRemove(GUIType gui, OwnerType owner, const ObjectType object)
			:CAppend<GUIType, OwnerType, ObjectType>(gui, owner, object)
		{
		}
	public:
		virtual void Free(CDocument& document, bool bInUndolist) override
		{
			if(CAppend< GUIType, OwnerType, ObjectType >::free == nullptr)
				return;

			if(bInUndolist == true)
			{
				CAppend< GUIType, OwnerType, ObjectType >::free(document, CAppend<GUIType, OwnerType, ObjectType>::m_GUI, CAppend<GUIType, OwnerType, ObjectType>::m_Owner, CAppend<GUIType, OwnerType, ObjectType>::m_Object);
			}
		}
	};
}