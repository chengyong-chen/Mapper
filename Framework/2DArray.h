#pragma once

class C2DArray
{
public:

	typedef void* W_PTR;

	C2DArray()
		: m_rows(0), m_cols(0), m_pData(0)
	{
	}

	virtual ~C2DArray()
	{
		delete[] m_pData;
	}

	void Init(int rows, int cols)
	{
		m_rows = rows;
		m_cols = cols;
		m_pData = new W_PTR[m_rows*m_cols];
		memset(m_pData, 0, m_rows*m_cols);
	}

	W_PTR& operator()(int row, int col) const
	{
		ASSERT(m_pData);
		ASSERT(row*m_cols+col<m_rows*m_cols);

		return *(m_pData+row*m_cols+col);
	}

protected:
	W_PTR* m_pData;
	int m_rows;
	int m_cols;
};
