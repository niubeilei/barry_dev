////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpTrafficGenClient.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpTrafficGenClient.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/Select.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Random/RandomUtil.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/ReturnCode.h"
#include "PacketCheck/PCTestsuite.h"


AosTcpTrafficGenClient::AosTcpTrafficGenClient(
			const AosTcpTrafficGenPtr &trafficGen,
			const OmnTcpCltGrpPtr &group,
			int clientId,
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int repeat, 
			const int bytesExpected, 
			char * dataToSend, 
			const int dataLen, 
			const AosSendBlockType sendBlockType,
			const u32 sendBlockSize,
			const SendMode sendMode,
			const int index, 
			const AosTcpTrafficGenListenerPtr &listener)
:
mClientId(clientId),
mLocalAddr(localAddr),
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mRepeat(repeat),
mSuccessed(0),
mFailed(0),
mLock(OmnNew OmnMutex()),
mIndex(index),
mStatus(eIdle),
mReceiveMode(eBouncing),
mGroup(group),
mListener(listener),
mTrafficGen(trafficGen),
mSentPos(0),
mBytesReceived(0),
mBytesExpected(bytesExpected),
mRecvCursor(0),
mDataToSend(dataToSend),
mDataLen(dataLen),
mDataSendingStarted(0),
mSendBlockType(sendBlockType),
mSendBlockSize(sendBlockSize),
mSendMode(sendMode),
mRecvFinished(false),
mConnStartTick(0),
mConnLastRcvTick(0),
mBytesSent(0),
mCheckContent(false),
mInputDataType(eBuffer)
{
}

AosTcpTrafficGenClient::AosTcpTrafficGenClient(
            const AosTcpTrafficGenPtr &trafficGen,
            const OmnTcpCltGrpPtr &group,
            int clientId,
            const OmnIpAddr &localAddr,
            const OmnIpAddr &remoteAddr,
            const int remotePort,
            const int repeat,
            const AosSendBlockType sendBlockType,
            const SendMode sendMode,
            const int index,
			const AosTcpTrafficGenListenerPtr &listener,
			AosPCTestsuite* testSuite)
:
mClientId(clientId),
mLocalAddr(localAddr),
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mRepeat(repeat),
mSuccessed(0),
mFailed(0),
mLock(OmnNew OmnMutex()),
mIndex(index),
mStatus(eIdle),
mReceiveMode(ePacketCheck),
mGroup(group),
mListener(listener),
mTrafficGen(trafficGen),
mSentPos(0),
mBytesReceived(0),
mRecvCursor(0),
mDataSendingStarted(0),
mSendBlockType(eAosSendBlockType_Fixed),
mSendMode(sendMode),
mRecvFinished(false),
mConnStartTick(0),
mConnLastRcvTick(0),
mBytesSent(0),
mCheckContent(false),
mTestSuite(testSuite),
mInputDataType(eTestCaseConfigFile)
{
}


//
//This function compares the difference of expected and result value.
//note that the comparing method is by random postion,considering efficiency.
//
//if same ,it returns 0;otherwise return 1
//
int
AosTcpTrafficGenClient::validateResult(char *expectedValue, char *resultValue)
{
	// if (!(expectedValue || resultValue))
	if (!expectedValue || !resultValue)
	{
		return 1;
	}

	size_t len = 0;
	int position = 0;
	if ((len = strlen(expectedValue)) != strlen(resultValue))
	{
		return 1;
	}

	if (len <= 0)
	{
		return 0;
	}

	//generate random position
	position = OmnRandom::nextInt1(0, len - 1);
	//end of generate random position
	
	if (expectedValue[position] == resultValue[position])
	{
		return 0;
	}

	return 1;
}

// 
// The client received something. This funciton updates the 
// following:
//	mBytesReceived
//	mSuccessed
//	mConn
//	mStatus
//
// If the mListener is not null, it informs the mListener.
//
void
AosTcpTrafficGenClient::msgReceived(const OmnConnBuffPtr &buff)
{
	OmnTrace << "Client: " << mClientId << " received: " 
		<< buff->getDataLength() << endl;
	// u32 len = buff->getDataLength();
	// bool failed = false;

	//
	// add by lxx update mConnLastRcvTick
	//
	mConnLastRcvTick = OmnTime::getSecTick();
	
	switch (mReceiveMode)
	{
/*
	case eInteractive:
		 // 
		 // The data received should match mDataToRecv.
		 //
		 if (!mDataToRecv)
		 {
			OmnAlarm << "DataToRecv is null" << enderr;
			return;
		 }

		 if (mRecvCursor < 0 || mRecvCursor >= mBytesExpected)
		 {
			OmnAlarm << "mRecvCursor incorrect: " 
				<< (unsigned int)mRecvCursor << enderr;
			failed = true;
		 }
		 else if (mBytesExpected - mRecvCursor < len)
		 {
			OmnAlarm << "Received is too long: " 
				<< (unsigned int)len << ", " 
				<< (unsigned int)mBytesExpected << ", " 
				<< (unsigned int)mRecvCursor << enderr;
			failed = true;
		 }
		 else if (memcmp(mDataToRecv + mRecvCursor, buff->getBuffer(), len) != 0)
		 {
			OmnAlarm << "Received mismatch: " << buff->getBuffer() << enderr;
			failed = true;
		 }
		 else
		 {
		 	mRecvCursor += len;
		 }

		 break;
*/

	case eBouncing:
		 // 
		 // The received is the same as the sent
		 // 
//cout << "Client: " << mClientId << " received total: " << mBytesReceived + buff->getDataLength() << ":" << 
// buff->getDataLength() << endl;
		 if (memcmp(mDataToSend + mBytesReceived, buff->getBuffer(), 
				buff->getDataLength()) == 0 &&
			 mBytesReceived + buff->getDataLength() <= mDataLen)
		 {
			// 
			// Receiving is correct
			// 
			break;
		 }
		
		 // 
		 // Receiving incorrect
		 // 
		 OmnAlarm << "Receive failed" << enderr;

		 if (mListener)
		 {
			AosTcpTrafficGenClientPtr selfPtr(this, false);
			AosTcpTrafficGen::Action action;
			mListener->recvFailed(selfPtr, buff, action);
			switch (action)
			{
			case AosTcpTrafficGen::eIgnore:
				 break;

			case AosTcpTrafficGen::eStop:
				 return;

			default:
				 aos_alarm("Unrecognized action: %d", action);
				 return;
			}
		 }
		 else
		 {
			mStatus = eFailed;
			mErrmsg << "Receiving failed. Received: "
				<< mBytesReceived << ". Expected: " << (int)mDataLen;
			return;
		 }
		 break;
	
	case ePacketCheck:
		break;
	
	default:
		 mErrmsg << "Unrecognized receive mode: " << mReceiveMode;
		 aos_alarm((char *)mErrmsg.data());
		 mStatus = eFailed;
		 return;
	}	

	if (mListener)
	{
		AosTcpTrafficGenClientPtr thisPtr(this, false);
		mListener->msgRecved(thisPtr, buff);
	}

	mLock->lock();
	mBytesReceived += buff->getDataLength();

	if(mInputDataType == eBuffer)
	{
		if (mBytesReceived >= mBytesExpected)
		{
			if (mSentPos < mDataLen)
			{
				mStatus = eFailed;
				mLock->unlock();
				mErrmsg << "Program error: "
					<< mBytesExpected << ", "
					<< mBytesExpected << ", " << mSentPos << ", " << mDataLen;
				aos_alarm((char *)mErrmsg.data());
				return;	
			}	
			//modified by <a href=mailto:xw_cn@163.com>xiawu</a>
			if (validateResult(mDataToSend, buff->getBuffer()) != 0)
			{
				cout<<"validate the content of result error randomly"<<endl;
				mStatus = eReceiveFinished;
				mConn = 0;
				mRecvFinished = true;
			}
			//end of modified
			
			mSuccessed++;
			mConn = 0;
			mStatus = eReceiveFinished;
			OmnTrace << "Client: " << mClientId << " recv finished" << endl;
			mRecvFinished = true;
		}
		else
		{
			mStatus = eReceivePartial;
		}
    	
		//
		// Check whether it needs to send more data
		//
		if (mSentPos < mDataLen)
		{
			// 
			// Not finished sending yet.
			//
			if (mSendMode == eReceiveDriven)
			{
				mLock->unlock();
				sendData();
			}
			else
			{
				mLock->unlock();
			}
		}
		else
		{
			mLock->unlock();
		}
	}
	else if (mInputDataType == eTestCaseConfigFile)
	{
		mSuccessed++;
		mConn = 0;
		mStatus = eReceiveFinished;
		OmnTrace << "Client: " << mClientId << " recv finished" << endl;
		mRecvFinished = true;
		mLock->unlock();
		restart();
	}	

	return;
}


int
AosTcpTrafficGenClient::restart()
{
	// calcute checksum
	if(mCheckContent)	
	{
		
	}
	
	// 
	// 1. Creates a new TCP connection
	// 2. Add the connection to the group
	// 3. Send the contents.
	//
	OmnString errmsg;
    mBytesReceived = 0;
	if(0)//mConn)
	{
		mConn->closeConn();
		mConn = 0;
	}
	//cout << "restart 1" << endl;
OmnTrace << "Client: " << mClientId << " to restart" << endl;

	if(mSuccessed + mFailed > mRepeat)
	{
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		cout << "!!!succ:" << mSuccessed << "  failed:" << mFailed << "    !!!!!" << endl;
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		return 0;
	}

	if(mSuccessed + mFailed == mRepeat)
	{
//cout << "no need to restart" << endl;
		OmnTrace << "no need to restart,the repeat time is over" << endl;
		return 0;
	}
	


	
	mLock->lock();
//cout << "restart 2" << endl;
    mSentPos = 0;
    mBytesReceived = 0;
    mDataSendingStarted = 0;
	mRecvFinished = false;
	mConnLastRcvTick = OmnTime::getSecTick();

	if (mSendBlockType == eAosSendBlockType_Random)
	{
		mSendBlockSize = OmnRandom::nextInt1(0, mSendBlockSize);
	}

	mConn = OmnNew OmnTcpClient(mRemoteAddr, 
				mRemotePort, 
				1,
				mLocalAddr,
				0,
				1,
				"TcpTrafficGen", 
				eAosTLT_NoLengthIndicator);
	mConn->setClientId(mClientId);

	if (!mConn->connect(errmsg))
	{
		mStatus = eFailedToConnect;
		mLock->unlock();
		return aos_alarm("Failed to connect: %s:%d. Errmsg: %s",
			mRemoteAddr.toString().data(), 
			mRemotePort, 
			errmsg.data());	
	}

	OmnTrace << "Create conn for client: " 
		<< mClientId << ". Sock: " << mConn->getSock() << endl;
	mStatus = eConnCreated;
	mConn->setUserData((void *)mIndex);
	mGroup->addConn(mConn);

	// 
	// If mListener is set, inform the listener that the connection
	// has been created.
	//
	mLock->unlock();
	AosTcpTrafficGenClientPtr thisPtr(this, false);
	if (mListener)
	{
		mListener->connCreated(thisPtr, mConn);
	}

	return sendData();
}


int
AosTcpTrafficGenClient::sendData()
{
	if (mInputDataType == eBuffer)
	{
		if (mSentPos >= mDataLen)
		{
			return aos_alarm("Sending has already finished: %d, %d", 
				mSentPos, mDataLen);
		}
    	
		//
		// Chen Ding, 11/07/2005
		//
		// u32 blockLen = (u32)OmnRandom::nextInt1(0xfff) + 1;
		// u32 blockLen = 10000;
		u32 sendLen = mDataLen - mSentPos;
		if (sendLen > mSendBlockSize) sendLen = mSendBlockSize;
		// if (sendLen > blockLen) sendLen = blockLen;
		char *start = mDataToSend + mSentPos;
    	
		bool needToSend = true;
		AosTcpTrafficGenClientPtr thisPtr(this, false);
		if (mListener)
		{
			// 
			// Inform the listener that the client is ready to send the
			// data.
			//
			mListener->readyToSend(thisPtr, start, sendLen, needToSend);
		}
    	
		mLock->lock();
		if (needToSend)
		{
			OmnRslt rslt;// = mConn->writeTo(start, sendLen);
			//
			// add by lxx for adding mBytesSent
			//
			mBytesSent += sendLen;
			
			if (!rslt)
			{
				OmnAlarm << "Failed to send: " << rslt.toString() << enderr;
				mStatus = eSendingFailed;
				mLock->unlock();
				if (mListener)
				{
					mListener->sendFailed(thisPtr, start, sendLen, rslt);
				}
			}
			else
			{
				mSentPos += sendLen;
    	
				if (mSentPos >= mDataLen)
				{
					mStatus = eSendingFinished;
					mLock->unlock();
					if (mListener)
					{
						mListener->sendFinished(thisPtr);
					}
				}
				else
				{
					mStatus = eIsSending;
					mLock->unlock();
					if (mListener)
					{
						mListener->dataSent(thisPtr, start, sendLen);
					}
				}
			}
		}
		else
		{
	OmnTrace << "*********** No need to send" << endl;
			mLock->unlock();
		}
	}
	else if(mInputDataType == eTestCaseConfigFile && mTestSuite != NULL)
	{
		TCData *pTCData;
		int testCaseNum = mTestSuite->mPCTestcases.size();
		
		if (testCaseNum <= 0)
		{
			return true;	
		}
		int index = OmnRandom::nextInt1(0,testCaseNum-1); 
		pTCData = mTestSuite->mPCTestcases[index]->getTCData();
		mListener = mTestSuite->mPCTestcases[index];
		
		u32 sendLen = pTCData->getSendLength();
		char *start;
		pTCData->getSendBuf(start);
		bool needToSend = true;
		AosTcpTrafficGenClientPtr thisPtr(this, false);
		if (mListener)
		{
			// 
			// Inform the listener that the client is ready to send the
			// data.
			//
			mListener->readyToSend(thisPtr, start, sendLen, needToSend);
		}
    	
		mLock->lock();
		if (needToSend)
		{
			OmnRslt rslt;// = mConn->writeTo(start, sendLen);

			mBytesSent += sendLen;
			
			if (!rslt)
			{
				OmnAlarm << "Failed to send: " << rslt.toString() << enderr;
				mStatus = eSendingFailed;
				mLock->unlock();
				if (mListener)
				{
					mListener->sendFailed(thisPtr, start, sendLen, rslt);
				}
			}
			else
			{
				mStatus = eSendingFinished;
				mLock->unlock();
				if (mListener)
				{
					mListener->sendFinished(thisPtr);
				}
			}
		}	
	}
	return true;
}


void		
AosTcpTrafficGenClient::printStatus() const
{
	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 0;

	int hasData = 0;
	int sock = -1;
	if (mConn && mConn->getSock() > 0)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(mConn->getSock(), &fds);
		OmnSocketSelect(mConn->getSock(), &fds, 0, 0, &t);
		hasData = FD_ISSET(mConn->getSock(), &fds)?1:0;
		sock = mConn->getSock();
	}

	cout << "Client: " << mClientId
		<< " finished: " << mSuccessed 
		<< " failed: " << mFailed
		<< " Status: " << mStatus
		<< " Bytes Received(KB): " << mBytesReceived/1000
		<< " Has Data: " << hasData 
		<< " Socket: " << sock << endl;
}


bool    
AosTcpTrafficGenClient::isFinished() const 
{
	return mRecvFinished;
//	return 	mBytesReceived >= mBytesExpected &&
//			mSentPos >= mDataLen;
}


bool    
AosTcpTrafficGenClient::needToRestart() const 
{
	// return mStatus != eFailed && 
	// 	   mStatus != eFinished &&
	// 	   mSuccessed < mRepeat;
	return mRecvFinished && mSuccessed + mFailed < mRepeat;
}


// 
// Return true is need to restart
//
bool
AosTcpTrafficGenClient::checkConn()
{
//cout << "no data time:" << OmnTime::getSecTick() - mConnLastRcvTick  << endl;
//cout << "maxconn time:" << eMaxConnTimer<< endl;

	if (OmnTime::getSecTick() - mConnLastRcvTick < eMaxConnTimer)
	{
		return false;
	}

	// 
	// 
	if (mStatus == eFailedToConnect)
	{
		if(mFailed + mSuccessed < mRepeat)
		{
			mFailed++;
		}
		mRecvFinished = true;
		restart();
		return true;
	}

	if (OmnTime::getSecTick() - mConnLastRcvTick < eMaxClientLife)
	{
		return false;
	}

	// 
	// Need to stop the current connection. 
	//
	if(mFailed + mSuccessed < mRepeat)
	{
		mFailed++;
	}
	mRecvFinished = true;
	restart();
	return true;
}

