////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "AttackMgr/Attacker.h"

#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"

AosAttacker::AosAttacker()
:
mSrcPort(-1),	
mDstPort(-1),	
mProtocol(eIcmp),
mNumPerSecond(0),
mCalcIntevel(0),
mTimerSec(0),
mTimerUSec(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}

AosAttacker::~AosAttacker()
{
}

bool
AosAttacker::start()
{
	bind();
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "attacker", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}

bool		
AosAttacker::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	while(mThread->isStopped())
	{
		OmnWait::getSelf()->wait(0,10000);
	}
	unbind();
	return true;
}


bool	
AosAttacker::isSame(const OmnString &proto,
				    const OmnIpAddr &sip,
				    const int &sport,
				    const OmnIpAddr &dip,
				    const int &dport,
				    const int &number,
				    const int &interval)
{
	switch(mProtocol)
	{
		case eUdp:
			if(proto != "UDP") return false;
			break;
		case eIcmp:
			if(proto != "ICMP") return false;
			break;
		case eSyn:
			if(proto != "SYN") return false;
			break;
		case eFin:
			if(proto != "FIN") return false;
			break;
		case eReset:
			if(proto != "RESET") return false;
			break;
		case eAck:
			if(proto != "ACK") return false;
			break;
		default:
			return false;
	}
	
	if(sip == mSrcIP 			&&
	   sport == mSrcPort		&&
	   dip == mDstIP			&&
	   dport == mDstPort		&&
	   number == mNumPerSecond 	&&
	   interval == mCalcIntevel)
	{
		return true;
	}
	
	return false;
}
				    

bool	
AosAttacker::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
        sendPacket();

	    OmnTimerObjPtr thisPtr(this, false);
		    
	    OmnTimerSelf->startTimer("Attacker",mTimerSec,mTimerUSec,thisPtr,0);
	    mLock->lock();
	    mCondVar->wait(mLock);
	    mLock->unlock();
    
	}

    return true;
}   


void		
AosAttacker::timeout(const int timerId, 
					const OmnString &timerName,
					void *parm)
{
	signal(0);
}


bool	
AosAttacker::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool    
AosAttacker::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}



