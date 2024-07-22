#pragma once

class CMapItem;
class CPointF;
class CPolyF;

#include "TreeNode.h"
#include "../Public/Global.h"

class __declspec(dllexport) CDeck : public CTreeNode1
{
	DECLARE_SERIAL(CDeck);
public:
	CDeck();

	~CDeck() override;

public:
	CTypedPtrList<CObList, CDeck*> m_Children;
	CDeck* m_pParent;

public:
	CString m_strFile;
	CString m_strHtml;
	CString m_strLegend;

	BOOL m_bNavigatable;
	BOOL m_bQueryble;

public:
	CRectF m_Rect;
	CTypedPtrList<CObList, CPolyF*> m_polylist;

	CMapItem* m_pMapItem;

public:
	CDocument* Open(CWnd* pWnd, unsigned int nScale) const;
	BOOL Open();
	HANDLE GetIamge(double dLng, double dLat, unsigned int nZoom, unsigned short nWidth, unsigned short nHeight);
	bool PointIn(const CPointF& point) const;
	void RecalRect();

public:
	CDeck* GetParent() const;
	CDeck* GetChild(const CPointF& point);

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void SerializeWEB(CArchive& ar) const override;

	void PublishPC(CDaoDatabase* pDB, CDaoRecordset& rs, CString strTable);
	void PublishCE();
	void PublishWEB() const;
};

typedef CTypedPtrList<CObList, CDeck*> CDeckList;
