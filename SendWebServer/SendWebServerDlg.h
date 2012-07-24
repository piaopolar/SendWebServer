
// SendWebServerDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CSendWebServerDlg dialog
class CSendWebServerDlg : public CDialog
{
// Construction
public:
	CSendWebServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SENDWEBSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSend();
	CEdit m_edtLog;
	CStatic m_staTip;
};
