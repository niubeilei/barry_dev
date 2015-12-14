////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
#include "ProcService/ProcRegister.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"
#include "ProcUtil/ManagedServer.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/SeXmlParser.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"
#include <deque>



using namespace std;

OmnSingletonImpl(AosProcRegisterSingleton,
                 AosProcRegister,
                 AosProcRegisterSelf,
                "AosProcRegister");

AosProcRegister::AosProcRegister()
:
mLock(OmnNew OmnMutex())
{
	mRundata = OmnApp::getRundata();
}


AosProcRegister::~AosProcRegister()
{
}

bool
AosProcRegister::start()
{
	return true;
}


bool
AosProcRegister::start(const AosXmlTagPtr &config)
{
	// 	<config ...>
	// 		<procmgr>
	// 			<servers>
	// 				<server ip="xxx" port="xxx"/>
	// 				<server ip="xxx" port="xxx"/>
	// 				...
	// 			</servers>
	// 		</procmgr>
	// 		...
	// 	</config>
	//
	mConfig = config;
	initServers();
	OmnThreadPtr thread;
	OmnThreadedObjPtr thisPtr(this, false);
	thread = OmnNew OmnThread(thisPtr, "ProcRegisterThrd", 0, true, true, __FILE__, __LINE__);
	thread->start();
	return true;
}

bool
AosProcRegister::initServers()
{
	mLock->lock();
	aos_assert_r(mConfig, false);
	AosXmlTagPtr servers = mConfig->getFirstChild("servers");
	aos_assert_r(servers, false);

	//clear mServers
	mServers.clear();
	//get the servers 
	AosXmlTagPtr server = servers->getFirstChild();
	while (server)
	{
		try
		{
			AosManagedServerPtr ss = OmnNew AosManagedServer(server, mRundata);
			OmnString errmsg;
			ss->connect(errmsg);
			mServers.push_back(ss);
		}
	
		catch (...)
		{
			OmnAlarm << "Failed to create managed server: " <<mRundata->getErrmsg() << enderr;
		}

		server = servers->getNextChild();
	}

	mLock->unlock();
	return true;
}

void
AosProcRegister::prepareReq(OmnString &req)
{
	OmnString machine_name = mConfig->getAttrStr("machine_name");
	OmnString machine_doc = mConfig->getAttrStr("machine_doc");
	OmnString local_addr= mConfig->getAttrStr("local_addr");
	OmnString local_port= mConfig->getAttrStr("local_port");
	req = "<req><servers><server remote_addr=\"127.0.0.1\" remote_port=\"5599\" >";
	req << "<request><item name=\"reqid\"><![CDATA[register_service]]></item><register machine_doc=\"" << machine_doc << "\" machine_name=\"" << machine_name <<"\" machine_ip=\"" << local_addr << "\" machine_port=\"" << local_port << "\">allinfo</register></request>"
		<< "</server></servers></req>";
	return;
}

bool    
AosProcRegister::registerToServer(
				const OmnString &req,
				OmnString &resp,
				OmnString &errmsg)
{
	//	<req>
	//		<servers>
	//    		<server ip="xxx" port="xxx">
	//    			<request>
	//    				<item name="reqid"><![CDATA[fromservice]]></item>
	//					<register machine_name="xxx" machine_ip="xxx" machine_port="xxx">xxx</register>
	//    			</request>
	//    		</server>
	//       	<server ip="xxx" port="xxx">
	//    			<request>
	//    				<item name="reqid"/>
	//					<register machine_name="xxx" machine_ip="xxx" machine_port="xxx">xxx</register>
	//    			</request>
	//    		</server>
	//            ...
	//      </servers>
	//	</req>
	//	<response>
	//		<servers>
	//			<server ip="xxx" port="xxx">
	//				<content>
	//				</content>
	//			</server>
	//			<server ip="xxx" port="xxx">
	//				<content>
	//				</content>
	//			</server>
	//		</servers>
	//	</response>
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(req, "" AosMemoryCheckerArgs);
	AosXmlTagPtr servers = root->getFirstChild("servers");
	aos_assert_r(servers, false);
	AosXmlTagPtr server = servers->getFirstChild();
	resp << "<response><servers>";
	while (server)
	{
		OmnString addr = server->getAttrStr(AOSCONFIG_REMOTE_ADDR);
		int port = server->getAttrInt(AOSCONFIG_REMOTE_PORT, 0);
		OmnString content;
		for (int i=0; i<(int )mServers.size(); i++)
		{
			if(addr == mServers[i]->getAddr() && port == mServers[i]->getPort())
			{
				AosXmlTagPtr request = server->getFirstChild("request");
				mServers[i]->connect(errmsg);
				mServers[i]->sendRequest(request->toString(), content, errmsg);
	//			mServers[i]->closeConn();
				resp << content;
			}
		}
		server = servers->getNextChild();
	}
	resp << "</servers></response>";
	return true;
	
}

bool
AosProcRegister::stop()
{
    return true;
}


bool
AosProcRegister::config(const AosXmlTagPtr&def)
{
	return true;
}


bool    
AosProcRegister::signal(const int threadLogicId)
{
	return true;
}


bool    
AosProcRegister::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosProcRegister::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		OmnString req;
		prepareReq(req);
		OmnString resp;
		OmnString errmsg;
		registerToServer(req, resp, errmsg);
		OmnSleep(10);
	}
	return true;
}
