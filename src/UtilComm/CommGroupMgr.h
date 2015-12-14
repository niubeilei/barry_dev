////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommGroupMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_CommGroupMgr_h
#define Omn_UtilComm_CommGroupMgr_h

#include "Debug/Rslt.h"
#include "Message/MsgId.h"
#include "Message/Ptrs.h"
#include "Network/NetEnum.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommProt.h"
#include "XmlParser/Ptrs.h"
#include <vector>


class OmnCommGroupMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	//
	// Temporarily use array. Will be replaced by hash table
	//
	//struct REntry
	//{
	//	OmnCommGroupPtr	mCommGroup;
	//	OmnIpAddr		mRemoteIpAddr;
	//	int				mRemotePort;
	//	bool			mIsDirect;
	//	
	//};

	OmnCommGroupPtr		mCommGroups[OmnNetEnum::eMaxNumGroup];

	//REntry				mCommsMgcp[OmnNetEnum::eMaxNumEntity];
	//REntry				mCommsNms[OmnNetEnum::eMaxNumEntity];

	OmnMutexPtr			mGroupLock;
	OmnCommMgrPtr		mCommMgr;

public:
	OmnCommGroupMgr(const OmnCommMgrPtr &commMgr);
	~OmnCommGroupMgr();

	OmnRslt				config(const OmnXmlItemPtr &def);

	OmnCommGroupPtr		getGroup(const int groupId);
	OmnCommGroupPtr		getGroup(const OmnString &def);
	OmnCommGroupPtr		getGroup(const OmnXmlItemPtr &def);

	// 
	// Chen Ding, 05/10/2003
	//
	OmnCommGroupPtr		getGroupByTargetId(const int targetId);

	OmnRslt				removeGroup(const int groupId);
	bool				addGroup(const OmnCommGroupPtr &group);

	OmnRslt				sendTo(const OmnMsgPtr &msg, const int entityId);
	OmnRslt				sendTo(const OmnConnBuffPtr &buff, const int entityId);
	OmnRslt				sendTo(const OmnString &data,
							   const int toEntityId,
							   const OmnMsgId::E protocol);
	OmnRslt				sendTo(const char *data, 
							   const int len, 
							   const int entityId,
							   const OmnMsgId::E msgCat);

	//OmnRslt				startReadingEntity(const int entityId, 
	//								 const OmnMsgId::E protocol,
	//								 const OmnCommListenerPtr &callback);
	OmnRslt				startReadingComm( const int groupId, 
									 const OmnCommListenerPtr &callback);
	//OmnRslt				stopReadingEntity(const int entityId, 
	//								 const OmnMsgId::E prot,
	//								 const OmnCommListenerPtr &requester);
	OmnRslt				stopReadingComm(const int entityId,
									 const OmnCommListenerPtr &requester);

	OmnRslt				checkGroupId(const int groupId);

    bool            	isCommGroupDefined(const int groupId) const;
    bool            	usingSameProtocol(const int group1, const int group2) const;
    OmnCommProt::E     	getCommGroupProtocol(const int groupId) const;

	//OmnRslt				getEntityAddr(const int entityId,
	//								 const OmnMsgId::E protocol,
	//								 OmnIpAddr &ipAddr,
	//								 int &port);

private:
	//OmnRslt 			removeCommGroup(REntry theArray[], const int groupId);

};

#endif

