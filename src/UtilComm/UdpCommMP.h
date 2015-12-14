////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpCommMP.h
// Description:
//	This class maintains multiple ports. Otherwise, it is the same as 
//  OmnUdpCommMP.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_UdpCommMP_h
#define Omn_UtilComm_UdpCommMP_h

#include "UtilComm/UdpComm.h"
#include "UtilComm/StreamReaderObj.h"

#include "Debug/ErrId.h"
#include "Message/MsgId.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"


class OmnUdpCommMP : public OmnUdpComm, public OmnStreamReaderObj
{
private:
	OmnStreamReaderPtr				mConn;
	OmnValList<OmnConnBufferPtr>	mQueue;
	OmnMutexPtr						mQueueLock;
	OmnCondVarPtr					mQueueCondVar;
	int								mStartPort;
	int								mNoPorts;

	//
	// Do not use the following
	//
	OmnUdpCommMP(const OmnUdpCommMP &rhs);
	OmnUdpCommMP & operator = (const OmnUdpCommMP &rhs);

public:
	OmnUdpCommMP(const OmnString &name,
			   const int niid,
			   const OmnIpAddr &localIpAddr,
			   const int startPort,
			   const int noPorts,
			   const OmnMsgId::E msgCat);
	virtual ~OmnUdpCommMP();

	//
	// Writes
	//
	virtual OmnRslt		readFrom(OmnConnBufferPtr &buff, const int timerSec = -1,
							const int timerMsec = 0);
	virtual OmnRslt		sendTo(const OmnMsgPtr &msg);
	virtual OmnRslt		sendTo(const OmnString &data, 
							const int localPort,
							const OmnIpAddr &remoteIpAddr,
							const int remotePort);

	virtual bool		isConnGood() const {return true;}
	virtual OmnRslt		connect() {return 0;}
	virtual bool		closeConn() {return true;}
	virtual OmnString	toString() const;

	virtual OmnIpAddr	getLocalIpAddr() const {return mLocalIpAddr;}
	virtual int			getLocalPort() const {return -1;} // Not supposed to call this

	//
	// OmnStreamReaderObj interface
	//
	virtual OmnRslt		procStream(const OmnConnBufferPtr &buffer);

	virtual void		addRef() {mRCObj.addRef();}
	virtual int		removeRef(const bool delFlag) 
					{
						int result = mRCObj.removeRef();
						if (result <= 0 && delFlag) OmnDelete this;
						return result;
					}
};
#endif