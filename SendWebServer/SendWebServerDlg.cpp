
	 // SendWebServerDlg.cpp : implementation file
#include "stdafx.h"

#include "wininet.h"
#include <string>

#include "SendWebServer.h"
#include "SendWebServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
static CEdit * s_pEdit = NULL;
static CStatic *s_pStaTip = NULL;

// ============================================================================
//    CAboutDlg dialog used for App About
// ============================================================================
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support

// ----------------------------------------------------------------------------
//    Implementation
// ----------------------------------------------------------------------------
protected:
	DECLARE_MESSAGE_MAP()
};

// ============================================================================
// ==============================================================================

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

// ============================================================================
// ==============================================================================
void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ============================================================================
//    CSendWebServerDlg dialog
// ============================================================================
CSendWebServerDlg::CSendWebServerDlg(CWnd *pParent /* NULL */ ) : CDialog(CSendWebServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// ============================================================================
// ==============================================================================
void CSendWebServerDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edtLog);
	DDX_Control(pDX, IDC_STA_TIP, m_staTip);
}

BEGIN_MESSAGE_MAP(CSendWebServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSendWebServerDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()

// ============================================================================
//    CSendWebServerDlg message handlers
// ============================================================================
BOOL CSendWebServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu. IDM_ABOUTBOX must be in the
	// system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (pSysMenu != NULL) {

		//~~~~~~~~~~~~~~~~~
		BOOL bNameValid;
		CString strAboutMenu;
		//~~~~~~~~~~~~~~~~~

		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);		// Set big icon
	SetIcon(m_hIcon, FALSE);	// Set small icon

	s_pEdit = &m_edtLog;
	s_pStaTip = &m_staTip;

	m_staTip.SetWindowText("请点击send按钮，测试并发送结果");

	// TODO: Add extra initialization here
	return TRUE;				// return TRUE unless you set the focus to a control
}

// ============================================================================
// ==============================================================================
void CSendWebServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {

		//~~~~~~~~~~~~~~~
		CAboutDlg dlgAbout;
		//~~~~~~~~~~~~~~~

		dlgAbout.DoModal();
	} else {
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ============================================================================
//    If you add a minimize button to your dialog, you will need the code below
//    to draw the icon. For MFC applications using the document/view model, this
//    is automatically done for you by the framework.
// ============================================================================
void CSendWebServerDlg::OnPaint()
{
	if (IsIconic()) {

		//~~~~~~~~~~~~~~
		CPaintDC dc(this);	// device context for painting
		//~~~~~~~~~~~~~~

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM> (dc.GetSafeHdc()), 0);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GetClientRect(&rect);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

// ============================================================================
//    The system calls this function to obtain the cursor to display while the
//    user drags the minimized window.
// ============================================================================
HCURSOR CSendWebServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

const int _MAX_STRING = 256;
const int _MAX_LONG_STRING = 1024;

// ============================================================================
// ==============================================================================

std::string GetCmdResult(std::string strCmd)
{
	if (s_pStaTip) {
		s_pStaTip->SetWindowText(strCmd.c_str());
	}

	//~~~~~~~~~~~~~~~~~~~
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	//~~~~~~~~~~~~~~~~~~~

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		return FALSE;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szFullCmd[_MAX_LONG_STRING] = "Cmd.exe /C ";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	strcat(szFullCmd, strCmd.c_str());

	//~~~~~~~~~~~~~~~~~~~
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//~~~~~~~~~~~~~~~~~~~

	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;	// 把创建进程的标准错误输出重定向到管道输入
	si.hStdOutput = hWrite; // 把创建进程的标准输出重定向到管道输入
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	// 关键步骤，CreateProcess函数参数意义请查阅MSDN
	if (!CreateProcess(NULL, szFullCmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return FALSE;
	}

	CloseHandle(hWrite);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szReadBuffer[_MAX_LONG_STRING];
	DWORD bytesRead;
	std::string strResult;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	while (true) {
		if (!ReadFile(hRead, szReadBuffer, sizeof(szReadBuffer) - 1, &bytesRead, NULL) || bytesRead == 0) {
			break;
		}

		szReadBuffer[bytesRead] = 0;

		strResult += szReadBuffer;

		if (s_pEdit) {
			s_pEdit->SetWindowText(strResult.c_str());
			s_pEdit->LineScroll(s_pEdit->GetLineCount());
		}
	}

	CloseHandle(hRead);
	return strResult;
}

// ============================================================================
// ==============================================================================
void ReplaceStdString(std::string &str, const std::string &strSrc, const std::string &strDest)
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	CString cstr = str.c_str();
	//~~~~~~~~~~~~~~~~~~~~~~~

	cstr.Replace(strSrc.c_str(), strDest.c_str());

	str = cstr.GetBuffer(0);
}

// ============================================================================
// ==============================================================================
void Send(void *)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	TCHAR szServer[_MAX_STRING] = _T("haha.91.com");
	TCHAR szAction[_MAX_STRING] = _T("zferrormsg/msg.php");
	std::string strFmtData;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	strFmtData = "client=";
	strFmtData += "testSever";
	strFmtData += "&msg=";
	strFmtData += GetCmdResult("ping 216.93.179.133");
	strFmtData += GetCmdResult("ping 216.93.179.133 -l 1002 -n 20");
	strFmtData += GetCmdResult("tracert 216.93.179.133");
	ReplaceStdString(strFmtData, "\r", "");
	ReplaceStdString(strFmtData, "\n", "<br>");

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static TCHAR hdrs[] = _T("Content-Type: application/x-www-form-urlencoded");
	static const TCHAR *accept = _T("Accept: */*");
	HINTERNET hSession = InternetOpen("MyAgent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (hSession) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		HINTERNET hConnect = InternetConnect(hSession, szServer, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL,
											 INTERNET_SERVICE_HTTP, 0, 1);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (hConnect) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", szAction, NULL, NULL, &accept, 0, 1);
			BOOL b = HttpSendRequest(hRequest, hdrs, strlen(hdrs), (LPVOID) strFmtData.c_str(), strFmtData.length());
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		}
	}

	if (s_pStaTip) {
		s_pStaTip->SetWindowText("open url http://haha.91.com/zferrormsg/zfmsg.php");
	}

	ShellExecute(NULL, NULL, "http://haha.91.com/zferrormsg/zfmsg.php", NULL, NULL, SW_SHOW);
}

// ============================================================================
// ==============================================================================
void CSendWebServerDlg::OnBnClickedButtonSend()
{
	_beginthread(Send, 0, NULL);
}

#if 0

// ============================================================================
//    1.重定向子进程的输入输出：
// ============================================================================
BOOL CConsoleView::CreateConsoleRedirect()
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	SECURITY_ATTRIBUTES saAttr;
	BOOL fSuccess;
	//~~~~~~~~~~~~~~~~~~~~~~~

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);

	saAttr.bInheritHandle = TRUE;

	saAttr.lpSecurityDescriptor = NULL;

	// 重定向子进程的标准输出... 保存当前标准输出的句柄
	hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// 为子进程的标准输出创建一个管道
	if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
		TRACE0(_T("Stdout   pipe   creation   failed\n "));

		return FALSE;
	}

	// 设置一个写句柄到管道，使之成为标准输出
	if (!SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr)) {
		TRACE0(_T("Redirecting   STDOUT   failed\n "));

		return FALSE;
	}

	// 创建不可继承的读句柄并关闭可继承的读句柄
	fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd, GetCurrentProcess(), &hChildStdoutRdDup, 0, FALSE,
							   DUPLICATE_SAME_ACCESS);

	if (!fSuccess) {
		TRACE0(_T("DuplicateHandle   failed\n "));

		return FALSE;
	}

	CloseHandle(hChildStdoutRd);

	// 重定向子进程的标准输入... 保存当前标准输入的句柄
	hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

	// 为子进程的标准输入创建一个管道
	if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) {
		TRACE0(_T("Stdin   pipe   creation   failed\n "));

		return FALSE;
	}

	// 设置一个写句柄到管道，使之成为标准输入
	if (!SetStdHandle(STD_INPUT_HANDLE, hChildStdinRd)) {
		TRACE0(_T("Redirecting   Stdin   failed\n "));

		return FALSE;
	}

	// 复制写句柄到管道，这样它就不可继承 Duplicate the write handle to the
	// pipe so it is not inherited.
	fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(), &hChildStdinWrDup, 0, FALSE,	// not
																														///inherited
																														///

	DUPLICATE_SAME_ACCESS);

	if (!fSuccess) {
		TRACE0(_T("DuplicateHandle   failed\n "));

		return FALSE;
	}

	CloseHandle(hChildStdinWr);

	// 创建子进程
	if (!CreateChildProcess(dwProcessId)) {
		TRACE0(_T("CreateChildProcess   failed\n "));

		return FALSE;
	}

	// 子进程创建完毕，重置系统标准输入输出
	if (!SetStdHandle(STD_INPUT_HANDLE, hSaveStdin)) {
		TRACE0(_T("Re-redirecting   Stdin   failed\n "));

		return FALSE;
	}

	if (!SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout)) {
		TRACE0(_T("Re-redirecting   Stdout   failed\n "));

		return FALSE;
	}

	m_pReadThread = AfxBeginThread((AFX_THREADPROC) ReadPipeThreadProc, (LPVOID) this);

	if (!m_pReadThread) {
		TRACE0(_T("Cannot   start   read-redirect   thread!\n "));

		return FALSE;
	}

	return TRUE;
}

// ============================================================================
//    2．创建子进程：
// ============================================================================
BOOL CConsoleView::CreateChildProcess(DWORD &dwProcessId)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

	siStartInfo.cb = sizeof(STARTUPINFO);

	siStartInfo.dwFlags = STARTF_USESTDHANDLES;

	siStartInfo.hStdInput = hChildStdinRd;

	siStartInfo.hStdOutput = hChildStdoutWr;

	siStartInfo.hStdError = hChildStdoutWr;

	//~~~~~~~~~~~~~~~~~~~~~~
	TCHAR shellCmd[_MAX_PATH];
	//~~~~~~~~~~~~~~~~~~~~~~

	if (!GetEnvironmentVariable(_T("ComSpec "), shellCmd, _MAX_PATH)) {
		return FALSE;
	}

#ifdef _UNICODE
	_tcscat(shellCmd, _T("   /U "));

#else
	_tcscat(shellCmd, _T("   /A "));
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 创建子进程
	BOOL ret = CreateProcess(NULL, shellCmd,	// applicatin name
							 NULL,				// process security attributes
							 NULL,				// primary thread security attributes
							 TRUE,				// handles are inherited
							 DETACHED_PROCESS,	// creation flags
							 NULL,				// use parent 's environment
							 NULL,				// use parent 's current directory
							 &siStartInfo,		// STARTUPINFO pointer
							 &piProcInfo);		// receives PROCESS_INFORMATION
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (ret) {
		dwProcessId = piProcInfo.dwProcessId;
	}

	return ret;
}

// ============================================================================
//    3．写入到管道：
// ============================================================================
void CConsoleView::WriteToPipe(LPCTSTR line)
{
	//~~~~~~~~~~~~
	DWORD dwWritten;
	//~~~~~~~~~~~~

	WriteFile(hChildStdinWrDup, line, _tcslen(line) * sizeof(TCHAR), &dwWritten, NULL);
}

// ============================================================================
//    4．读管道线程函数：
// ============================================================================
UINT CConsoleView::ReadPipeThreadProc(LPVOID pParam)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	DWORD dwRead;
	TCHAR chBuf[BUFSIZE];
	CConsoleView *pView = (CConsoleView *)pParam;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TRACE0(_T("ReadPipe   Thread   begin   run\n "));

	for (;;) {
		if (!ReadFile(pView->hChildStdoutRdDup, chBuf, BUFSIZE, &dwRead, NULL) || dwRead == 0) {
			break;
		}

		chBuf[dwRead / sizeof(TCHAR)] = _T('\0 ');

		pView->AddTexts(chBuf);

		pView->m_nLength = pView->GetEditCtrl().SendMessage(WM_GETTEXTLENGTH);
	}

	CloseHandle(pView->hChildStdinRd);

	CloseHandle(pView->hChildStdoutWr);

	CloseHandle(pView->hChildStdinWrDup);

	CloseHandle(pView->hChildStdoutRdDup);

	pView->m_pReadThread = NULL;

	pView->dwProcessId = DWORD(-1);

	pView->PostMessage(WM_CLOSE);

	return 1;
}

// ============================================================================
//    5．退出程序时清除进程、线程：
// ============================================================================
void CConsoleView::OnDestroy()
{
	if (dwProcessId != DWORD(-1)) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (hProcess) {
			TerminateProcess(hProcess, 0);

			CloseHandle(hProcess);
		}
	}

	if (m_pReadThread) {
		TerminateThread(m_pReadThread->m_hThread, 0);

		delete m_pReadThread;
	}

	CEditView::OnDestroy();
}
#endif
