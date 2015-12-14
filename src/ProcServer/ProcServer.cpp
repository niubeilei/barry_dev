////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
#include "ProcServer/ProcServer.h"

#include "AppMgr/App.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"

#include "ProcRequest/ProcRequest.h"

#include "XmlUtil/SeXmlParser.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpServer.h"
#include "Util/StrParser.h"
#include <deque>



using namespace std;

AosProcServer::AosProcServer(const AosXmlTagPtr &config)
:
mLock(OmnNew OmnMutex()),
mIsSeServer(false)
{
	mTransId = 100;
	AosXmlTagPtr procserver = config->getFirstChild("procserver");
	aos_assert(procserver);
	OmnString addr = procserver->getAttrStr("local_addr", "");
	aos_assert(addr != "");
	int port = procserver->getAttrInt("local_port", 0);
	aos_assert(port);
	mServer = OmnNew OmnTcpServer(addr, port, 1, "Proc Server", eAosTLT_FirstFourHigh);

	OmnTcpListenerPtr thisptr(this, false);
	mServer->setListener(thisptr);
	mServer->startReading();
	//init seserver 
	AosXmlTagPtr seserver = config->getFirstChild("seserver");
	if(seserver)
	{
		mSeServerIp = seserver->getAttrStr("remote_addr", "");
		aos_assert(mSeServerIp != "");
		mSeServerPort = seserver->getAttrInt("remote_port", 0);
		aos_assert(mSeServerPort);
		mIsSeServer = true;
	}
}


AosProcServer::~AosProcServer()
{
	mServer->stopReading();
}


void
AosProcServer::msgRecved(const OmnConnBuffPtr &buff, const OmnTcpClientPtr &conn)
{
	//information reqid by three ways
	//1. procservice send messsage to me for register 
	//2. seserver  send command to me
	//<request>
	//	<item name="reqid"><![CDATA[service_cmds]]></item>
	//	<cmds>
	//		<cmd type="getotherinfo" >
	//			<process machine_name="xxx"  process_name="xxx"/>
	//			<process machine_name="xxx"  process_name="xxx"/>
	//		</cmd>
	//		<cmd type="ctrlprocess" >
	//			<process machine_name="xxx" process_name="xxx" action="startup"/>
	//			<process machine_name="xxx" process_name="xxx" action="shutdown"/>
	//			<process machine_name="xxx" process_name="xxx" action="getinformation"/>
	//		</cmd>
	//	</cmds>
	//	<register machine_doc="xxx" machine_name="xxx" machine_ip="xxx" machine_port="xxx">
	//		<![CDATA[xxx]]>
	//	</register>
	//</request>
	//notice : type as follow
	//			1.from the seserve
	//			2.from the jsp
	//			3.from the serviceserver
	mRundata = OmnApp::getRundata(); 
	aos_assert(buff && conn);
	char *data = buff->getData();
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "" AosMemoryCheckerArgs);
	mRundata->reset();
	if (!root)
	{
		OmnString errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << "Failed to parse the request!";
		sendResp(mRundata, conn);
		return;
	}
OmnScreen << "request -----:  " << root->toString() << endl;
	mRundata->setRequestRoot(root);
	OmnString  reqid = root->getChildTextByAttr("name", "reqid");
	if (reqid == "")
	{
		OmnString errmsg = "Failed to get the reqid!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << "Failed to get the reqid!";
		sendResp(mRundata, conn);
		return;
	}
	AosProcRequestPtr proc = AosProcRequest::getProc(reqid);
	if (proc)
	{
		proc->setProcServer(this);
		proc->proc(mRundata);
		sendResp(mRundata, conn);
		return;
	}
	return;

	if(reqid == "fromses")
	{
		//1.update machine's doc on the seserver
		//2.tell the seserver that it can get the latest message now
		OmnString objid;
		objid << "machine1";
		//retrieve doc by objid
		AosXmlTagPtr doc;
		bool objflag = retrieveDocByObjid(objid, doc);
		if(!objflag)
		{
			OmnString errmsg = "Fail to get the obj on seserver!";
			conn->smartSend(errmsg);
			return;
		}
		doc->setAttr("temperature", "60");
		bool rslt = modifyDocOnServer(doc);
		if(!rslt)
		{
			OmnString errmsg = "Fail to update the doc on seserver!";
			OmnAlarm << errmsg << enderr;
			conn->smartSend(errmsg);
			return;
		}

		//tell seserve now
		OmnString sendmsg;
		sendmsg<< "now you can get the latest message";
		conn->smartSend(sendmsg);
		return;
	}

	return;
}


void
AosProcServer::sendResp(const AosRundataPtr &rdata, const OmnTcpClientPtr conn)
{
	sendResp(conn, rdata->getErrcode(),
		rdata->getErrmsg(), rdata->getContents());
}

void
AosProcServer::sendResp(
	const OmnTcpClientPtr conn,
	const AosXmlRc errcode,
	const OmnString &errmsg,
	const OmnString &contents)
{
		OmnString resp;
		resp << "<Contents>";
		resp << contents;
		resp << "</Contents>";
		resp << "<status ";
		resp << " error=\"";
		if (errcode == eAosXmlInt_Ok) 
		{
			resp << "false\" code=\"200\"/>";
		}
		else
		{
			resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";
		}
		conn->smartSend(resp);
		return;
}

bool
AosProcServer::sendToProcService(const OmnString &req, const OmnString & machine_name, OmnString &resp)
{
	//1.connect to one machine which has defined in the cmd
	//2.get some messages from the machine.
	OmnString ip;
	int port;
	bool find = false;
	OmnString response;
	for(u32 i=0; i<mMachines.size(); i++)
	{
		if(mMachines[i].mMachineName== machine_name)
		{
			find = true;
			ip = mMachines[i].mMachineIp;
			port = mMachines[i].mMachinePort;
			break;
		}
	}
	if(!find)
	{
		response << "<record machine_name=\"" << machine_name << "\"><![CDATA[ there is no such machine ]]></record>";
		resp << response;
		return false;
	}
OmnScreen << "ip----: " << ip << "  port: " << port << endl;
	bool rslt = sendToServer(ip, port, req, response, mRundata);
	if(response == "")
	{
		response << "<record><![CDATA[ it is wrong to get the response ]]></record>";
		resp << response;
		return false;
	}
	if(!rslt)
	{
		return false;
	}
	resp << response;
	return true;
}

bool
AosProcServer::retrieveDocByObjid(const OmnString &objid, AosXmlTagPtr &doc)
{
	//login 
	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	bool loginrslt = login("yuhui", "12345", "yunyuyan_account", ssid, userid, userdoc, "100");
	if(!loginrslt)
	{
		OmnString errmsg = "Fail to login!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	OmnString siteid = "100";
	OmnString resp;
	AosXmlTagPtr orig_doc = retrieveDoc(siteid, ssid, objid, resp);
	if(!orig_doc)
	{
		OmnString errmsg = "Fail to retrieve the doc!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	doc = orig_doc;
	return true;
	
}

AosXmlTagPtr
AosProcServer::retrieveDoc(
		const OmnString &siteid, 
		const OmnString &ssid, 
		const OmnString &objid,
		OmnString &resp)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	

	//retrieve doc
	OmnString request = "<request >";
	request << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"objid\">" << objid << "</item>"
		<< "<item name=\"zky_editor\">true</item>"
		<< "</request>";

	resp << "<response>";
	sendToServer(mSeServerIp, mSeServerPort, request, resp, mRundata);
	aos_assert_r(resp != "", 0);
	resp << "</response>";
	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if(!child)
	{
		OmnAlarm << "Missing Child Tag!" <<enderr;
		return 0;
	}
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		return 0;
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if(!child1)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}
	AosXmlTagPtr child2 = child1->getFirstChild();
	if(!child2)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, ""  AosMemoryCheckerArgs);
	return redoc;
}

bool
AosProcServer::modifyDocOnServer(const AosXmlTagPtr &doc)
{
	//1.login on the seserver
	//2.update about the serviceserver information on the seserver
		
	//login
	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	bool loginrslt = login("yuhui", "12345", "yunyuyan_account", ssid, userid, userdoc, "100");
	if(!loginrslt)
	{
		OmnString errmsg = "Fail to login!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	//prepare the doc
	OmnString docstr = doc->toString();
	//update
	bool updaterslt = updateDocOnServer(ssid, docstr);
	if(!updaterslt)
	{
		OmnString errmsg = "Fail to update the doc on the seserver!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
}

bool
AosProcServer::updateDocOnServer(OmnString &ssid, OmnString &docstr)
{
	u32 trans_id = mTransId++;
	OmnString send_req = "<request>";
	send_req << "<item name=\"operation\">modifyObj</item>"
			 << "<item name=\"" << AOSTAG_SITEID << "\">" << "100" << "</item>"
			 << "<item name=\"rename\"><![CDATA[false]]></item>"
			 << "<item name=\"trans_id\">" << trans_id<< "</item>"
			 << "<item name=\"zky_ssid\">" << ssid << "</item>"
			 << "<xmlobj>" << docstr << "</xmlobj>"
			 << "</request>";
	OmnString resp;
	sendToServer(mSeServerIp, mSeServerPort, send_req, resp, mRundata);
	if(resp == "")
	{
		OmnString errmsg = "Failed to get the seserve's response!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
}

bool
AosProcServer::login(
			const OmnString &username,
			const OmnString &passwd,
			const OmnString &ctnr_name,
			OmnString &ssid,
			u64 &userid,
			AosXmlTagPtr &userdoc,
			const OmnString &siteid,
			const OmnString &cid)
{
	u32 trans_id = mTransId++;
	OmnString obj;
	obj << "<embedobj "
		<< AOSTAG_USERNAME << "=\""<< username <<"\" "
		<< " opr=\"login\" "
		<< AOSTAG_CLOUDID << "=\"" << cid <<"\" "
		<< AOSTAG_HPVPD << "=\"yyy_room_frame\" "
		<< AOSTAG_LOGIN_VPD << "=\"yyy_login\" "
		<< AOSCONFIG_CTNR << "=\""<< ctnr_name <<"\"><" 
		<< AOSTAG_PASSWD <<">"<< passwd <<"</"<< AOSTAG_PASSWD
		<<"></embedobj>";

	OmnString req = "<request>";
	req << "<command>" << obj <<"</command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
	 	<< "<item name='loginobj'>true</item>"
		<< "</request>";
	// <request>
	// <item name='zky_siteid'><![CDATA[100]]></item>
	// 		<item name='operation'><![CDATA[serverCmd]]></item>
	// 		<item name='loginobj'><![CDATA[true]]></item>
	// 		<item name='trans_id'><![CDATA[5]]></item>
	// 		<command>
	// 			<embedobj container="yunyuyan_account" zky_hpvpd="yyy_room_frame" 
	// 				rattrs="zky_category|sep418|zky_hpvpd|sep418|zky_objimg|sep418|zky_fans|sep418|zky_abmctnr|sep418|zky_realnm|sep418|zky_uname"
	// 				zky_lgnvpd="yyy_login" opr="login" zky_uname="yuhui"><zky_passwd>&lt;![CDATA[12345]]&gt;</zky_passwd>
	// 			</embedobj>
	// 		</command>
	// 		<zky_cookies>
	// 			<cookie zky_name="aos_userid_ck"><![CDATA[201074183101135610747320097]]></cookie>
	// 			<cookie zky_name="JSESSIONID"><![CDATA[999454DA3E45597FD08E8C9B0D6482C2]]></cookie>
	// 		</zky_cookies>
	// </request>

	OmnString resp;
	resp << "<response>";
	sendToServer(mSeServerIp, mSeServerPort, req, resp, mRundata);
	aos_assert_r(resp != "", 0);
	resp << "</response>";

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, ""  AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	AosXmlTagPtr contents = resproot ->getFirstChild("Contents");
	AosXmlTagPtr record = contents->getFirstChild();
	ssid = record->getAttrStr(AOSTAG_SESSIONID);
	userid = record->getAttrU64(AOSTAG_DOCID, 0);

	AosXmlTagPtr doc = parser.parse(record->toString(), ""  AosMemoryCheckerArgs);
	aos_assert_r(doc, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid!="", false);
//	userdoc = retrieveDocByObjid(siteid, ssid, objid, false);
//	aos_assert_r(userdoc, false);
//	userid = userdoc->getAttrU64(AOSTAG_DOCID, 0);
//	aos_assert_r(userid, false);
	return true;
}

bool
AosProcServer::sendToServer(
		const OmnString &addr,
		const int &port,
		const OmnString &send_req,
		OmnString &resp,
		const AosRundataPtr &rdata)
{
	OmnTcpClientPtr conn= getConn(addr, port);
	OmnString errmsg;
	if (!conn->connect(errmsg))
	{
		conn = 0;
		errmsg = "Failed to connect to the process server!";
		rdata->setError() << errmsg;
		return false;
	}
	aos_assert_rl(conn->smartSend(send_req.data(), send_req.length()), mLock, false);
	OmnConnBuffPtr buff;
	conn->smartRead(buff);
	returnConn(conn, addr);
	if (!buff)
	{
		errmsg = "Failed to read response!";
		rdata->setError() << errmsg;
		return false;
	}
	resp << buff->getData();
	return true;
}

OmnTcpClientPtr
AosProcServer::getConn(const OmnString &addr, const int &port)
{
	OmnTcpClientPtr conn;
    mLock->lock();
	deque<OmnTcpClientPtr> conns;
	map<OmnString, deque<OmnTcpClientPtr> >::iterator iter = mIdleConnsMap.find(addr);
	if(iter != mIdleConnsMap.end())
	{
		 conns = iter->second;
	}
	else
	{
		mIdleConnsMap[addr] = conns;
	}
    if (conns.size() > 0)
    {
     	deque<OmnTcpClientPtr>::reverse_iterator itr;
        itr = conns.rbegin();
        if (itr != conns.rend())
            conn = *itr;
        conns.pop_back();
        mLock->unlock();
        return conn;
    }
    else
    {
        conn = OmnNew OmnTcpClient("nn",
                addr, port, 1, eAosTLT_FirstFourHigh);
        OmnString errmsg;
        aos_assert_rl(conn->connect(errmsg), mLock, false);
        mLock->unlock();
        return conn;
    }
    mLock->unlock();
    return conn;
}

void
AosProcServer::returnConn(const OmnTcpClientPtr &conn, const OmnString &addr)
{
    mLock->lock();
	deque<OmnTcpClientPtr> conns;
	map<OmnString, deque<OmnTcpClientPtr> >::iterator iter = mIdleConnsMap.find(addr);
	if(iter != mIdleConnsMap.end())
	{
		 conns = iter->second;
	}
    conns.push_back(conn);
    mLock->unlock();
}
