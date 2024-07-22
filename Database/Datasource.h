#pragma once

class __declspec(dllexport) CHeaderProfile abstract : public CObject
{
	DECLARE_DYNAMIC(CHeaderProfile)

public:
	CHeaderProfile();

	CHeaderProfile(CHeaderProfile& source);

public:
	CString m_strKeyTable;
	CString m_strTables;
	CString m_strFields;
	CString m_strWhere;

public:
	CString m_strIdField;

	CString m_strAnnoField;

public:
	virtual CString MakeSelectStatment() const;
	virtual CString MakeSelectStatment(CString strField) const;
	virtual CString MakeSelectStatment(CString strField, CString strWhere) const;
public:
	virtual CHeaderProfile& operator =(const CHeaderProfile& source);
	virtual BOOL operator ==(const CHeaderProfile& source) const;
	virtual BOOL operator !=(const CHeaderProfile& source) const;

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};

class __declspec(dllexport) CATTHeaderProfile : public CHeaderProfile
{
	DECLARE_DYNAMIC(CATTHeaderProfile)

public:
	CATTHeaderProfile();

public:
	CString m_strCodeField;

public:
	CHeaderProfile& operator =(const CHeaderProfile& source) override;

	BOOL operator ==(const CHeaderProfile& source) const override;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};

class __declspec(dllexport) CPOUHeaderProfile : public CHeaderProfile
{
	DECLARE_DYNAMIC(CPOUHeaderProfile)

public:
	CPOUHeaderProfile();

	CPOUHeaderProfile(CPOUHeaderProfile& source);

public:
	CString m_strXField;
	CString m_strYField;

	enum COORDINATORTYPE : BYTE
	{
		Geo = 0,
		Map = 1
	} m_coordinatortype;
public:
	CString MakeSelectStatment() const override;

public:
	CHeaderProfile& operator =(const CHeaderProfile& source) override;

	BOOL operator ==(const CHeaderProfile& source) const override;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};

class __declspec(dllexport) CGEOHeaderProfile : public CHeaderProfile
{
	DECLARE_DYNAMIC(CGEOHeaderProfile)

public:
	CGEOHeaderProfile();

public:
	CString m_tableA;
	CString m_tableB;
	CString m_tableC;
	CString m_whereA;
	CString m_whereB;

	CString m_strAnnoField;

public:
	CString MakeSelectStatment() const override;

public:
	CHeaderProfile& operator =(const CHeaderProfile& source) override;

	BOOL operator ==(const CHeaderProfile& source) const override;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};

class __declspec(dllexport) CATTMasterProfile : public CHeaderProfile
{
};

class __declspec(dllexport) CGEOMasterProfile : public CHeaderProfile
{
};

class __declspec(dllexport) CDatasource abstract : public CObject
{
	DECLARE_DYNAMIC(CDatasource);

public:
	CDatasource();

	CDatasource(CString database);

public:
	CString m_strDatabase;

public:
	CDatasource* Clone() const;

public:
	virtual CDatasource& operator =(const CDatasource& source);

	virtual BOOL operator ==(const CDatasource& source) const;

	virtual BOOL operator !=(const CDatasource& source) const;

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};

class __declspec(dllexport) CATTDatasource : public CDatasource
{
	DECLARE_SERIAL(CATTDatasource);

public:
	CATTDatasource();

	CATTDatasource(CString database);

private:
	mutable CATTHeaderProfile m_headerprofile;
	mutable CATTMasterProfile m_masterprofile;

public:
	virtual CATTHeaderProfile& GetHeaderProfile() const
	{
		return m_headerprofile;
	};

public:
	CDatasource& operator =(const CDatasource& source) override;

	BOOL operator ==(const CDatasource& source) const override;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};

class __declspec(dllexport) CPOUDatasource : public CDatasource
{
	DECLARE_SERIAL(CPOUDatasource);

public:
	CPOUDatasource();

	CPOUDatasource(CString database);

private:
	mutable CPOUHeaderProfile m_headerprofile;

public:
	virtual CPOUHeaderProfile& GetHeaderProfile() const
	{
		return m_headerprofile;
	};

public:
	CDatasource& operator =(const CDatasource& source) override;

	BOOL operator ==(const CDatasource& source) const override;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};

class __declspec(dllexport) CGEODatasource : public CDatasource
{
	DECLARE_SERIAL(CGEODatasource);

public:
	CGEODatasource();

	CGEODatasource(CString database);

private:
	mutable CGEOHeaderProfile m_headerprofile;

public:
	virtual CGEOHeaderProfile& GetHeaderProfile() const
	{
		return m_headerprofile;
	};
	virtual CString GetTableA() const
	{
		return m_headerprofile.m_tableA;
	};
public:
	CDatasource& operator =(const CDatasource& source) override;

	BOOL operator ==(const CDatasource& source) const override;

	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
};

void AFXAPI operator <<(CArchive& ar, CPOUHeaderProfile::COORDINATORTYPE& coortype);

void AFXAPI operator >>(CArchive& ar, CPOUHeaderProfile::COORDINATORTYPE& coortype);
