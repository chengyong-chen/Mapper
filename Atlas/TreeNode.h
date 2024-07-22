#pragma once

class __declspec(dllexport) CTreeNode1 : public CObject
{
	DECLARE_SERIAL(CTreeNode1);
public:
	CTreeNode1();

	~CTreeNode1() override;

public:
	WORD m_wId;
	WORD m_wParent;
	CString m_strName;

	WORD m_wType;

public:
	virtual WORD Gettype() const{ return m_wType; }
public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void SerializeWEB(CArchive& ar) const;
};

typedef CTypedPtrList<CObList, CTreeNode1*> CTreeNodeList;
