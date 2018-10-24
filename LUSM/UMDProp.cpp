// UMDProp.cpp : 实现文件
//

#include "stdafx.h"
#include "LUSM.h"
#include "MainFrm.h"
#include "UMDProp.h"
#include "gdef.h"

// CUMDProp
IMPLEMENT_DYNCREATE(CUMDProp, CFormView)

CUMDProp::CUMDProp()
	: CFormView(CUMDProp::IDD)
{
	m_Resize = FALSE;
	m_curIndex = 0;
	pRtnIndex = NULL;
	pRtnIndexBuf = NULL;
	pPropValue = NULL;
}

CUMDProp::~CUMDProp()
{
	if (m_pPropCtrl != NULL) {
		delete m_pPropCtrl;
		m_pPropCtrl = NULL;
	}
}

void CUMDProp::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUMDProp, CFormView)
	ON_MESSAGE(WM_USER_INIT, &CUMDProp::OnUserInit)
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, CUMDProp::OnPropertyChanged)
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUMDProp 诊断

#ifdef _DEBUG
void CUMDProp::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUMDProp::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CUMDProp 消息处理程序

/* 初始化，消息由CMainFrame发送 */
afx_msg LRESULT CUMDProp::OnUserInit(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty*	prop;

	CRect	rc;
	GetClientRect(rc);

	m_pPropCtrl = new CMFCPropertyGridCtrl;	/* 设置空间风格 */
	m_pPropCtrl->Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_UMDPROP);
	m_pPropCtrl->EnableHeaderCtrl(TRUE);
	m_pPropCtrl->EnableDescriptionArea(TRUE);
	m_pPropCtrl->SetVSDotNetLook(FALSE);
	m_pPropCtrl->MarkModifiedProperties(TRUE);
	m_pPropCtrl->SetAlphabeticMode(FALSE);
	m_pPropCtrl->ModifyStyleEx(0, WS_EX_TRANSPARENT, 0);
	
	CFont *pFont = m_pPropCtrl->GetFont();	/* 设置控件字体大小 */
	LOGFONT	logfont;
	pFont->GetLogFont(&logfont);
	logfont.lfHeight = 16;
	logfont.lfCharSet = ANSI_CHARSET;
	strcpy(logfont.lfFaceName, "Times New Rome");
	pFont->Detach();
	pFont->CreateFontIndirectA(&logfont);
	m_pPropCtrl->SetFont(pFont, TRUE);
	pFont->Detach();

	m_pPropCtrl->SetRedraw(FALSE);	/* 禁止控件重绘 */

	m_pGroupControl = new CMFCPropertyGridProperty("Control Parameters");	/* Group->Control */
	prop = new CMFCPropertyGridProperty("Mode", "", "Run mode", INDEX_RUNMODE);
	prop->AddOption("0-Idle");
	prop->AddOption("1-Manual");
	prop->AddOption("2-Step");
	prop->AddOption("3-Position");
	prop->AllowEdit(FALSE);
	m_pGroupControl->AddSubItem(prop);
	prop = new CMFCPropertyGridProperty("Method", "", "Run method", INDEX_METHOD);
	prop->AddOption("0-None");
	prop->AddOption("1-PID");
	prop->AddOption("2-Follow Sin");
	prop->AllowEdit(FALSE);
	m_pGroupControl->AddSubItem(prop);

	m_pGroupManual = new CMFCPropertyGridProperty("Manual Parameters");		/* Group->Manual */
	m_pGroupManual->AddSubItem(new CMFCPropertyGridProperty("Frequency/kHz", "", "set frequency", INDEX_FREQ));
	m_pGroupManual->AddSubItem(new CMFCPropertyGridProperty("Duty A/%", "", "set duty A", INDEX_DUTYA));
	m_pGroupManual->AddSubItem(new CMFCPropertyGridProperty("Duty B/%", "", "set duty B", INDEX_DUTYB));
	m_pGroupManual->AddSubItem(new CMFCPropertyGridProperty("Phase/°", "", "set phase difference", INDEX_PHASE));
	prop = new CMFCPropertyGridProperty("Duty Equal", "", "duty A equals duty B", INDEX_DUTYEQUAL);
	prop->AddOption("YES");
	prop->AddOption("NO");
	prop->AllowEdit(FALSE);
	m_pGroupManual->AddSubItem(prop);

	m_pGroupStep = new CMFCPropertyGridProperty("Step Parameters");		/* Group->Step */
	m_pGroupStep->AddSubItem(new CMFCPropertyGridProperty("Number", "", "number of drive signal", INDEX_NUMBER));
	m_pGroupStep->AddSubItem(new CMFCPropertyGridProperty("Group", "", "group of drive signal", INDEX_GROUP));
	m_pGroupStep->AddSubItem(new CMFCPropertyGridProperty("Interval", "", "time interval of drive signal", INDEX_INTERVAL));
	m_pGroupStep->Expand(FALSE);

	m_pGroupPosition = new CMFCPropertyGridProperty("Position Parameters");	/* Group->Position */
	m_pGroupPosition->AddSubItem(new CMFCPropertyGridProperty("Position", "", "set position", INDEX_POSITION));
	m_pGroupPosition->AddSubItem(new CMFCPropertyGridProperty("Speed", "", "set speed", INDEX_SPEED));
	m_pGroupPosition->AddSubItem(new CMFCPropertyGridProperty("ZP", "", "positive offset", INDEX_ZP));
	m_pGroupPosition->AddSubItem(new CMFCPropertyGridProperty("ZN", "", "negative offset", INDEX_ZN));
	m_pGroupPosition->Expand(FALSE);

	m_pGroupPID = new CMFCPropertyGridProperty("PID Parameters");	/* Group->PID */
	m_pGroupPID->AddSubItem(new CMFCPropertyGridProperty("P", "", "PID parameters - P", INDEX_PID_P));
	m_pGroupPID->AddSubItem(new CMFCPropertyGridProperty("I", "", "PID parameters - I", INDEX_PID_I));
	m_pGroupPID->AddSubItem(new CMFCPropertyGridProperty("D", "", "PID parameters - D", INDEX_PID_D));
	m_pGroupPID->AddSubItem(new CMFCPropertyGridProperty("μ", "", "PID parameters - μ", INDEX_PID_MU));
	m_pGroupPID->AddSubItem(new CMFCPropertyGridProperty("λ", "", "PID parameters - λ", INDEX_PID_LAMBDA));
	m_pGroupPID->Expand(FALSE);

	m_pGroupFun = new CMFCPropertyGridProperty("Fun Parameters");	/* Group->Fun */
	m_pGroupFun->AddSubItem(new CMFCPropertyGridProperty("Fun0", "", "Functional parameters-A", INDEX_FUN_PARA0));
	m_pGroupFun->AddSubItem(new CMFCPropertyGridProperty("Fun1", "", "Functional parameters-F", INDEX_FUN_PARA1));
	m_pGroupFun->Expand(FALSE);

	m_pPropCtrl->AddProperty(m_pGroupControl);	/* Add property */
	m_pPropCtrl->AddProperty(m_pGroupManual);
	m_pPropCtrl->AddProperty(m_pGroupStep);
	m_pPropCtrl->AddProperty(m_pGroupPosition);
	m_pPropCtrl->AddProperty(m_pGroupPID);
	m_pPropCtrl->AddProperty(m_pGroupFun);

	m_pPropCtrl->SetRedraw(TRUE);	/* 重绘控件 */
	Invalidate(TRUE);
	
	CMainFrame*	pManiFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;	/* 获取其他文件中的变量地址 */
	pPropValue = pManiFrame->pPropValue;
	pRtnIndex = (UINT8 *)(&(pManiFrame->m_pComm.m_RtnIndex));
	pRtnIndexBuf = pManiFrame->m_pComm.m_pRtnIndexBuf;

	m_Resize = TRUE;
	SetTimer(2u, 50u, NULL);
	return 0;
}

void CUMDProp::OnDestroy()
{	
	KillTimer(2u);
	CFormView::OnDestroy();
}

/* 工作区大小改变，重绘控件 */
void CUMDProp::OnSize(UINT nType, int cx, int cy)
{
	if (m_Resize == TRUE) {
		CRect	rc;
		GetClientRect(rc);
		m_pPropCtrl->MoveWindow(&rc)	;
	}
	CFormView::OnSize(nType, cx, cy);
}

/* 属性值发生改变，向串口发送相关命令 */
afx_msg LRESULT CUMDProp::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty*	prop = (CMFCPropertyGridProperty *)lParam;
	UINT8						opt	= (UINT8)(prop->GetData());
	CString						str = prop->GetValue();
	UINT32						uVal;
	
	// 发送设定的参数
	switch (opt) {
	case INDEX_RUNMODE:
		if (str[0] == '1') {		/* Manual */
			uVal = MODE_MANUAL;
			m_pGroupManual->Expand(TRUE);
			m_pGroupStep->Expand(FALSE);
			m_pGroupPosition->Expand(FALSE);	
		} else if (str[0] == '2') {	/* Step */
			uVal = MODE_PULSE;
			m_pGroupManual->Expand(FALSE);
			m_pGroupStep->Expand(TRUE);
			m_pGroupPosition->Expand(FALSE);
		}else if (str[0] == '3') {	/* Position */
			uVal = MODE_POSITION;
			m_pGroupManual->Expand(FALSE);
			m_pGroupStep->Expand(FALSE);
			m_pGroupPosition->Expand(TRUE);
		} else uVal = MODE_IDLE;	/* Idle */
		break;
	case INDEX_METHOD: 
		if (str[0] == '1') uVal = INDEX_METHOD_PID;
		else if (str[0] == '2') uVal = INDEX_METHOD_FOLLOW_SIN;
		else uVal = INDEX_METHOD_NONE;
		break;		 
	case INDEX_FREQ: uVal = (UINT32)(INT32)(1000 * atof(str)); break;
	case INDEX_DUTYA:
	case INDEX_DUTYB:
	case INDEX_ZP:
	case INDEX_ZN:
	case INDEX_PHASE: uVal = (UINT32)(INT32)(10 * atof(str)); break;
	case INDEX_DUTYEQUAL: 
		if (str[0] == 'Y') uVal = DEF_ON;
		else uVal = DEF_OFF;
		break;
	case INDEX_NUMBER:
	case INDEX_GROUP:
	case INDEX_INTERVAL:
	case INDEX_SPEED: uVal = (UINT32)atoi(str); break;
	case INDEX_POSITION:  uVal = (UINT32)(INT32)(1000 * atof(str)); break;
	case INDEX_PID_P:
	case INDEX_PID_I:
	case INDEX_PID_D:
	case INDEX_PID_MU:
	case INDEX_PID_LAMBDA:
	case INDEX_FUN_PARA0:
	case INDEX_FUN_PARA1: uVal = (UINT32)(INT32)(1000 * atof(str)); break;	
	default: break;
	}
	// COM Send Command

	CMainFrame*	pManiFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;	/* 获取其他文件中的变量地址 */
	pManiFrame->m_pComm.SendCommand((UINT8)opt, (UINT32)uVal);

	return 0;
}

void CUMDProp::OnTimer(UINT_PTR nIDEvent)
{
	if ((nIDEvent != 2) || (pRtnIndex == NULL)) {CFormView::OnTimer(nIDEvent); return;}

	UINT8						opt;
	CString						str;
	CMFCPropertyGridProperty*	prop;

	while (m_curIndex != (*pRtnIndex)) {
		opt = pRtnIndexBuf[m_curIndex];
		if (m_curIndex >= (INDEX_MAX - 1)) m_curIndex = 0;
		else m_curIndex++;

		if ((opt == INDEX_CONNECT) && (pPropValue[INDEX_CONNECT] == DEF_CONNECT)) {
			::PostMessageA(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_USER_CHANGE_IMAGE, 1, (LPARAM)ID_BTN_CNT);
			continue;
		}

		prop = m_pPropCtrl->FindItemByData((DWORD)opt);
		if (prop == NULL) continue;

		switch (opt) {
		case INDEX_RUNMODE:
			switch (pPropValue[INDEX_RUNMODE]) {
			case MODE_MANUAL: str = "1-Manual"; break;
			case MODE_PULSE: str = "2-Step"; break;
			case MODE_POSITION: str = "3-Position"; break;
			default: str = "0-Idle"; break;
			}
			break;
		case INDEX_FREQ: str.Format("%-4.2f", 0.001 * (INT32)pPropValue[opt]); break;
		case INDEX_DUTYA:
		case INDEX_DUTYB:
		case INDEX_ZP:
		case INDEX_ZN:
		case INDEX_PHASE: str.Format("%-3.1f", 0.1 * (INT32)pPropValue[opt]); break;
		case INDEX_DUTYEQUAL:
			if (pPropValue[opt]) str = "YES";
			else str = "NO";
			break;
		case INDEX_NUMBER:
		case INDEX_GROUP:
		case INDEX_INTERVAL:
		case INDEX_SPEED: str.Format("%-d", (INT32)pPropValue[opt]); break;
		case INDEX_POSITION: str.Format("%-3.3f", 0.001 * (INT32)pPropValue[opt]); break;
		case INDEX_METHOD:
			if (pPropValue[INDEX_METHOD] == INDEX_METHOD_PID) str = "1-PID";
			else if (pPropValue[INDEX_METHOD] == INDEX_METHOD_FOLLOW_SIN) str = "2-Follow Sin";
			else str = "0-None";
			break;
		case INDEX_PID_P:
		case INDEX_PID_I:
		case INDEX_PID_D:
		case INDEX_PID_MU:
		case INDEX_PID_LAMBDA:
		case INDEX_FUN_PARA0:
		case INDEX_FUN_PARA1: str.Format("%-3.3f", 0.001 * (INT32)pPropValue[opt]); break;
		default: break;
		}

		prop->SetOriginalValue(COleVariant(str));
		prop->ResetOriginalValue();
	}

	CFormView::OnTimer(nIDEvent);
}

/* set MODE, METHOD state */
void CUMDProp::SetPropertyState(BOOL newState)
{
	if (newState == TRUE) {
		((CMFCPropertyGridProperty *)m_pGroupControl->GetSubItem(0))->Enable(TRUE);
		((CMFCPropertyGridProperty *)m_pGroupControl->GetSubItem(1))->Enable(TRUE);
	} else {
		((CMFCPropertyGridProperty *)m_pGroupControl->GetSubItem(0))->Enable(FALSE);
		((CMFCPropertyGridProperty *)m_pGroupControl->GetSubItem(1))->Enable(FALSE);
	}
}
