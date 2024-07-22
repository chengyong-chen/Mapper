#pragma once

#include "Modify.h"

namespace Undoredo
{
	template<class OwnerType,class ObjectType,class ContentType> 
	class CSwap : public CModify<OwnerType,ObjectType,ContentType>
	{
	public:
		CSwap(OwnerType container,ObjectType object,const ContentType original,const ContentType modified)
			:CModify<OwnerType, ObjectType, ContentType>(container,object,original,modified)
		{	
		}
	public:
		virtual void Free(CDocument& document,bool bInUndolist) override
		{
			CModify<OwnerType, ObjectType, ContentType>::Free(document,true);
		}
	};
}