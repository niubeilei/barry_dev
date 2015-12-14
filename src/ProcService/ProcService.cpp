////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
#include "ProcService/ProcService.h"
#include "ProcService/ProcControler.h"

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
#include "UtilComm/TcpServer.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"
#include <deque>



using namespace std;

OmnSingletonImpl(AosProcServiceSingleton,
                 AosProcService,
                 AosProcServiceSelf,
                "AosProcService");

AosProcService::AosProcService()
:
mLock(OmnNew OmnMutex())
{
}


AosProcService::~AosProcService()
{
}

bool
AosProcService::start()
{
	return true;
}


bool
AosProcService::start(const AosXmlTagPtr &config)
{
	// 	<config ...>
	// 		<procservice local_addr="xxx" local_port="xxx">
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
	mAddr = mConfig->getAttrStr("local_addr", "");
	if(mAddr == "")
	{
		OmnString errmsg = "Failed to get the address!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mPort = mConfig->getAttrInt("local_port", 0);
	if(!mPort)
	{
		OmnString errmsg = "Failed to get the port!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	startListening();
	OmnThreadPtr thread;
	OmnThreadedObjPtr thisPtr(this, false);
	thread = OmnNew OmnThread(thisPtr, "ProcServiceThrd", 0, true, true, __FILE__, __LINE__);
	thread->start();
	return true;
}

void
AosProcService::msgRecved(const OmnConnBuffPtr &buff, const OmnTcpClientPtr &conn)
{
	//	<request>
	//		<cmds>
	//			<cmd type="ctrlprocess" >
	//				<proccess process_name="xxx" action="startup"/>
	//			</cmd>
	//			<cmd type="ctrlprocess" >
	//				<proccess process_name="xxx" action="shutdown"/>
	//			</cmd>
	//			<cmd type="ctrlprocess" >
	//				<proccess process_name="xxx" action="getinformation"/>
	//			</cmd>
	//		</cmds>
	//	</request>
	//	response:
	//		<cmds>
	//			<cmd>
	//				<content>
	//				</content>
	//			</cmd>
	//			<cmd>
	//				<content>
	//				</content>
	//			</cmd>
	//		</cmds>
	aos_assert(buff && conn);
	char *data = buff->getData();
	AosXmlParser parser;
	AosXmlTagPtr req = parser.parse(data, "" AosMemoryCheckerArgs);
	AosXmlTagPtr cmds = req->getFirstChild("cmds");
	aos_assert(cmds);
	AosXmlTagPtr cmd = cmds->getFirstChild();
	OmnString resp;
	while (cmd)
	{
		OmnString type = cmd->getAttrStr("type", "");
		if (type == "ctrlprocess")
		{
			AosXmlTagPtr child = cmd->getFirstChild();
			bool rslt = doComand(child, resp);
			if(!rslt)
			{
				OmnString errmsg = "Failed to control proccess!";
				OmnAlarm << errmsg << enderr;
			}
		}
		else
		{
			OmnString errmsg = "there is not such type!";
			OmnAlarm << errmsg << enderr;
		}
		cmd = cmds->getNextChild();
	}
	//response
OmnScreen << "procservice response : " << resp.toString() << endl;
	conn->smartSend(resp);
	return;
	
}

bool
AosProcService::doComand(AosXmlTagPtr &cmd, OmnString &resp)
{
	bool rslt = AosProcControler::getSelf()->controlProcess(cmd, resp);
	if(!rslt)
	{
		OmnString errmsg = "Fail to do the comand!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
}

bool
AosProcService::stop()
{
    return true;
}


bool
AosProcService::config(const AosXmlTagPtr &def)
{
	return true;
}


bool    
AosProcService::signal(const int threadLogicId)
{
	return true;
}


bool    
AosProcService::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosProcService::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		OmnSleep(10);
	}
	return true;
}

void
AosProcService::startListening()
{
	mServer = OmnNew OmnTcpServer(mAddr, mPort, 1, "Proc Service", eAosTLT_FirstFourHigh);

	OmnTcpListenerPtr thisptr(this, false);
	mServer->setListener(thisptr);
	mServer->startReading();
}
