// AgentSimulatorDoc.h : interface of the CAgentSimulatorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTSIMULATORDOC_H__1DE996AC_A165_4CAC_82C8_F94E57CDDD27__INCLUDED_)
#define AFX_AGENTSIMULATORDOC_H__1DE996AC_A165_4CAC_82C8_F94E57CDDD27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAgentSimulatorDoc : public CDocument
{
protected: // create from serialization only
	CAgentSimulatorDoc();
	DECLARE_DYNCREATE(CAgentSimulatorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentSimulatorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgentSimulatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAgentSimulatorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGENTSIMULATORDOC_H__1DE996AC_A165_4CAC_82C8_F94E57CDDD27__INCLUDED_)
