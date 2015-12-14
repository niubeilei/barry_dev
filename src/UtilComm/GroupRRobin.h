////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GroupRRobin.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_GroupRobin_h
#define Omn_UtilComm_GroupRobin_h

#include "Message/MsgId.h"
#include "Thread/Ptrs.h"
#include "Util/BasicTypes.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/CommProt.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"


class OmnGroupRRobin : public OmnCommGroup
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxComms = 20
	};

	int						mCommIndex;
	int						mNumComms;
	OmnCommPtr				mComms[eMaxComms];
	OmnMutexPtr				mCommLock;
	bool					mIsReading;

public:
	OmnGroupRRobin(const OmnCommMgrPtr &commMgr, 
				   const OmnXmlItemPtr &def);
	OmnGroupRRobin(const OmnCommMgrPtr &commMgr,
				   const OmnString &name,
			   	   const OmnCommProt::E protocol,
				   const bool proxySenderAddr,
			   	   const OmnIpAddr &localAddr,
			   	   const int localPort);
	~OmnGroupRRobin();

	OmnRslt			config(const OmnXmlItemPtr &def);

	GroupType		getType() const {return eRoundRobin;}

	bool			addComm(const OmnCommPtr &ni);
	bool			removeComm(const int niid);

	virtual bool	commRestored(const OmnCommPtr &comm);
    virtual OmnRslt startReading(const OmnCommListenerPtr &requester);
    virtual OmnRslt stopReading(const OmnCommListenerPtr &requester);

	OmnRslt			readFrom(OmnConnBuffPtr &buff,
							 const int timerSec,
							 const int timerMsec,
							 bool &isTimeout);
	OmnRslt			sendMsgTo(const OmnMsgPtr &msg);
	OmnRslt			sendTo(const OmnConnBuffPtr &buff);
	OmnRslt			sendTo(const char *data, const int length);
	OmnRslt			sendTo(const char *data, 
						   const int length,
						   const OmnIpAddr &recvIpAddr,
						   const int recvPort);
	bool			forward(const OmnConnBuffPtr &buff);

	int				getNiid() const;
	OmnIpAddr		getLocalIpAddr() const;
	int				getLocalPort() const;

	OmnString		toString() const;

	// 
	// Chen Ding, 05/10/2003
	//
	virtual OmnIpAddr		getRemoteAddr() const;
	virtual int				getRemotePort() const;
};
#endif

