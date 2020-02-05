#if !defined(AFX_TRAPGENDLG_H__AF543287_FF3B_4DCE_8F52_4ECF612852BC__INCLUDED_)
#define AFX_TRAPGENDLG_H__AF543287_FF3B_4DCE_8F52_4ECF612852BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrapGenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrapGenDlg dialog
class CAgentWorkThread;
class CTrapGenDlg : public CDialog
{
// Construction
public:
	CTrapGenDlg(CAgentWorkThread* pAgent, CWnd* pParent = NULL);   // standard constructor

	IXMLDOMDocument2Ptr m_xmlDomMib;
	bool m_bStop;
// Dialog Data
	//{{AFX_DATA(CTrapGenDlg)
	enum { IDD = IDD_DIALOG_TRAPGEN };
	CComboBox	m_trapType;
	CListCtrl	m_trapDetail;
	CIPAddressCtrl	m_trapDestIp;
	DWORD	m_trapDestPort;
	UINT	m_traptime;
	UINT	m_trapspace;
	int		m_trapThreadNum;
	//}}AFX_DATA

	CAgentWorkThread* m_agent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrapGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrapGenDlg)
	afx_msg void OnTrapStart();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTrapType();
	afx_msg void OnTrapStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAPGENDLG_H__AF543287_FF3B_4DCE_8F52_4ECF612852BC__INCLUDED_)
