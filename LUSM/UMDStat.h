#pragma once

#include "gdef.h"

// CUMDStat 窗体视图
class CUMDStat : public CFormView
{
	DECLARE_DYNCREATE(CUMDStat)

protected:
	CUMDStat();           // 动态创建所使用的受保护的构造函数
	virtual ~CUMDStat();

public:
	enum { IDD = IDD_UMDSTAT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl	m_List;
	BOOL		m_Resize;
	UINT32*		pPropValue;
	float*		pCoefficient[CHANNEL_MAX];

protected:
	afx_msg LRESULT CUMDStat::OnUserInit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


