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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 07/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgUtil/SmsProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "ShortMsgUtil/CommOpr.h"
#include "ShortMsgUtil/ShortMsgUtil.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include <set>

const int sgNumFreeRead    = 50;
bool AosSmsProc::smShowLog = false;
const int cMaxOutstandingMsgs = 50;
static  AosShortMsgUtil	sgSmsUtil;

AosSmsProc::AosSmsProc(
		const int &port, 
		const int &try_read,
		const OmnString &phone)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mNumReqs(0),
mTotalAccepted(0),
mTotalProcessed(0),
mListener(AosShortMsgSvr::getSelf()),
mRundata(OmnApp::getRundata()),
mTryToRead(try_read),
mSerialPort(port),
mPhone(phone)
{
	aos_assert(init());
}


AosSmsProc::~AosSmsProc()
{
}


bool
AosSmsProc::init()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mProcThread = OmnNew OmnThread(thisPtr, "ProcThrd", eProcThrdId, true, true, __FILE__, __LINE__);
	//mProcThread->start();
	mHandlerThread = OmnNew OmnThread(thisPtr, "HandlerThrd", eHandlerThrdId, true, true, __FILE__, __LINE__);
	//mHandlerThread->start();
	return true;
}


bool    
AosSmsProc::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSmsProc::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSmsProc::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		if (thread->getLogicId() == eProcThrdId)
		{
			return procSendShortMsg(state, thread);
		}
		if (thread->getLogicId() == eHandlerThrdId)
		{
			return handlerShortMsg(state, thread);
		}
	}
	return true;
}



bool
AosSmsProc::procSendShortMsg(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	// This function proc a request to send short message.
	AosSmsReqPtr request;
	char buff[512];
	mLock->lock();
	if (mNumReqs == 0)
	{
		// There is no more cached entries. Start a timer to 
		// wake this thread up later. 
		for (int i=0; i<sgNumFreeRead; i++)
		{
			ReadComm(mSerialPort, buff, 512);
			if (checkBuffOk(buff)) return true;
			checkBuffOut(buff);
		}
		mLock->unlock();
	}
	// There are something to be processed. Get the first entry
	// from the list, and then release the lock.
	request = mTail;
	mTail = request->prev;
	if (mTail) mTail->next = 0;
	mNumReqs--;
	if (mNumReqs == 0)
	{
		mHead = 0;
		mTail = 0;
	}
	mLock->unlock();
	procSmsPriv(request);
	request = 0;
	return true;
}


bool
AosSmsProc::handlerShortMsg(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	// This functon hand all request from short message.
	mLock->lock();
	if (mHandlerQueue.size() == 0)
	{
		mLock->unlock();
		return true;
	}
	AosShortMsg handler = mHandlerQueue.front();
	mHandlerQueue.pop();

	OmnString sender = handler.mSenderNumber;
	OmnString message = handler.mMsg;
	OmnString modem_num = handler.mGsmPhone;
	
	mListener->shortMsgReceived(message, sender, modem_num, mRundata);

	mLock->unlock();
	
	return true;
}


bool
AosSmsProc::procSmsPriv(const AosSmsReqPtr &request)
{
	char buff[512];
	int fd = request->fd;
	aos_assert_r(fd>-1, false);
	OmnString iNumStr = request->iNumStr;
	aos_assert_r(iNumStr != "", false);
	OmnString sContent = request->sContent;
	aos_assert_r(sContent != "", false);

	// 1. Write "AT+CMGS=%d\r" to comm
	WriteComm(fd, iNumStr.data());
	for (int i=0; i<mTryToRead; i++)
	{
		memset(buff, 0, 512);
		ReadComm(fd, buff, 512);
		if (checkBuffInput(buff))
		{
			break;
		}
		else
		{
			// Comm not reponse to the request
			return false;
		}
	}
	
	// 2. Write contents to comm
	WriteComm(fd, sContent.data());
	for (int i=0; i<mTryToRead; i++)
	{
		memset(buff, 0, 512);
		ReadComm(fd, buff, 512);
		if (checkBuffOk(buff))
		{
			break;
		}
		else
		{
			// Comm not reponse to the request
			return false;
		}
	}
	
	// sContent > 70
	if (request->mtPart)
	{
		procSmsPriv(request);
	}

	return true;
}


bool
AosSmsProc::addProcSmsRequest(
		const AosSmsReqPtr &req,
		const AosRundataPtr &rdata)
{
	aos_assert_r(req, false);
	AosSmsReqPtr qq = req;
	if (mNumReqs > cMaxOutstandingMsgs) OmnSleep(1);
	mLock->lock();
	mTotalAccepted++;
	mNumReqs++;
	if (mHead) mHead->prev = qq;
	qq->next = mHead;
	qq->prev = 0;
	mHead = qq;
	if (!mTail) mTail = qq;
	writeToPort("AT\r");
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosSmsProc::checkBuffInput(const char * const buff)
{
	// This function check whether the buff is ">"
	if (strncmp(buff, ">", 1) == 0) return true;
	checkBuffOut(buff);
	return false;
}


bool
AosSmsProc::checkBuffOk(const char * const buff)
{
	// This function check whether the buff is "0K"
	if (strncmp(buff, "OK", 2) == 0) return true;
	checkBuffOut(buff);
	return false;
}


bool
AosSmsProc::checkBuffOut(const char * const buff)
{
	// This function check whether the buff is "0891"
	char str1[512];
	char str2[512];
	AosShortMsg msg;
	if (strncmp(buff, "0891", 4) == 0)
	{
		// The character between four and fourteen in buff is the information of
		// short message server center's phone.
		memset(str1, 0, 512);
		memset(str2, 0, 512);
		sgSmsUtil.substr((unsigned char *)str1, (unsigned char *)buff, 4, 14);
	    sgSmsUtil.pduToTel((unsigned char *)str1, (unsigned char *)str2);
		cout << "The phone of short message server center is: " << str2 << endl;
		msg.setServerCenterNumber(str2);
	
		memset(str1, 0, 512);
		memset(str2, 0, 512);
		sgSmsUtil.substr((unsigned char *)str1, (unsigned char *)buff, 24, 14);
    	sgSmsUtil.pduToTel((unsigned char *)str1, (unsigned char *)str2);
		cout << "Sender: " << str2 << endl;
		msg.setSenderNumber(str2);
		
		memset(str1, 0, 512);
		memset(str2, 0, 512);
		sgSmsUtil.substr((unsigned char *)str1, (unsigned char *)buff, 42, 14);
		sgSmsUtil.pduToTel((unsigned char *)str1, (unsigned char *)str2);
		string time((str2));
		OmnString rcvTime = "20";
		rcvTime << time.substr(0, 2) << "/" //year
			<< time.substr(2, 2) << "/"     //month
			<< time.substr(4, 2) << " "     //day
			<< time.substr(6, 2) << ":"     //hour
			<< time.substr(8, 2) << ":"     //minute
			<< time.substr(10, 2);
       	OmnScreen << "Received time: " << rcvTime << endl;
		msg.setRcvTime(OmnString(rcvTime));

		// Retrieve the message. 
		memset(str1, 0, 512);
		memset(str2, 0, 512);
		sgSmsUtil.substr((unsigned char *)str1, (unsigned char *)buff, 58, strlen((char *)buff)-59);
		sgSmsUtil.hex2str((unsigned char *)str2, (unsigned char *)str1);
		cout << "Received message: " << str2 << endl;
       	msg.setMsg(str2);
		
		msg.setGsmPhone(mPhone);
	}
	mHandlerQueue.push(msg);
	return true;
}


void
AosSmsProc::writeToPort(const char * const buff)
{
	// Now we read and write port just by this class
	mLock->lock();
	WriteComm(mSerialPort, buff);
	mLock->unlock();
}


bool
AosSmsProc::readOk()
{
	// Read contents from comm, if get "Ok", return true.
	char buff[512];
	for (int i=0; i<mTryToRead; i++)
	{
		memset(buff, 0, 512);
		mLock->lock();
		ReadComm(mSerialPort, buff, 512);
		mLock->unlock();
		if (strncmp(buff, "OK", 2) == 0)
		{
			return true;
		}
	}
	return false;
}


void
AosSmsProc::startThread()
{
	mProcThread->start();
	mHandlerThread->start();
}
