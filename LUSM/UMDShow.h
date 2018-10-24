#pragma once
#include "gdef.h"
#include "tchart.h"

// CUMDShow 窗体视图

class CUMDShow : public CFormView
{
	DECLARE_DYNCREATE(CUMDShow)

protected:
	CUMDShow();           // 动态创建所使用的受保护的构造函数
	virtual ~CUMDShow();

public:
	enum { IDD = IDD_UMDSHOW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	static HANDLE			hEvent;
	static CWinThread*		hThread;
	static long				preIndex;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTchart					m_TChart;
	BOOL					m_Resize;
	static int				m_curChan;
	static long				m_curIndex;
	static long				m_totalIndex;
	static float			coefficient[CHANNEL_MAX][2];
	static COleSafeArray	XValues;
	static COleSafeArray	YValues[CHANNEL_MAX];
	static long*			pChannel[CHANNEL_MAX];
	static long*			pSampleIndex;
	static UINT32*			pPropValue;

protected:
	afx_msg LRESULT OnUserInit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUserRedraw(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserChangecurve(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserExport(WPARAM wParam, LPARAM lParam);
	static UINT DataProcessingProc(LPVOID lPvoid);
	static void MethodOpenLoop(long sampleIndex);
};


