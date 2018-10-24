#include "stdafx.h"
#include "MyPComm.h"
#include "PCOMM.H"
#include "gdef.h"

/* 静态变量初始化 */
HANDLE		CMyPComm::hExitEvent = NULL;
CWinThread* CMyPComm::hThreadRx = NULL;
UINT8*		CMyPComm::rxBuf = NULL;
UINT8*		CMyPComm::cmdBuf = NULL;
UINT32*		CMyPComm::m_pPropValue = NULL;
long*		CMyPComm::m_pChannel[CHANNEL_MAX] = {NULL};
UINT8*		CMyPComm::m_pRtnIndexBuf = NULL;
UINT8		CMyPComm::m_RtnIndex = 0;
long		CMyPComm::m_SampleIndex = 0;
BOOL		CMyPComm::m_IsRx = FALSE;

/* 类初始化 */
CMyPComm::CMyPComm(void)
{
	int	i;

	m_Port = 0;
	m_Baud = 0;

	rxBuf = new UINT8[10000];
	cmdBuf = new UINT8[512];
	m_pPropValue = new UINT32[INDEX_MAX];
	for (i = 0; i < INDEX_MAX; i++) m_pPropValue[i] = 0;
	for (i = 0; i < CHANNEL_MAX; i++) m_pChannel[i] = new long[SAMPLE_MAX + 8u];
	m_pRtnIndexBuf = new UINT8[INDEX_MAX];
}

CMyPComm::~CMyPComm(void)
{
	int	i;
	PortClose();
	if (rxBuf != NULL) {delete []rxBuf; rxBuf = NULL;}
	if (cmdBuf != NULL) {delete []cmdBuf; cmdBuf = NULL;}
	if (m_pPropValue != NULL) {delete []m_pPropValue; m_pPropValue = NULL;}
	for (i = 0; i < CHANNEL_MAX; i++) {
		if (m_pChannel[i] != NULL) {delete []m_pChannel[i]; m_pChannel[i] = NULL;}
	}
	if (m_pRtnIndexBuf != NULL) {delete []m_pRtnIndexBuf; m_pRtnIndexBuf = NULL;}
}

// open port
int CMyPComm::PortOpen(int portNum, int baud)
{
	CString		str;
	int			ret;

	// open port and config parameters
	m_Port = 0;
	switch (baud) {
	case 9600:	 m_Baud = B9600;   break;
	case 115200: m_Baud = B115200; break;
	case 230400: m_Baud = B230400; break;
	case 460800: m_Baud = B460800; break;
	case 921600: m_Baud = B921600; break;
	default: AfxMessageBox("Invaid baud rate!"); break;
	}

	ret = sio_open(portNum);
	ret += sio_ioctl(portNum, m_Baud, P_NONE | BIT_8 | STOP_1);
	ret += sio_flowctrl(portNum, 0);
	ret += sio_SetReadTimeouts(portNum, MAXDWORD, 0);
	ret += sio_SetWriteTimeouts(portNum, MAXDWORD);
	ret += sio_flush(portNum, 2);
	if (ret != SIO_OK) return -1;

	// creat event and thread
	hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL) ;
	hThreadRx = AfxBeginThread(CommReceiveProc, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL);	
	if ((hThreadRx == NULL) || (hExitEvent == NULL)) {
		PortClose();
		AfxMessageBox("Comm thread create error!");
		return -1;
	}

	m_Port = portNum;
	return SIO_OK;
}

/* close port */
int CMyPComm::PortClose(void)
{
	if ((0 == m_Port) || (hThreadRx == NULL) || (hExitEvent == NULL)) return -1;

	sio_close(m_Port);
	m_Port = 0;
	SetEvent(hExitEvent);
	if (WaitForSingleObject(hThreadRx->m_hThread, 1000) == WAIT_TIMEOUT) 
		TerminateThread(hThreadRx->m_hThread, 0);
	CloseHandle(hExitEvent);
	hExitEvent = NULL;
	hThreadRx = NULL;
	return 0;
}

/*	send command : 低位先传输，高位后传输
	0x5A 0xA5 帧头
	0x06		后续数据个数
	最后一个字节 check sum
*/
int CMyPComm::SendCommand(UINT8 addr, UINT32 para)
{
	UINT8	buf[9] = {0x5A, 0xA5, 0x06};
	UINT8*	p = (UINT8 *)(&para);
	UINT8	tmp = 0;
	int		i;
	
	if (m_Port == 0) {
		AfxMessageBox("Port is closed!");
		return -1;
	}

	buf[3] = addr;
	for (i = 0; i < 4; i++) buf[i + 4] = p[i];
	for (i = 0; i < 6; i++) tmp += buf[i + 2];
	buf[8] = tmp;

	sio_write(m_Port, (char *)buf, 9u);
	return 0;
}

/* data receive */
UINT CMyPComm::CommReceiveProc(LPVOID pParam)
{
	DWORD	ret;
	DWORD	dwExitCode;
	int		port = ((CMyPComm *)pParam)->m_Port;
	UINT8	state = 0u;
	UINT32	cmdIndex = 0;
	UINT8	temp;
	int		len;
	int		i;
	
	if ((cmdBuf == NULL) || (rxBuf == NULL)) { AfxMessageBox("Buffer is not enough!"); return 0;}

	while (1) {
		ret = WaitForSingleObject(hExitEvent, 50u);		// 每50ms读取一次缓冲区
		if (ret == WAIT_OBJECT_0) {
			GetExitCodeThread(hThreadRx->m_hThread, &dwExitCode);
			AfxEndThread(dwExitCode, TRUE);			
		} else if (ret == WAIT_TIMEOUT) {
			len = sio_read(port, (char *)rxBuf, 2048);
			if (len <= 0) continue;
			m_IsRx = TRUE;
			for (i = 0; i < len; i++) {
				temp = rxBuf[i];
				switch (state) {
				case 0u:
					if (temp == 0x5A) state++;
					else state = 0u;
					break;
				case 1u:
					if (temp == 0xA5) state++;
					else state = 0u;
					break;
				case 2:
					cmdBuf[0] = temp;
					cmdIndex = 0u;
					state++;
					break;
				case 3:
					cmdBuf[++cmdIndex] = temp;
					if (cmdIndex >= cmdBuf[0]) {
						CommandParsing(pParam);
						state = 0;
					}
					break;
				default: break;
				}
			} /* end of for loop */
		} else {
			::AfxMessageBox("Thread error in MyPComm.cpp");
			break;
		}
	}
	return 0;
}

/* 对接收到的消息进行解析 */
int CMyPComm::CommandParsing(LPVOID lParam)
{
	int			i;
	UINT8		sumCheck = 0;
	UINT8		len = cmdBuf[0];
	UINT8		opt = cmdBuf[1];
	UINT8*		p;
	UINT8*		p16;
	long		tmp;
	UINT16		tmp16;

	// 累加和校验
	for (i = 0; i < len; i++) sumCheck += cmdBuf[i];
	if (sumCheck != cmdBuf[len]) return -1;
	// 数据发送到指定位置
	if (opt != INDEX_DATA) {
		p = (UINT8 *)(&(m_pPropValue[opt]));
		for (i = 0; i < 4; i++)	p[i] = cmdBuf[i + 2];
		if (opt < INDEX_STATUS_START) {	/* UMD PROP参数更新 */
			m_pRtnIndexBuf[m_RtnIndex] = opt;
			if (m_RtnIndex >= (INDEX_MAX - 1)) m_RtnIndex = 0;
			else m_RtnIndex++;
		}
	} else if (m_pPropValue[INDEX_CURVE] != DEF_ON) {
		m_SampleIndex = 0;
	} else {
		if (m_SampleIndex >= SAMPLE_MAX) return 0;
		
		p = (UINT8 *)(&tmp);
		p16 = (UINT8 *)(&tmp16);

		for (i = 0; i < (len - 2);) {
			m_pChannel[0][m_SampleIndex] = (long)(INT8)cmdBuf[i + 2];
			p[0] = cmdBuf[i + 3];
			p[1] = cmdBuf[i + 4];
			p[2] = cmdBuf[i + 5];
			p[3] = cmdBuf[i + 6];
			m_pChannel[1][m_SampleIndex] = tmp;

			p[0] = cmdBuf[i + 7];
			p[1] = cmdBuf[i + 8];
			p[2] = cmdBuf[i + 9];
			p[3] = cmdBuf[i + 10];
			m_pChannel[2][m_SampleIndex] = tmp;

			p16[0] = cmdBuf[i + 11];
			p16[1] = cmdBuf[i + 12];
			m_pChannel[3][m_SampleIndex] = (long)(INT16)tmp16;
			
			i += 11;
			m_SampleIndex++;
			if (m_SampleIndex >= SAMPLE_MAX) return 0;
		}
	}
	return 0;
}
