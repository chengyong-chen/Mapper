// Frame.h: interface for the CFrame class.

//////////////////////////////////////////////////////////////////////

#pragma once

#include <comdef.h>

// #include <afxdisp.h>

typedef enum
{
	WAVCOM_SEND_AT, // WAVCOM发送指令
	WAVCOM_RECV_AT, // WAVCOM接收指令
	AT_CMG_HASMSG, // AT 指令, 有返回消息内容
	AT_CMG_NOMSG // AT 指令, 无返回消息内容
} AT_ID;

// 事件类型
typedef enum
{
	EVENT_RECV_MSG = 0 // 收到一条消息事件
} COMM_EVENT_TYPE;

// 数据帧基类
class CBaseFrame
{
public:
	CBaseFrame();
	virtual ~CBaseFrame();

	UINT m_ATID; // 指令id

	_variant_t m_Vt; // VARIANT 发送数据内容
	_variant_t m_RecvTime; // 接收消息事件(最后一条
	_variant_t m_vtPhoneNo; // 电话号码
	_variant_t m_MsgContent; // 解码后的消息内容

	BOOL m_bSendUseUnicode; // 使用UNICODE格式发送消息
};

// 数据帧类
class AFX_EXT_CLASS MSCommFrame : public CBaseFrame
{
public:
	int nOutBufferSize; // 写缓冲区大小
	short nErrCode; // 错误代码

public:
	MSCommFrame();
	~MSCommFrame() override;

	bool ParseSendData(); // 解析发送数据格式
	bool ParseRecvData(); // 解析接收数据格式
};
