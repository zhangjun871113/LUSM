// ComSel.cpp : 实现文件
//

#include "stdafx.h"
#include "LUSM.h"
#include "ComSel.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CComSel 对话框

IMPLEMENT_DYNAMIC(CComSel, CDialogEx)

CComSel::CComSel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CComSel::IDD, pParent)
{

}

CComSel::~CComSel()
{
}

void CComSel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_BAUD, m_Baud);
	DDX_Control(pDX, IDC_COMB_PORT, m_Port);
}


BEGIN_MESSAGE_MAP(CComSel, CDialogEx)
END_MESSAGE_MAP()

// CComSel 消息处理程序

BOOL CComSel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Baud.SetCurSel(2);	/* 230400 */
	GetComList();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CComSel::OnOK()
{
	CString		str;
	CMainFrame*	pManiFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);

	m_Port.GetWindowTextA(str);		// 获取COM端口号
	if (str != "") str = str.Right(str.GetLength() - 3);
	(pManiFrame->m_Port) = _ttoi(str);

	m_Baud.GetWindowTextA(str);
	(pManiFrame->m_Baud) = _ttoi(str);

	CDialogEx::OnOK();
}

void CComSel::GetComList(void)
{
	HKEY	hKey;
	char	valueName[32];
	DWORD	valueNameLength = 32;
	DWORD	rDataType;
	char	rData[8];
	DWORD	rDataLength = 8;
	LONG	lRet;
	DWORD	i;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) { /*get key handle */
		m_Port.ResetContent();	/* clear content*/
		for (i = 0; i < 32u; i++) { /* get port number in register */
			lRet =  RegEnumValue(hKey, i, valueName, &valueNameLength, NULL, &rDataType,  (LPBYTE)rData, &rDataLength);
			if (lRet == ERROR_NO_MORE_ITEMS) break;
			m_Port.InsertString(i, (LPCTSTR)rData);
		}
		RegCloseKey(hKey);
		m_Port.SetCurSel(0u); /* select the first option */
	}	
}

BOOL CComSel::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)))
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
