#pragma once
#include "gdef.h"

class CMyPComm
{
public:
	CMyPComm(void);
	~CMyPComm(void);

private:
	static HANDLE		hExitEvent;
	static CWinThread*	hThreadRx;
	static UINT8*		rxBuf;
	static UINT8*		cmdBuf;

	static UINT CommReceiveProc(LPVOID pParam);
	static int CommandParsing(LPVOID lParam);
public:
	int					m_Port;
	int					m_Baud;

	static long			m_SampleIndex;
	static BOOL			m_IsRx;
	static UINT32*		m_pPropValue;
	static long*		m_pChannel[CHANNEL_MAX];
	static UINT8*		m_pRtnIndexBuf;
	static UINT8		m_RtnIndex;

	int PortOpen(int portNum, int baud);		// open port
	int PortClose(void);			// close port
	int SendCommand(UINT8 addr, UINT32 para);
};

