// AgentSimulatorView.cpp : implementation of the CAgentSimulatorView class
//

#include "stdafx.h"
#include "AgentSimulator.h"

#include "AgentSimulatorDoc.h"
#include "AgentSimulatorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorView

IMPLEMENT_DYNCREATE(CAgentSimulatorView, CRichEditView)

BEGIN_MESSAGE_MAP(CAgentSimulatorView, CRichEditView)
	//{{AFX_MSG_MAP(CAgentSimulatorView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorView construction/destruction

CAgentSimulatorView::CAgentSimulatorView()
{
	// TODO: add construction code here

}

CAgentSimulatorView::~CAgentSimulatorView()
{
}

BOOL CAgentSimulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);

}

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorView drawing

void CAgentSimulatorView::OnDraw(CDC* pDC)
{
	CAgentSimulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorView diagnostics

#ifdef _DEBUG
void CAgentSimulatorView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CAgentSimulatorView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CAgentSimulatorDoc* CAgentSimulatorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgentSimulatorDoc)));
	return (CAgentSimulatorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAgentSimulatorView message handlers

void CAgentSimulatorView::OnInitialUpdate() 
{
	CRichEditView::OnInitialUpdate();

	GetRichEditCtrl().SetReadOnly();
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);

	//cf.dwMask|=CFM_BOLD;
	//cf.dwEffects|=CFE_BOLD;//设置粗体，取消用cf.dwEffects&=~CFE_BOLD;

	//cf.dwMask|=CFM_ITALIC;
	//cf.dwEffects|=CFE_ITALIC;//设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;

	//cf.dwMask|=CFM_UNDERLINE;
	//cf.dwEffects|=CFE_UNDERLINE;//设置下划线，取消用cf.dwEffects&=~CFE_UNDERLINE;

	cf.dwMask |= CFM_COLOR;
	cf.crTextColor = RGB(0,0,255);//设置颜色

	cf.dwMask |= CFM_SIZE;
	cf.yHeight = 200;//设置高度

	cf.dwMask |= CFM_FACE;
	strcpy(cf.szFaceName ,_T("宋体"));//设置字体
	GetRichEditCtrl().SetDefaultCharFormat(cf);
}
