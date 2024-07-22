// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件


#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

#include <afxctl.h>         // ActiveX 控件的 MFC 支持
#include <afxext.h>         // MFC 扩展
#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT




// 如果不希望使用 MFC 数据库类，
//  请删除下面的两个包含文件
#include <afxdb.h>			// MFC 数据库类
#include <afxdao.h>			// MFC DAO 数据库类

