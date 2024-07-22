#pragma once

// CMatrix ����Ŀ��

class __declspec(dllexport) CMatrix
{
public:
	CMatrix();
	~CMatrix();

public:
	void SetDimention(int nDimension);
	void Increase();
	void Remove(int index);

public:
	BYTE m_nDimension;
	BYTE* m_pArray;

	bool GetValue(BYTE bRow, BYTE nCol) const;
	void SetValue(BYTE bRow, BYTE nCol, bool bValue) const;

public:
	BYTE GetByteLength() const;
	void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ExportPC2(CFile& file) const;

public:
	void operator=(const CMatrix& Source);
};
