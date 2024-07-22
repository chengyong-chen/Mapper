#include "stdafx.h"
#include "Probe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProbeVect::~CProbeVect()
{
}

DWORD CProbeVect::FetchMini()
{
	const iterator first = begin();
	if(*first==nullptr)
		return -1;
	else
	{
		const DWORD dwIndex = ((Probe*)(*first))->dwNodeIndex;
		this->erase(first);
		return dwIndex;
	}
}

void CProbeVect::Insert(Probe* pProbe)
{
	const float length = pProbe->m_dShortestLength;
	const iterator head = begin();
	if(*head==nullptr)
	{
		this->push_back(pProbe);
	}
	else if(length<=((Probe*)*head)->m_dShortestLength)
	{
		this->insert(begin()-1, pProbe);
	}
	else
	{
		const iterator tail = end();
		if(length>=((Probe*)*tail)->m_dShortestLength)
		{
			this->push_back(pProbe);
		}
		else
		{
			iterator left = begin();
			iterator right = end();
			do
			{
				const int count = right-left;
				iterator mid = left+count/2;
				if(((Probe*)*mid)->m_dShortestLength>length)
					right = mid;
				else if(((Probe*)*mid)->m_dShortestLength<length)
					left = mid;
				else
					left = right = mid;
			} while((right-left)>1);

			this->insert(left, pProbe);
		}
	}
}

void CProbeVect::Changed(Probe* pProbe)
{
}
