#pragma once

#include "Deck.h"

class __declspec(dllexport) CPyramid : public CObject
{
	DECLARE_SERIAL(CPyramid);
public:
	CPyramid();

	~CPyramid() override;

public:
	CDeckList m_DeckList;
	CDeck* m_pApex;

public:
	void Release();

public:
	DWORD GetMaxItemId();

public:
	bool BuildRelation();

	static void FindChildren(CDeck* pParent, CDeckList& decklist);

public:
	CDeck* GetDeck(WORD wId) const;
	CDeck* GetDeck(CString strFile) const;
	CDeck* GetDeck(const CPointF& geoPoint) const;
	CDeck* GetDeck(const CPointF& geoPoint1, const CPointF& geoPoint2) const;

	CDeck* GetNavyDeck(const CPointF& geoPoint1, const CPointF& geoPoint2) const;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void SerializeWEB(CArchive& ar) const;

	void PublishPC(CDaoDatabase* pDB, CDaoRecordset& rs, CString strTable);

	static void PublishCE();

	static void PublishWEB();
};
