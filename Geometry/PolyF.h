#pragma once

#include "../Public/Global.h"

class AFX_EXT_CLASS CPolyF : public CObject
{
	DECLARE_SERIAL(CPolyF);
	CPolyF();

public:
	CRectF m_rect;

public:
	virtual CPointF& GetAnchor(const unsigned int& nAnchor) const;
	virtual void AddAnchor(const CPointF& point);

public:
	virtual void RecalRect();

public:
	unsigned int m_nAllocs;
	unsigned int m_nAnchors;
	CPointF* m_pPoints;

public:
	virtual bool PickIn(const CPointF& point) const;
	virtual bool PickOn(const CPointF& point, const double& gap) const;

	~CPolyF() override;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	static void SerializeList(CArchive& ar, CObList& oblist);
};
