/********************************************************************
	created:	2003/06/03
	created:	2003/6/3   13:44
	filename: 	e:\server\mapday_gps_server_code\giserver\dlgmonitorview_for_inforprocess.cpp
	file path:	e:\server\mapday_gps_server_code\giserver
	file base:	dlgmonitorview_for_inforprocess
	file ext:	cpp
	author:		Liu Xiaorui
	
	purpose:	处理信息服务终端的数据以及处理警告信息。
*********************************************************************/


#include "stdafx.h"
#include "Trasvr.h"
#include "MonitorDlg.h"
#include "global.h"
#include "../include/ExtPopupMenuWnd.h"
#include "MainFrm.h"

#include <queue>
#include <afxdb.h>

using namespace std;
#include "..\SMSCommu\ExportFunctions.h"
#include "..\Socket\Exstdcfunction.h"
#include "..\SMSCommu\WhoSendWhoRecv.h"
#include "..\SMSCommu\DlgSmsContainter.h"
#include "..\Coding\Coder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




