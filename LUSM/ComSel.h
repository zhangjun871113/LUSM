#pragma once


// CComSel 对话框

class CComSel : public CDialogEx
{
	DECLARE_DYNAMIC(CComSel)

public:
	CComSel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CComSel();

// 对话框数据
	enum { IDD = IDD_COMSEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Baud;
	CComboBox m_Port;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void GetComList(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
