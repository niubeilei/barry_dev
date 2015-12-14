////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServerGrp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpServerGrp_h 
#define Omn_UtilComm_TcpServerGrp_h 

#include "Porting/Socket.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/SPtr.h"
#include "Util/Array10.h"
#include "Util/IpAddr.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommTypes.h"
#include "Util/RCObjImp.h"


class OmnTcpServerGrp : public OmnTcp,
						public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eAcceptQueueSize = 2000,//50,
		eDefaultMaxAllowedConns = 4000,//500
	};

private:
	OmnVList<OmnTcpCltGrpPtr>	mGroups;
	OmnMutexPtr					mLock;
	OmnThreadPtr				mNewConnThread;
	int							mNumConns;
	int							mMaxAllowedConns;
	OmnTcpCltGrpListenerPtr		mListener;
	bool						mIsReading;

public:
	OmnTcpServerGrp(const OmnIpAddr &localIpAddr,
			const int localPort,
			const int numPorts,
			const OmnString &name,
			const int maxConns,
			const AosTcpLengthType lt = eAosTLT_LengthIndicator);
	OmnTcpServerGrp(AosAddrDef &addrDef, 
			AosPortDef &ports,
			const OmnString &name,
			const int maxConns, 
			const AosTcpLengthType length_type);
	~OmnTcpServerGrp();
   
	OmnRslt		connect(OmnString &err); 
	OmnString	getConnInfo() const;
	OmnString	getName() const {return mName;}
	bool		anyToRead();
	OmnRslt		closeConn();
	bool		connClosed(const OmnTcpCltGrpPtr &, 
					const OmnTcpClientPtr &client);

	void		startReading();
	void		stopReading();
	void		setListener(const OmnTcpCltGrpListenerPtr &listener);

	// 
	// Thread interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

	OmnRslt		removeConn(const OmnTcpClientPtr &client);

private:
	bool 		addConn(const OmnTcpClientPtr &client);
	bool		initialize();
	void		checkConns();
	OmnTcpClientPtr		acceptNewConn();
};
#endif

