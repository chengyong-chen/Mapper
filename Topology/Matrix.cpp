#include "stdafx.h"
#include "Matrix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CMatrix

CMatrix::CMatrix()
{
	m_nDimension = 0;
	m_pArray = nullptr;
}

CMatrix::~CMatrix()
{
	delete m_pArray;
}

void CMatrix::operator =(const CMatrix& source)
{
	if(&source!=this)
	{
		m_nDimension = source.m_nDimension;
		delete m_pArray;
		m_pArray = nullptr;
		const BYTE nBits = m_nDimension*m_nDimension;
		const BYTE nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;
		if(nBytes>0)
		{
			m_pArray = new BYTE[nBytes];
			memcpy(m_pArray, source.m_pArray, nBytes);
		}
	}
}

void CMatrix::SetDimention(int nDimension)
{
	if(nDimension==0)
	{
		delete m_pArray;
		m_pArray = nullptr;

		m_nDimension = 0;
	}
	else if(nDimension!=m_nDimension)
	{
		const BYTE nBits = nDimension*nDimension;
		const BYTE nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;
		BYTE* pArray = new BYTE[nBytes];
		memset(pArray, 0XFF, nBytes);

		if(m_nDimension!=0)
		{
			delete[] m_pArray;
		}

		m_pArray = pArray;
		m_nDimension = nDimension;
	}
	else
	{
		const BYTE nBits = m_nDimension*m_nDimension;
		const BYTE nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;
		memset(m_pArray, 0XFF, nBytes);
	}
}

void CMatrix::Increase()
{
	const BYTE nBits = (m_nDimension+1)*(m_nDimension+1);
	const BYTE nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;

	BYTE* pArray = new BYTE[nBytes];
	memset(pArray, 0XFF, nBytes);

	for(int x = 0; x<m_nDimension; x++)
		for(int y = 0; y<m_nDimension; y++)
		{
			//ȡԭ����ֵ	
			const int pos1 = x*m_nDimension+y;
			BYTE byte = m_pArray[pos1/8];
			byte = (byte>>(7-pos1%8));
			byte &= 0X01;

			//��ԭ����ֵ�ƶ����µ�λ��
			const int pos2 = x*(m_nDimension+1)+y;
			byte = (byte<<(7-pos2%8));

			pArray[pos2/8] |= byte;
		}

	delete[] m_pArray;
	m_pArray = pArray;

	m_nDimension += 1;
}

BYTE CMatrix::GetByteLength() const
{
	const BYTE nBits = m_nDimension*m_nDimension;
	const BYTE nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;
	return nBytes;
}

void CMatrix::Remove(int index)
{
	if(index<0)
		return;
	if(index>=m_nDimension)
		return;
	const BYTE nOrgBits = m_nDimension*m_nDimension;
	const BYTE nOrgBytes = nOrgBits%8==0 ? nOrgBits/8 : nOrgBits/8+1;
	const BYTE nNewBits = (m_nDimension-1)*(m_nDimension-1);
	const BYTE nNewBytes = nNewBits%8==0 ? nNewBits/8 : nNewBits/8+1;

	BYTE* pArray = nullptr;
	if(nNewBytes<nOrgBytes)
	{
		pArray = new BYTE[nNewBytes];
		memset(pArray, 0XFF, nNewBytes);
	}
	else
		pArray = m_pArray;

	int pos = 0;
	for(int x = 0; x<m_nDimension; x++)
		for(int y = 0; y<m_nDimension; y++)
		{
			if(x==index)
				continue;
			if(y==index)
				continue;

			//ȡԭ����ֵ	
			const int pos1 = x*m_nDimension+y;
			BYTE byte = m_pArray[pos1/8];
			byte = (byte>>(7-pos1%8));
			byte &= 0X01;
			//��ԭ����ֵ�ƶ����µ�λ��

			byte = (byte<<(7-pos%8));
			pArray[pos/8] |= byte;

			pos++;
		}

	if(pArray!=m_pArray)
	{
		delete m_pArray;
		m_pArray = pArray;
	}

	m_nDimension -= 1;
}

bool CMatrix::GetValue(BYTE bRow, BYTE nCol) const
{
	if(m_pArray!=nullptr)
	{
		const int nPos = bRow*m_nDimension+nCol;
		const int nByte = nPos/8;
		const int nBit = 7-nPos%8;
		BYTE byte = m_pArray[nByte];
		byte = (byte>>nBit);
		if((byte&0X01)==0X01)
			return true;
		else
			return false;
	}
	else
		return false;
}

void CMatrix::SetValue(BYTE bRow, BYTE nCol, bool bValue) const
{
	int nPos = bRow*m_nDimension+nCol;
	const int nByte = nPos/8;
	const int nBit = 7-nPos%8;
	BYTE byte = 0X01;
	byte = (byte<<nBit);
	if(bValue==true)
	{
		m_pArray[nByte] |= byte;
	}
	else
	{
		m_pArray[nByte] &= ~byte;
	}
}

void CMatrix::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_nDimension;
		const int nBits = m_nDimension*m_nDimension;
		const int nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;
		ar.Write(m_pArray, nBytes);
	}
	else
	{
		ar>>m_nDimension;
		const int nBits = m_nDimension*m_nDimension;
		const int nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;

		delete m_pArray;
		m_pArray = new BYTE[nBytes];
		ar.Read(m_pArray, nBytes);
	}
}

void CMatrix::ExportPC2(CFile& file) const
{
	const int nBits = m_nDimension*m_nDimension;
	const int nBytes = nBits%8==0 ? nBits/8 : nBits/8+1;
	file.Write(m_pArray, nBytes);
}
