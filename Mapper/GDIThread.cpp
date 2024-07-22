#include "stdafx.h"
#include "Mapper.h"
#include "GDIThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// critical section to protect while drawing to the DC
CRITICAL_SECTION CGDIThread::m_csGDILock;

// m_hAnotherDead is used to signal that one or more threads have ended
//  (it is an auto-reset event; and starts out not signaled)
HANDLE CGDIThread::m_hAnotherDead = CreateEvent(nullptr, FALSE, FALSE, nullptr);

/////////////////////////////////////////////////////////////////////////////
// CGDIThread

IMPLEMENT_DYNCREATE(CGDIThread, CWinThread)


CGDIThread::CGDIThread()
{
}

CGDIThread::CGDIThread(CWnd* pWnd, HDC hDC)
{
	m_bAutoDelete = FALSE;
	m_pMainWnd    = pWnd;
	m_hDC = hDC;

	// kill event starts out in the signaled state
	m_hEventKill = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_hEventDead = CreateEvent(nullptr, TRUE, FALSE, nullptr);
}

CGDIThread::~CGDIThread()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
}

BOOL CGDIThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CGDIThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGDIThread, CWinThread)
	//{{AFX_MSG_MAP(CGDIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGDIThread message handlers

void CGDIThread::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
	VERIFY(SetEvent(m_hAnotherDead));
}

void CGDIThread::KillThread()
{
	// Note: this function is called in the context of other threads,
	//  not the thread itself.

	// reset the m_hEventKill which signals the thread to shutdown
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	WaitForSingleObject(m_hEventDead, INFINITE);
	WaitForSingleObject(m_hThread, INFINITE);

	// now delete CWinThread object since no longer necessary
	delete this;
}
