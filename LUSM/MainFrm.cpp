// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "LUSM.h"
#include "gdef.h"
#include "MainFrm.h"
#include "UMDProp.h"
#include "UMDStat.h"
#include "UMDShow.h"
#include "MyPComm.h"
#include "ComSel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI_RANGE(ID_BTN_CNT, ID_BTN_CFG, &CMainFrame::OnUpdateUMD)
	ON_UPDATE_COMMAND_UI(ID_STATUS_LED, &CMainFrame::OnUpdateStatusBar)
	ON_COMMAND_RANGE(ID_BTN_CNT, ID_BTN_CFG, &CMainFrame::OnCmdUMD)
	ON_MESSAGE(WM_USER_CHANGE_IMAGE, &CMainFrame::OnUserChangeImage)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	/* 窗口分割后各视图指针 */
	m_pLeft = NULL;
	m_pTop = NULL;
	m_pBottom = NULL;
	/* 串口通讯端口号及波特率初始化 */
	m_Port = 0;
	m_Baud = 0;
	/* 初始参数指针初始化 */
	pPropValue = NULL;
	/* 状态栏禁止 */
	bFlag = FALSE;
}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_IconLED = AfxGetApp()->LoadIconA(IDI_LED);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane((UINT)ID_STATUS_LED, "", TRUE, m_IconLED), "");
	m_wndStatusBar.RedrawWindow();

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/* 分割窗体，分成三部分，然后发送消息给各个子视图 */
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect	rc;
	GetClientRect(rc);	/* 获取客户区大小 */

	/* 分割视图 */
	if (m_leftSplitter.CreateStatic(this, 1, 2) == FALSE) return FALSE;
	if (m_rightSplitter.CreateStatic(&m_leftSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_leftSplitter.IdFromRowCol(0, 1)) == FALSE) return FALSE;
	/* 关联视图对应的窗体 */
	if (m_leftSplitter.CreateView(0, 0, RUNTIME_CLASS(CUMDProp), CSize(0, 0), pContext) == FALSE) return FALSE;
	if (m_rightSplitter.CreateView(0, 0, RUNTIME_CLASS(CUMDShow), CSize(0, 0), pContext) == FALSE) return FALSE;
	if (m_rightSplitter.CreateView(1, 0, RUNTIME_CLASS(CUMDStat), CSize(0, 0), pContext) == FALSE) return FALSE;
	/* 视图大小 */
	m_leftSplitter.SetColumnInfo(0, (int)(0.18f * rc.Width()), 1);	/* property */
	m_rightSplitter.SetRowInfo(1, (int)(0.15f * rc.Height()), 1);	/* status */
	m_rightSplitter.SetRowInfo(0, (int)(0.7f * rc.Height()), 1);	/* show curve */
	/* 视图指针 */
	m_pLeft = (CFormView *)(m_leftSplitter.GetPane(0, 0));
	m_pBottom = (CFormView *)(m_rightSplitter.GetPane(1, 0));
	m_pTop = (CFormView *)(m_rightSplitter.GetPane(0, 0));
	
	pPropValue = m_pComm.m_pPropValue;								/* get prorperty pointer from CMyPComm */
	/* 创建子视图 */
	::PostMessageA(m_pLeft->GetSafeHwnd(), WM_USER_INIT, 0, 0);		/* send message to child view */
	::PostMessageA(m_pBottom->GetSafeHwnd(), WM_USER_INIT, 0, 0);
	::PostMessageA(m_pTop->GetSafeHwnd(), WM_USER_INIT, 0, 0);
	/* 创建定时器 */
	SetTimer(1u, 50u, NULL);										/* open timer1, cycle = 50ms */
	return TRUE;
}

/* 定时刷新状态栏 */
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != 1) {
		CFrameWndEx::OnTimer(nIDEvent);
		return;
	} else if (m_pComm.m_IsRx == TRUE) {	/* 当串口接收到数据时，更新bFlag变量，控制状态栏LED闪烁 */
		bFlag = TRUE;
		m_pComm.m_IsRx = FALSE;
	} else bFlag = FALSE;

	CFrameWndEx::OnTimer(nIDEvent);
}

afx_msg void CMainFrame::OnClose()
{
	KillTimer(1u);
	CFrameWndEx::OnClose();
}

/* Ribbon button更新事件 */
void CMainFrame::OnUpdateUMD(CCmdUI *pCmdUI)
{
	switch (pCmdUI->m_nID) {
	case ID_BTN_CNT: pCmdUI->Enable(TRUE); break;
	case ID_BTN_SAMPLE:
	case ID_BTN_CCW:
	case ID_BTN_STOP:
	case ID_BTN_CW:
		if (pPropValue[INDEX_CONNECT] == DEF_CONNECT) pCmdUI->Enable(TRUE);
		else pCmdUI->Enable(FALSE);
		break;
	case ID_COMB_CHANNEL:
	case ID_EDIT_COE:
	case ID_EDIT_OFFSET:
	case ID_BTN_OK:
	case ID_BTN_EXPORT:
	case ID_BTN_CFG:
		if (pPropValue[INDEX_CURVE] == DEF_OFF) pCmdUI->Enable(TRUE);
		else pCmdUI->Enable(FALSE);
		break;
	default: break;
	}
}

/* Ribbon status bar 更新事件 */
afx_msg void CMainFrame::OnUpdateStatusBar(CCmdUI *pCmdUI)
{
	if (bFlag == TRUE) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

/* Ribbon button响应事件 */
afx_msg void CMainFrame::OnCmdUMD(UINT nID)
{
	CComSel				dlg;
	CString				str;
	int					idx;
	float				tmp;

	switch (nID) {
	case ID_BTN_CNT:
		if (pPropValue[INDEX_CONNECT] == DEF_DISCONNECT) {
			if ((IDOK == dlg.DoModal()) && (0 == m_pComm.PortOpen(m_Port, m_Baud)))
			 	m_pComm.SendCommand(INDEX_CONNECT, DEF_CONNECT);
		} else {
			m_pComm.SendCommand(INDEX_CURVE, DEF_OFF);
			m_pComm.SendCommand(INDEX_CONNECT, DEF_DISCONNECT);
			Sleep(50u);
			pPropValue[INDEX_CONNECT] = DEF_DISCONNECT;
			::PostMessageA(GetSafeHwnd(), WM_USER_CHANGE_IMAGE, 0, (LPARAM)nID);
			::PostMessageA(GetSafeHwnd(), WM_USER_CHANGE_IMAGE, 0, (LPARAM)ID_BTN_SAMPLE);
			m_pComm.PortClose();
		}
		break;
	case ID_BTN_SAMPLE:
		if (pPropValue[INDEX_CURVE] == DEF_ON) {	/* 停止采集 */
			pPropValue[INDEX_CURVE] = DEF_OFF;
			((CUMDProp *)m_pLeft)->SetPropertyState(TRUE);
			::PostMessageA(GetSafeHwnd(), WM_USER_CHANGE_IMAGE, 0, (LPARAM)nID);
			m_pComm.SendCommand(INDEX_CURVE, (UINT32)(pPropValue[INDEX_CURVE]));
		} else {									/* 开始采集 */
			pPropValue[INDEX_CURVE] = DEF_ON;
			((CUMDProp *)m_pLeft)->SetPropertyState(FALSE);
			::PostMessageA(m_pTop->GetSafeHwnd(), WM_USER_CHANGECURVE, 0, 0);
			::PostMessageA(GetSafeHwnd(), WM_USER_CHANGE_IMAGE, 1, (LPARAM)nID);
		}
		break;
	case ID_BTN_CCW: 
		((CUMDProp *)m_pLeft)->SetPropertyState(FALSE);
		m_pComm.SendCommand(INDEX_RUNSTATE, (UINT32)INDEX_RUNSTATE_CCW);
		break;
	case ID_BTN_STOP:
		((CUMDProp *)m_pLeft)->SetPropertyState(TRUE);	
		m_pComm.SendCommand(INDEX_RUNSTATE, (UINT32)INDEX_RUNSTATE_STOP);
		break;
	case ID_BTN_CW:
		((CUMDProp *)m_pLeft)->SetPropertyState(FALSE);
		m_pComm.SendCommand(INDEX_RUNSTATE, (UINT32)INDEX_RUNSTATE_CW);
		break;
	case ID_BTN_OK:
		idx = ((CMFCRibbonComboBox *)DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMB_CHANNEL)))->GetCurSel();
		str = ((CMFCRibbonEdit *)DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_COE)))->GetEditText();
		tmp = (float)_ttof(str);
		((CUMDShow *)m_pTop)->coefficient[idx][0] = tmp;
		str.Format("%-5.2f", tmp);
		((CMFCRibbonEdit *)DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_COE)))->SetEditText(str);
		str = ((CMFCRibbonEdit *)DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_OFFSET)))->GetEditText();
		tmp = (float)_ttof(str);
		((CUMDShow *)m_pTop)->coefficient[idx][1] = tmp;
		str.Format("%-5.2f", tmp);
		((CMFCRibbonEdit *)DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_OFFSET)))->SetEditText(str);	
		::PostMessageA(((CUMDShow *)m_pTop)->GetSafeHwnd(), WM_USER_REDRAW, 0, 0);
		break;
	case ID_COMB_CHANNEL:
		idx = ((CMFCRibbonComboBox *)DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMB_CHANNEL)))->GetCurSel();
		str.Format("%-5.2f", ((CUMDShow *)m_pTop)->coefficient[idx][0]);
		((CMFCRibbonEdit *)DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_COE)))->SetEditText(str);
		str.Format("%-5.2f", ((CUMDShow *)m_pTop)->coefficient[idx][1]);
		((CMFCRibbonEdit *)DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_OFFSET)))->SetEditText(str);
		break;
	case ID_BTN_EXPORT:
		::PostMessageA(m_pTop->GetSafeHwnd(), WM_USER_EXPORT, 0, 0);
		break;
	case ID_BTN_CFG: break;
	default: break;
	}
}

/* 改变Ribbon button图片
 * wParam -> state		lParam -> Button ID
 */
afx_msg LRESULT CMainFrame::OnUserChangeImage(WPARAM wParam, LPARAM lParam)
{
	CMFCRibbonButton *pRibbonBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, m_wndRibbonBar.FindByID((UINT)lParam));
	
	switch ((UINT)lParam) {
	case ID_BTN_CNT:
		if (wParam) pRibbonBtn->SetImageIndex(4u, TRUE);		/* connect */
		else pRibbonBtn->SetImageIndex(3u, TRUE);	/* disconnect */
		m_wndRibbonBar.Invalidate();
		break;
	case ID_BTN_SAMPLE:
		if (wParam) pRibbonBtn->SetImageIndex(5u, TRUE);		/* start sample */
		else pRibbonBtn->SetImageIndex(6u, TRUE);	/* stop sample */
		m_wndRibbonBar.Invalidate();
		break;
	default: break;
	}
	return 0;
}
