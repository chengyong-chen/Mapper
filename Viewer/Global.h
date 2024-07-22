#pragma once

#define	TIMER_GETGPSMSE        1
#define	TIMER_VEHICLE          2
#define TIMER_FLASHGEOM      3
#define TIMER_LINK             4
#define TIMER_REPLAY           13
#define TIMER_PULSE            14

#define WM_KEYQUERY          (WM_USER + 750)
#define WM_RGNQUERY          (WM_USER + 752)
#define WM_GPSMESSAGE        (WM_USER + 753)
#define WM_ACTIVEVIEW        (WM_USER + 754)
#define WM_VIEWWILLDRAW      (WM_USER + 756)
#define WM_VIEWDRAWOVER      (WM_USER + 757)
#define WM_SETCOUNT          (WM_USER + 758)
#define WM_IDLE              (WM_USER + 762)
#define WM_GETVEHICLE        (WM_USER + 763)
#define WM_ISMAINCHILD       (WM_USER + 764)
#define WM_OPENDOCUMENTFILE  (WM_USER + 765)
#define WM_ADDHISTORY        (WM_USER + 766)
#define WM_CHILDDOCCHANGED   (WM_USER + 768)
#define WM_BUOYMOVED         (WM_USER + 770)
#define WM_ACTIVEMANAGER     (WM_USER + 771)
#define WM_URLCHANGED        (WM_USER + 772)
#define WM_GETPORT           (WM_USER + 774)
#define WM_GETGPSROUTEHUB    (WM_USER + 775)
#define WM_ACTIVATEVEHICLE    (WM_USER + 776)
#define WM_SETGEOSIGN        (WM_USER + 777)
#define WM_GEOPOINTINMAP     (WM_USER + 778)
#define WM_SEARCHROUTE       (WM_USER + 779)

#define WM_MAPROAD           (WM_USER + 780)

#define WM_BROWSEHTML        (WM_USER + 782)
#define WM_DESTROYHTML       (WM_USER + 783)

#define WM_BEFORNAVIGATE     (WM_USER + 784)

#define WM_GETATLASMANAGER   (WM_USER + 785)
#define WM_MATCHMAP         (WM_USER + 786)
#define WM_CANZOOMIN         (WM_USER + 787)
#define WM_GETMAINCHILD      (WM_USER + 789)

#define WM_GETSCROLLBAR      (WM_USER + 792)

#define WM_VEHICLEDISPATCH   (WM_USER + 793)

bool IsInternetConnection();
