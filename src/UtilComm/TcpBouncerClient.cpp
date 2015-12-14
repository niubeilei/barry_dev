////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncerClient.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpBouncerClient.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/Select.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/BandWidthMgr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncer.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/ReturnCode.h"
#include "util_c/modules.h"



AosTcpBouncerClient::AosTcpBouncerClient(
			const AosTcpBouncerPtr &bouncer,
			const OmnTcpClientPtr &conn, 
			const AosTcpBouncer::BounceType bm, 
			const u32 bytesExpected)
:
mClientId(0),
mStatus(eIdle),
mBounceType(bm),
mConn(conn),
mBouncer(bouncer), 
mBytesReceived(0),
mBytesExpected(bytesExpected)
{
}


// 
// The client received something. This funciton updates the 
// following:
//	mBytesReceived
//	mRepeated
//	mConn
//	mStatus
//
// If the mListener is not null, it informs the mListener.
//
void
AosTcpBouncerClient::msgReceived(const OmnConnBuffPtr &buff)
{
	OmnTrace << "Client: " << mClientId << " received: " 
		<< buff->getDataLength() << endl;

	AosTcpBouncerClientPtr selfPtr(this, false);

	//
	// Inform the listener
	//
	AosTcpBouncer::Action action;
	if (mListener)
	{
		action = mListener->msgReceived(selfPtr, buff);
		switch (action)
		{
		case AosTcpBouncer::eInvalidAction:
			 // 
			 // This is an error
			 // 
			 aos_alarm("InvalidAction");
			 return;

		case AosTcpBouncer::eIgnore:
			 // 
			 // Do nothing about the receiving. 
			 // 
			 return;

		case AosTcpBouncer::eContinue:
			 break;

		default:
			 aos_alarm("Unrecognized action: %d", action);
			 return;
		}
	}

	// 
	// Process the received
	//
	OmnRslt rslt;
	
	int totalLen = 0;
	int offset = 0;
	int lengthToSend = 0;
	
	switch (mBounceType)
	{
	case AosTcpBouncer::eSinker:
		 // 
		 // Simply do nothing
		 // 
		 mBytesReceived += buff->getDataLength();
		 if (mBytesReceived >= mBytesExpected)
		 {
		 	mBouncer->connFinished(mClientId);
			mConn->closeConn();
		 }
		 break;

	case AosTcpBouncer::eSimpleBounce:
		 // 
		 // We will bounce whatever received back without any manipulations
		 // 
		 if (mListener)
		 {
			// 
			// Before bouncing, inform the listener.
			//
			action = mListener->readyToBounce(
				selfPtr, buff->getBuffer(), buff->getDataLength());
			switch (action)
			{
			case AosTcpBouncer::eInvalidAction:
			 	 aos_utilcomm_alarm("InvalidAction");
			 	 return;

			case AosTcpBouncer::eContinue:
				 break;

			case AosTcpBouncer::eIgnore:
				 // 
				 // Do not bounce.
				 // 
				 return;
	
			default:
				 mErrmsg = "Unrecognized action: %d";
				 mErrmsg << action;
				 aos_utilcomm_alarm((char *)mErrmsg.data());
				 mStatus = eFailed;
				 mBouncer->removeClient(selfPtr);
				 return;
			}
		 }

		 // 
		 // To send
		 //
		//rslt = mConn->writeTo(buff->getBuffer(), 
		//		buff->getDataLength());
		 mStatus = eBouncing;
		 if (!rslt)
		 {
			aos_utilcomm_alarm("Failed to send data: %d", 
				rslt.toString().data());
			mStatus = eFailed;

			if (mListener)
			{
				action = mListener->bounceFailed(selfPtr, 
					buff->getBuffer(), 
					buff->getDataLength());

				switch (action)
				{
				case AosTcpBouncer::eIgnore:
					 break;

				case AosTcpBouncer::eAbort:
					 mErrmsg = "Failed to send: ";
					 mErrmsg << rslt.toString();
					 mBouncer->removeClient(selfPtr);
					 return;

				case AosTcpBouncer::eInvalidAction:
					 mErrmsg = "Invalid action";
					 aos_utilcomm_alarm(mErrmsg.data());
					 mBouncer->removeClient(selfPtr);
					 return;

				default:
					 mErrmsg = "Unrecognized action: %d";
					 mErrmsg << action;
					 aos_utilcomm_alarm(mErrmsg.data());
					 mBouncer->removeClient(selfPtr);
					 return;
				}
			}
		 }
		 return;

	case AosTcpBouncer::eSpeedLimite:
		 // 
		 // We will bounce whatever received back without any manipulations
		 // 
		 if (mListener)
		 {
			// 
			// Before bouncing, inform the listener.
			//
			action = mListener->readyToBounce(
				selfPtr, buff->getBuffer(), buff->getDataLength());
			switch (action)
			{
			case AosTcpBouncer::eInvalidAction:
			 	 aos_utilcomm_alarm("InvalidAction");
			 	 return;

			case AosTcpBouncer::eContinue:
				 break;

			case AosTcpBouncer::eIgnore:
				 // 
				 // Do not bounce.
				 // 
				 return;
	
			default:
				 mErrmsg = "Unrecognized action: %d";
				 mErrmsg << action;
				 aos_utilcomm_alarm((char *)mErrmsg.data());
				 mStatus = eFailed;
				 mBouncer->removeClient(selfPtr);
				 return;
			}
		 }

		 // 
		 // To send
		 //
		 totalLen = buff->getDataLength();
		 offset = 0;
		 lengthToSend = 0;
		 while(offset < totalLen)
		 {
			 AosBandWidthMgrPtr bwMgr = mBouncer->getBWMgr();
			 if(bwMgr)
			 {
			 	bwMgr->getPermission(totalLen - offset,lengthToSend);
			 }
		 
			 //rslt = mConn->writeTo(buff->getBuffer()+offset , 
			//		lengthToSend);
			 offset += lengthToSend;
			 mStatus = eBouncing;
			 if (!rslt)
			 {
				aos_utilcomm_alarm("Failed to send data: %d", 
					rslt.toString().data());
				mStatus = eFailed;

				if (mListener)
				{
					action = mListener->bounceFailed(selfPtr, 
						buff->getBuffer(), 
						buff->getDataLength());

					switch (action)
					{
					case AosTcpBouncer::eIgnore:
						 break;

					case AosTcpBouncer::eAbort:
						 mErrmsg = "Failed to send: ";
						 mErrmsg << rslt.toString();
						 mBouncer->removeClient(selfPtr);
						 return;

					case AosTcpBouncer::eInvalidAction:
						 mErrmsg = "Invalid action";
						 aos_utilcomm_alarm(mErrmsg.data());
						 mBouncer->removeClient(selfPtr);
						 return;

					default:
						 mErrmsg = "Unrecognized action: %d";
						 mErrmsg << action;
						 aos_utilcomm_alarm(mErrmsg.data());
						 mBouncer->removeClient(selfPtr);
						 return;
					}
				}
			 }
		 }
		 return;

	default:
		 mErrmsg << "Unrecognized bounce type: " << mBounceType;
		 aos_utilcomm_alarm((char *)mErrmsg.data());
		 mStatus = eFailed;
		 return;
	}	

	return;
}


void
AosTcpBouncerClient::registerListener(const AosTcpBouncerListenerPtr &l)
{
	mListener = l;
}


int
AosTcpBouncerClient::getSock() const
{
	return mConn->getSock();
}


OmnTcpClientPtr
AosTcpBouncerClient::getConn() const
{
	return mConn;
}

