#pragma once


// CComSel �Ի���

class CComSel : public CDialogEx
{
	DECLARE_DYNAMIC(CComSel)

public:
	CComSel(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CComSel();

// �Ի�������
	enum { IDD = IDD_COMSEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Baud;
	CComboBox m_Port;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void GetComList(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
