////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCommClt.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_TcpCommClt_h
#define Omn_UtilComm_TcpCommClt_h

#include "Message/MsgId.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Comm.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/CommProt.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/TcpClient.h"
#include "XmlParser/Ptrs.h"




class OmnTcpCommClt : public OmnComm, public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnTcpClientPtr		mConn;
	int					mNIID;
	OmnCommProt::E		mProtocol;
	OmnThreadPtr		mThread;
	OmnCommListenerPtr	mRequester;
	bool				mIsReading;
	bool				mIsSmartReading;
	OmnMutexPtr			mLock;
	OmnIpAddr			mRemoteAddr;
	int		 			mRemotePort;
	int 				mNumPorts;
	AosTcpLengthType    mLengthType;
	OmnString 			mFname;
	int					mLine;

public:
	OmnTcpCommClt(const OmnCommProt::E protocol, 
				  const OmnXmlItemPtr &def,
                  const int targetId,
                  const OmnNetEtyType::E targetType,
				  const OmnCommGroupPtr &group, 
				  const OmnString &fname, 
				  const int line,
				  const bool single_flag = false);
	OmnTcpCommClt(const OmnString &path, 
				  const OmnString &sfname,
				  const AosTcpLengthType length_type, 
				  const OmnString &fname, 
				  const int line,
				  const bool single_flag = false);

	OmnTcpCommClt(const OmnIpAddr &remoteAddr, 
				  const int remotePort, 
				  const int num_ports,
				  const AosTcpLengthType length_type, 
				  const OmnString &fname, 
				  const int line,
				  const bool single_flag = false);

	~OmnTcpCommClt();

	// OmnRslt				config(const OmnXmlItemPtr &def);

    OmnRslt     sendTo(const char *data, 
                       const int length,
                       const OmnIpAddr &recvIpAddr,
                       const int recvPort);

	OmnRslt		readFrom(const OmnMsgPtr &msg);
	OmnRslt		readFrom(OmnConnBuffPtr &buff, const int sec, const int msec, bool &isTimeout){return false;}
	OmnRslt		sendTo(const OmnMsgPtr &msg);
	OmnRslt		sendTo(const char *, const int);

	OmnRslt 	startReading(const OmnCommListenerPtr &callback);
	OmnRslt 	stopReading(const OmnCommListenerPtr &callback);

	virtual int	getSock() const;
    virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool signal(const int threadLogicId);
    virtual void heartbeat();
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 


	OmnString	toString() const;

	virtual OmnIpAddr	getLocalIpAddr() const;
	virtual int			getLocalPort() const;
	virtual bool		isConnGood() const;
	virtual OmnRslt		connect(OmnString &err);
	virtual OmnRslt		reconnect(OmnString &err);
	bool 				reconnect();
	virtual OmnRslt		closeConn();
	virtual int			getNiid() const {return mNIID;}
	virtual OmnRslt		forceStop();

	virtual OmnCommProt::E	getProtocol() const {return mProtocol;}

	// 
	// Chen Ding, 05/10/2003
	//
	virtual OmnIpAddr	getRemoteAddr() const {return OmnIpAddr::eInvalidIpAddr;}
	virtual int			getRemotePort() const {return -1;}

	// Chen Ding, 11/09/2010
	bool	smartSend(const char *data, const int len);
	bool	smartSend(const char *, const int, const char *, const int);
	bool	smartSend(const OmnString &data)
	{
		return smartSend(data.data(), data.length());
	}
	bool	startSmartReading(const OmnCommListenerPtr &callback);
	bool	destroyConn();

	//for async reading, there may be problems. by White 2015-8-20 16:15
	void	setBlockingType(const bool blocking){mConn->setBlockingType(blocking);}
	bool	getBlockingType() { return mConn->getBlockingType(); }
};
#endif

