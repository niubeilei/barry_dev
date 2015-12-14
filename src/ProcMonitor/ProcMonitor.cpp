////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class is used to manage a process. The monitored process supports
// the Heartbeat interface, which means the monitored process may send
// heartbeat message to this monitor; this monitor can send heartbeat
// messages to the monitored. This class can be configured with a 
// frequency at which it sends heartbeat messages to the monitored. 
// Each hearbeat message must be responded within a given time. If not,
// the monitor will send a second heartbeat message and waits for the
// response. If the monitored still fails responding, the monitored
// is considered dead. It will kill the process and restart the process.
//
// Modification History:
// 09/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ProcMonitor/ProcMonitor.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "Thread/Mutex.h"


AosProcMonitor::AosProcMonitor()
{
}


AosProcMonitor::~AosProcMonitor()
{
}


AosNetReqProcPtr	
AosProcMonitor::clone()
{
	return OmnNew AosProcMonitor();
}


bool
AosProcMonitor::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosProcMonitor::stop()
{
	OmnScreen << "AosProcMonitor is stopping!" << endl;
	mIsStopping = true;
	return true;
}


bool
AosProcMonitor::procRequest(const OmnConnBuffPtr &buff)
{
	mConn = buff->getConn();
	aos_assert_r(mConn, false);

	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "\nTo process request: (transid: " << req->getTransId()
			<< "): " << req->getData() << endl;
	}

	AosXmlRc errcode = eAosXmlInt_General;
	if (mIsStopping)
	{
		sendResp(req, errcode, "Server is stopping", "");
		return false;
	}

	OmnString errmsg;
	char *data = req->getData();

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "");
	if (!root) 
	{
		errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	mOperation = AosProcMtrOpr::toEnum(root->getAttrStr(AOSTAG_OPERATION));

	bool rslt = false;
	switch (mOperation)
	{
	case AosProcMtrOpr::eHeartbeat:
		 rslt = procHeartbeat(req, root);
		 break;

	case AosProcMtrOpr::eHeartbeatResp:
		 rslt = procHeartbeatResp(req, root);
		 break;

	default:
		 OmnAlarm << "Unrecognized request: " << root->toString() << enderr;
		 sendResp(req, eAosXmlInt_General, "Unrecognized request", "");
		 rslt = false;
		 break;
	}

	return false;
}


bool
AosProcMonitor::procHeartbeat(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &msg)
{
	// The heartbeat message should be in the form:
	// 	<heartbeat procid="xxx" name="xxx"/>
	mProcessid = req->getAttrStr(AOSTAG_PROCESSID);
	mProcessName = req->getAttrStr(AOSTAG_NAME);
	sendResp(req, eAosXmlInt_General, "", "");
	mLastHeartbeatTime = OmnGetSecond();
	mHeartbeatReceived = true;
	return true;
}


bool
AosProcMonitor::procHeartbeatResp(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &msg)
{
	// A heartbeat response was received.
	mHeartbeatRespReceived = true;
	return true;
}


bool
AosProcMonitor::restartMonitored()
{
	// The monitored process failed. It needs to restart the 
	// process. The command line for restarting the process
	// is in mStartScript. After restarting, it should obtain
	// the process ID of the process being started.
	//
	// IMPORTANT: the class is locked when it is called and
	// will be locked during the entire restart process
	
	// 1. Kill the process
	killMonitored();

	// 2. Wait a little while before restarting it
	if (mWaitBeforeRestartSec > 0)
	{
		OmnWait::getSelf()->wait(mWaitBeforeRestartSec, 0);
	}

	// 3. Start the process
	mHeartbeatReceived = false;
	mLastHeartbeatTime = 0;
	system(mStartScript);
	
	// 4. Wait until the process sends a heartbeat 
	OmnWait::getSelf()->wait(mRestartWaitSec, 0);
	aos_assert_r(mHeartbeatReceived, false);
	return true;
}


bool
AosProcMonitor::killMonitored()
{
	// This function kills the monitored process
	OmnNotImplementedYet;
	return false;
}


bool
AosProcMonitored::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	// This thread function wakes up at the given frequency, send 
	// a heartbeat message to the monitored, wait for the response. 
	// If the monitored failed responding, it will re-send the heartbeat.
	// If still failed responding, it will kick off the restart procedure.
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (!mStarted)
		{
			// The monitored process has not been started yet.
			// Sleep!
			mLock->unlock();
			OmnWait::getSelf()->wait(mMonitorFreqSec, 0);
			continue;
		}

		if (!mConn)
		{
			// This should never happen
			mLock->unlock();
			OmnAlarm << "Missing the connection!" << enderr;
			OmnWait::getSelf()->wait(mMonitorFreqSec, 0);
			continue;
		}

		// 1. Send a heartbeat to the monitored
		mHeartbeatRespReceived = false;
		sendHeartbeat();
		mLock->unlock();

		// 2. Wait for the response
		OmnWait::getSelf()->wait(mRespWaitSec, 0);

		// 3. Check whether response has been received
		mLock->lock();
		if (mHeartbeatRespReceived)
		{
			// The response was received.
			mLock->unlock();
			OmnWait::getSelf()->wait(mMonitorFreqSec, 0);
			continue;
		}

		OmnAlarm << "Failed to receive the response!" << enderr;
		// No response was received. Re-send the heartbeat
		resendHeartbeat();
		mLock->unlock();

		// 4. Wait for the re-send response
		OmnWait::getSelf()->wait(mResendRespWaitSec, mResendRespWaitMsec);

		// 5. Check the response
		mLock->lock();
		if (mHeartbeatRespReceived)
		{
			// Response is retrieved
			mLock->unlock();
			OmnWait::getSelf()->wait(mMonitorFreqSec, 0);
			continue;
		}

		// 6. No response was received for the second heartbeat. 
		// The monitored failed. Need to restart it.
		restartMonitored();
		mLock->unlock();
	}
	return true;
}


void
AosProcMonitor::sendResp(
		const AosWebRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents) 
{
	// All responses are in the form:
	// 	<status error="true|false" code="xxx">error message(as needed)</status>
	// 	contents
	OmnString resp = "<status ";
	resp << " error=\"";
	if (errcode == eAosXmlInt_Ok) 
	{
		resp << "false\" code=\"200\"/>";
	}
	else 
	{
		resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";
	}

	if (contents != "" ) resp << contents;

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< "): " << resp << endl;
	}
	req->sendResponse(resp);
}

