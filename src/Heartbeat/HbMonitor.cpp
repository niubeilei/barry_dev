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
// 	This class is used to monitor a target. It assumes that 
// 	two UDP connections to the monitored. Periodically, it sends 
// 	a heartbeat message through both connections (now it is UDP).
// 	As soon as the monitored receives the heartbeat message, it 
// 	should respond the message. 
//
// 	One must call start() to start the monitoring.
//
// 	1. OmnHeartbeat calls sendHeartbeat() to send heartbeat messages.
// 	2. OmnHeartbeat waits for a 'while'
// 	3. OmnHeartbeat calls checkHeartbeat() to determine heartbeat.
// 	   If:
// 		a. A negative response has been read, or
// 		b. None response was read.
// 	   then the heartbeat failed.
//
//  When heartbeat fails, it informs its callback (i.e., OmnHeartbeatMgr)
//  (by calling heartbeatFailed() function), which may call its callback 
//  to inform the fact. But none of them needs to do anything. 
//
//	Recovery process is as follows:
//	1. When heartbeat failure is detected, it sets flags.
//	2. In the next two beats, it does nothing.
//	3. Then it calls restore() to restore the program.
//
//	State Machine:
//	1. When program starts, state = eStartup.
//	2. In eStartup, when sendHeartbeat() is called, mControlTick 
//	   decrements. When mControlTick becomes 0, it changes state to eNormal.
//	3. In eNormal, when sendHeartbeat() is called, it sends heartbeat
//	   messages through both comm to the target. 
//	4. In eNormal, when checkHeartbeat() is called, it checks whether
//	   heartbeat failed. If yes, it changes the state to eToRecover.
//	   Otherwise, no state change.
//	5. In eToRecover, when sendHeartbeat() is called, it decrements
//	   mControlTick. If mControlTick is not 0, do not send heartbeat.
//	   Otherwise, it calls restore(), which will change the state to 
//	   eRecovering. 
//	6. In eRecovering, when sendHeartbeat() is called, it decrements
//	   mControlTick. If not 0, do not send heartbeat. Othewrise, 
//	   it changes state to eNormal.    
//
// Modification History:
// 
// 	
// 		Change the communication part from TCP to UDP. It appeared that
// 		TCP had some problems. 
// 		1. Make mComm2 mandatory.
//
// 	
// 		It appeared that when running this class, after sending a 
// 		request and before setting mResp1Received = false, the response
// 		is already back. When this happens, we will miss the response
// 		and consider the heartbeat failed. We will set mResp1Received = 
// 		false before sending. In addition, we will check heartbeat 
// 		for 3 times before declare heartbeat fails.
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Heartbeat/HbMonitor.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Event/Event.h"
#include "Heartbeat/HbObj.h"
#include "Logger/Log.h"
#include "Message/Msg.h"
#include "Util/SerialFrom.h"
#include "Porting/Random.h"
#include "Porting/GetTime.h"
#include "Util/ProcUtil.h"
#include "UtilComm/ConnBuff.h"
#include "Util1/Time.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"


OmnHbMonitor::OmnHbMonitor(const OmnHbObjPtr &callback, const AosXmlTagPtr &def)
:
OmnCommObj("HbMonitor"),
mIsMonitoredOk(true),
mHeartbeatReq(OmnNew OmnSmHbReq()),
mTransId(100),
mCallback(callback),
mNeedToKill(false),
mStatus(eStartup),
mControlTick(eStartTick)
{
	configure(def);
}


bool
OmnHbMonitor::config(const AosXmlTagPtr &def)
{
	//
	// 'def' contains the following
	// 	<def AOSTAG_NAME="xxx" AOSTAG_MONITORED_ID="xxx" ...>
	// 		<Comm1 .../>
	// 		<Comm2 .../>
	//	<StartCommand>
	//	<RecoverCommand>
	//	<DbMonitoredId>
	//	<ProgramName>	; Used to kill the program
	//		
	aos_assert_r(!def.isNull(), false);

	mName = def->getAttrStr(AOSTAG_NAME);
	mMonitorId = def->getAttrStr(AOSTAG_MONITORED_ID);
	mProgramName = def->getAttrStr(AOSTAG_PROGRAM_NAME);
	mStartCommand = def->getAttrStr(AOSTAG_STARTSCRIPT);
	aos_assert_r(mStartCommand != "", false);

	OmnString err;
	AosXmlTagPtr comm1 = def->firstChild("Comm1");
	aos_assert_r(!comm1.isNull(), false);
	mComm1 = OmnCommGroup::createGroup(comm1);

	AosXmlTagPtr comm2 = def->getFirstChild("Comm2");
	aos_assert_r(!comm2.isNull(), false);
	mComm2 = OmnCommGroup::createGroup(comm2);
	return true;
}


OmnHbMonitor::~OmnHbMonitor()
{
}


bool
OmnHbMonitor::startHeartbeat()
{
	// It starts reading on the two comms.
	aos_assert_r(!mComm1.isNull(), false);
	aos_assert_r(!mComm1.isNull(), false);
	OmnCommObjPtr thisPtr(this, false);
	mComm1->startReading(thisPtr);
	mComm2->startReading(thisPtr);

	OmnScreen << "To start HbMonitor: " << mName << endl;
	return true;
}


void
OmnHbMonitor::sendHeartbeat()
{
	// It sends a heartbeat message to the remote peer on mComm1
	// and mComm2, if mComm2 is valid. 
	
	mIsNegativeResp = false;
	mRespReceived = false;

	// 
	// Send the heartbeat message through mComm1
	//
	mTransId1 = mTransId++;
	mHeartbeatReq->setAttr(AOSTAG_TRANSID, mTransId1);
	mComm1->sendTo(mHeartbeatReq->toString());

	// 
	// Send the heartbeat message through mComm2
	//
	mTransId2 = mTransId++;
	mHeartbeatReq->setAttr(AOSTAG_TRANSID, mTransId2);
	mComm2->sendTo(mHeartbeatReq->toString());
	return;
}


bool		
OmnHbMonitor::checkHeartbeat()
{
	// This function assumes heartbeat messages have been sent to 
	// both mComm1 and mComm2 a while ago and it has left enough
	// time to get the responses back (currently 5 seconds). 
	//
	// Heartbeat failed if:
	// 1. A negative response has been read, or
	// 2. None response has been read
	//
	// If heartbeat failed, it returns false. Otherwise, return true.
	
	if (mIsNegativeResp || !mRespReceived)
	{
		OmnAlarm << OmnErrId::eAlarmHeartbeatError
			<< "Negative response read: " 
			<< mName << ":"
			<< mErrmsg << enderr;
		return heartbeatFailed();
	}
	return true;
}


void
OmnHbMonitor::restore()
{
			<< OmnSysObj::getMsgId(buff) << flushlog;
		mCallback->controlMsgRead(mName, buff);
	}
	else
	{
		//
		// It is a heartbeat response.
		//
		OmnSerialFrom s(buff);
		s.popMsgId();
		OmnSmHbResp hbRes; 
		hbRes.serializeFrom(s);

		OmnTraceHB << "\n<<<<<<<<<" << OmnGetTime() 
			<< ":" << mName << "\n"
			<< hbRes.toString()
			<< "\n<<<<<<<<<" << endl;

		// 
		// Read the response. If it's negative, the heartbeat fails.
		//
		if (!hbRes.isPositive())
		{
			OmnAlarm << OmnErrId::eAlarmHeartbeatError
				<< mName << " "
				<< "Failed the heartbeat: " 
				<< hbRes.toString() << enderr;

			OmnHeartbeatLog << "Read negative response: " 
				<< hbRes.toString() << flushlog;

			mIsNegativeResp = true;
			mNegativeResp = hbRes;
			return true;
		}

		if (hbRes.getTransId() == mTransId1)
		{
			mResp1Received = true;
		}
		else if (hbRes.getTransId() == mTransId2)
		{
			mResp2Received = true;
		}
		else
		{
			//
			// It is a heartbeat response, but the transaction ID
			// does not match. This is an error.
			//
			OmnCommAlarm << OmnErrId::eAlarmHeartbeatError
				<< "Read a heartbeat response for: " 
				<< mName 
				<< " but transId mismatch: ("
				<< hbRes.getTransId()
				<< ", )" << enderr;
		}
	}

	return true;
}


void
OmnHbMonitor::stopHeartbeat()
{
	OmnAlarm << OmnErrId::eAlarmProgError
		<< "Not implemented yet!" << enderr;
}


bool
OmnHbMonitor::killMonitored()
{
	return OmnProcUtil::killProc(mProgramName);
}

#endif
