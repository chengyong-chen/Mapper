#pragma once

class CDatainfo;
class CProjection;

#define CX_GRID 500
#define CY_GRID 500

#define WM_FIXATEVIEW          (WM_USER + 16)
#define WM_FIXATERECT          (WM_USER + 17)
#define WM_SCROLLVIEW          (WM_USER + 18)
#define WM_SLIDERSCROLL        (WM_USER + 19)
#define WM_GETSPEBROWSEBAR     (WM_USER + 27)
#define WM_DOCMODIFIED         (WM_USER + 28)
#define WM_SUBMITACTION        (WM_USER + 29)
#define WM_SHOWRECINFO         (WM_USER + 31)
#define WM_SHOWGEOMINFO         (WM_USER + 32)
#define WM_SETMODIFIED         (WM_USER + 33)
#define WM_ACTIVEGEOM           (WM_USER + 36)
#define WM_GETRULERTOOL        (WM_USER + 40)
#define WM_SETRULER            (WM_USER + 41)
#define WM_GETPAPERSIZE        (WM_USER + 44)
#define WM_APPLYGEOMID         (WM_USER + 49)
#define WM_POSITIONPOU         (WM_USER + 53)
#define WM_INJECTATT           (WM_USER + 54)
#define WM_GETQUERYCTRL        (WM_USER + 56)
#define WM_GETPANE             (WM_USER + 57)
#define WM_GETVIEWORG          (WM_USER + 59)
#define WM_CALLENGTH           (WM_USER + 60)
#define WM_CALAREA             (WM_USER + 61)

#define	WM_GETPAGEITEM        (WM_USER +  64)
#define	WM_PUBLISHPC           (WM_USER +  66)
#define	WM_PUBLISHCE           (WM_USER +  67)

#define WM_GETWAYPOINTSDATABASE (WM_USER +  69)

#define	WM_SETDRAWSTATUS  (WM_USER + 72)
#define WM_GETRECORDFORM  (WM_USER + 73)
#define WM_POSTDRAW       (WM_USER + 74)

#define	WM_MARKTOPOLE     (WM_USER + 75)

#define WM_PLUGINITEMS    (WM_USER + 1000)
