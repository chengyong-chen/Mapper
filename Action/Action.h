#pragma once
namespace Undoredo
{
	class CAction
	{
	public:
		CAction()
		{
		}
		virtual ~CAction()
		{
		}

		virtual void Undo(CDocument& document)=0;
		virtual void Redo(CDocument& document)=0;
		virtual void Free(CDocument& document,bool bInUndolist)=0;
	};
}