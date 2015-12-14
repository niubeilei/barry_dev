////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamSimuConn.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_StreamSimu_StreamSimuConn_h
#define Omn_StreamSimu_StreamSimuConn_h

#include "StreamSimu/Ptrs.h"
#include "StreamSimu/StreamType.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "UtilComm/Ptrs.h"


class AosStreamSimuConn : public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eCollectorLen = 10
	};	
private:
	OmnIpAddr				mSenderIp;
	int						mSenderPort;
	OmnIpAddr				mRecverIp;
	int						mRecverPort;
	OmnString				mProtocol;
                        	
	int						mBandwidth;
	int						mPacketSize;
	OmnThreadPtr			mThread;
	                    	
    OmnMutexPtr         	mLock;
    AosStreamSimuEntryPtr	mEntry;

	AosStreamType::E		mType;
	int						mMaxBw;
	int						mMinBw;
	int						mPacketNum;

	int						mBwList[1000];
	int						mCurBwPos;

public:	
	AosStreamSimuConn(const OmnIpAddr 				&senderIp,
					  const int		  				senderPort,
					  const OmnIpAddr 				&recverIp,
					  const int		  				recverPort,
					  const OmnString 				&protocol,
  					  const int		  				bandwidth,
					  const int		  				packetSize,
                      const AosStreamType::E		streamType,
             	      const int                     maxBw,
                      const int                     minBw,
					  const AosStreamSimuEntryPtr	entry);
					  
	virtual ~AosStreamSimuConn();
public:
	bool			connExist(const OmnIpAddr 		&senderIp,
					  		  const int		  		&senderPort);
	bool			modifyStream(const int				bandwidth,
						     	 const int				packetSize,
		                         const AosStreamType::E streamType,
		             	         const int              maxBw,
		                         const int              minBw);
	bool			getBandwidth(int			  &bw,
								 int			  &packetNum);
	bool			start();
	bool			stop();
	
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
private:
	void			adjustParms();
	int				calcPks(const int sendSize);
};
#endif

