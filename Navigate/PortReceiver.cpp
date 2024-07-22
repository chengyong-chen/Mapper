#include "stdafx.h"
#include <assert.h>

#include "Vehicle.h"
#include "Port.h"
#include "PortReceiver.h"
#include "../Viewer/Global.h"

#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CPortReceiver::m_RMCResult;
CString CPortReceiver::m_GGAResult;

bool CPortReceiver::m_bSyndrome;

IMPLEMENT_DYNAMIC(CPortReceiver, CPort)

CPortReceiver::CPortReceiver(CFrameWnd* pParents)
	: CPort(pParents)
{
	m_hPort = nullptr;

	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;
	memset(&m_ov, 0, sizeof(OVERLAPPED));

	m_ov.hEvent = nullptr;
	m_hKillEvent = nullptr;
	m_hWriteEvent = nullptr;

	m_bSyndrome = FALSE;

	m_RMCResult.Empty();
	m_GGAResult.Empty();
}

bool CPortReceiver::Open()
{
	const int wPort = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("Port"), 1);
	const int dwBaudRate = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("BaudRate"), 4800);
	const int wDataBits = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("DataBits"), 8);
	const int wStopBits = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("StopBits"), 0);
	const int wParity = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("Parity"), _T('N'));

	assert(wPort>0&&wPort<8);

	if(m_bThreadAlive==true)
	{
		do
		{
			SetEvent(m_hKillEvent);
		} while(m_bThreadAlive==true);

		m_hKillEvent = nullptr;
	}

	//create events
	if(m_ov.hEvent!=nullptr)
		ResetEvent(m_ov.hEvent);
	else
		m_ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

	if(m_hWriteEvent!=nullptr)//dele
		ResetEvent(m_hWriteEvent);//dele
	else
		m_hWriteEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);//dele

	if(m_hKillEvent!=nullptr)
		ResetEvent(m_hKillEvent);
	else
		m_hKillEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

	// initialize the event objects
	m_hEventArray[0] = m_hKillEvent; // highest priority
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent; //dele

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSync);

	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// ifthe port is already opened: close it
	if(m_hPort!=nullptr)
	{
		CloseHandle(m_hPort);
		m_hPort = nullptr;
	}

	TCHAR* szPort = new TCHAR[50];
	_stprintf(szPort, _T("COM%d"), wPort);
	m_hPort = CreateFile(szPort, // communication port string (COMX)
		GENERIC_READ|GENERIC_WRITE, // read types
		0, // comm devices must be opened with exclusive access
		nullptr, // no security attributes
		OPEN_EXISTING, // comm devices must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED, // Async I/O
		0); // template must be 0 for comm devices

	delete[] szPort;
	if(m_hPort==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("�˿��޷���!"));
		m_hPort = nullptr;
		return false;
	}

	COMMTIMEOUTS m_CommTimeouts;
	m_CommTimeouts.ReadIntervalTimeout = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;
	if(SetCommTimeouts(m_hPort, &m_CommTimeouts))
	{
		if(SetCommMask(m_hPort, EV_RXCHAR|EV_CTS))
		{
			DCB dcb;
			ZeroMemory(&dcb, sizeof(DCB));
			if(GetCommState(m_hPort, &dcb))
			{
				dcb.fRtsControl = RTS_CONTROL_ENABLE; // set RTS bit high!//dele
				dcb.BaudRate = dwBaudRate;
				dcb.ByteSize = (BYTE)wDataBits;
				dcb.fAbortOnError = FALSE; // Terminate Reads & Writes ifthere's an error
				dcb.fErrorChar = TRUE; // Replace any garbled bytes with ErrorChar
				dcb.ErrorChar = ' '; // Garbage bytes are spaces
				dcb.fBinary = TRUE; // Ignore

				switch(wParity)
				{
				case 'N':
					dcb.Parity = NOPARITY;
					break;
				case 'O':
					dcb.Parity = ODDPARITY;
					break;
				case 'E':
					dcb.Parity = EVENPARITY;
					break;
				default:
					ASSERT(FALSE);
					break;
				}
				switch(wStopBits)
				{
				case 0:
					dcb.StopBits = ONESTOPBIT;
					break;
				case 1:
					dcb.StopBits = ONE5STOPBITS;
					break;
				case 2:
					dcb.StopBits = TWOSTOPBITS;
					break;
				default:
					ASSERT(FALSE);
					break;
				}

				if(SetCommState(m_hPort, &dcb))
				{
				}
				else
				{
					CloseHandle(m_hPort);
					m_hPort = nullptr;
					ProcessErrorMessage("SetCommState()");
					return false;
				}
			}
			else
			{
				CloseHandle(m_hPort);
				m_hPort = nullptr;
				ProcessErrorMessage("GetCommState()");
				return false;
			}
		}
		else
		{
			CloseHandle(m_hPort);
			m_hPort = nullptr;
			ProcessErrorMessage("SetCommMask()");
			return false;
		}
	}
	else
	{
		CloseHandle(m_hPort);
		m_hPort = nullptr;
		ProcessErrorMessage("SetCommTimeouts()");
		return false;
	}

	// flush the port
	PurgeComm(m_hPort, PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);

	// release critical section
	LeaveCriticalSection(&m_csCommunicationSync);

	TRACE(_T("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n"), wPort);

	m_hThread = AfxBeginThread(ReceiverThread,
		this,
		THREAD_PRIORITY_NORMAL,
		0,
		CREATE_SUSPENDED,
		nullptr
	);
	if(m_hThread==nullptr)
	{
		CloseHandle(m_hPort);
		m_hPort = nullptr;
		TRACE(_T("GpsOpen, Failed to create background thread\n"));
		return false;
	}
	m_hThread->ResumeThread();

	CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;

	TCHAR szUser[256];
	DWORD size = 256;
	GetUserName(szUser, &size);

	Content content;
	content.dwId = m_dwId;
	content.strUser = szUser;
	content.strCode = _T("");

	pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_NEW, (long)&content);
	const BOOL bTrack = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Track"), 1);
	const BOOL bDrawTrace = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("DrawTrace"), 0);
	const BOOL bSaveTrace = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("SaveTrace"), 0);
	if(bTrack==TRUE)
		pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_TRACK, m_dwId);
	if(bDrawTrace==TRUE)
		pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_TRACE_DRAW, m_dwId);
	if(bSaveTrace==TRUE)
		pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_TRACE_SAVE, m_dwId);

	pAppFrame->SetTimer(TIMER_GETGPSMSE, 500, nullptr);
	return CPort::Open();
}

UINT CPortReceiver::ReceiverThread(LPVOID pParam)
{
	CPortReceiver* port = (CPortReceiver*)pParam;
	if(port==nullptr||port->m_hPort==nullptr)
		return 0;

	port->m_bThreadAlive = true;
	PurgeComm(port->m_hPort, PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);// Clear comm buffers at startup

	for(;;) // begin forever loop.  This loop will run as long as the thread is alive.
	{
		// Make a call to WaitCommEvent().  This call will return immediatly
		// because our port was created as an async port (FILE_FLAG_OVERLAPPED
		// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
		// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
		// be placed in a non-signeled state ifthere are no bytes available to be read,
		// or to a signeled state ifthere are bytes available.  ifthis event handle 
		// is set to the non-signeled state, it will be set to signeled when a 
		// character arrives at the port.
		// we do this for each port!

		DWORD Event = 0;
		if(WaitCommEvent(port->m_hPort, &Event, &port->m_ov)==FALSE) // ifWaitCommEvent() returns FALSE, process the last error to determin
		{
			const DWORD dwError = GetLastError();
			switch(dwError)
			{
			case ERROR_IO_PENDING:
			{
				// This is a normal return value ifthere are no bytes
				// to read at the port.
				// Do nothing and continue
				break;
			}
			case 87:
			{
				// Under Windows NT, this value is returned for some reason.
				// I have not investigated why, but it is also a valid reply
				// Also do nothing and continue.
				break;
			}
			default:
			{
				// All other error codes indicate a serious error has
				// occured.  Process this error.
				port->ProcessErrorMessage("WaitCommEvent()");
				break;
			}
			}
		}
		else
		{
			// ifWaitCommEvent() returns TRUE, check to be sure there are
			// actually bytes in the buffer to read.  
			//
			// ifyou are reading more than one byte at a time from the buffer 
			// (which this program does not do) you will have the situation occur 
			// where the first byte to arrive will cause the WaitForMultipleObjects() 
			// function to stop waiting.  The WaitForMultipleObjects() function 
			// resets the event handle in m_OverlappedStruct.hEvent to the non-signelead state
			// as it returns.  
			//
			// ifin the time between the reset of this event and the call to 
			// ReadFile() more bytes arrive, the m_OverlappedStruct.hEvent handle will be set again
			// to the signeled state. When the call to ReadFile() occurs, it will 
			// read all of the bytes from the buffer, and the program will
			// loop back around to WaitCommEvent().
			// 
			// At this point you will be in the situation where m_OverlappedStruct.hEvent is set,
			// but there are no bytes available to read.  ifyou proceed and call
			// ReadFile(), it will return immediatly due to the async port setup, but
			// GetOverlappedResults() will not return until the next character arrives.
			//
			// It is not desirable for the GetOverlappedResults() function to be in 
			// this state.  The thread shutdown event (event 0) and the WriteFile()
			// event (Event2) will not work ifthe thread is blocked by GetOverlappedResults().
			//
			// The solution to this is to check the buffer with a call to ClearCommError().
			// This call will reset the event handle, and ifthere are no bytes to read
			// we can loop back through WaitCommEvent() again, then proceed.
			// ifthere are really bytes to read, do nothing and proceed.

			DWORD dwError;
			COMSTAT comstat;
			ZeroMemory(&comstat, sizeof(comstat));
			ClearCommError(port->m_hPort, &dwError, &comstat);
			if(comstat.cbInQue==0)
				continue;
		}

		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);// Main wait function.  This function will normally block the threaduntil one of nine events occur that require action.
		switch(Event)
		{
		case 0: //m_hKillEvent
		{
			port->m_bThreadAlive = false;
			AfxEndThread(100);
			break;
		}
		case 1: // m_ov.hEvent read event
		{
			DWORD CommEvent = 0;
			GetCommMask(port->m_hPort, &CommEvent);
			if(CommEvent&EV_RXCHAR)
			{
				COMSTAT comstat;
				ZeroMemory(&comstat, sizeof(comstat));
				port->ReceiveChar(comstat);
			}
			if(CommEvent&EV_CTS)
			{
			}
			if(CommEvent&EV_RING)
			{
				//	::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RING_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			}
			break;
		}
		case 2: // m_hWriteEvent write event
		{
			port->WriteChar();
			break;
		}
		}
	}

	return 0;
}

//
// Restart the comm thread
void CPortReceiver::Close()
{
	CPort::Close();

	AfxGetMainWnd()->KillTimer(TIMER_GETGPSMSE);

	do
	{
		SetEvent(m_hKillEvent);
	} while(m_bThreadAlive==true);
	m_hKillEvent = nullptr;

	if(m_ov.hEvent)
	{
		CloseHandle(m_ov.hEvent);
		m_ov.hEvent = nullptr;
	}

	if(m_hWriteEvent!=nullptr)
	{
		CloseHandle(m_hWriteEvent);
		m_hWriteEvent = nullptr;
	}

	if(m_hKillEvent!=nullptr)
	{
		CloseHandle(m_hKillEvent);
		m_hKillEvent = nullptr;
	}

	//	DeleteCriticalSection(&m_csCommunicationSync);	

	if(m_hPort!=nullptr)
	{
		CloseHandle(m_hPort);
		m_hPort = nullptr;
	}
}

void CPortReceiver::ProcessErrorMessage(const char* ErrorText)
{
	char* Temp = new char[200];

	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		nullptr
	);

	_stprintf((LPTSTR)Temp, _T("WARNING:  %s Failed with the following error: \n%s\nPort\n"), (char*)ErrorText, lpMsgBuf);
	MessageBox(nullptr, (LPTSTR)Temp, _T("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

void CPortReceiver::ReceiveChar(COMSTAT comstat)
{
	BOOL bRead = TRUE;
	BOOL bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	unsigned char RXBuff;

	for(;;)
	{
		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 
		if(WaitForSingleObject(m_hKillEvent, 0)==WAIT_OBJECT_0)
		{
			m_bThreadAlive = false;
			AfxEndThread(100); // Kill this thread.  break is not needed, but makes me feel better.
		}

		EnterCriticalSection(&m_csCommunicationSync);
		bResult = ClearCommError(m_hPort, &dwError, &comstat);// ClearCommError() will update the COMSTAT structure and clear any other errors.
		LeaveCriticalSection(&m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.

		if(comstat.cbInQue==0)// break out when all bytes have been read
		{
			break;
		}

		EnterCriticalSection(&m_csCommunicationSync);

		if(bRead)
		{
			bResult = ReadFile(m_hPort, // Handle to COMM port 
				&RXBuff, // RX Buffer Pointer
				1, // Read one byte
				&BytesRead, // Stores number of bytes read
				&m_ov); // pointer to the m_ov structure
// deal with the error code 
			if(!bResult)
			{
				switch(dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
				{
					// asynchronous i/o is still in progress 
					// Proceed on to GetOverlappedResults();
					bRead = FALSE;
					break;
				}
				default:
				{
					// Another error has occured.  Process this error.
					ProcessErrorMessage("ReadFile()");
					break;
				}
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		} // close if(bRead)

		if(!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(m_hPort, // Handle to COMM port 
				&m_ov, // Overlapped structure
				&BytesRead, // Stores number of bytes read
				TRUE); // Wait flag

// deal with the error code 
			if(!bResult)
			{
				ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}
		} // close if(!bRead)

		LeaveCriticalSection(&m_csCommunicationSync);

		ProcessRead(RXBuff);
	}
}

void CPortReceiver::ReceiveMsg()
{
	if(m_RMCResult.IsEmpty())
		return;

	if(m_bSyndrome==true)
		return;

	if(m_RMCResult.IsEmpty()==FALSE)
	{
		ParseRMC(m_RMCResult, m_message);
		m_RMCResult.Empty();
	}
	if(m_GGAResult.IsEmpty()==FALSE)
	{
		ParseGGA(m_GGAResult, m_message);
		m_GGAResult.Empty();
	}

	const CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	pAppFrame->SendMessage(WM_VEHICLEMOVE, m_dwId, (LONG)&m_message);

	CPort::ReceiveMsg();
}

bool CPortReceiver::ProcessRead(char RXBuff)
{
	static CString Sentence;

	if(RXBuff==0X0A)
	{
		m_bSyndrome = true;

		if(Sentence.Left(6).Compare(_T("$GPRMC"))==0)
		{
			m_RMCResult = Sentence.Left(Sentence.GetLength()-1);
		}
		else if(Sentence.Left(6).Compare(_T("$GPGGA"))==0)
		{
			m_GGAResult = Sentence.Left(Sentence.GetLength()-1);
		}

		Sentence.Empty();
		m_bSyndrome = false;
	}
	else
	{
		Sentence += (char)RXBuff;
	}

	return true;
}
