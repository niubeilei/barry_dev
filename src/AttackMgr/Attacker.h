////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AttackMgr_Attacker_h
#define Aos_AttackMgr_Attacker_h

/*#include "aosUtil/CharPtree.h"
#include "CliClient/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "TcpAppServer/Ptrs.h"
#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"
*/
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util1/TimerObj.h"

class AosAttacker: public OmnThreadedObj,OmnTimerObj
{
	OmnDefineRCObject;
public:	

	enum
	{
		eDefaultCalcIntevel = 100
	};
	

	enum Protocol
	{
		eSyn,
		eFin,
		eReset,
		eAck,
		eUdp,
		eIcmp
	};
	

protected:
	OmnIpAddr		mSrcIP;	
	int				mSrcPort;	
	OmnIpAddr		mDstIP;	
	int				mDstPort;	
	Protocol		mProtocol;
	int				mNumPerSecond;// per second
	int				mCalcIntevel;//ms

	int				mTimerSec;
	int				mTimerUSec;
	OmnThreadPtr	mThread;
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	
	
public:
	AosAttacker();
	virtual ~AosAttacker();

	virtual bool		bind() = 0;
	virtual bool		unbind() = 0;
	virtual bool		start();
	virtual bool		stop();

	// Threaded Object functions	
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual bool	sendPacket() = 0;

	virtual bool	isSame(const OmnString &proto,
						   const OmnIpAddr &sip,
						   const int &sport,
						   const OmnIpAddr &dip,
						   const int &dport,
						   const int &number,
						   const int &interval);
	
	//
	//	Functions of OmnTimerObj
	//

	virtual void		timeout(const int timerId, 
								const OmnString &timerName,
								void *parm);

protected:

};
#endif
