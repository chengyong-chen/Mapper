#pragma once

class CSQLClause abstract
{
public:
	virtual bool IsValid() const = 0;

	virtual bool Clear() = 0;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion) = 0;
};

class __declspec(dllexport) CClauseSelect : public CSQLClause
{
public:
	CClauseSelect();

public:
	CString m_strFields;

	CString m_strTables;

	CString m_strWhere;

	CString m_strGroupby;

	CString m_strOrderby;

public:
	virtual CString MakeSelectStatment() const;

public:
	BOOL operator ==(const CClauseSelect& source) const;

	BOOL operator !=(const CClauseSelect& source) const;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};
