////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamSimuMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_StreamSimu_StreamSimuMgr_h
#define Aos_StreamSimu_StreamSimuMgr_h

#include "StreamSimu/Ptrs.h"

#include "StreamSimu/StreamType.h"
#include "StreamSimu/StreamSimuConn.h"
#include "StreamSimu/StreamSimuEntry.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
//#include "TcpAppServer/Ptrs.h"
//#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(AosStreamSimuMgrSingleton, 
						AosStreamSimuMgr,
						AosStreamSimuMgrSelf,
						OmnSingletonObjId::eStreamSimuMgr, 
						"StreamSimuMgr");

typedef void (*AosStreamSimuMgrCallback)(const AosStreamSimuEntryPtr &entry);

class AosStreamSimuMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	OmnVList<AosStreamSimuEntryPtr>	mStreamSimuEntryList;
    OmnMutexPtr         			mLock;
//	AosBandWidthMgrPtr				mBandWidthMgr;
	AosStreamSimuMgrCallback		mCallback;	
	
public:
	AosStreamSimuMgr();
	~AosStreamSimuMgr();

	static AosStreamSimuMgr *	getSelf();

	bool				config(const OmnXmlParserPtr &conf);

	bool				start();
	bool				stop();
	void	setCallback(AosStreamSimuMgrCallback c) {mCallback = c;}

	// 
	// ThrededObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	void			setStartFlag(const bool flag);
	void			setRespFlag(const bool flag);

	bool 			addStream(const OmnIpAddr 	     	&senderIp,
						  	  const int		  		    senderPort,
						  	  const OmnIpAddr 			&recverIp,
						  	  const int		  			recverPort,
						  	  const AosStreamType::E	streamType);

	bool 			addStream(const OmnString 						&protocol,
						  	  const OmnIpAddr 						&senderIp,
						  	  const int		  						senderPort,
						  	  const OmnIpAddr 						&recverIp,
						  	  const int		  						recverPort,
						  	  const int		  						bandwidth,
						  	  const int		  						packetSize,
						  	  const AosStreamType::E	streamType,
						  	  const int								maxBw,
							  const int								minBw);

	bool 			removeStream(const OmnString &protocol,
						  const OmnIpAddr &senderIp,
						  const int		  senderPort,
						  const OmnIpAddr &recverIp,
						  const int		  recverPort);

	bool			getBandwidth( const OmnString &protocol,
								  const OmnIpAddr &senderIp,
								  const int		  senderPort,
								  const OmnIpAddr &recverIp,
								  const int		  recverPort,
								  int			  &bw,
								  int			  &packetNum);

	bool 			modifyStream(const OmnString 					&protocol,
						  		 const OmnIpAddr 					&senderIp,
						  		 const int		  					senderPort,
						  		 const OmnIpAddr 					&recverIp,
						  		 const int		  					recverPort,
						  		 const int		  					bandwidth,
						  		 const int		  					packetSize,
						  	  	 const AosStreamType::E	streamType,
						  	  	 const int							maxBw,
							  	 const int							minBw);

private:
	

};
#endif
