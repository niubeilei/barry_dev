////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamSimuEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_StreamSimu_StreamSimuEntry_h
#define Omn_StreamSimu_StreamSimuEntry_h

#include "StreamSimu/Ptrs.h"

#include "StreamSimu/StreamType.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"


class AosStreamSimuEntry : public virtual OmnRCObject
{
	OmnDefineRCObject;

protected:
	enum 
	{
		eMaxPacketSize = 1024,
	};

	enum Status
	{
		eConnNotCreatedYet,
		eFailedCreateConn,
		eSuccess
	};

	OmnVList<AosStreamSimuConnPtr>		mConnList;
    OmnMutexPtr         				mLock;
	
	OmnIpAddr							mSenderIp;
	int									mSenderPort;
	OmnString							mProtocol;
	
	static char							mContent[eMaxPacketSize];

	Status								mStatus;
	u32									mBytesSent;
	u32									mBytesRcvd;
	u32									mStartTime;
	u32									mDuration;

public:
	AosStreamSimuEntry(const OmnIpAddr	&sendIp,
					const int		&sendPort,
					const OmnString &protocol, 
					const u32 duration);
	AosStreamSimuEntry();

	static void		initContent();

	virtual ~AosStreamSimuEntry();
public:
	virtual bool	entryExist(const OmnIpAddr 		&senderIp,
							   const int		  	senderPort,
							   const OmnString 		&protocol);

	virtual bool	addConn(const OmnIpAddr 					&recverIp,
							const int		  					recverPort,
						    const int		  					bandwidth,
						    const int		  					packetSize,
                            const AosStreamType::E 				streamType,
                     	    const int                          	maxBw,
                            const int                          	minBw);
					
	virtual bool	removeConn(const OmnIpAddr 		&recverIp,
							   const int		  	recverPort);
									
	virtual bool	getBandwidth(const OmnIpAddr 		&recverIp,
							     const int		  		recverPort,
								 int			  		&bw,
								 int			  		&packetNum);
	virtual int		connNum();								
					   
	virtual bool	modifyStream(const OmnIpAddr 					&recverIp,
							     const int		  					recverPort,
							     const int		  					bandwidth,
							     const int		  					packetSize,
	                             const AosStreamType::E 			streamType,
	                     	     const int                          maxBw,
	                             const int                          minBw);

	virtual bool    sendPacket(const OmnIpAddr &recverIp, 
							   const int recverPort,
							   const int size) = 0;

	virtual bool	start();						
	virtual bool	stop();						

	u32			getBytesSent() const {return mBytesSent;}
	u32			getBytesRcvd() const {return mBytesRcvd;}
	bool		isFinished() const;
	u32			getDuration() const {return mDuration;}
	OmnIpAddr	getSenderIP() const {return mSenderIp;}
	u16			getSenderPort() const {return mSenderPort;}
};
#endif

