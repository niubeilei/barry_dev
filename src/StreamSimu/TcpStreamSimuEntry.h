////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpStreamSimuEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_StreamSimu_TcpStreamSimuEntry_h
#define Omn_StreamSimu_TcpStreamSimuEntry_h

#include "StreamSimu/StreamSimuEntry.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"


class AosTcpStreamSimuEntry :public AosStreamSimuEntry,public OmnThreadedObj
{
public:
    enum
	{
		eBwListSize = 100, 
		eMinBytePerSlot = 10
	};

private:
	OmnTcpClientPtr			mTcpClient;
	OmnThreadPtr			mThread;
	bool					mConnOn;
	OmnIpAddr				mRecverIp;
	int						mRecverPort;

	int						mBandwidth;
	int						mPacketSize;
	
	AosStreamType::E		mType;
	int						mMaxBw;
	int						mMinBw;
	int						mPacketNum;
	
	int						mBwList[eBwListSize];
	int						mCurBwPos;
	bool					mConnected;		
	
public:
	AosTcpStreamSimuEntry(const OmnIpAddr& 	senderIp, 
						  const int 		senderPort);
	virtual ~AosTcpStreamSimuEntry();

public:
	virtual bool	start();						
	virtual bool	stop();						

	virtual bool    sendPacket(const OmnIpAddr &recverIp, 
							   const int recverPort,
							   const int size);


	virtual bool	addConn(const OmnIpAddr 					&recverIp,
							const int		  					recverPort,
						    const int		  					bandwidth,
						    const int		  					packetSize,
                            const AosStreamType::E 	streamType,
                     	    const int                          	maxBw,
                            const int                          	minBw);
					
	virtual bool	removeConn(const OmnIpAddr 		&recverIp,
							   const int		  	recverPort);

	virtual bool	getBandwidth(const OmnIpAddr 	&recverIp,
								 const int		  	recverPort,
								 int			  	&bw,
								 int			  	&packetNum);
									
	virtual int		connNum();								
					   
	virtual bool	modifyStream(const OmnIpAddr 					&recverIp,
							     const int		  					recverPort,
							     const int		  					bandwidth,
							     const int		  					packetSize,
	                             const AosStreamType::E streamType,
	                     	     const int                          maxBw,
	                             const int                          minBw);

	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	void			adjustParms();
	int				calcPks(const int sendSize);

};
#endif
