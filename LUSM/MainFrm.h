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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ChildView.h"
#include "gdef.h"
#include "MyPComm.h"

class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar				m_wndRibbonBar;
	CMFCRibbonStatusBar			m_wndStatusBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages			m_PanelImages;
	CChildView					m_wndView;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);

	DECLARE_MESSAGE_MAP()

public:	
	CMyPComm		m_pComm;			/* Com config */
	HICON			m_IconLED;			/* LED HICON */
	CSplitterWnd	m_leftSplitter;		/* ����ָ� */
	CSplitterWnd	m_rightSplitter;
	CFormView*		m_pLeft;			/* ��ͼָ�� */
	CFormView*		m_pTop;
	CFormView*		m_pBottom;
	int				m_Port;				/* ���ڶ˿ں� */
	int				m_Baud;				/* ���ڲ����� */
	UINT32*			pPropValue;			/* ����ָ�� */
	BOOL			bFlag;				/* �Ƿ�ʹ��statusBar */

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);	/* ��д�ͻ��� */
	afx_msg void OnClose();	/* �رճ��� */
	afx_msg void OnTimer(UINT_PTR nIDEvent);	/* ������ʱ��������� */
	afx_msg void OnUpdateUMD(CCmdUI *pCmdUI);	/* Ribbon UI���� */
	afx_msg void OnUpdateStatusBar(CCmdUI *pCmdUI);	/* Ribbon StatusBar UI ���� */
	afx_msg void OnCmdUMD(UINT nID);	/* Ribbon buttton ��Ӧ */
	afx_msg LRESULT OnUserChangeImage(WPARAM wParam, LPARAM lParam); /* �ı�Ribbon button ͼƬ���� */
};


