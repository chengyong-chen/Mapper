#include "stdafx.h"
#include "History.h"

#include "Go.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CGo::CGo()
{
	m_nCurent = 0;
}

CGo::~CGo()
{
	m_nCurent = 0;
	m_historys.RemoveAll();
}

CHistory* CGo::GetCurentHistory()
{
	if(m_nCurent>m_historys.GetSize()||m_nCurent<1)
		return nullptr;

	return &(m_historys[m_nCurent-1]);
}

CHistory* CGo::GetHistory(int nIndex)
{
	if(nIndex>m_historys.GetSize()||nIndex<1)
		return nullptr;

	return &(m_historys[nIndex-1]);
}

BOOL CGo::GoBack(CFrameWnd* pChildFrm)
{
	if(m_nCurent>1)
	{
		CHistory* history = GetHistory(m_nCurent-1);
		history->Into(pChildFrm);
		m_nCurent--;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CGo::GoForward(CFrameWnd* pChildFrm)
{
	if(m_nCurent<m_historys.GetSize())
	{
		CHistory* history = GetHistory(m_nCurent+1);
		history->Into(pChildFrm);
		m_nCurent++;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CGo::GoHome(CFrameWnd* pChildFrm)
{
	if(m_nCurent>1&&m_historys.GetSize()>1)
	{
		CHistory* history = GetHistory(1);
		history->Into(pChildFrm);
		m_nCurent = 1;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CGo::Refresh(CFrameWnd* pChildFrm)
{
	if(m_historys.GetSize())
	{
		CHistory* history = GetCurentHistory();
		history->Into(pChildFrm);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CGo::AddHistory(CHistory& ni)
{
	if(m_nCurent<m_historys.GetSize())
	{
		m_historys.RemoveAt(m_nCurent, m_historys.GetSize()-m_nCurent);
	}

	m_historys.Add(ni);
	m_nCurent++;

	return TRUE;
}

BOOL CGo::CanBack() const
{
	return m_nCurent>1 ? TRUE : FALSE;
}

BOOL CGo::CanForward() const
{
	return m_nCurent<m_historys.GetSize() ? TRUE : FALSE;
}

BOOL CGo::CanHome() const
{
	return m_nCurent>1&&m_historys.GetSize()>1;
}

const CGo& CGo::operator =(const CGo& Src)
{
	if(&Src!=this)
	{
		m_nCurent = 0;
		m_historys.RemoveAll();

		for(int i = 0; i<Src.m_historys.GetSize(); i++)
		{
			const CHistory history = Src.m_historys.GetAt(i);

			CHistory history1;
			history1 = history;
			m_historys.Add(history1);
		}
		m_nCurent = Src.m_nCurent;
	}

	return *this;
}
