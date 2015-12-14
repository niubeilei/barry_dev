////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: F:\source0620\AOS\src\UtilComm\Comm.h
// Description:
//	A communicator can send and receive OmnMsg. This class defines
//  the interface for Communicator. Note that Communicator is a 
//  high level class. It should hide as much details as possible
//  so that users of this class is relieved from detailed work.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_Comm_h
#define Omn_UtilComm_Comm_h

#include "Debug/ErrId.h"
#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "Message/MsgId.h"
#include "Network/NetEtyType.h"
#include "Network/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommProt.h"
#include "Util/IpAddr.h"
#include "XmlParser/Ptrs.h"


class OmnComm : public virtual OmnRCObject
{
protected:
	int					mCommId;
	OmnString			mName;
	OmnCommProt::E		mProtocol;
	OmnCommGroupPtr		mCommGroup;
	int					mNiid;
	int					mTargetId;
	OmnNetEtyType::E	mTargetType;
	bool				mProxySenderAddr;


public:
	OmnComm(const OmnCommProt::E prot,
		    const bool proxySenderAddr,
			const OmnCommGroupPtr &comm,
			const int targetId,
			const OmnNetEtyType::E targetType);

	OmnComm(const OmnCommProt::E prot,
			const OmnString &name);
	OmnComm(const OmnString &name);
	virtual ~OmnComm();

	enum CommType
	{
		eInvalidComm,
		eUdpComm,
		eTcpServer,
		eTcpClient
	};

	enum
	{
		eTimeout = 1,
		eReconnect,
		eReadingTimer = 30
	};

	int					getCommId() const {return mCommId;}
	OmnString			getName() const {return mName;}
	OmnCommProt::E		getProtocol() const {return mProtocol;} 
	void				setProtocol(const OmnCommProt::E p) {mProtocol = p;}
	int					getNiid() const {return mNiid;}

	virtual int			getSock() const = 0;

	virtual OmnRslt		readFrom(OmnConnBuffPtr &buff,
							const int timerSec,
							const int timerUsec, 
							bool &isTimeout) = 0;
	// virtual OmnRslt		sendTo(const OmnMsgPtr &msg) = 0;
	virtual OmnRslt		sendTo(const char *data, 
							const int length,
							const OmnIpAddr &recvIpAddr,
							const int recvPort) = 0;

	virtual OmnIpAddr	getLocalIpAddr() const = 0;
	virtual int			getLocalPort() const = 0;
	virtual bool		isConnGood() const = 0;
	virtual OmnRslt		connect(OmnString &) = 0;
	virtual OmnRslt		reconnect(OmnString &) = 0;
	virtual OmnRslt		closeConn() = 0;
	virtual OmnRslt		startReading(const OmnCommListenerPtr &requester) = 0;
	virtual OmnRslt		stopReading(const OmnCommListenerPtr &requester) = 0;
	virtual OmnRslt		forceStop() = 0;


	virtual OmnString	toString() const = 0;

	static OmnCommPtr	createComm(const OmnCommProt::E protocol, 
								   const bool proxySenderAddr,
								   const OmnXmlItemPtr &def,
                  				   const int targetId,
                  				   const OmnNetEtyType::E targetType,
								   const OmnCommGroupPtr &comm, 
								   const OmnString &fname, 
								   const int line);
	static OmnCommPtr	createComm(const OmnIpAddr &addr,
								   const int port,
								   const OmnCommProt::E prot,
								   const OmnString &name);
	static CommType		strToEnum(OmnString &name);

	// 
	// Chen Ding, 05/10/2003
	//
	virtual OmnIpAddr	getRemoteAddr() const = 0;
	virtual int			getRemotePort() const = 0;
};

#endif
