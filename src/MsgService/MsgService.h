////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MsgService_MsgService_h
#define AOS_MsgService_MsgService_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"

OmnDefineSingletonClass(AosMsgServiceSingleton,
						AosMsgService,
						AosMsgServiceSelf,
						OmnSingletonObjId::eMsgService,
						"MsgService");

class AosMsgService : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	static AosDocClientObjPtr	smDocClient;
	static AosIILClientObjPtr	smIILClient;

public:
	AosMsgService();
	~AosMsgService();

    // OmnThreadedObj Interface

    // Singleton class interface
    static AosMsgService *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);

	//define MsgService management function
	bool getUserOnlineStatus(
		int &status,
		const OmnString &friend_cid,
		const AosRundataPtr &rdata);

	bool setUserOnlineStatus(
		const OmnString &status,
		const AosRundataPtr &rdata);

	bool createAccount(
		const OmnString &account_objid,
		const AosRundataPtr &rdata);

	bool removeAccount(
		const OmnString &account_objid,
		const AosRundataPtr &rdata);
	
	bool addSender(
		const OmnString &sender,
		const OmnString &sender_type,
		const OmnString &account_objid,
		const AosRundataPtr &rdata);

	bool removeSender(
		const OmnString &account_objid,
		const AosRundataPtr &rdata);

	bool addFollowerToAcct(
		const OmnString &follower_cid,
		const OmnString &account_objid,
		const AosRundataPtr &rdata);

	bool removeFollowerFromAcct(
		const OmnString &follower_cid,
		const OmnString &account_objid,
		const AosRundataPtr &rdata);

	bool sendMsgByAccount(
		const OmnString &msg_type,
		const OmnString &account_objid,
		const AosXmlTagPtr &contents,
		const AosRundataPtr &rdata);

	bool sendMsgToUser(
		const AosXmlTagPtr &contents,
		const AosRundataPtr &rdata);
private:
	bool existInSenderList(
		OmnString &sender_objid,
		const OmnString &account_objid,
		const AosRundataPtr &rdata);
};
#endif
