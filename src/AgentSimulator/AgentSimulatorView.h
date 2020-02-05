// AgentSimulatorView.h : interface of the CAgentSimulatorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTSIMULATORVIEW_H__F27FA7E6_9744_4E39_931A_C103477F8151__INCLUDED_)
#define AFX_AGENTSIMULATORVIEW_H__F27FA7E6_9744_4E39_931A_C103477F8151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAgentSimulatorView : public CRichEditView
{
protected: // create from serialization only
	CAgentSimulatorView();
	DECLARE_DYNCREATE(CAgentSimulatorView)

// Attributes
public:
	CAgentSimulatorDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentSimulatorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgentSimulatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAgentSimulatorView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgentSimulatorView.cpp
inline CAgentSimulatorDoc* CAgentSimulatorView::GetDocument()
   { return (CAgentSimulatorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGENTSIMULATORVIEW_H__F27FA7E6_9744_4E39_931A_C103477F8151__INCLUDED_)
