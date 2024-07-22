#pragma once

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

////////////////////////////////////////////////////////////////////////////

// tips & triks. Speed optimization and bug detecting compromise

#ifdef _DEBUG
#define DATA_ACCESS_OPERATOR(i) ((*this)[i]) // for better bug tracking
#define FAST_ACCESS_OPERATOR(var,i) ((var)[(i)]) // for better bug tracking
#else
#define DATA_ACCESS_OPERATOR(i) (m_pData[i]) // 10 times faster
#define FAST_ACCESS_OPERATOR(var,i) ((var).GetData()[(i)]) 
#endif

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

// Special compare functions for Sorted Array

/* // this compare function copy from mfc
template<class TYPE, class ARG_TYPE>
AFX_INLINE BOOL AFXAPI CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	ASSERT(AfxIsValidAddress(pElement1, sizeof(TYPE), FALSE));
	ASSERT(AfxIsValidAddress(pElement2, sizeof(ARG_TYPE), FALSE));

	return *pElement1 == *pElement2;
}
*/

template<class TYPE, class ARG_TYPE>
BOOL AFXAPI CompareElementsLess(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	ASSERT(AfxIsValidAddress(pElement1, sizeof(TYPE), FALSE));
	ASSERT(AfxIsValidAddress(pElement2, sizeof(ARG_TYPE), FALSE));

	return *pElement1<*pElement2;
}

////////////////////////////////////////////////////////////////////////////

template<class TYPE, class ARG_TYPE>
class CAgrArray : public CArray<TYPE, ARG_TYPE>
{
public:
	CAgrArray();

	CAgrArray(const CAgrArray& x);

	// Assigment
public:
	CAgrArray& operator =(const CAgrArray& x);

	// Comparison 
public:
	BOOL operator <(const CAgrArray& x) const;

	BOOL operator <=(const CAgrArray& x) const;

	BOOL operator ==(const CAgrArray& x) const;

	BOOL operator !=(const CAgrArray& x) const;

	BOOL operator >(const CAgrArray& x) const;

	BOOL operator >=(const CAgrArray& x) const;

	// Operator
public:
	operator CArray<TYPE, ARG_TYPE>();

public:
	int Find(LPCTSTR s, bool bCaseSensetive = true) const; /*@return index of search string. returns -1 if string is not found or if array is not of type CString*/
};

template<class TYPE, class ARG_TYPE>
CAgrArray<TYPE, ARG_TYPE>::CAgrArray()
{
}

template<class TYPE, class ARG_TYPE>
CAgrArray<TYPE, ARG_TYPE>::CAgrArray(const CAgrArray& x)
{
	*this = x;
}

template<class TYPE, class ARG_TYPE>
CAgrArray<TYPE, ARG_TYPE>& CAgrArray<TYPE, ARG_TYPE>::operator =(const CAgrArray& x)
{
	if(this!=&x)
	{
		Copy(x);
	}

	return *this;
}

template<class TYPE, class ARG_TYPE>
BOOL CAgrArray<TYPE, ARG_TYPE>::operator <(const CAgrArray& x) const
{
	register int nSize(GetSize());

	if(nSize!=x.GetSize())
	{
		return FALSE;
	}

	for(register int i = 0; i<nSize; i++)
	{
		if(!(DATA_ACCESS_OPERATOR(i)<FAST_ACCESS_OPERATOR(x, i)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

template<class TYPE, class ARG_TYPE>
BOOL CAgrArray<TYPE, ARG_TYPE>::operator <=(const CAgrArray& x) const
{
	register int nSize(GetSize());

	if(nSize!=x.GetSize())
	{
		return FALSE;
	}

	for(register int i = 0; i<nSize; i++)
	{
		if(!(DATA_ACCESS_OPERATOR(i)<=FAST_ACCESS_OPERATOR(x, i)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

template<class TYPE, class ARG_TYPE>
BOOL CAgrArray<TYPE, ARG_TYPE>::operator ==(const CAgrArray& x) const
{
	register int nSize(GetSize());

	if(nSize!=x.GetSize())
	{
		return FALSE;
	}

	for(register int i = 0; i<nSize; i++)
	{
		if(!(DATA_ACCESS_OPERATOR(i)==FAST_ACCESS_OPERATOR(x, i)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

template<class TYPE, class ARG_TYPE>
BOOL CAgrArray<TYPE, ARG_TYPE>::operator !=(const CAgrArray& x) const
{
	register int nSize(GetSize());

	if(nSize==x.GetSize())
	{
		for(register int i = 0; i<nSize; i++)
		{
			if(!(DATA_ACCESS_OPERATOR(i)!=FAST_ACCESS_OPERATOR(x, i)))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

template<class TYPE, class ARG_TYPE>
BOOL CAgrArray<TYPE, ARG_TYPE>::operator >(const CAgrArray& x) const
{
	register int nSize(GetSize());

	if(nSize!=x.GetSize())
	{
		return FALSE;
	}

	for(register int i = 0; i<nSize; i++)
	{
		if(!(DATA_ACCESS_OPERATOR(i)>FAST_ACCESS_OPERATOR(x, i)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

template<class TYPE, class ARG_TYPE>
BOOL CAgrArray<TYPE, ARG_TYPE>::operator >=(const CAgrArray& x) const
{
	register int nSize(GetSize());

	if(nSize!=x.GetSize())
	{
		return FALSE;
	}

	for(register int i = 0; i<nSize; i++)
	{
		if(!(DATA_ACCESS_OPERATOR(i)>=FAST_ACCESS_OPERATOR(x, i)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

template<class TYPE, class ARG_TYPE>
CAgrArray<TYPE, ARG_TYPE>::operator CArray<TYPE, ARG_TYPE>()
{
	return *this;
}

template<class TYPE, class ARG_TYPE>
int CAgrArray<TYPE, ARG_TYPE>::Find(LPCTSTR s, const bool bCaseSensetive/*=true*/) const
{
	const long lRet = -1;
	/*if(IsKindOf(RUNTIME_CLASS(CString))==FALSE)
		return lRet;*/

	for(int i = 0; i<GetCount(); i++)
	{
		if(bCaseSensetive==true)
		{
			if(GetAt(i).Compare(s)==0)
				return i;
		}
		else
		{
			if(GetAt(i).CompareNoCase(s)==0)
				return i;
		}
	}

	return lRet;
}
