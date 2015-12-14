////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommGroup.h
// Description:
//	Connections in PacketLink networks are through comm
//  groups. A comm group is created for a specific purpose, such
//  as for exchanging MGCP messages between two PacketLink network nodes.
//  If there are more than one comms in a group, by definition,
//  any comm serves equally well as other comms do. To the 
//  user of this group, it cares nothing about which member comm
//  to use. For instance, if one wants to send an MGCP message to 
//  another SPNR, if it knows through which group to send the message, 
//  it can ask the group to send the message.
//  In this case, it does not care through which comm to 
//  send the message.
//
//	Each Comm Group handles only one type of message category.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_CommGroup_h
#define Omn_UtilComm_CommGroup_h

#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "Message/MsgId.h"
#include "Network/NetEtyType.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommProt.h"
#include "XmlParser/Ptrs.h"

class OmnIpAddr;


class OmnCommGroup : public OmnRCObject
{
public:
	enum GroupType
	{
		eInvalidGroupType,
		eRoundRobin,
		eReliable,
		eGroupComm
	};

protected:
	int					mGroupId;
	OmnString			mName;
	OmnCommProt::E		mProtocol;
	int					mTargetId;
	OmnNetEtyType::E	mTargetType;
	OmnCommMgrPtr		mCommMgr;

	OmnCommGroupPtr		mPeer;
	bool				mProxySenderAddr;

public:
	OmnCommGroup(const OmnCommMgrPtr &commMgr);
	OmnCommGroup(const OmnString &name,
		const OmnCommProt::E protocol,
		const bool proxySenderAddr,
		const int targetId);
	~OmnCommGroup();

	static OmnCommGroupPtr	createGroup(const OmnCommMgrPtr & commMgr, 
										const OmnXmlItemPtr &def);
	static OmnCommGroupPtr	createGroup(const OmnXmlItemPtr &def);
	static GroupType	strToEnum(OmnString &name);
	static int			getNewGroupId();

	virtual OmnRslt		readFrom(OmnConnBuffPtr &buff,
								 const int timerSec, 
								 const int timerMsec, 
								 bool &isTimeout) = 0;
	virtual OmnRslt		sendMsgTo(const OmnMsgPtr &msg) = 0;
	virtual OmnRslt		sendTo(const OmnConnBuffPtr &buff) = 0;
	virtual OmnRslt		sendTo(const char *data, const int len) = 0;
	virtual OmnRslt		sendTo(const char *data, const int len,
							   const OmnIpAddr &recverIpAddr,
							   const int recverPort) = 0;
	virtual bool		forward(const OmnConnBuffPtr &buff) = 0;

	virtual bool		commRestored(const OmnCommPtr &comm) = 0;
	virtual GroupType	getType() const = 0;
    virtual OmnRslt     startReading(const OmnCommListenerPtr &callback) = 0;
    virtual OmnRslt     stopReading(const OmnCommListenerPtr &callback) = 0;
	virtual int			getNiid() const = 0;
	virtual OmnIpAddr	getLocalIpAddr() const = 0;
	virtual int			getLocalPort() const = 0;
	virtual OmnString	toString() const;

	int					getGroupId() const {return mGroupId;}
	OmnString			getName() const {return mName;}
	OmnCommProt::E		getProtocol() const {return mProtocol;}
	int					getTargetId() const {return mTargetId;}
	OmnNetEtyType::E	getTargetType() const {return mTargetType;}

	void				setPeer(const OmnCommGroupPtr &peer);
	OmnCommGroupPtr		getPeer() const {return mPeer;}

	// 
	// Chen Ding, 05/10/2003
	//
	virtual OmnIpAddr	getRemoteAddr() const = 0;
	virtual int			getRemotePort() const = 0;
};
#endif
