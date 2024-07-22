#include "stdafx.h"
#include "Resource.h"
#include  <io.h>
#include  <stdlib.h>
#include  <mmsystem.h>

#include "Vehicle.h"
#include "../Public/Function.h"
#include "../Dataview/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CVehicle, CObject, 0)

CImageList CVehicle::m_imagelist1;
CImageList CVehicle::m_imagelist2;
CImageList CVehicle::m_imagelist3;
CImageList CVehicle::m_imagelist4;

CImageList CVehicle::m_imagelistbn;
CImageList CVehicle::m_imagelistbw;

int CVehicle::m_nAlarmCount = 0;

CVehicle::CVehicle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_dwId = 0;

	m_docLast = CPoint(0, 0);
	m_docCurrent = CPoint(0.0f, 0.0f);
	m_geoCurrent = CPointF(0.0f, 0.0f);
	m_nBearing = 0;
	m_fSpeed = 0.0f;

	m_bmpBG = nullptr;
	m_bmpTag = nullptr;
	const int nIcon1 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon1"), 1);
	const int nIcon2 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon2"), 2);
	const int nIcon3 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon3"), 3);
	const int nIcon4 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon4"), 4);

	if(m_imagelist1.m_hImageList==nullptr)
	{
		CString strImage1;
		strImage1.Format(_T("IDB_VEHICLE%d"), nIcon1);
		m_imagelist1.Create(strImage1, 32, 0, RGB(255, 0, 255));
	}

	if(m_imagelist1.m_hImageList==nullptr)
	{
		switch(nIcon1)
		{
		case 1:
			m_imagelist1.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		case 2:
			m_imagelist1.Create(IDB_VEHICLE2, 32, 0, RGB(255, 0, 255));
			break;
		case 3:
			m_imagelist1.Create(IDB_VEHICLE3, 32, 0, RGB(255, 0, 255));
			break;
		case 4:
			m_imagelist1.Create(IDB_VEHICLE4, 32, 0, RGB(255, 0, 255));
			break;
		default:
			m_imagelist1.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		}
	}

	if(m_imagelist2.m_hImageList==nullptr)
	{
		CString strImage2;
		strImage2.Format(_T("IDB_VEHICLE%d"), nIcon2);
		m_imagelist2.Create(strImage2, 32, 0, RGB(255, 0, 255));
	}

	if(m_imagelist2.m_hImageList==nullptr)
	{
		switch(nIcon2)
		{
		case 1:
			m_imagelist2.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		case 2:
			m_imagelist2.Create(IDB_VEHICLE2, 32, 0, RGB(255, 0, 255));
			break;
		case 3:
			m_imagelist2.Create(IDB_VEHICLE3, 32, 0, RGB(255, 0, 255));
			break;
		case 4:
			m_imagelist2.Create(IDB_VEHICLE4, 32, 0, RGB(255, 0, 255));
			break;
		default:
			m_imagelist2.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		}
	}

	if(m_imagelist3.m_hImageList==nullptr)
	{
		CString strImage3;
		strImage3.Format(_T("IDB_VEHICLE%d"), nIcon3);
		m_imagelist3.Create(strImage3, 32, 0, RGB(255, 0, 255));
	}

	if(m_imagelist3.m_hImageList==nullptr)
	{
		switch(nIcon3)
		{
		case 1:
			m_imagelist3.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		case 2:
			m_imagelist3.Create(IDB_VEHICLE2, 32, 0, RGB(255, 0, 255));
			break;
		case 3:
			m_imagelist3.Create(IDB_VEHICLE3, 32, 0, RGB(255, 0, 255));
			break;
		case 4:
			m_imagelist3.Create(IDB_VEHICLE4, 32, 0, RGB(255, 0, 255));
			break;
		default:
			m_imagelist3.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		}
	}

	if(m_imagelist4.m_hImageList==nullptr)
	{
		CString strImage4;
		strImage4.Format(_T("IDB_VEHICLE%d"), nIcon4);
		m_imagelist4.Create(strImage4, 32, 0, RGB(255, 0, 255));
	}

	if(m_imagelist4.m_hImageList==nullptr)
	{
		switch(nIcon4)
		{
		case 1:
			m_imagelist4.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		case 2:
			m_imagelist4.Create(IDB_VEHICLE2, 32, 0, RGB(255, 0, 255));
			break;
		case 3:
			m_imagelist4.Create(IDB_VEHICLE3, 32, 0, RGB(255, 0, 255));
			break;
		case 4:
			m_imagelist4.Create(IDB_VEHICLE4, 32, 0, RGB(255, 0, 255));
			break;
		default:
			m_imagelist4.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
			break;
		}
	}
	if(m_imagelistbn.m_hImageList==nullptr)
	{
		m_imagelistbn.Create(IDB_VEHICLEBN, 32, 0, RGB(255, 0, 255));
	}
	if(m_imagelistbw.m_hImageList==nullptr)
	{
		m_imagelistbw.Create(IDB_VEHICLEBW, 32, 0, RGB(255, 0, 255));
	}

	m_bTrack = false;
	m_bDrawTrace = false;

	m_bIcon = 0;
	m_dwStatus = 0;

	m_bAlarm = false;

	m_Route.m_hFile = INVALID_HANDLE_VALUE;
	m_pFrame = nullptr;
	nFlip = 0;
}

CVehicle::~CVehicle()
{
	if(m_Route.m_hFile!=INVALID_HANDLE_VALUE)
	{
		m_Route.Close();
		m_Route.m_hFile = INVALID_HANDLE_VALUE;
	}

	if(m_bmpBG!=nullptr&&m_pFrame!=nullptr)
	{
		CWnd* pView = m_pFrame->GetActiveView();
		if(pView!=nullptr)
		{
			CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
			Gdiplus::Point point = pViewinfo->DocToClient<Gdiplus::Point>(m_docLast);
			point.X -= 16;
			point.Y -= 16;

			CClientDC dc(pView);
			CDC mdc;
			mdc.CreateCompatibleDC(&dc);
			const HANDLE oldBM = mdc.SelectObject(m_bmpBG);
			dc.BitBlt(point.X, point.Y, 32, 32, &mdc, 0, 0, SRCCOPY);

			mdc.SelectObject(oldBM);
			mdc.DeleteDC();
		}

		::DeleteObject(m_bmpBG);
		m_bmpBG = nullptr;
	}
	if(m_bmpTag!=nullptr&&m_pFrame!=nullptr)
	{
		CWnd* pView = m_pFrame->GetActiveView();
		if(pView!=nullptr)
		{
			CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
			Gdiplus::Point point = pViewinfo->DocToClient<Gdiplus::Point>(m_docCurrent);
			point.X -= 16;
			point.Y -= 16;

			CClientDC dc(pView);
			CDC mdc;
			mdc.CreateCompatibleDC(&dc);
			const HANDLE oldBM = mdc.SelectObject(m_bmpTag);
			dc.BitBlt(point.X, point.Y, 32, 32, &mdc, 0, 0, SRCCOPY);

			mdc.SelectObject(oldBM);
			mdc.DeleteDC();
		}

		::DeleteObject(m_bmpTag);
		m_bmpTag = nullptr;
	}
	m_pFrame = nullptr;

	m_tracelist.RemoveAll();

	if(m_bAlarm==true)
	{
		m_bAlarm = false;
		CVehicle::DecreaseAlarm();
	}
}

void CVehicle::Move(CWnd* pWnd, const CDatainfo& datainfo, const double& lng, const double& lat, const double& altitude, const float& nBearing, const float& fSpeed, const long& nTime)
{
	if(pWnd==nullptr)
		return;

	CPointF geoPoint = CPointF(lng, lat);

	if(m_geoCurrent!=geoPoint)
	{
		if(m_bDrawTrace==true)
		{
			m_tracelist.AddTail(geoPoint);
		}

		if(m_Route.m_hFile!=INVALID_HANDLE_VALUE)
		{
			m_Route.Write(&lng, 8);
			m_Route.Write(&lat, 8);
			m_Route.Write(&fSpeed, 4);
			m_Route.Write(&nBearing, 4);
			m_Route.Write(&nTime, 4);
		}

		m_geoCurrent = geoPoint;
		const CPoint docPoint = datainfo.GeoToDoc(geoPoint);
		m_docCurrent = docPoint;
	}

	m_nBearing = nBearing;
	m_fSpeed = fSpeed;
}

void CVehicle::Flash(CWnd* pWnd, const CViewinfo& viewinfo, long nViewAngle)
{
	if(m_imagelist1.GetImageCount()<1)
		return;
	if(m_imagelist2.GetImageCount()<1)
		return;
	const Gdiplus::Point lastPoint = viewinfo.DocToClient<Gdiplus::Point>(m_docLast);
	Gdiplus::Point curtPoint = viewinfo.DocToClient<Gdiplus::Point>(m_docCurrent);

	CRect rect;
	pWnd->GetClientRect(rect);
	if(rect.PtInRect(CPoint(curtPoint.X, curtPoint.Y))==FALSE)
	{
		if(m_bmpBG!=nullptr)
		{
			::DeleteObject(m_bmpBG);
			m_bmpBG = nullptr;
		}
		if(m_bmpTag!=nullptr)
		{
			::DeleteObject(m_bmpTag);
			m_bmpTag = nullptr;
		}
		return;
	}

	CClientDC dc(pWnd);
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);

	CFont font;
	font.CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, _T("MS SansSerif"));

	CFont* pOldFont = dc.SelectObject(&font);
	CSize size = dc.GetTextExtent(m_strTag);
	size.cx += 4;
	size.cx += 2;

	if(m_bmpBG!=nullptr)
	{
		const HANDLE oldBM = mdc.SelectObject(m_bmpBG);
		dc.BitBlt(lastPoint.X-16, lastPoint.Y-16, 32, 32, &mdc, 0, 0, SRCCOPY);

		mdc.SelectObject(oldBM);

		::DeleteObject(m_bmpBG);
		m_bmpBG = nullptr;
	}
	if(m_bmpTag!=nullptr)
	{
		const HANDLE oldBmp = mdc.SelectObject(m_bmpTag);
		dc.BitBlt(lastPoint.X+15, lastPoint.Y-30, size.cx, size.cy, &mdc, 0, 0, SRCCOPY);

		mdc.SelectObject(oldBmp);

		::DeleteObject(m_bmpTag);
		m_bmpTag = nullptr;
	}

	if(m_bDrawTrace==true)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 5, RGB(150, 150, 150));
		CPen* oldPen = (CPen*)dc.SelectObject(&pen);

		if(abs(curtPoint.X-lastPoint.X)<100&&abs(curtPoint.Y-lastPoint.Y)<100)
		{
			dc.MoveTo(curtPoint.X, curtPoint.Y);
			dc.LineTo(lastPoint.X, lastPoint.Y);
		}

		dc.SelectObject(oldPen);
		pen.DeleteObject();
	}

	if(m_bmpBG==nullptr)
	{
		m_bmpBG = ::CreateBitmap(32, 32, 1, mdc.GetDeviceCaps(BITSPIXEL), nullptr);
		const HANDLE oldBmpBG = mdc.SelectObject(m_bmpBG);
		mdc.BitBlt(0, 0, 32, 32, &dc, curtPoint.X-16, curtPoint.Y-16, SRCCOPY);

		mdc.SelectObject(oldBmpBG);
	}
	if(m_bmpTag==nullptr&&m_strTag.IsEmpty()==FALSE)
	{
		m_bmpTag = ::CreateBitmap(size.cx, size.cy, 1, mdc.GetDeviceCaps(BITSPIXEL), nullptr);
		const HANDLE oldBmp = mdc.SelectObject(m_bmpTag);
		mdc.BitBlt(0, 0, size.cx, size.cy, &dc, curtPoint.X+15, curtPoint.Y-30, SRCCOPY);

		mdc.SelectObject(oldBmp);
	}
	mdc.DeleteDC();

	curtPoint.X -= 16;
	curtPoint.Y -= 16;
	const CPoint point(curtPoint.X, curtPoint.Y);
	if(m_fSpeed<4.0f)
	{
		switch(m_bIcon)
		{
		case 0:
			m_imagelist1.Draw(&dc, 17, point, ILD_TRANSPARENT);
			break;
		case 1:
			m_imagelist2.Draw(&dc, 17, point, ILD_TRANSPARENT);
			break;
		case 2:
			m_imagelist3.Draw(&dc, 17, point, ILD_TRANSPARENT);
			break;
		case 3:
			m_imagelist4.Draw(&dc, 17, point, ILD_TRANSPARENT);
			break;
		default:
			m_imagelist1.Draw(&dc, 17, point, ILD_TRANSPARENT);
		}
	}
	else
	{
		m_nBearing %= 360;
		const int n = (int)(((float)m_nBearing-nViewAngle+7.5f)/22.5f);

		switch(m_bIcon)
		{
		case 0:
			m_imagelist1.Draw(&dc, n, point, ILD_TRANSPARENT);
			break;
		case 1:
			m_imagelist2.Draw(&dc, n, point, ILD_TRANSPARENT);
			break;
		case 2:
			m_imagelist3.Draw(&dc, n, point, ILD_TRANSPARENT);
			break;
		case 3:
			m_imagelist4.Draw(&dc, n, point, ILD_TRANSPARENT);
			break;
		default:
			m_imagelist1.Draw(&dc, n, point, ILD_TRANSPARENT);
		}
	}

	if(m_bAlarm==false)
	{
		m_imagelistbn.Draw(&dc, nFlip, point, ILD_TRANSPARENT);
	}
	else
	{
		m_imagelistbw.Draw(&dc, nFlip, point, ILD_TRANSPARENT);
	}
	nFlip = (++nFlip)%4;

	curtPoint.X += 16;
	curtPoint.Y += 16;
	if(m_strTag.IsEmpty()==FALSE)
	{
		CPen penBlack(PS_SOLID, 0, COLORREF(RGB(0, 0, 0)));
		CBrush brushToolTag(GetSysColor(COLOR_INFOBK));
		CPen* pOldPen = dc.SelectObject(&penBlack);
		CBrush* pOldBrush = dc.SelectObject(&brushToolTag);

		CRect rect(CPoint(curtPoint.X, curtPoint.Y), size);
		rect.OffsetRect(15, -30);
		dc.Rectangle(rect);

		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);

		dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
		dc.SetTextAlign(TA_LEFT);
		dc.SetBkMode(TRANSPARENT);

		dc.TextOut(curtPoint.X+15+3, curtPoint.Y-30, m_strTag);
	}
	dc.SelectObject(pOldFont);
	font.DeleteObject();

	m_docLast = m_docCurrent;
}

void CVehicle::DrawTrace(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_bDrawTrace!=true)
		return;

	Gdiplus::PointF fPoint;
	POSITION pos = m_tracelist.GetHeadPosition();
	if(pos!=nullptr)
	{
		const CPointF geoPoint = m_tracelist.GetNext(pos);
		fPoint = viewinfo.WGS84ToClient(geoPoint);
	}

	CClientDC dc(pWnd);
	CPen pen;
	pen.CreatePen(PS_SOLID, 5, RGB(150, 150, 150));
	CPen* oldPen = (CPen*)dc.SelectObject(&pen);
	while(pos!=nullptr)
	{
		const CPointF geoPoint = m_tracelist.GetNext(pos);
		const Gdiplus::PointF point = viewinfo.WGS84ToClient(geoPoint);
		if(abs(point.X-fPoint.X)<100&&abs(point.Y-fPoint.Y)<100)
		{
			dc.MoveTo(fPoint.X, fPoint.Y);
			dc.LineTo(point.X, point.Y);
		}
		fPoint = point;
	}

	dc.SelectObject(oldPen);
	pen.DeleteObject();
}

void CVehicle::Store()
{
	if(m_Route.m_hFile!=INVALID_HANDLE_VALUE)
	{
		m_Route.Close();
		m_Route.m_hFile = INVALID_HANDLE_VALUE;
	}
	else if(AfxGetApp()->m_pszRegistryKey!=nullptr)
	{
		const CString strRegistryKey = AfxGetApp()->m_pszRegistryKey;
		CString strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE, strRegistryKey, _T(""), _T("Path"), nullptr);
		CString strFold;
		strFold.Format(_T("%d"), m_dwId);
		strPath = strPath+_T("\\")+strFold;
		if(_taccess(strPath, 00)==-1)
		{
			CreateDirectory(strPath, nullptr);
		}
		SYSTEMTIME SystemTime;
		GetSystemTime(&SystemTime);
		strFold.Format(_T("%d%2d%2d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay);
		strFold.Replace(_T(' '), _T('0'));

		strPath = strPath+_T("\\")+strFold;
		if(_taccess(strPath, 00)==-1)
		{
			CreateDirectory(strPath, nullptr);
		}

		CString strFile;
		long index = 1;
		TCHAR ch[5];
		do
		{
			_itot(index++, ch, 10);
			strFile.Format(_T("%s\\%s.Grf"), strPath, ch);
		} while(_taccess(strFile, 00)!=-1);

		CFileException e;
		if(m_Route.Open(strFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary, &e)==FALSE)
		{
			m_Route.m_hFile = INVALID_HANDLE_VALUE;
		}

		m_Route.Write(&m_dwId, 4);
	}
}

void CVehicle::ViewWillDraw(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_bmpBG!=nullptr)
	{
		if(m_pFrame!=nullptr)
		{
			CWnd* pView = m_pFrame->GetActiveView();
			if(pView!=nullptr)
			{
				Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_docLast);
				point.X -= 16;
				point.Y -= 16;

				CClientDC dc(pView);
				CDC mdc;
				mdc.CreateCompatibleDC(&dc);
				const HANDLE oldBM = mdc.SelectObject(m_bmpBG);
				dc.BitBlt(point.X, point.Y, 32, 32, &mdc, 0, 0, SRCCOPY);

				mdc.SelectObject(oldBM);
				mdc.DeleteDC();
			}
		}

		::DeleteObject(m_bmpBG);
		m_bmpBG = nullptr;
	}

	if(m_bmpTag!=nullptr)
	{
		if(m_pFrame!=nullptr)
		{
			CWnd* pView = m_pFrame->GetActiveView();
			if(pView!=nullptr)
			{
				Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_docCurrent);
				point.X -= 16;
				point.Y -= 16;

				CClientDC dc(pView);
				CDC mdc;
				mdc.CreateCompatibleDC(&dc);
				const HANDLE oldBM = mdc.SelectObject(m_bmpTag);
				dc.BitBlt(point.X, point.Y, 32, 32, &mdc, 0, 0, SRCCOPY);

				mdc.SelectObject(oldBM);
				mdc.DeleteDC();
			}
		}

		::DeleteObject(m_bmpTag);
		m_bmpTag = nullptr;
	}
}

void CVehicle::ViewDrawOver(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_bmpBG!=nullptr)
	{
		if(m_pFrame!=nullptr)
		{
			CWnd* pView = m_pFrame->GetActiveView();
			if(pView!=nullptr)
			{
				Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_docLast);
				point.X -= 16;
				point.Y -= 16;

				CClientDC dc(pView);
				CDC mdc;
				mdc.CreateCompatibleDC(&dc);
				const HANDLE oldBM = mdc.SelectObject(m_bmpBG);
				dc.BitBlt(point.X, point.Y, 32, 32, &mdc, 0, 0, SRCCOPY);

				mdc.SelectObject(oldBM);
				mdc.DeleteDC();
			}
		}

		::DeleteObject(m_bmpBG);
		m_bmpBG = nullptr;
	}

	if(m_bmpTag!=nullptr)
	{
		if(m_pFrame!=nullptr)
		{
			CWnd* pView = m_pFrame->GetActiveView();
			if(pView!=nullptr)
			{
				Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_docCurrent);
				point.X -= 16;
				point.Y -= 16;

				CClientDC dc(pView);
				CDC mdc;
				mdc.CreateCompatibleDC(&dc);
				const HANDLE oldBM = mdc.SelectObject(m_bmpTag);
				dc.BitBlt(point.X, point.Y, 32, 32, &mdc, 0, 0, SRCCOPY);

				mdc.SelectObject(oldBM);
				mdc.DeleteDC();
			}
		}

		::DeleteObject(m_bmpTag);
		m_bmpTag = nullptr;
	}

	if(m_bDrawTrace==true&&pWnd!=nullptr)
	{
		DrawTrace(pWnd, viewinfo);
	}

	if(pWnd!=nullptr)
	{
		const CPoint docPoint = viewinfo.m_datainfo.GeoToDoc(m_geoCurrent);
		m_docLast = docPoint;
		m_docCurrent = docPoint;
	}
}

void CVehicle::DecreaseAlarm()
{
	if(m_nAlarmCount==0)
		return;

	m_nAlarmCount--;
	if(m_nAlarmCount==0)
	{
		CString strSound = AfxGetApp()->GetProfileString(_T(""), _T("Sound"), nullptr);
		strSound = strSound+_T("1.wav");
		PlaySound(nullptr, nullptr, SND_FILENAME|SND_ASYNC);
		//	PlaySound(nullptr, nullptr, SND_PURGE|SND_RESOURCE);
	}
}

void CVehicle::IncreaseAlarm()
{
	m_nAlarmCount++;
	if(m_nAlarmCount==1)
	{
		CString strSound = AfxGetApp()->GetProfileString(_T(""), _T("Sound"), nullptr);
		strSound = strSound+_T("1.wav");
		PlaySound(strSound, nullptr, SND_FILENAME|SND_ASYNC|SND_LOOP);
	}
}
