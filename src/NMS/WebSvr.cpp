////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WebSvr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/WebSvr.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Network/Network.h"
#include "NMS/Nms.h"
#include "NMS/CapProc.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/XmlParser.h"



#include "Util1/Time.h"



OmnSingletonImpl(OmnWebSvrSingleton,
				 OmnWebSvr,
				 OmnWebSvrSelf,
				 "OmnWebSvr");



OmnWebSvr::OmnWebSvr()
:
mScanDbFreq(eScanDbFreq)
{
}


OmnWebSvr::~OmnWebSvr()
{
}


bool
OmnWebSvr::config(const OmnXmlParserPtr &parser)
{
	// 
	// Start the NMS server, which is defined by
	// 		<WebSvr>
	// 			<LocalNiid>
	// 			<LocalPort>
	// 			<LocalNumPorts>
	// 		</WebSvr>
	//
	int niid;
	int localPort;
	int numPorts;
	if (OmnNms::isAlg())
	{
		// 
		// If it is an ALG, we assume the following:
		//
		niid = 6;
		localPort = 5168;
		numPorts = 1;
	}
	else
	{
		OmnXmlItemPtr conf = parser->tryItem(OmnXmlItemName::eNmsConfig);
		if (conf.isNull())
		{
			return true;
		}

		conf = conf->tryItem(OmnXmlItemName::eWebSvr);
		if (conf.isNull())
		{
			return true;
		}

		niid = conf->getInt(OmnXmlItemName::eLocalNiid, -1);
		localPort = conf->getInt(OmnXmlItemName::eLocalPort, -1);
		numPorts = conf->getInt(OmnXmlItemName::eLocalNumPorts, 1);
	}

	OmnIpAddr ipaddr = OmnNetworkSelf->getIpAddr(niid);
	if (!ipaddr.isValid())
	{
		OmnAlarm << "Invalid NMS NIID: " << niid
			<< enderr;
		return false;
	}

	if (localPort <= 0)
	{
		OmnAlarm << OmnErrId::eAlarmConfigError
			<< "Invalid local port: " << localPort 
			<< ". " << enderr;
		return false;
	}


	OmnTrace << "To create WebSvr: " << ipaddr.toString()
		<< ":" << localPort << ":" << numPorts << endl;

	// 
	// Chen Ding, 05/20/2003
	//
	// mServer = OmnNew OmnTcpServer(ipaddr, localPort, 
	// 	numPorts, "WebSvr", OmnTcp::eTermByNewLine);

	return true;
}


bool
OmnWebSvr::start()
{
	if (!mServer.isNull())
	{
		OmnString err;
		if (!mServer->connect(err))
		{
			OmnAlarm << OmnErrId::eAlarmConfigError
				<< "Failed to start WebSvr TCP Server" << enderr;
			return false;
		}
	}

	if (mThread.isNull())
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "WebSvr", 0, false, true, __FILE__, __LINE__);
	}

	mThread->start();
	return true;
} 


bool
OmnWebSvr::stop()
{
	return true;
}


bool
OmnWebSvr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &)
{
/*    OmnTrace << "Enter OmnWebSvr::threadFunc." << endl;

	OmnConnBuffPtr buf;
	OmnTcpClientPtr conn;
	while ( state == OmnThrdStatus::eActive )
	{
		// 
		// If mServer is null, it reads the data from the database.
		//
		if (mServer.isNull())
		{
			if (OmnNms::isPrimary())
			{
				OmnTraceCP << "It is primary. Check Web Action" << endl;
				OmnValList<OmnSoWebActionPtr> actions;
				OmnSoWebAction so;
				so.getActions(actions);
				actions.reset();

				while (actions.hasMore())
				{
					OmnSoWebActionPtr action = actions.crtValue();
					actions.next();

					OmnTraceCP << "Process web action: " 
						<< action->toString() << endl;
					procAction(action);
					action->marketEntry();
					OmnTraceCP << "Marked" << endl;
				}
			}
			else
			{
				OmnTraceCP << "It is not primary" << endl;
			}

			OmnSleep(mScanDbFreq);
		}
		else
		{
			mServer->readFrom(buf, conn);
			procMsg(buf, conn);
		}
	}

    OmnTrace << "Leaving OmnWebSvr::threadFunc." << endl;
	*/
	return true;
}


/*
bool
OmnWebSvr::procMsg(const OmnConnBuffPtr &buf, const OmnTcpClientPtr &)
{
	OmnAlarm << OmnErrId::eAlarmProgError
		<< "No longer supported" << enderr;
	return false;
    if (buf.isNull())
	{
		//
		// Did not read anything. This should not happen.
		//
		OmnCommWarn << OmnErrId::eWarnCommError
			<< "Did not read anything from WebSvr: "
			<< enderr;
		return false;
	}

	const char *data = buf->getBuffer();

	OmnTraceCP << "<<<<<<<<<<<<<<<<<<<<<<<<<\n"
		<< data
		<< "\n<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;


	int cmd = atoi(data);
	int uid = atoi(&data[2]);

	switch (cmd)
	{
	case eEpAdded:
		 return epAdded(uid);
		 
	case eEpDeleted:
		 return epDeleted(uid);

	case eEpModified:
		 return epModified(uid);

	default:
		 OmnWarn << OmnErrId::eWarnProgError
			<< "Unrecognized Web command!" << enderr;
		 return false;
	}
}
	*/



bool
OmnWebSvr::signal(const int threadLogicId)
{
		    return true;
}


bool
OmnWebSvr::checkThread(OmnString &, const int tid) const
{
	return true;
}


/*	// 
bool
OmnWebSvr::procAction(const OmnSoWebActionPtr &action)
{
	// ObjId: Endpoint UID
	// String1: public EID
	// String2: private Eid
	//
OmnTrace << "procAction: " << action->toString() << endl;

	int cmd = action->getActionId();
	switch (cmd)
	{
	case eEpAdded:
		 epAdded(action->getString1());
		 break;
		 
	case eEpDeleted:
		 // 
		 // Chen Ding, 08/06/2003, 2003-0104
	 	 // 
		 // epDeleted(action->getString1());
		 epDeleted(action->getObjId());
		 break;

	case eEpModified:
		 epModified(action->getObjId());
		 break;

	case eAlgAdded:
		 algAdded(action->getObjId());
		 break;

	case eAlgDeleted:
		 algDeleted(action->getObjId());
		 break;

	case eAlgModified:
		 algModified(action->getObjId());
		 break;

	case eStartCapture:
		 if (!mCapProc.isNull())
		 {
			 mCapProc->startCapture(action);
		 }
		 else
		 {
			 OmnWarn << OmnErrId::eWarnProgError
				<< "No one registered to process capture!" << enderr;
		 }
		 break;

	case eStopCapture:
		 if (!mCapProc.isNull())
		 {
			 mCapProc->stopCapture(action);
		 }
		 else
		 {
			 OmnWarn << OmnErrId::eWarnProgError
				<< "No one registered to process capture!" << enderr;
		 }
		 break;

	default:
		 OmnWarn << OmnErrId::eWarnProgError
			<< "Unrecognized Web command: " << cmd 
			<< ":" << eStartCapture << enderr;
		 return false;
	}
	return true;
}
*/


bool
OmnWebSvr::registerCapProc(const OmnCapProcPtr &requester)
{
	if (!mCapProc.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "mCapProc is not null: " 
			<< mCapProc->getName()
			<< ". New requester: " 
			<< requester->getName()
			<< ". Will override!" << enderr;
	}

	mCapProc = requester;
	return true;
}

