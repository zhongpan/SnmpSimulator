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
// 	char helpstr[] = "ʹ����ʾ��\n"
// 		             "1.AgentSimulator.ini\n"
// 					 "ͨ��ip��port��������Agent�󶨵ĵ�ַ��\n"
// 					 "ͨ��read_community��write_community��������Agent�Ķ�дcommunity��\n"
// 					 "�޸�AgentSimulator.ini���������Agent������Ч��\n\n"
// 					 "2.mib.xml\n"
// 					 "(1)mib.xml����Mib������ݣ�������ڵ㣺�����ͱ������������leaf�ڵ㣬��table�ڵ㡣"
// 					 "ͨ���޸�mib.xml��������Mib�⣬ʹ��ӽ�ʵ���豸��\n"
// 					 "(2)leaf����oid��name��access��type��value������ԣ�����oid��access��type����Ϊ�գ�"
// 					 "���ڱ���oid������'.0'��β��type֧�ֵ�������octets��oid��int��int32��ipaddr��cntr32��"
// 					 "uint32��gauge32��timeticks��access֧��n��r��rw��rc��value������ascii�ַ�����Ҳ����"
// 					 "��16�����ַ�����16�����ַ���������#��ͷ������'#6161'��ʾ'aa'���м���԰����հ��ַ���\n"
// 					 "(3)leaf������tp��sr�������ԣ�tp��ʾ�Ƿ�֧��͸������tpֵΪtrueʱ��ʾ֧�֣�Ϊfalse��"
// 					 "����ʱ��ʾ��֧�֣���֧���򷵻�״̬Ϊ19��setʱ���Է������ݣ�sr��ʾsetʱ�ķ������ݣ�����"
// 					 "����16�����ַ�������'#'��ͷ��sr������tp������Ϊ��ʱ��Ч��͸��֧��ֻ�Ա�����Ч��\n"
// 					 "(4)table����oid��name�������ԣ�����oid����Ϊ�ա�����table����leaf��row�����ӽڵ㣬"
// 					 "leaf�ӽڵ��ʾtable���У�����oid�������䱾���oid��row�ӽڵ��ʾtable���У�����"
// 					 "index���Ա�ʾ�е�����������Ϊ�գ�row������Ϊ�е�ֵ����','�ָ�������ͱ��������ͬ��\n"
// 					 "(5)�޸�mib.xml���������Agent������Ч��\n\n"
// 					 "3.��ҿ��������������һ���豸��ip��Ϊ��Agent�ĵ�ַ��Ȼ��Уʱ�����������ÿ���Ч����"
// 					 "��ӭ���ᱦ�������:)";
// 	help->SetWindowText(helpstr);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
