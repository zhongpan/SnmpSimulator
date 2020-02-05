// AgentWorkThread.cpp : implementation file
//

#include "stdafx.h"
#include "AgentSimulator.h"
#include "AgentWorkThread.h"
#include "MainFrm.h"

#include <signal.h>

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_group.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
//#include <agent_pp/notification_originator.h>
#include <agent_pp/mib_complex_entry.h>
//#include <agent_pp/v3_mib.h>

//#include <snmp_pp/oid_def.h>
//#include <snmp_pp/mp_v3.h>
//#include <snmp_pp/log.h>

#include <IPHlpApi.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_run = false;
CEvent g_stop;

//-------------------------- 工作线程 -------------------------
UINT AgentDemoThread(LPVOID param)
{
	CAgentWorkThread* workThread = (CAgentWorkThread*)param;
	CString msg = "Agent start listen";
	workThread->Log(msg);

	Request* req;
	while ( g_run ) 
	{
		req = workThread->m_reqList->receive(1);

		if (req) 
		{			
			workThread->m_mib->process_request(req);
			//workThread->m_mib->save_all();
		}
		else 
		    workThread->m_mib->cleanup();
		
		req = NULL;
	}

	msg = "Agent stop listen";
	workThread->Log(msg);
	g_stop.SetEvent();
	return 1;
}

//------------------------------ 工作线程 --------------------------------
/////////////////////////////////////////////////////////////////////////////
// CAgentWorkThread

IMPLEMENT_DYNCREATE(CAgentWorkThread, CWinThread)

CAgentWorkThread::CAgentWorkThread()
{
}

CAgentWorkThread::~CAgentWorkThread()
{
}

SnmpSyntax* CAgentWorkThread::GetMibLeafValue(const CString& type, 
											  const CString& value)
{
/*
type:   bits                   not supported
        octets
        null                   not supported
        oid
        int(int32)
        ipaddr
        cntr32
        uint32(gauge32)
        timeticks
        opaque                 not supported
        cntr64                 not supported
*/
	CString tempstr = value;
	tempstr.TrimLeft();
	tempstr.TrimRight();
	if( type == "int" || type == "int32" )
		return new SnmpInt32(atoi((LPCSTR)tempstr));
	else if( type == "octets" )
	{
		if( !tempstr.IsEmpty() && tempstr[0] == '#' )
		{
			tempstr.Remove(' ');
			tempstr.Remove('\x09');
			tempstr.Remove('\x0a');
			tempstr.Remove('\x0d');
			OctetStr* v = new OctetStr();
			OctetStr hexstr(tempstr.Mid(1).GetBuffer(0));
			(*v) = OctetStr::from_hex_string(hexstr);
			return v;
		}
		else
			return new OctetStr((LPCSTR)tempstr);
	}
	else if( type == "oid" )
		return new Oid((LPCSTR)tempstr);
	else if( type == "ipaddr" )
		return new IpAddress((LPCSTR)tempstr);
	else if( type == "gauge32" || type == "uint32" )
		return new SnmpUInt32(atoi((LPCSTR)tempstr));
	else if( type == "timeticks" )
		return new TimeTicks(atoi((LPCSTR)tempstr));
	else if( type == "cntr32" )
		return new Counter32(atoi((LPCSTR)tempstr));
	else
		return NULL;
}

MibLeaf* CAgentWorkThread::GetMibLeaf(MSXML2::IXMLDOMNodePtr node, int leaftype)
{
	CString oid,access,type,value,name,tpstr,setreturnstr;
	MSXML2::IXMLDOMNamedNodeMapPtr attMap = node->Getattributes();
	MSXML2::IXMLDOMNodePtr att = attMap->getNamedItem("oid");
	if( att != 0 )
		oid = (char*)att->Gettext();
	att = attMap->getNamedItem("access");
	if( att != 0 )
		access = (char*)att->Gettext();
	att = attMap->getNamedItem("type");
	if( att != 0 )
		type = (char*)att->Gettext();
	att = attMap->getNamedItem("value");
	if( att != 0 )
		value = (char*)att->Gettext();
	att = attMap->getNamedItem("name");
	if( att != 0 )
		name = (char*)att->Gettext();
	att = attMap->getNamedItem("sr");
	if( att != 0 )
		setreturnstr = (char*)att->Gettext();
	att = attMap->getNamedItem("tp");
	if( att != 0 )
		tpstr = (char*)att->Gettext();

	if( att != 0 ) att.Release();
	attMap.Release();

	CString msg;
	if( oid.IsEmpty() )
	{
		msg.Format("leaf's property 'oid' must no empty\n%s", (char*)node->Getxml());
		Log(msg);
		return NULL;
	}
	//标量
	if( leaftype == 0 && oid.Right(2) != ".0" )
	{
		msg.Format("scalar quantity 'oid' must ending with '.0'\n%s", (char*)node->Getxml());
		Log(msg);
		return NULL;
	}

	//透传只对标量有效
	if( leaftype == 0 )
	{
		if( tpstr.IsEmpty() || tpstr == "false" )
		{
			TP_STRUCT tps;
			tps.tp = false;
			m_tp[oid.GetBuffer(0)] = tps;
		}
		else if( tpstr == "true" )
		{
			TP_STRUCT tps;
			tps.tp = true;
			if( !setreturnstr.IsEmpty() )
			{
				if( setreturnstr[0] != '#' )
				{
					msg.Format("'sr' must begin with '#'\n%s", (char*)node->Getxml());
					Log(msg);
					return NULL;
				}
				setreturnstr.Remove(' ');
				setreturnstr.Remove('\x09');
				setreturnstr.Remove('\x0a');
				setreturnstr.Remove('\x0d');
				tps.sr = setreturnstr.GetBuffer(0);
			}
			m_tp[oid.GetBuffer(0)] = tps;
		}
	}

	SnmpSyntax* v = GetMibLeafValue(type, value);
	if( !v )
	{
		msg.Format("leaf's property 'type' error\n%s", (char*)node->Getxml());
		Log(msg);
		return NULL;
	}
/*
access: n     noaccess
        r     readonly
        rw    read and write
        rc    read and create
*/
	mib_access ac;
	if( access == "r" )
		ac = READONLY;
	else if( access == "rw" )
		ac = READWRITE;
	else if( access == "rc" )
		ac = READCREATE;
	else if( access == "n" )
		ac = NOACCESS;
	else
	{
		msg.Format("leaf's property 'access' error\n%s", (char*)node->Getxml());
		Log(msg);
		return NULL;
	}

	msg = "leaf : " + oid + "(" + name + ")" + " : " + value;
	Log(msg, false);

	MibLeaf* leaf = new MibLeaf(oid.GetBuffer(0), ac, v);
	return leaf;
}

BOOL CAgentWorkThread::AddMibRow(MibTable* table, MSXML2::IXMLDOMNodePtr node)
{
	int colnum = table->get_columns()->size();
	CString index,msg;
	MSXML2::IXMLDOMNodePtr att = node->Getattributes()->getNamedItem("index");
	if( att != 0 )
	{
		index = (char*)att->Gettext();
		att.Release();
	}

	if( index.IsEmpty() )
	{
		msg.Format("row's property 'index' must not empty\n%s", (char*)node->Getxml());
		Log(msg);
		return FALSE;
	}
	
	CString values = (char*)node->Gettext();
	int n = values.Replace(',', '|');
	if( n + 1 != colnum )
	{
		msg.Format("row's value num is not same as table's colunm\n%s", (char*)node->Getxml());
		Log(msg);
		return FALSE;
	}

	msg = "row : " + values;
	Log(msg, false);
	MibTableRow* row = table->add_row(index.GetBuffer(0));
	int loc = 0;
	int start = 0;
	CString value;
	for( int i = 0; i < n + 1; i++ )
	{
		loc = values.Find('|', start);
		if( loc < 0 )
			loc = values.GetLength();
		value = values.Mid(start, loc - start);
		start = loc + 1;
		MibLeaf* leaf = row->get_nth(i);
		SmiUINT32 type = leaf->get_syntax();
		SnmpSyntax* v = GetMibLeafValue(GetSMITypeStr(type), value);
		if( !v )
		{
			msg.Format("add row error\n%s : %s", (char*)node->Getxml(), value);
			Log(msg);
			return FALSE;
		}
		leaf->replace_value(v);
	}
	return TRUE;
}

BOOL CAgentWorkThread::InitMib(MyMib* mib)
{
	CString msg = "start init Mib";
	Log(msg);
	IXMLDOMDocument2Ptr mibScript;
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString mibFilePath = path;
	mibFilePath = mibFilePath.Left(mibFilePath.ReverseFind('\\'));
	mibFilePath += "\\mib.xml";

	if( CoInitialize(NULL) < 0 )
	{
		msg = "init COM failed";
		Log(msg);
		return FALSE;
	}
	if( FAILED( mibScript.CreateInstance("Msxml2.DOMDocument") ) )
	{
		msg = "create MSXML instanse failed";
		Log(msg);
		return FALSE;
	}
	_variant_t varXml( mibFilePath.GetBuffer(0) );
	if( mibScript->load( varXml ) != VARIANT_TRUE )
	{
		msg = "loca mib.xml failed";
		MSXML2::IXMLDOMParseErrorPtr err = mibScript->GetparseError();
		if( err != 0 )
		{
			msg.Format("load mib.xml failed, reason：%s\n%s(%d)", (char*)err->Getreason(),
				                                        (char*)err->GetsrcText(),
														(char*)err->Getline());
			err.Release();
		}
		Log(msg);
		mibScript.Release();
		return FALSE;
	}

	CString xpath = "//mib/leaf";
	MSXML2::IXMLDOMNodeListPtr leafs = mibScript->selectNodes(xpath.GetBuffer(0));
	for( int i = 0; i < leafs->Getlength(); i++ )
	{
		MibLeaf* leaf = GetMibLeaf(leafs->Getitem(i), 0);
		if( !leaf )
		{
			leafs.Release();
			mibScript.Release();
			return FALSE;
		}
		mib->add(leaf);
	}
	leafs.Release();

	xpath = "//mib/table";
	MSXML2::IXMLDOMNodeListPtr tables = mibScript->selectNodes(xpath.GetBuffer(0));

	for(int i = 0; i < tables->Getlength(); i++ )
	{
		MSXML2::IXMLDOMNodePtr node = tables->Getitem(i);
		CString oid,name;
		MSXML2::IXMLDOMNodePtr att = node->Getattributes()->getNamedItem("oid");
		if( att != 0 )
		{
			oid = (char*)att->Gettext();
			att.Release();
		}
		att = node->Getattributes()->getNamedItem("name");
		if( att != 0 )
		{
			name = (char*)att->Gettext();
			att.Release();
		}
		if( oid.IsEmpty() )
		{
			msg.Format("table's property 'oid' must not empty\n%s", (char*)node->Getxml());
			Log(msg);
			tables.Release();
			node.Release();
			mibScript.Release();
			return FALSE;
		}
		msg = "table : " + oid + "(" + name + ")";
		Log(msg, false);

		MibTable* tb = new MibTable(oid.GetBuffer(0));

		xpath = "./leaf";
		MSXML2::IXMLDOMNodeListPtr tls = node->selectNodes(xpath.GetBuffer(0));
		for( int j = 0; j < tls->Getlength(); j++ )
		{
			MibLeaf* leaf = GetMibLeaf(tls->Getitem(j), 1);
			if( !leaf )
			{
				tables.Release();
				node.Release();
				tls.Release();
				mibScript.Release();
				delete tb;
				return FALSE;
			}
			tb->add_col(leaf);
		}
		tls.Release();

		xpath = "./row";
		MSXML2::IXMLDOMNodeListPtr rows = node->selectNodes(xpath.GetBuffer(0));
		for(int j = 0; j < rows->Getlength(); j++ )
		{
			if( !AddMibRow(tb, rows->Getitem(j)) )
			{
				tables.Release();
				node.Release();
				rows.Release();
				mibScript.Release();
				delete tb;
				return FALSE;
			}
		}
		rows.Release();

		node.Release();
		mib->add(tb);
	}
	tables.Release();
	
	mibScript.Release();

	msg = "init Mib succesful";
	Log(msg);
	return TRUE;
}

BOOL CAgentWorkThread::InitAgent()
{
	CString msg;
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString iniFilePath = path;
	iniFilePath = iniFilePath.Left(iniFilePath.ReverseFind('\\'));
	iniFilePath += "\\AgentSimulator.ini";

	char ip[1024];
	char rcommunity[128];
	char wcommunity[128];
	u_short port;

	//Port
	port = GetPrivateProfileInt("AGENT", "port", 161, (LPCSTR)iniFilePath);
	if( port == 0 )
		port = 161;

	//IP
	m_ips.clear();
	GetPrivateProfileString("AGENT", "ip", "0.0.0.0", ip, 1024, (LPCSTR)iniFilePath);
	CString strip = ip;
	strip.TrimLeft();
	strip.TrimRight();
	msg.Format("in AgentSimulator.ini, ip is:%s", strip);
	Log(msg);
	if( strip != "0.0.0.0" )
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		
		// Make an initial call to GetAdaptersInfo to get
		// the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			free(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
		}
		if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			while (pAdapter) {
				MIB_IFROW mibIfRow;
				mibIfRow.dwIndex = pAdapter->Index;
				if ((dwRetVal = GetIfEntry(&mibIfRow)) == NO_ERROR) {
					if( mibIfRow.dwOperStatus == MIB_IF_OPER_STATUS_NON_OPERATIONAL
						|| mibIfRow.dwOperStatus == MIB_IF_OPER_STATUS_DISCONNECTED )
						continue;
					else
						break;
				}
				else {
					msg.Format("call GetIfEntry error:%d", dwRetVal);
					Log(msg);
					free(pAdapterInfo);
					return FALSE;
				}
				pAdapter = pAdapter->Next;
			}
		}
		else {
			msg.Format("call GetAdaptersInfo error:%d", dwRetVal);
			Log(msg);
			return FALSE;
		}
		if( !pAdapter )
		{
			msg == "no available network card";
			Log(msg);
			free(pAdapterInfo);
			return FALSE;
		}
		DWORD index = pAdapter->Index;
		free(pAdapterInfo);
		
		int loc = -1;
		int cnt = 0;
		do {
			int lastloc = loc+1;
			loc = strip.Find(',', lastloc);
			CString ipscope;
			if( loc > 0 )
				ipscope = strip.Mid(lastloc, loc-lastloc);
			else
				ipscope = strip.Mid(lastloc);
			int loc1 = ipscope.Find('/');
			if( loc1 < 0 )
			{
				msg.Format("AgentSimulator.ini, ip error, no mask:%s", ip);
				Log(msg);
				return FALSE;
			}
			CString strip = ipscope.Mid(0, loc1);
			CString mask = ipscope.Mid(loc1+1);
			UINT imask = atoi((LPCSTR)mask);
			if( imask <=0 || imask >= 32 )
			{
				msg.Format("AgentSimulator.ini, ip error, mask error:%d", imask);
				Log(msg);
				return FALSE;
			}
			UINT imask2 = 0;
			for(int i = 0; i < imask; i++ )
				imask2 += 1 << (31-i);
			imask2 = htonl(imask2);
			int loc2 = strip.Find('-');
			if( loc2 > 0 )
			{
				CString stripstart = strip.Mid(0, loc2);
				CString stripend = strip.Mid(loc2+1);
				UINT ipstart = ntohl(inet_addr((LPCSTR)stripstart));
				UINT ipend = ntohl(inet_addr((LPCSTR)stripend));
				if( ipstart == INADDR_NONE || ipend == INADDR_NONE || ipstart >= ipend )
				{
					msg.Format("AgentSimulator.ini, ip error, ip range error:%s", strip);
					Log(msg);
					return FALSE;
				}
				for( UINT i = ipstart; i <= ipend; i++ )
				{
					UINT iaddr = htonl(i);
					UINT hostaddr = iaddr & (~imask2);
					if( hostaddr == 0 || hostaddr == ~imask2 )
						continue;
					
					map<UINT,IPENTRY>::const_iterator it;
					it = m_ips.find(iaddr);
					if( it != m_ips.end() )
					{
						msg.Format("AgentSimulator.ini, ip error, ip repeat:%s", strip);
						Log(msg);
						return FALSE;
					}
					IPENTRY ipentry = {0};
					ipentry.iaddr = iaddr;
					ipentry.imask = imask2;
					ipentry.bAdd = false;
					m_ips[iaddr] = ipentry;
					cnt++;
				}
			}
			else
			{
				UINT iaddr = inet_addr((LPCSTR)strip);
				if( iaddr == INADDR_NONE )
				{
					msg.Format("AgentSimulator.ini, ip error:%s", strip);
					Log(msg);
					return FALSE;
				}
				map<UINT,IPENTRY>::const_iterator it;
				it = m_ips.find(iaddr);
				if( it != m_ips.end() )
				{
					msg.Format("AgentSimulator.ini, ip error, ip repeat:%s", strip);
					Log(msg);
					return FALSE;
				}
				IPENTRY ipentry = {0};
				ipentry.iaddr = iaddr;
				ipentry.imask = imask2;
				ipentry.bAdd = false;
				m_ips[iaddr] = ipentry;
				cnt++;
			}
			if( cnt > MAX_IP_COUNT )
			{
				msg.Format("AgentSimulator.ini, to much ip, must less than %d", MAX_IP_COUNT);
				Log(msg);
				return FALSE;
			}
		} while(loc > 0);

		map<UINT,IPENTRY>::iterator it;
		for( it = m_ips.begin(); it != m_ips.end(); ++it )
		{
			if( !_AddIPAddress(&it->second, index) )
			{
				for( it = m_ips.begin(); it != m_ips.end(); ++it )
				{
					IPENTRY ipentry = it->second;
					if( ipentry.bAdd )
						DeleteIPAddress(ipentry.NTEContext);
				}
				m_ips.clear();
				return FALSE;
			}
		}

		strcpy(ip, "0.0.0.0");
	}
	
	//Community
	GetPrivateProfileString("AGENT", "read_community", "adsl", rcommunity, 128, (LPCSTR)iniFilePath);
	GetPrivateProfileString("AGENT", "write_community", "adsl", wcommunity, 128, (LPCSTR)iniFilePath);

	int status;
	UdpAddress inaddr(ip);
	inaddr.set_port(port);
	if( !inaddr.valid() )
	{
		msg.Format("AgentSimulator.ini, ip error\n%s:%d", ip, port);
		Log(msg);
		return FALSE;
	}
	m_snmp = new Snmpx(status, inaddr);

	if (status == SNMP_CLASS_SUCCESS) 
	{
		msg.Format("Agent session create succesful(%s:%d:%s:%s)", m_snmp->get_listen_address().get_printable(), 
			                                         port,
													 rcommunity,
													 wcommunity);
		Log(msg);
	}
	else {
		msg.Format("Agent session create failed, reason:\n%s(%s:%d)", m_snmp->error_msg(status), ip, port);
		Log(msg);
		return FALSE;
	}

	m_mib = new MyMib(this);
	m_reqList = new RequestList();
	m_mib->set_request_list(m_reqList);
	m_reqList->set_snmp(m_snmp);

	m_reqList->set_read_community(rcommunity);
	m_reqList->set_write_community(wcommunity);

	if( !InitMib(m_mib) || !m_mib->init() )
	{
		msg = "init Mib failed";
		Log(msg);
		return FALSE;
	}

	return TRUE;
}

BOOL CAgentWorkThread::_AddIPAddress(IPENTRY* ipentry, DWORD index)
{
	DWORD dwRetVal = 0;
	ULONG NTEInstance = 0;

	dwRetVal = AddIPAddress(ipentry->iaddr, 
		ipentry->imask, 
		index, 
		&ipentry->NTEContext, 
		&NTEInstance);
	
	if ( !(dwRetVal == NO_ERROR || dwRetVal == ERROR_DUP_DOMAINNAME) ) 
	{
		LPVOID lpMsgBuf;
		if (FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetVal,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL )) {
			CString msg = (LPTSTR)lpMsgBuf;
			msg = "call AddIPAddress error:" + msg;
			Log(msg);
		}
		LocalFree( lpMsgBuf );
		return FALSE;
	}
	ipentry->bAdd = true;

	return TRUE;
}


void CAgentWorkThread::DestroyAgent()
{
	CString msg;
	if( m_mib )
	{
		delete m_mib;
		m_mib = NULL;
		msg = "clear Mib";
		Log(msg);
	}
	if( m_snmp )
	{
		delete m_snmp;
		m_snmp = NULL;
		msg = "destory Agent session";
		Log(msg);
	}
	m_tp.clear();
}

CAgentWorkThread::CAgentWorkThread(CMainFrame* mainFrame)
{
	m_pEditMsg = &((CRichEditView*)mainFrame->GetActiveView())->GetRichEditCtrl();
	m_mainFrame = mainFrame;
	m_mib = NULL;
	m_reqList = NULL;
	m_snmp = NULL;
} 

BOOL CAgentWorkThread::InitInstance()
{
	Snmp::socket_startup();  // Initialize socket subsystem
	return TRUE;
}

int CAgentWorkThread::ExitInstance()
{
	DestroyAgent();
	Snmp::socket_cleanup();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CAgentWorkThread, CWinThread)
	//{{AFX_MSG_MAP(CAgentWorkThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentWorkThread message handlers

void CAgentWorkThread::Log(CString &msg, bool newlog)
{
	CTime tm = CTime::GetCurrentTime();
	if( m_pEditMsg )
	{
		if( newlog )
		{
			if( m_pEditMsg->GetLineCount() > 10000 )
			{
				m_pEditMsg->SetReadOnly(FALSE);
				m_pEditMsg->SetSel(0, -1);
				m_pEditMsg->Clear();
				m_pEditMsg->SetReadOnly();
			}
			m_pEditMsg->SetSel(-1, -1);
			if( m_pEditMsg->GetTextLength() != 0 )
				m_pEditMsg->ReplaceSel("\n");
			m_pEditMsg->SetSel(-1, -1);
			m_pEditMsg->ReplaceSel(tm.Format("%Y-%M-%d %H:%M:%S > ") + msg + "\n");
		}
		else
		{
			m_pEditMsg->SetSel(-1, -1);
			m_pEditMsg->ReplaceSel(msg + "\n");
		}
	}
}

CString CAgentWorkThread::GetPduTypeStr(int type)
{
	switch(type) {
	case sNMP_PDU_GET:
		return "get";
		break;
	case sNMP_PDU_GETNEXT:
		return "get_next";
		break;
	case sNMP_PDU_RESPONSE:
		return "response";
		break;
	case sNMP_PDU_SET:
		return "set";
		break;
	case sNMP_PDU_V1TRAP:
		return "v1trap";
		break;
	case sNMP_PDU_GETBULK:
		return "get_bulk";
		break;
	case sNMP_PDU_INFORM:
		return "inform";
		break;
	case sNMP_PDU_TRAP:
		return "trap";
		break;
	case sNMP_PDU_REPORT:
		return "report";
		break;
	default:
		return "unknown";
	}
}

CString CAgentWorkThread::GetSMITypeStr(int type)
{
	switch(type) {
	case sNMP_SYNTAX_BITS:
		return "bits";
		break;
	case sNMP_SYNTAX_OCTETS:
		return "octets";
		break;
	case sNMP_SYNTAX_NULL:
		return "null";
		break;
	case sNMP_SYNTAX_OID:
		return "oid";
		break;
	case sNMP_SYNTAX_INT:
	//case sNMP_SYNTAX_INT32:
		return "int";
		break;
	case sNMP_SYNTAX_IPADDR:
		return "ipaddr";
		break;
	case sNMP_SYNTAX_CNTR32:
		return "cntr32";
		break;
	case sNMP_SYNTAX_GAUGE32:
	//case sNMP_SYNTAX_UINT32:
		return "uint32";
		break;
	case sNMP_SYNTAX_TIMETICKS:
		return "timeticks";
		break;
	case sNMP_SYNTAX_OPAQUE:
		return "opaque";
		break;
	case sNMP_SYNTAX_CNTR64:
		return "cntr64";
		break;
	default:
		return "unknown";
	}
}

void CAgentWorkThread::PrintHexData(const unsigned char *data, int len, CString& printstr)
{
	CString ch;
	for( int i = 0; i < len; i++ )
	{
		ch.Format("%02x ", data[i]);
		printstr += ch;
		if( i != len - 1 && (i + 1) % 16 == 0 )
		{
			CString str;;
			for( int j = 0; j < 16; j++ )
			{
				if( isalnum(data[16 * (i / 16) + j]) )
					ch.Format("%c", data[16 * (i / 16) + j]);
				else
					ch = '.';
				str += ch;
			}
			printstr += "\t" + str + "\n";
		}
		else if( i == len - 1 )
		{
			CString str = CString(' ', (16 - len % 16) * 3);
			CString str1;
			for( int j = 0; j < len % 16; j++ )
			{
				if( isalnum(data[16 * (i / 16) + j]) )
					ch.Format("%c", data[16 * (i / 16) + j]);
				else
					ch = '.';
				str1 += ch;
			}
			printstr += str + "\t" + str1;
		}
	}
}

BOOL CAgentWorkThread::StartAgentDemo()
{
	g_run = true;
	if( !InitAgent() )
	{
		DestroyAgent();
		m_mainFrame->m_bStarted = FALSE;
		return FALSE;
	}
	AfxBeginThread(AgentDemoThread, (LPVOID)this);

	return TRUE;
}
BOOL CAgentWorkThread::StopAgentDemo()
{
	g_run = false;
	while( WaitForSingleObject(g_stop, INFINITE) != WAIT_OBJECT_0 ){}
	DestroyAgent();
	map<UINT,IPENTRY>::const_iterator it;
	for( it = m_ips.begin(); it != m_ips.end(); ++it )
	{
		IPENTRY ipentry = it->second;
		if( ipentry.bAdd )
			DeleteIPAddress(ipentry.NTEContext);
	}
	m_ips.clear();
				
	g_stop.ResetEvent();
	
	return TRUE;
}

int CAgentWorkThread::Run() 
{
	MSG msg;
	CString str;
	for(;;)
	{
		GetMessage(&msg, NULL, NULL, NULL);
		
		switch(msg.message) 
		{
		case MSG_START_AGENT:
			StartAgentDemo();
			break;
		case MSG_STOP_AGENT:
			StopAgentDemo();
			break;
		case MSG_RESTART_AGENT:
			StopAgentDemo();
			StartAgentDemo();
			break;
		case MSG_SAVE_MIB:
			break;
		}
	}
	
	return CWinThread::Run();
}


//------------------------ class MyMib -----------------------
void MyMib::finalize(Request* req)
{
	printmsg(req);

	Pdux* pdu = req->get_pdu();
	Vb vb;
	pdu->get_vb(vb, 0);
	
	//处理透传
	if( pdu->get_vb_count() == 1 && vb.get_syntax() == sNMP_SYNTAX_OCTETS )
	{
		Oid o;
		vb.get_oid(o);
		map<string, TP_STRUCT>::const_iterator it;
		it = m_workThread->m_tp.find(o.get_printable());
		if( it != m_workThread->m_tp.end() )
		{
			TP_STRUCT tps = it->second;
			if( tps.tp )
			{
				if( req->get_type() == sNMP_PDU_SET && !tps.sr.empty() )
				{
					OctetStr v(tps.sr.substr(1).c_str());
					v = OctetStr::from_hex_string(v);
					vb.set_value(v);
					pdu->set_vb(vb, 0);
				}
				req->set_error_status(19);
			}
		}
	}

	Mib::finalize(req);
}

void MyMib::printmsg(Request* req)
{
	CString tempmsg,msg;
	tempmsg.Format("accept request(%s : %s)", 
		req->get_address()->get_address().get_printable(),
		m_workThread->GetPduTypeStr(req->get_type()));
	
	msg = tempmsg;
	msg += "\n----------------------- oid and value -----------------------";
	for( int i = 0; i < req->subrequests(); i++ )
	{
		Vbx vb = req->get_value(i);
		SmiUINT32 type = vb.get_syntax();
		if( type != sNMP_SYNTAX_OCTETS )
			tempmsg.Format("\n%s(%s) : %s", vb.get_printable_oid(), 
			m_workThread->GetSMITypeStr(type),
			vb.get_printable_value());
		else
		{
			OctetStr val;
			vb.get_value(val);
			tempmsg.Format("\n%s(%s) :\n", vb.get_printable_oid(), m_workThread->GetSMITypeStr(type));
			m_workThread->PrintHexData(val.data(), val.len(), tempmsg);
		}
		msg += tempmsg;
	}
	m_workThread->Log(msg);
}