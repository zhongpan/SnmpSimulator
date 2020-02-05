// AgentSimulator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AgentSimulator.h"

#include "MainFrm.h"
#include "AgentSimulatorDoc.h"
#include "AgentSimulatorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorApp

BEGIN_MESSAGE_MAP(CAgentSimulatorApp, CWinApp)
	//{{AFX_MSG_MAP(CAgentSimulatorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorApp construction

CAgentSimulatorApp::CAgentSimulatorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAgentSimulatorApp object

CAgentSimulatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorApp initialization

BOOL CAgentSimulatorApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgentSimulatorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAgentSimulatorView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAgentSimulatorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorApp message handlers


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
// 	CRichEditCtrl* help = (CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT_HELP);
// 
// 	char helpstr[] = "使用提示：\n"
// 		             "1.AgentSimulator.ini\n"
// 					 "通过ip和port可以设置Agent绑定的地址；\n"
// 					 "通过read_community和write_community可以设置Agent的读写community。\n"
// 					 "修改AgentSimulator.ini后必须重启Agent方可生效。\n\n"
// 					 "2.mib.xml\n"
// 					 "(1)mib.xml定义Mib库的内容，有两类节点：标量和表，标量即顶层的leaf节点，表即table节点。"
// 					 "通过修改mib.xml可以扩充Mib库，使其接近实际设备。\n"
// 					 "(2)leaf包含oid、name、access、type、value五个属性，其中oid、access、type不能为空，"
// 					 "对于标量oid必须以'.0'结尾；type支持的类型有octets、oid、int、int32、ipaddr，cntr32、"
// 					 "uint32、gauge32、timeticks；access支持n、r、rw、rc；value可以是ascii字符串，也可以"
// 					 "是16进制字符串，16进制字符串必须以#开头，例如'#6161'表示'aa'，中间可以包含空白字符；\n"
// 					 "(3)leaf还包含tp和sr两个属性，tp表示是否支持透传，当tp值为true时表示支持，为false或不"
// 					 "存在时表示不支持，若支持则返回状态为19，set时可以返回数据；sr表示set时的返回数据，必须"
// 					 "采用16进制字符串，以'#'开头，sr属性在tp存在且为真时有效。透传支持只对标量有效。\n"
// 					 "(4)table包含oid、name两个属性，其中oid不能为空。另外table包含leaf和row两个子节点，"
// 					 "leaf子节点表示table的列，它的oid属性是其本身的oid；row子节点表示table的行，它的"
// 					 "index属性表示行的索引，不能为空，row的内容为列的值，用','分隔，必须和表的列数相同。\n"
// 					 "(5)修改mib.xml后必须重启Agent方可生效。\n\n"
// 					 "3.大家可以在网管上添加一款设备，ip设为本Agent的地址，然后校时或检测物理配置看看效果，"
// 					 "欢迎多提宝贵意见。:)";
// 	help->SetWindowText(helpstr);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
