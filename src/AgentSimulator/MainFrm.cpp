// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AgentSimulator.h"
#include "AgentWorkThread.h"

#include "MainFrm.h"

#include "TrapGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_AGENT_START, OnAgentStart)
	ON_COMMAND(ID_AGENT_STOP, OnAgentStop)
	ON_UPDATE_COMMAND_UI(ID_AGENT_START, OnUpdateAgentStart)
	ON_UPDATE_COMMAND_UI(ID_AGENT_STOP, OnUpdateAgentStop)
	ON_COMMAND(ID_AGENT_RESTART, OnAgentRestart)
	ON_UPDATE_COMMAND_UI(ID_AGENT_RESTART, OnUpdateAgentRestart)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOOL_TRAPGEN, OnToolTrapgen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bStarted = FALSE;
	m_agent    = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnAgentStart() 
{
	if( m_agent == NULL )
	{
		m_agent = new CAgentWorkThread(this);
		m_agent->CreateThread();
	}

	if( m_agent )
		m_agent->PostThreadMessage(MSG_START_AGENT, 0, 0);

	m_bStarted = TRUE;
}

void CMainFrame::OnAgentStop()
{
	if( m_agent )
		m_agent->PostThreadMessage(MSG_STOP_AGENT, 0, 0);

	m_bStarted = FALSE;
}

void CMainFrame::OnUpdateAgentStart(CCmdUI* pCmdUI) 
{
	if( m_bStarted )
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateAgentStop(CCmdUI* pCmdUI) 
{
	if( !m_bStarted )
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnAgentRestart() 
{
	if( m_agent && m_bStarted )
		m_agent->PostThreadMessage(MSG_RESTART_AGENT, 0, 0);
}

void CMainFrame::OnUpdateAgentRestart(CCmdUI* pCmdUI) 
{
	if( !m_bStarted )
		pCmdUI->Enable(FALSE);	
}

void CMainFrame::OnClose() 
{
	if( m_bStarted )
	{
		AfxMessageBox("service already start, please stop first");
		return;
	}
	
	CFrameWnd::OnClose();
}

void CMainFrame::OnToolTrapgen() 
{
	CTrapGenDlg trapGenDlg(m_agent);
	trapGenDlg.DoModal();
}
