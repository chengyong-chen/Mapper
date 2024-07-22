#pragma once

#include "../Dataview/Datainfo.h"
#include "../Dataview/Viewinfo.h"
#include "../Public/Global.h"

#define WM_VEHICLEMESSAGE (WM_USER + 401)
#define WM_VEHICLEMOVE    (WM_USER + 403)
#define WM_VEHICLECONTROL (WM_USER + 404)
#define WM_VEHICLEICON    (WM_USER + 406)
#define WM_VEHICLESTATUS  (WM_USER + 407)
#define WM_VEHICLELOCATION (WM_USER + 408)

class CVehicleTag;

class __declspec(dllexport) CVehicle : public CObject
{
	protected:
	DECLARE_SERIAL(CVehicle);
	CVehicle();

	~CVehicle() override;

	public:
	DWORD m_dwId;
	BYTE m_bIcon;
	DWORD m_dwStatus;
	bool m_bAlarm;

	CString m_strUser;
	CString m_strCode;
	CString m_strTag;
	CStringA m_strIVU;

	public:
	virtual void Move(CWnd* pWnd, const CDatainfo& datainfo, const double& lng, const double& lat, const double& altitude, const float& nBearing, const float& fSpeed, const long& nTime);

	public:
	CPoint m_docCurrent;
	CPointF m_geoCurrent;
	CPoint m_docLast;

	private:
	int m_nBearing;
	float m_fSpeed;
	HBITMAP m_bmpBG;
	HBITMAP m_bmpTag;

	public:
	void ViewWillDraw(CWnd* pWnd, const CViewinfo& viewinfo);
	void ViewDrawOver(CWnd* pWnd, const CViewinfo& viewinfo);

	public:
	static CImageList m_imagelist1;
	static CImageList m_imagelist2;
	static CImageList m_imagelist3;
	static CImageList m_imagelist4;

	static CImageList m_imagelistbn;
	static CImageList m_imagelistbw;

	public:
	virtual void Flash(CWnd* pWnd, const CViewinfo& viewinfo, long nViewAngle = 0);
	virtual void DrawTrace(CWnd* pWnd, const CViewinfo& viewinfo);

	public:
	bool m_bTrack;
	bool m_bDrawTrace;

	CFile m_Route;
	virtual void Store();
	CFrameWnd* m_pFrame;
	public:
	int nFlip;

	CList<CPointF, CPointF&> m_tracelist;

	public:
	static int m_nAlarmCount;
	static void DecreaseAlarm();
	static void IncreaseAlarm();
};

typedef CTypedPtrMap<CMapWordToOb, DWORD, CVehicle*> CMapVehicle;

struct Content
{
	DWORD dwId;
	CString strCode;
	CString strUser;
	CString strLink;
	CString strType;
	CString strColor;
	CString strDriver;
	CString strIVU;
	CString strPass;
};
