// UMDStat.cpp : 实现文件
//

#include "stdafx.h"
#include "LUSM.h"
#include "MainFrm.h"
#include "UMDStat.h"
#include "UMDShow.h"
#include "gdef.h"
// CUMDStat

IMPLEMENT_DYNCREATE(CUMDStat, CFormView)

CUMDStat::CUMDStat()
	: CFormView(CUMDStat::IDD)
{
	m_Resize = FALSE;	
	pPropValue = NULL;
	for (int i = 0; i < CHANNEL_MAX; i++)
		pCoefficient[i] = NULL;
}

CUMDStat::~CUMDStat()
{
}

void CUMDStat::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PARA, m_List);
}

BEGIN_MESSAGE_MAP(CUMDStat, CFormView)
	ON_MESSAGE(WM_USER_INIT, &CUMDStat::OnUserInit)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUMDStat 诊断

#ifdef _DEBUG
void CUMDStat::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUMDStat::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUMDStat 消息处理程序
/* 初始化，消息来自CMainFrame */
afx_msg LRESULT CUMDStat::OnUserInit(WPARAM wParam, LPARAM lParam)
{
	int		i;

	CRect	rc;
	GetClientRect(rc);
	m_List.MoveWindow(rc);

	m_List.SetRedraw(FALSE);
	m_List.EnableWindow(FALSE);
	for (i = 0; i < 8; i++) {
		m_List.InsertColumn(i, "");
		m_List.SetColumnWidth(i, 180 >> (i % 2u));
	}
	m_List.InsertItem(0, "");
	m_List.SetItemText(0, 0, "Dir:");
	m_List.SetItemText(0, 2, "Freq/kHz:");
	m_List.SetItemText(0, 4, "Phase/d:");
	m_List.InsertItem(1, "");
	m_List.SetItemText(1, 0, "DutyA/%:");
	m_List.SetItemText(1, 2, "DutyB/%:");
	m_List.SetItemText(1, 4, "Position:");
	m_List.SetItemText(1, 6, "Group:");
	m_List.InsertItem(2, "");
	m_List.SetItemText(2, 0, "CH0-Scale:");
	m_List.SetItemText(2, 2, "CH0-Offset:");
	m_List.SetItemText(2, 4, "CH1-Scale:");
	m_List.SetItemText(2, 6, "CH1-Offset:");
	m_List.InsertItem(3, "");
	m_List.SetItemText(3, 0, "CH2-Scale:");
	m_List.SetItemText(3, 2, "CH2-Offset:");
	m_List.SetItemText(3, 4, "CH3-Scale:");
	m_List.SetItemText(3, 6, "CH3-Offset:");

	m_List.SetRedraw(TRUE);
	m_List.Invalidate();

	CMainFrame *pManiFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pPropValue = pManiFrame->pPropValue;
	for (i = 0; i < CHANNEL_MAX; i++)
		pCoefficient[i] = &(((CUMDShow *)pManiFrame->m_pBottom)->coefficient[i][0]);

	m_Resize = TRUE;
	SetTimer(3u, 200u, NULL);
	return 0;
}

void CUMDStat::OnDestroy()
{
	KillTimer(3u);
	CFormView::OnDestroy();
}

/* 窗体大小发生改变，重绘控件 */
void CUMDStat::OnSize(UINT nType, int cx, int cy)
{
	if (m_Resize == TRUE) {
		CRect	rc;
		GetClientRect(rc);
		m_List.MoveWindow(rc);	
	}

	CFormView::OnSize(nType, cx, cy);
}

void CUMDStat::OnTimer(UINT_PTR nIDEvent)
{
	if ((nIDEvent != 3u) || (pPropValue[INDEX_CONNECT] == DEF_DISCONNECT)) {
		CFormView::OnTimer(nIDEvent); 
		return;
	}
	
	CString	str;
	
	m_List.SetRedraw(FALSE);
	switch (pPropValue[INDEX_RUNSTATE]) {
	case INDEX_RUNSTATE_CCW: m_List.SetItemText(0, 1, "CCW"); break;
	case INDEX_RUNSTATE_CW: m_List.SetItemText(0, 1, "CW"); break;
	default: m_List.SetItemText(0, 1, "STOP"); break;
	}
	str.Format("%-.2f", 0.001f * pPropValue[INDEX_CURFREQ]);
	m_List.SetItemText(0, 3, str);
	str.Format("%-.1f", 0.1f * pPropValue[INDEX_CURPHASE]);
	m_List.SetItemText(0, 5, str);
	str.Format("%-.1f", 0.1f * pPropValue[INDEX_CURDUTYA]);
	m_List.SetItemText(1, 1, str);
	str.Format("%-.1f", 0.1f * pPropValue[INDEX_CURDUTYB]);
	m_List.SetItemText(1, 3, str);
	str.Format("%-d", pPropValue[INDEX_CURPOSITION]);
	m_List.SetItemText(1, 5, str);
	str.Format("%-d", pPropValue[INDEX_CURGROUP]);
	m_List.SetItemText(1, 7, str);
	str.Format("%-5.2f", pCoefficient[0][0]);
	m_List.SetItemText(2, 1, str);
	str.Format("%-5.2f", pCoefficient[0][1]);
	m_List.SetItemText(2, 3, str);
	str.Format("%-5.2f", pCoefficient[1][0]);
	m_List.SetItemText(2, 5, str);
	str.Format("%-5.2f", pCoefficient[1][1]);
	m_List.SetItemText(2, 7, str);
	str.Format("%-5.2f", pCoefficient[2][0]);
	m_List.SetItemText(2, 1, str);
	str.Format("%-5.2f", pCoefficient[2][1]);
	m_List.SetItemText(2, 3, str);
	str.Format("%-5.2f", pCoefficient[3][0]);
	m_List.SetItemText(2, 5, str);
	str.Format("%-5.2f", pCoefficient[3][1]);
	m_List.SetItemText(2, 7, str);
	m_List.SetRedraw(TRUE);

	CFormView::OnTimer(nIDEvent);
}

