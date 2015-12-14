////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Network.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetworkMgr_h
#define Omn_Network_NetworkMgr_h

#include "Message/Ptrs.h"
#include "Message/MsgId.h"
#include "Network/Ptrs.h"
#include "Network/NetEtyType.h"
#include "Network/NetRoute.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Array10.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include <vector>


OmnDefineSingletonClass(OmnNetworkSingleton, 
						OmnNetwork,
						OmnNetworkSelf,
						OmnSingletonObjId::eNetwork, 
						"Network");


class OmnIpAddr;

class OmnNetwork
{
private:
	enum
	{
		eMaxCa = 5
	};

	struct CaMapEntry
	{
		OmnIpAddr		mAddr;
		OmnCommGroupPtr	mComm;
		int				mCaId;
	};

	// 
	// Chen Ding, 08/01/2003, 2003-0094
	// 
	struct NotifyEntityEntry
	{
		int			targetId;
		OmnIpAddr	localAddr;
		int			localPort;
	};
	OmnArray10<NotifyEntityEntry>	mNotified;


	OmnCommMgrPtr			mCommMgr;
	OmnNetEntityMgrPtr		mEntityMgr;
	OmnNetIfMgrPtr			mNetIfMgr;
	OmnCommGroupMgrPtr		mCommGroupMgr;
	OmnNetEntityPtr			mSelf;
	int						mSelfId;
	int						mNumCaMapEntries;
	static OmnString		mDfltHostName;

public:
	OmnNetwork();
	virtual ~OmnNetwork();

	static OmnNetwork *	getSelf();
	bool			config(const AosXmlTagPtr &def);
	bool			start();
	bool			stop();

	//
	// member functions
	//
	OmnNetIfPtr			getNetInterface(const int netIfId) const;

	OmnNetEntityPtr		getNetEntity(const int entityId) const;
	OmnIpBubblePtr		getIpBubble(const int ipBubbleId) const;
	//OmnString			getPID() const;

	OmnCommGroupMgrPtr	getCommGroupMgr() const;
	OmnCommGroupPtr		getCommGroup(const OmnString &def) const;
	OmnCommGroupPtr		getCommGroup(const OmnXmlItemPtr &def) const;
	OmnCommGroupPtr		getCommGroup(const int groupId) const;
	OmnCommGroupPtr		getCommGroup(const OmnMsgId::E protocol,
								const int targetEntity,
								OmnIpAddr &remoteIpAddr,
								int &remotePort);
	bool			getSendingLocalAddr(
						const OmnMsgId::E protocol,
						const int targetEntity,
						OmnIpAddr &localAddr,
						int &localPort);
						
	bool			amIRoot(const int callAgentId) const;
	bool			isLocal(const int ipBubbleId) const;

	OmnRslt			addRoutes(const OmnString &str);
	OmnRslt			removeCommGroup(const int groupId);
	OmnRslt			addCommGroup(const OmnCommGroupPtr &commGroup);

	OmnRslt			getEntityAddr(const int entityId,
						   const OmnMsgId::E protocol,
						   OmnIpAddr &ipAddr,
						   int &port);

	OmnRslt			sendMsgTo(const OmnMsgPtr &msg);
	//OmnRslt			sendTo(const OmnConnBuffPtr &buff);
    OmnRslt        	sendMsgTo(const OmnMsgPtr &msg, const int toEntityId);
    OmnRslt        	sendTo(const OmnConnBuffPtr &buff, const int toEntityId);
	OmnRslt			sendTo(const OmnString &data, 
						   const int toEntityId,
						   const OmnMsgId::E protocol);
    OmnRslt        	sendTo(const char *data,
                           const int len,
                           const int toEntityId,
                           const OmnMsgId::E protocol);

    OmnRslt        	startReading(const int entityId,
							   const OmnMsgId::E protocol,
                               const OmnCommListenerPtr &callback);
    OmnRslt        	startReadingComm(const int commGroupId,
                               const OmnCommListenerPtr &callback);
    OmnRslt         stopReading(const int entityId, 
							   const OmnMsgId::E protocol,
							   const OmnCommListenerPtr &requester);	
    OmnRslt         stopReadingComm(const int groupId,
							   const OmnCommListenerPtr &requester);	

	OmnNetEntityPtr	getEntity(const int entityId);
	OmnIpAddr		getIpAddr(const int niid) const;

	OmnString		getMyName() const;
	int				getMyEntityId() const;
	OmnNetEntityPtr	getMyself() const;
	OmnIpAddr		getAlgDbServerIpAddr() const;
	int				getAlgDbServerPort() const;


	//
	// Get entities
	//

	OmnNetEtyType::E	getNextEntityType(const int netEntityId);
	OmnNetEtyType::E	getEntityType(const int entityId) const;

	int					getServingSpnrId(const int callAgentId);
	//int					getDefaultCallAgentId() const;

	//
	// Get adjacent entities
	//
	OmnRslt			getAdjEntities(const OmnMsgId::E prot, 
							std::vector<OmnNetRoute> &);
	OmnRslt			getAdjEntities(const OmnNetEtyType::E type, 
							std::vector<OmnNetRoute> &);
	
	bool			isEntityDefined(const int entitiId) const;
	bool			isNiidDefined(const int niid) const;
	bool			isCommGroupDefined(const int groupId) const;
	bool			usingSameProtocol(const int group1, const int group2) const;
	OmnMsgId::E		getCommGroupProtocol(const int groupId) const;

	virtual OmnString       getSysObjName() const {return "OmnNetwork";}
	virtual OmnSingletonObjId::E  getSysObjId() const 	
					{return OmnSingletonObjId::eNetwork;}

	bool			getCaInfo(const OmnIpAddr &addr,
							  OmnCommGroupPtr &comm,
							  int &entityId);

	static OmnIpAddr	getDfltIpAddr();
	static void			setDfltHostName(const OmnString &n)
						{mDfltHostName = n;}

	//
	// Chen Ding, 09/22/2003, 2003-0255
	//
	OmnCommGroupPtr 	getCommGroup(const OmnMsgId::E protocol, const int targetEntity);

private:
	OmnString 		getAlgConfig(const OmnString &remoteIpAddr) const;

	// 
	// Chen Ding, 05/10/2003, Change-0002
	//
	bool			createCaMap();
};
#endif

