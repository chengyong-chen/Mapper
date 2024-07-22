#pragma once


class CGeom;
class CGeomstack : public CTypedPtrList<CObList, CGeom*>
{
public:
	void Push(CGeom* pGeom)
	{
		AddTail(pGeom);
	}

	CGeom* Peek()
	{
		return IsEmpty() ? nullptr : GetTail();
	}

	CGeom* Pop()
	{
		return IsEmpty() ? nullptr : RemoveTail();
	}
};
