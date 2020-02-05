// TrapGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgentSimulator.h"
#include "TrapGenDlg.h"
#include "AgentWorkThread.h"
#include <vector>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdlib.h>
#include <signal.h>

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_group.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/v3_mib.h>

#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>

//#include "module_includes.h"
//#include "module_init.h"

#include "AgentWorkThread.h"

#ifdef AGENTPP_NAMESPACE 
using namespace Agentpp;
#endif

struct TRAPGENTHREADPARAM {
	UdpAddress dest;
	int        nTrapVersion;
	CString    strTrapOid;
	Vbx*       vbs;
	int        nVbNum;
	CTrapGenDlg* pTrapGenDlg;
	Mib*         pMib;
	CAgentWorkThread* pAgent;
	bool bAutoAlarmStop;
};

int g_nTrapNum = 0;
int g_nExitThreadNum = 0;

void ClearSnmp(vector<Snmpx*>& vecSnmp)
{
	for( size_t i = 0; i < vecSnmp.size(); i++ )
		delete vecSnmp[i];
	vecSnmp.clear();
}

UINT TrapGenThread(LPVOID param)
{
	TRAPGENTHREADPARAM* pTrapGenParam = (TRAPGENTHREADPARAM*)param;
	if( !pTrapGenParam )
	{
		InterlockedIncrement((LPLONG)(&g_nExitThreadNum));
		return 0;
	}

	NotificationOriginator no;
	if( pTrapGenParam->nTrapVersion == 1 )
		no.add_v1_trap_destination(pTrapGenParam->dest, "defaultV1Trap", "V1trap", "adsl");
	else if( pTrapGenParam->nTrapVersion == 2 )
		no.add_v2_trap_destination(pTrapGenParam->dest, "defaultV2Trap", "V2trap", "adsl");
	else
		no.add_v3_trap_destination(pTrapGenParam->dest, "defaultV3Trap", "V3trap", "adsl");

	int status;
	vector<Snmpx*> vecSnmp;
	RequestList rl;
	pTrapGenParam->pMib->set_request_list(&rl);

	if( pTrapGenParam->pAgent )
	{
		map<UINT,IPENTRY>::const_iterator it = pTrapGenParam->pAgent->m_ips.begin();
		for( ; it != pTrapGenParam->pAgent->m_ips.end(); ++it )
		{
			in_addr inaddr;
			inaddr.s_addr = it->first;
			UdpAddress udpaddr(inet_ntoa(inaddr));
			Snmpx* pSnmp = new Snmpx(status, udpaddr);
			vecSnmp.push_back(pSnmp);
		}
	}

	if( vecSnmp.empty() )
	{
		Snmpx* pSnmp = new Snmpx(status, 0);
		vecSnmp.push_back(pSnmp);
	}

	bool bIsAlarm = false;
	int nAlarmStatusPos = 0;
	for( int i = 0; i < pTrapGenParam->nVbNum; i++ )
	{
		Oidx oid;
		pTrapGenParam->vbs[i].get_oid(oid);
		if( oid == "1.3.6.1.4.1.5875.88.4.6.0" )
		{
			bIsAlarm = true;
			nAlarmStatusPos = i;
			break;
		}
	}

	srand( (unsigned)time( NULL ) ); 

	for( int j = 0; j < pTrapGenParam->pTrapGenDlg->m_traptime && !pTrapGenParam->pTrapGenDlg->m_bStop; j++ )
	{
		int nSrcPos = rand() % vecSnmp.size(); 

		rl.set_snmp(vecSnmp[nSrcPos]);

		int status;
		if( bIsAlarm && pTrapGenParam->bAutoAlarmStop )
		{
			//¿ªÊ¼
			pTrapGenParam->vbs[nAlarmStatusPos].set_value(1);
			status = pTrapGenParam->pMib->notify("", 
				(LPCTSTR)pTrapGenParam->strTrapOid, 
				pTrapGenParam->vbs, 
				pTrapGenParam->nVbNum);
			InterlockedIncrement((LPLONG)(&g_nTrapNum));

			//½áÊø
			if( SNMP_CLASS_SUCCESS == status )
			{
				pTrapGenParam->vbs[nAlarmStatusPos].set_value(0);
				status = pTrapGenParam->pMib->notify("", 
					(LPCTSTR)pTrapGenParam->strTrapOid, 
					pTrapGenParam->vbs, 
					pTrapGenParam->nVbNum);
				InterlockedIncrement((LPLONG)(&g_nTrapNum));
			}
		}
		else
		{
			status = pTrapGenParam->pMib->notify("", 
				(LPCTSTR)pTrapGenParam->strTrapOid, 
				pTrapGenParam->vbs, 
				pTrapGenParam->nVbNum);
			InterlockedIncrement((LPLONG)(&g_nTrapNum));
		}

		if( status != SNMP_CLASS_SUCCESS )
		{
			AfxMessageBox("generate trap failed");
			ClearSnmp(vecSnmp);
			InterlockedIncrement((LPLONG)(&g_nExitThreadNum));
			
			return 0;
		}

		Sleep(pTrapGenParam->pTrapGenDlg->m_trapspace);
		
	}

	ClearSnmp(vecSnmp);

	InterlockedIncrement((LPLONG)(&g_nExitThreadNum));

	return 0;
}

UINT TrapGenThreadMgr(LPVOID param)
{
	CTrapGenDlg* pThisDlg = (CTrapGenDlg*)param;
	if( !pThisDlg )
		return -1;

	TRAPGENTHREADPARAM* ptrapgenparam = new TRAPGENTHREADPARAM;
	CString destaddr,trapname,xPath,vbtype,vbvalue,temp;
	int i;
	BYTE fd0,fd1,fd2,fd3;
	MSXML2::IXMLDOMNodePtr trapnode,attNode;
	MSXML2::IXMLDOMNodeListPtr xmlNodeList;
	clock_t timestart,timeend;
	float speed;
	int nAutoAlarmStop;

	CTrapGenDlg* pTrapGenDlg = (CTrapGenDlg*)param;
	if( !pTrapGenDlg )
		goto cleanup;

	ptrapgenparam->pTrapGenDlg = pTrapGenDlg;

	pTrapGenDlg->m_trapDestIp.GetAddress(fd0, fd1, fd2, fd3);
	destaddr.Format("%d.%d.%d.%d/%d", fd0, fd1, fd2, fd3, pTrapGenDlg->m_trapDestPort);
	ptrapgenparam->dest = (LPCTSTR)destaddr;
	if( !ptrapgenparam->dest.valid() )
	{
		AfxMessageBox("trap target invalid");
		goto cleanup;
	}
	
	if( ((CButton*)pTrapGenDlg->GetDlgItem(IDC_TRAPVERSION1))->GetCheck() )
		ptrapgenparam->nTrapVersion = 1;
	else if( ((CButton*)pTrapGenDlg->GetDlgItem(IDC_TRAPVERSION2))->GetCheck() )
		ptrapgenparam->nTrapVersion = 2;
	else
		ptrapgenparam->nTrapVersion = 3;


	pTrapGenDlg->m_trapType.GetWindowText(trapname);
	xPath.Format("//trap[@name='%s']", trapname);
	trapnode = pTrapGenDlg->m_xmlDomMib->selectSingleNode((LPCTSTR)xPath);
	attNode = trapnode->Getattributes()->getNamedItem("oid");
	if( attNode == 0 )
	{
		AfxMessageBox("'trap' element must contains property 'oid'");
		goto cleanup;
	}
	ptrapgenparam->strTrapOid = (char*)attNode->Gettext();

	xmlNodeList = trapnode->GetchildNodes();
	if( xmlNodeList->Getlength() == 0 )
	{
		ptrapgenparam->vbs = NULL;
		ptrapgenparam->nVbNum = 0;
	}
	else
	{
		ptrapgenparam->vbs = new Vbx[xmlNodeList->Getlength()];
		ptrapgenparam->nVbNum = xmlNodeList->Getlength();
		for( i = 0; i < xmlNodeList->Getlength(); i++ )
		{
			trapnode = xmlNodeList->Getitem(i);
			attNode = trapnode->Getattributes()->getNamedItem("oid");
			if( attNode == 0 )
			{
				AfxMessageBox("'vb' element must contains property 'oid'");
				goto cleanup;
			}
			ptrapgenparam->vbs[i].set_oid((char*)attNode->Gettext());
			attNode.Release();
			attNode = trapnode->Getattributes()->getNamedItem("type");
			if( attNode == 0 )
			{
				AfxMessageBox("'vb' element must contains property 'type'");
				goto cleanup;
			}
			vbtype = (char*)attNode->Gettext();
			attNode.Release();
			attNode = trapnode->Getattributes()->getNamedItem("value");
			if( attNode == 0 )
			{
				AfxMessageBox("'vb' element must contains property 'value'");
				goto cleanup;
			}
			vbvalue = (char*)attNode->Gettext();
			trapnode.Release();
			attNode.Release();

			SnmpSyntax* v = CAgentWorkThread::GetMibLeafValue(vbtype, vbvalue);
			ptrapgenparam->vbs[i].set_value(*v);
			delete v;
		}
	}
	
	Snmp::socket_startup();

	ptrapgenparam->pMib = new Mib();
	ptrapgenparam->pMib->add(new snmp_target_mib());
	ptrapgenparam->pMib->add(new snmp_notification_mib());
	ptrapgenparam->pMib->init();

	ptrapgenparam->pAgent = pThisDlg->m_agent;
	nAutoAlarmStop = ((CButton*)(pThisDlg->GetDlgItem(IDC_CHECK_TRAP_AUTO_STOP)))->GetCheck();
	ptrapgenparam->bAutoAlarmStop = (nAutoAlarmStop == 1);

	g_nTrapNum = 0;
	g_nExitThreadNum = 0;
	timestart = clock();
	for( i = 0; i < pTrapGenDlg->m_trapThreadNum; i++ )
		AfxBeginThread(TrapGenThread, (LPVOID)ptrapgenparam);
	
	while(1)
	{
		timeend = clock();
		unsigned long tmdif = difftime(timeend,timestart);
		if( tmdif == 0 )
			speed = 0;
		else
			speed = g_nTrapNum*1000.0/tmdif;
		temp.Format("%.2f", speed);
		pTrapGenDlg->GetDlgItem(IDC_TRAP_SPEED)->SetWindowText(temp);

		temp.Format("%02d:%02d:%02d.%d", tmdif/1000/3600, ((tmdif/1000)/60)%60, (tmdif/1000)%60, tmdif%1000);
		pTrapGenDlg->GetDlgItem(IDC_TOTAL_TIME)->SetWindowText(temp);

		temp.Format("%d", g_nTrapNum);
		pTrapGenDlg->GetDlgItem(IDC_TRAP_NUM)->SetWindowText(temp);

		if( g_nExitThreadNum == pTrapGenDlg->m_trapThreadNum )
		{
			break;
		}
		Sleep(1);
	}

cleanup:
	Snmp::socket_cleanup();
	if( trapnode ) trapnode.Release();
	if( attNode ) attNode.Release();
	if( xmlNodeList ) xmlNodeList.Release();

	//if( ptrapgenparam->pMib) delete ptrapgenparam->pMib;
	if( ptrapgenparam->vbs ) delete[] ptrapgenparam->vbs;
	if( ptrapgenparam ) delete ptrapgenparam;

	pTrapGenDlg->m_bStop  = false;

	pTrapGenDlg->GetDlgItem(IDC_TRAP_START)->EnableWindow(TRUE);
	pTrapGenDlg->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	pTrapGenDlg->GetDlgItem(IDC_TRAP_STOP)->EnableWindow(FALSE);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTrapGenDlg dialog


CTrapGenDlg::CTrapGenDlg(CAgentWorkThread* pAgent, CWnd* pParent /*=NULL*/)
	: CDialog(CTrapGenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrapGenDlg)
	m_trapDestPort = 162;
	m_traptime = 1;
	m_trapspace = 0;
	m_bStop = false;
	m_trapThreadNum = 1;
	m_agent = pAgent;
	//}}AFX_DATA_INIT
}


void CTrapGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrapGenDlg)
	DDX_Control(pDX, IDC_TRAP_TYPE, m_trapType);
	DDX_Control(pDX, IDC_TRAP_DETAIL, m_trapDetail);
	DDX_Control(pDX, IDC_TRAP_DESTIP, m_trapDestIp);
	DDX_Text(pDX, IDC_TRAP_DESTPORT, m_trapDestPort);
	DDX_Text(pDX, IDC_TRAP_TIME, m_traptime);
	DDX_Text(pDX, IDC_TRAP_SPACE, m_trapspace);
	DDX_Text(pDX, IDC_TRAP_THREADNUM, m_trapThreadNum);
	DDV_MinMaxInt(pDX, m_trapThreadNum, 1, 1024);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrapGenDlg, CDialog)
	//{{AFX_MSG_MAP(CTrapGenDlg)
	ON_BN_CLICKED(IDC_TRAP_START, OnTrapStart)
	ON_CBN_SELCHANGE(IDC_TRAP_TYPE, OnSelchangeTrapType)
	ON_BN_CLICKED(IDC_TRAP_STOP, OnTrapStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrapGenDlg message handlers

void CTrapGenDlg::OnTrapStart() 
{
	if( !UpdateData() )
		return;
	CString trapname;
	m_trapType.GetWindowText(trapname);
	if( trapname.IsEmpty() )
	{
		AfxMessageBox("please select trap type");
		return;
	}

	GetDlgItem(IDC_TRAP_START)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_TRAP_STOP)->EnableWindow(TRUE);

	AfxBeginThread(TrapGenThreadMgr, (LPVOID)this);
}

BOOL CTrapGenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_trapDestIp.SetAddress(10, 22, 16, 22);

	m_trapDetail.InsertColumn(0, "name", LVCFMT_LEFT, 100);
	m_trapDetail.InsertColumn(1, "type", LVCFMT_LEFT, 50);
	m_trapDetail.InsertColumn(2, "value", LVCFMT_LEFT, 100);

	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString mibFilePath = path;
	mibFilePath = mibFilePath.Left(mibFilePath.ReverseFind('\\'));
	mibFilePath += "\\mib.xml";

	CString msg;
	if( CoInitialize(NULL) < 0 )
	{
		msg = "init COM failed";
		AfxMessageBox(msg);
		return FALSE;
	}
	if( FAILED( m_xmlDomMib.CreateInstance("Msxml2.DOMDocument") ) )
	{
		msg = "create MSXML instance failed";
		AfxMessageBox(msg);
		return FALSE;
	}
	_variant_t varXml( mibFilePath.GetBuffer(0) );
	if( m_xmlDomMib->load( varXml ) != VARIANT_TRUE )
	{
		msg = "load mib.xml failed";
		MSXML2::IXMLDOMParseErrorPtr err = m_xmlDomMib->GetparseError();
		if( err != 0 )
		{
			msg.Format("load mib.xml failed, error:%s\n%s(%d)", (char*)err->Getreason(),
				                                        (char*)err->GetsrcText(),
														(char*)err->Getline());
			err.Release();
		}
		AfxMessageBox(msg);
		m_xmlDomMib.Release();
		return FALSE;
	}

	CString xPath = "//trap";
	MSXML2::IXMLDOMNodeListPtr trapnodes = m_xmlDomMib->selectNodes((LPCTSTR)xPath);
	for( int i = 0; i < trapnodes->Getlength(); i++ )
	{
		MSXML2::IXMLDOMNodePtr trapnode = trapnodes->Getitem(i);
		MSXML2::IXMLDOMNodePtr attNode = trapnode->Getattributes()->getNamedItem("name");
		if( attNode == 0 )
		{
			AfxMessageBox("'trap' element must contains property 'name'");
			trapnode.Release();
			trapnodes.Release();
			m_xmlDomMib.Release();
			return FALSE;
		}
		m_trapType.AddString((char*)attNode->Gettext());
		trapnode.Release();
		attNode.Release();
	}
	trapnodes.Release();

	((CButton*)GetDlgItem(IDC_TRAPVERSION2))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_TRAP_AUTO_STOP))->SetCheck(1);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrapGenDlg::OnSelchangeTrapType() 
{
	CString trapname;
	m_trapType.GetWindowText(trapname);

	if( trapname.IsEmpty() )
		return;

	CString xPath;
	xPath.Format("//trap[@name='%s']", trapname);
	MSXML2::IXMLDOMNodePtr trapnode = m_xmlDomMib->selectSingleNode((LPCTSTR)xPath);
	if( trapnode == 0 )
		return;
	m_trapDetail.DeleteAllItems();
	MSXML2::IXMLDOMNodeListPtr trapvbs = trapnode->GetchildNodes();
	for( int i = 0; i < trapvbs->Getlength(); i++ )
	{
		MSXML2::IXMLDOMNodePtr trapvbnode = trapvbs->Getitem(i);
		MSXML2::IXMLDOMNamedNodeMapPtr attMap = trapvbnode->Getattributes();
		MSXML2::IXMLDOMNodePtr attNode = attMap->getNamedItem("name");
		if( attNode == 0 )
		{
			AfxMessageBox("'trap' child 'vb' element must contains property 'name'");
			trapvbnode.Release();
			attMap.Release();
			trapvbs.Release();
			return;
		}
		m_trapDetail.InsertItem(i, (char*)attNode->Gettext());
		attNode = attMap->getNamedItem("type");
		if( attNode == 0 )
		{
			AfxMessageBox("'trap' child 'vb' element must contains property 'type'");
			trapvbnode.Release();
			attMap.Release();
			trapvbs.Release();
			return;
		}
		m_trapDetail.SetItemText(i, 1, (char*)attNode->Gettext());
		attNode = attMap->getNamedItem("value");
		if( attNode == 0 )
		{
			AfxMessageBox("'trap' child 'vb' element must contains property 'value'");
			trapvbnode.Release();
			attMap.Release();
			trapvbs.Release();
			return;
		}
		m_trapDetail.SetItemText(i, 2, (char*)attNode->Gettext());
	}
}

void CTrapGenDlg::OnTrapStop() 
{
	m_bStop = true;
}
