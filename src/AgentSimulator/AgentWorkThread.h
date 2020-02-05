#if !defined(AFX_AGENTWORKTHREAD_H__CDD42DC6_86B2_4369_9A97_818B4E4B330E__INCLUDED_)
#define AFX_AGENTWORKTHREAD_H__CDD42DC6_86B2_4369_9A97_818B4E4B330E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgentWorkThread.h : header file
//

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_group.h>
#include <string>
#include <MAP>
using namespace std;

using namespace MSXML2;


#define MSG_START_AGENT   WM_USER+101
#define MSG_STOP_AGENT    WM_USER+102
#define MSG_RESTART_AGENT WM_USER+103
#define MSG_SAVE_MIB      WM_USER+104

struct TP_STRUCT
{
	bool tp;
	string sr;
};

struct IPENTRY {
	UINT iaddr;
	UINT imask;
	ULONG NTEContext;
	bool bAdd;
};

#define MAX_IP_COUNT 1024

class MyMib;
class CMainFrame;
/////////////////////////////////////////////////////////////////////////////
// CAgentWorkThread thread
class CAgentWorkThread : public CWinThread
{
	DECLARE_DYNCREATE(CAgentWorkThread)
protected:
	CAgentWorkThread();           // protected constructor used by dynamic creation

public:
	CAgentWorkThread(CMainFrame*);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentWorkThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAgentWorkThread();

	// Generated message map functions
	//{{AFX_MSG(CAgentWorkThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	CString GetPduTypeStr(int type);
	CString GetSMITypeStr(int type);
	void PrintHexData(const unsigned char* data, int len, CString& printstr);
	void Log(CString& msg, bool newlog = true);

	MyMib*         m_mib;
	RequestList*   m_reqList;
	map<string, TP_STRUCT> m_tp;
	static SnmpSyntax* GetMibLeafValue(const CString& type, const CString& value);

	map<UINT,IPENTRY> m_ips;

private:
	BOOL InitMib(MyMib* mib);
	BOOL _AddIPAddress(IPENTRY* ipentry, DWORD index);
	MibLeaf* GetMibLeaf(MSXML2::IXMLDOMNodePtr node, int leaftype/*1表示表，0表示标量*/);
	BOOL AddMibRow(MibTable* table, MSXML2::IXMLDOMNodePtr node);
	BOOL InitAgent();
	void DestroyAgent();
	BOOL StartAgentDemo();
	BOOL StopAgentDemo();
	CRichEditCtrl* m_pEditMsg;
	Snmpx*         m_snmp;
	CMainFrame*    m_mainFrame;
	
};

//--------------------------class MyMib---------------------------------
class MyMib : public Mib
{
public:
	MyMib(CAgentWorkThread* workThread){ m_workThread = workThread; }
	virtual void finalize(Request* req);
	void printmsg(Request* req);
private:
	CAgentWorkThread* m_workThread;
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGENTWORKTHREAD_H__CDD42DC6_86B2_4369_9A97_818B4E4B330E__INCLUDED_)
