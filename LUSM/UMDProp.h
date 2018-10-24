#pragma once

// CUMDProp ������ͼ
class CUMDProp : public CFormView
{
	DECLARE_DYNCREATE(CUMDProp)

protected:
	CUMDProp();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CUMDProp();

public:
	enum { IDD = IDD_UMDPROP };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CMFCPropertyGridCtrl*		m_pPropCtrl;
	CMFCPropertyGridProperty*	m_pGroupControl;
	CMFCPropertyGridProperty*	m_pGroupManual;
	CMFCPropertyGridProperty*	m_pGroupStep;
	CMFCPropertyGridProperty*	m_pGroupPosition;
	CMFCPropertyGridProperty*	m_pGroupPID;
	CMFCPropertyGridProperty*	m_pGroupFun;
	BOOL						m_Resize; /* �ػ��С */
	UINT8						m_curIndex;
	UINT8*						pRtnIndex;
	UINT8*						pRtnIndexBuf;
	UINT32*						pPropValue;

	void SetPropertyState(BOOL newState);

protected:
	afx_msg LRESULT OnUserInit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
};


