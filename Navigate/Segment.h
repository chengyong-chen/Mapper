#pragma once

#include "../Dataview/Viewinfo.h"
#include "../Public/Global.h"

class CPoly;

class __declspec(dllexport) CSegment : public CObject
{
	protected:
	DECLARE_SERIAL(CSegment);
	CSegment();

	~CSegment() override;

	public:
	CPoly* m_pPoly;

	public:
	float m_fLength;

	public:
	static bool Over();
	void CheckPromt(const CPoint& point);
	void Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, Gdiplus::Pen* pen, const CRectF& geoRect) const;

	public:
	CPoint m_Prompt;
	BYTE m_nTurnType;
	bool m_bPromted;
};

typedef CTypedPtrList<CObList, CSegment*> CSegmentList;
