#pragma once

#include "../Coding/Message.h"

bool WriteReceivedMsgToDataBase(CMessageReceived* pMsg, ADOCG::_ConnectionPtr& pDBConnect);

bool VariantTimeToYYMMDDhhmmString(DATE vDate, CString& strDate);

bool Generate_V_SQL(CMessageReceived* pMsg, CString& strSQL, const ADOCG::_ConnectionPtr& pDBConnect);// 保存收到的车机位置
bool Generate_M_SQL(CMessageReceived* pMsg, CString& strSQL, const ADOCG::_ConnectionPtr& pDBConnect);// 保存收到的车机短信
bool Generate_D_SQL(CMessageReceived* pMsg, CString& strSQL, ADOCG::_ConnectionPtr& pDBConnect);// dispatch msg
//////////////////////////////////////////////////////////////////////////

bool OpenCenterDataBase(ADOCG::_ConnectionPtr& pDBConnect, LPCTSTR strConnectionString, LPCTSTR lpszUserName, LPCTSTR lpszPassWord);
bool CloseCenterDatabase(const ADOCG::_ConnectionPtr& pDBConnect);
void ImportRouteFileToDatabase(const ADOCG::_ConnectionPtr& pDBConnect, LPCTSTR lpszSourceFile, DWORD dwIdParam);
UINT GetRecordsCount(LPCTSTR lpszTableName, const ADOCG::_ConnectionPtr& pDBConnec);
