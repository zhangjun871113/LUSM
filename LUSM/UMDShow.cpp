// UMDShow.cpp : 实现文件
//

#include "stdafx.h"
#include "LUSM.h"
#include "UMDShow.h"
#include "gdef.h"
#include "MainFrm.h"
#include "tchart.h"
#include "CSeries.h"
#include "CExport.h"
#include "CExportFormat.h"

// CUMDShow

HANDLE			CUMDShow::hEvent = NULL;
CWinThread*		CUMDShow::hThread = NULL;
long			CUMDShow::preIndex = 0;
long			CUMDShow::m_curIndex = 0;
long			CUMDShow::m_totalIndex = 0;
int				CUMDShow::m_curChan = 0;
float			CUMDShow::coefficient[CHANNEL_MAX][2u] = {0};
COleSafeArray	CUMDShow::XValues;
COleSafeArray	CUMDShow::YValues[CHANNEL_MAX];
long*			CUMDShow::pChannel[CHANNEL_MAX] = {NULL};
long*			CUMDShow::pSampleIndex = NULL;
UINT32*			CUMDShow::pPropValue = NULL;

IMPLEMENT_DYNCREATE(CUMDShow, CFormView)

CUMDShow::CUMDShow()
	: CFormView(CUMDShow::IDD)
{
	int		i;
	DWORD	numElements[] = {SAMPLE_MAX + 8u};

	m_Resize = FALSE;
	for (i = 0; i < CHANNEL_MAX; i++) {
		coefficient[i][0] = 1;
		coefficient[i][1] = 0;
	}

	for (i = 0; i < CHANNEL_MAX; i++) pChannel[i] = NULL;

	XValues.Create(VT_I4, 1, numElements);
	for (i = 0; i < CHANNEL_MAX; i++) YValues[i].Create(VT_R8, 1, numElements);
}

CUMDShow::~CUMDShow()
{
}

void CUMDShow::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART, m_TChart);
}

BEGIN_MESSAGE_MAP(CUMDShow, CFormView)
	ON_MESSAGE(WM_USER_INIT, &CUMDShow::OnUserInit)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_REDRAW, &CUMDShow::OnUserRedraw)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_CHANGECURVE, &CUMDShow::OnUserChangecurve)
	ON_MESSAGE(WM_USER_EXPORT, &CUMDShow::OnUserExport)
END_MESSAGE_MAP()


// CUMDShow 诊断

#ifdef _DEBUG
void CUMDShow::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUMDShow::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CUMDShow 消息处理程序
/* 初始化，消息来自CMainFrame */
afx_msg LRESULT CUMDShow::OnUserInit(WPARAM wParam, LPARAM lParam)
{	
	int		i;

	CRect	rc;
	GetClientRect(rc);
	m_TChart.MoveWindow(&rc);
	
	((CSeries)m_TChart.Series(0)).put_Name("Flag");
	((CSeries)m_TChart.Series(1)).put_Name("Position");
	((CSeries)m_TChart.Series(2)).put_Name("Speed");

	for (i = 3; i < CHANNEL_MAX; i++)
		((CSeries)m_TChart.Series(i)).put_ShowInLegend(FALSE);

	CMainFrame*	pMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pPropValue = pMainFrame->pPropValue;
	pSampleIndex = (long *)(&(pMainFrame->m_pComm.m_SampleIndex));
	for (i = 0; i < CHANNEL_MAX; i++) {
		pChannel[i] = pMainFrame->m_pComm.m_pChannel[i];
	}

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	/* 创建新线程 */
	if (hEvent == NULL) {
		AfxMessageBox("Create event error!");
		return -1;
	}
	hThread = AfxBeginThread(DataProcessingProc, (LPVOID)this, THREAD_PRIORITY_NORMAL);
	if (hThread == NULL) {
		CloseHandle(hEvent);
		hEvent = NULL;
		AfxMessageBox("Create thread error!");
		return -1;
	}

	m_Resize = TRUE;
	SetTimer(4u, 500u, NULL);
	return 0;
}

afx_msg void CUMDShow::OnDestroy()
{
	KillTimer(4u);

	SetEvent(hEvent);
	if (WaitForSingleObject(hThread->m_hThread, 1000) == WAIT_TIMEOUT)
		TerminateThread(hThread->m_hThread, 0);
	CloseHandle(hEvent);
	hEvent = NULL;
	hThread = NULL;

	CFormView::OnDestroy();
}

/* 工作区大小发生改变，重绘控件 */
afx_msg void CUMDShow::OnSize(UINT nType, int cx, int cy)
{
	if (m_Resize == TRUE) {
		CRect	rc;
		GetClientRect(rc);
		m_TChart.MoveWindow(&rc);
	}

	CFormView::OnSize(nType, cx, cy);
}

afx_msg void CUMDShow::OnTimer(UINT_PTR nIDEvent)
{
	int		i;
	long	temp = m_totalIndex;

	if ((nIDEvent != 4u) || (temp == preIndex)) {preIndex = temp; CFormView::OnTimer(nIDEvent); return;}

	for (i = 0; i< m_curChan; i++) {
		((CSeries)m_TChart.Series(i)).AddArray(temp, YValues[i], XValues);
	}
	
	preIndex = temp;
	CFormView::OnTimer(nIDEvent);
}

/* 重绘 */
afx_msg LRESULT CUMDShow::OnUserRedraw(WPARAM wParam, LPARAM lParam)
{
	m_curIndex = 0;
	preIndex = 0;
	MethodOpenLoop(m_totalIndex);
	return 0;
}

/* 改变曲线特性 */
afx_msg LRESULT CUMDShow::OnUserChangecurve(WPARAM wParam, LPARAM lParam)
{		
	if ((pPropValue[INDEX_RUNMODE] == MODE_MANUAL) && (pPropValue[INDEX_METHOD] == INDEX_METHOD_FOLLOW_SIN)) {
		((CSeries)m_TChart.Series(3)).put_Name("Reference");
		((CSeries)m_TChart.Series(3)).put_ShowInLegend(TRUE);
		m_curChan = 4;
	} else {
		((CSeries)m_TChart.Series(3)).put_Name("DutyA");
		((CSeries)m_TChart.Series(3)).put_ShowInLegend(TRUE);
		m_curChan = 4;
	}

	CMainFrame*	pManiFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;	/* 获取其他文件中的变量地址 */
	pManiFrame->m_pComm.SendCommand((UINT8)INDEX_CURVE, (UINT32)(pPropValue[INDEX_CURVE]));

	return 0;
}

afx_msg LRESULT CUMDShow::OnUserExport(WPARAM wParam, LPARAM lParam)
{
	CString		strPath;
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files(*.CSV)|*.CSV||", this);

	if (IDOK == dlg.DoModal()) {
		strPath = dlg.GetPathName();
		if (strPath.Right(4) != ".CSV") strPath += ".CSV";
		CStdioFile	sfile;
		if (FALSE == sfile.Open((LPCTSTR)strPath, CFile::modeCreate | CFile::modeWrite, NULL)) {
			AfxMessageBox("Save error!");
			return -1;
		}
		/* format */
		CString	str, str1;
		int		i, j;

		for (i = 0; i < m_totalIndex; i++) {
			str1.Format("%-3.8f", i * 0.001);
			str = str1 + ", ";
			for (j = 0; j < m_curChan; j++) {
				str1.Format("%-3.8f", (double)pChannel[j][i]);
				str = str + str1 + ", ";
			}
			str += "\n";
			sfile.WriteString((LPCTSTR)str);
		}
		sfile.Close();
		strPath += ".bmp";
		((CExportFormat)((CExport)m_TChart.get_Export()).get_asBMP()).SaveToFile(strPath);
	}

	return 0;
}

/* 线程-获取数据 */
UINT CUMDShow::DataProcessingProc(LPVOID lPvoid)
{
	DWORD		ret;
	DWORD		dwExitCode;

	while (1) {
		ret = WaitForSingleObject(hEvent, 50u);
		if (ret == WAIT_OBJECT_0) {
			GetExitCodeThread(hThread->m_hThread, &dwExitCode);
			AfxEndThread(dwExitCode, TRUE);
		} else if ((ret == WAIT_TIMEOUT) && (pPropValue[INDEX_CURVE] == DEF_ON)) {
			MethodOpenLoop(*pSampleIndex);
		} else {
			m_curIndex = 0;
		}
	}
	return 0;
}

/* 计算数据 */
void CUMDShow::MethodOpenLoop(long sampleIndex)
{
	double	val;
	long	index;

	index = m_curIndex;
	while (index < sampleIndex) {
		XValues.PutElement(&index, &index);

		val = (double)(pChannel[0][index]) * coefficient[0][0] + coefficient[0][1];	/* flag */
		YValues[0].PutElement(&index, &val);

		val = (double)(pChannel[1][index] - pChannel[1][0]) * coefficient[1][0] + coefficient[1][1];	/* position */
		YValues[1].PutElement(&index, &val);

		val = (double)(pChannel[2][index]) * coefficient[2][0] + coefficient[2][1];	/* speed */
		YValues[2].PutElement(&index, &val);

		val = (double)(pChannel[3][index]) * coefficient[3][0] + coefficient[3][1];	/* cmdVoltage */
		YValues[3].PutElement(&index, &val);

		index++;
	}
	m_curIndex = index;
	m_totalIndex = index;
}
