
// Page_SMDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CPage_SMDlg 对话框
class CPage_SMDlg : public CDialogEx
{
// 构造
public:
	CPage_SMDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PAGE_SM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void Random();
	static UINT threadFIFO(LPVOID lpParam);
	static UINT threadLRU(LPVOID lpParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton2();
//	CListBox m_list3;
	CListBox m_list1;
	CListBox m_list2;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	CListBox m_list3;
	CListBox m_list5;
	afx_msg void OnEnKillfocusEdit2();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit5();
	CListCtrl m_ctllist;
};
