////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Network.cpp
// Description:
//	This is a singleton class that manages all the network properties
//	and resources.
//
// Modification History:
// 	Change-0001, 
// 	Chen Ding, 05/10/2003
// 		mDefaultCaComm;	
// 		mDefaultCaId;
// 		mDefaultCallAgentId;   
// 		Added a member function: createCaMap(). Refer to the comments
// 		in that member function. 
//
////////////////////////////////////////////////////////////////////////////

#include "Network/Network.h"

#include "Alarm/Alarm.h"
#include "Alarm/AlarmMgr.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Message/Msg.h"
#include "Network/IpBubble.h"
#include "Network/NetIf.h"
#include "Network/NetEntityMgr.h"
#include "Network/NetIfMgr.h"
#include "NMS/Nms.h"
#include "Porting/IPv6.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Tracer/Tracer.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/CommMgr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommGroupMgr.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlItemName.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(OmnNetworkSingleton, 
				 OmnNetwork, 
				 OmnNetworkSelf,
 			    "OmnNetwork");

extern OmnAlarmMgr *	OmnAlarmMgrSelf;

// 
// Chen Ding, 09/08/2003
//
// OmnString OmnNetwork::mDfltHostName = "NoHostName";
OmnString OmnNetwork::mDfltHostName = "mr.uptech.com";

OmnNetwork::OmnNetwork()
:
mCommMgr(OmnNew OmnCommMgr()),
mEntityMgr(OmnNew OmnNetEntityMgr()),
mNetIfMgr(OmnNew OmnNetIfMgr()),
mCommGroupMgr(OmnNew OmnCommGroupMgr(mCommMgr))
{
}


OmnNetwork::~OmnNetwork()
{
}


bool
OmnNetwork::config(const AosXmlTagPtr &conf)
{
    //
    //  It assumes the following:
    //
    //  <NetConfig>
	//		<ThisNodeDef>
	//			...
	//		</ThisNodeDef>
    //      <EntityDefs>		; Optional
    //          ...             ; Refer to NetEntityMgr constructor 
    //      </EntityDefs>
	//		<NetInterfaces>		; Optional
	//			...
	//		</NetInterfaces>
    //      <Routes>			; Optional
    //          ...             ; Refer to NetTopology 
    //      </Routes>
    //      <CommGroupMgr>		; Optional
    //          ...             ; refer to configCommGroups
    //      </CommGroupMgr>
	//		<NotifyEntities>
	//			<NotifyEntity>
	//				<Entity>
	//				<LocalAddr>
	//				<LocalPort>
	//			</NotifyEntity>
	//			...
	//		</NotifyEntities>
	//	</NetConfig>
    //

    //
    // Create OmnNetEntityMgr 
    //
	if (!conf)
	{
		return true;
	}

	AosXmlTagPtr def;
	def = conf->getFirstChild(OmnXmlItemName::eNetConfig);
	if (def.isNull())
	{
		return true;
	}

	/* Chen Ding, 08/15/2011
    AsoXmlTagPtr entityDefs = def->getFirstChild(OmnXmlItemName::eEntityDefs);
	OmnRslt rslt;
	if (!entityDefs.isNull())
	{
		rslt = mEntityMgr->config(entityDefs);
		if (!rslt)
		{
			return rslt;
		}
	}

	//
	// Retrieve the <NetInterfaces> item and configure OmnNetIfMgr
	//
	AosXmlTagPtr netIfMgrDef = def->getFirstChild(OmnXmlItemName::eNetInterfaces);
	if (!netIfMgrDef.isNull())
	{
		rslt = mNetIfMgr->config(netIfMgrDef);
		if (!rslt)
		{
			return rslt;
		}
	}

    //
    // Retrieve the <CommGroupMgr> item and configure comm groups
    //
    AosXmlTagPtr commGroups = def->getFirstChild(OmnXmlItemName::eCommGroupMgr);
	if (!commGroups.isNull())
	{
		rslt = mCommGroupMgr->config(commGroups);
		if (!rslt)
		{
			return rslt;
		}
	}

	//
	// Retrieve ThisNodeDef
	//
	mSelfId = def->getAttrInt(OmnXmlItemName::eSelfId, -1);
	if (mSelfId > 0)
	{
		mSelf = mEntityMgr->getNetEntity(mSelfId);
	}

	//
	// AlarmMgr needs a comm group to send alarms to a central manager.
	// Since AlarmMgr is the first class to start up, we have to postpone
	// the configuration. 
	//
	//int alarmMgrComm = def->getInt(OmnXmlItemName::eCommGroupId, -1);
	//OmnCommGroupPtr comm = getCommGroup(alarmMgrComm);
	//if (!comm.isNull())
	//{
	//	OmnAlarmMgrSelf->setCommGroup(comm);
	//}
	//OmnAlarmMgrSelf->setSelfId(mSelfId);

	//
	// Chen Ding, 08/01/2003, 2003-0094
	//
	AosXmlTagPtr notifieds = def->getFirstChild(OmnXmlItemName::eNotifyEntities);
	if (!notifieds.isNull())
	{
		AosXmlTagPtr item= notifieds->getFirstChild();
		while (item)
		{
			if (!mNotified.hasMoreSpace())
			{
				OmnAlarm << "Too many notified: " << enderr;
				return false;
			}

			int entity = item->getAttrInt(OmnXmlItemName::eEntity, -1);
			OmnIpAddr addr(item->getAttrStr(OmnXmlItemName::eLocalAddr));
			int port = item->getAttrInt(OmnXmlItemName::eLocalPort, -1);
			if (entity < 0 || 
				!addr.isValid() ||
				port < 0)
			{
				OmnAlarm << "Invalid Notified Entity config: "
					<< item->toString() << enderr;
				return false;
			}

			NotifyEntityEntry entry;
			entry.targetId = entity;
			entry.localAddr = addr;
			entry.localPort = port;
			mNotified.append(entry);
		}	
	}
	*/
	OmnNotImplementedYet;
	return false;

	return true;
}


OmnString
OmnNetwork::getMyName() const
{
	if (mSelf.isNull())
	{
		return "NoName";
	}

	return mSelf->getName();
}


bool
OmnNetwork::start()
{
	// 
	// Chen Ding, 05/21/2003, Change-0002
	//
	// createCaMap();
	return true;
}


bool
OmnNetwork::stop()
{
	return true;
}


OmnCommGroupPtr
OmnNetwork::getCommGroup(const int groupId) const
{
	return mCommGroupMgr->getGroup(groupId);
}


OmnCommGroupPtr
OmnNetwork::getCommGroup(const OmnXmlItemPtr &def) const
{
	return mCommGroupMgr->getGroup(def);
}


OmnCommGroupPtr
OmnNetwork::getCommGroup(const OmnString &def) const
{
	return mCommGroupMgr->getGroup(def);
}


/*
OmnCommGroupPtr
OmnNetwork::getCommGroup(const OmnMsgId::E protocol, 
						 const int targetEntity,
						 OmnIpAddr &remoteIpAddr,
						 int &remotePort)
{
	return mNetConnMgr->getGroup(protocol, targetEntity,
				remoteIpAddr, remotePort);
}


OmnCommGroupPtr
OmnNetwork::getCommGroup(const OmnMsgId::E protocol, const int targetEntity)
{
	return mNetConnMgr->getGroup(protocol, targetEntity);
}
*/


bool			
OmnNetwork::getSendingLocalAddr(
						const OmnMsgId::E protocol,
						const int targetEntity,
						OmnIpAddr &localAddr,
						int &localPort)
{
	//
	// Chen Ding, 08/01/2003, 2003-0094
	//
	// return mNetConnMgr->getSendingLocalAddr(protocol,
	// 			targetEntity, localAddr, localPort);
	for (int i=0; i<mNotified.size(); i++)
	{
		if (mNotified[i].targetId == targetEntity)
		{
			localAddr = mNotified[i].localAddr;
			localPort = mNotified[i].localPort;
			return true;
		}
	}

	OmnWarn << "Comm group for Entity: " << targetEntity 
        << " and protocol: " << protocol
        << " not defined!" << enderr;
	return false;
}


OmnRslt
OmnNetwork::removeCommGroup(const int groupId)
{
	return mCommGroupMgr->removeGroup(groupId);
}


OmnRslt
OmnNetwork::addCommGroup(const OmnCommGroupPtr &group)
{
	return mCommGroupMgr->addGroup(group);
}


OmnRslt
OmnNetwork::sendMsgTo(const OmnMsgPtr &msg, const int toEntity)
{
	//
	// If the message contains valid remote port, use its
	// remote IP address and port. Otherwise, use the 
	// IP address and port retrieved from the CommGroupMgr.
	//
/*	OmnIpAddr remoteIpAddr;
	int remotePort;
	OmnCommGroupPtr commGroup = 
		mNetConnMgr->getGroup(msg->getMsgCat(), toEntity,
			remoteIpAddr, remotePort);

	if (commGroup.isNull())
	{
		return OmnAlarm << OmnAlarmId::eAlarmCommError
			<< "No comm group found for: " 
		 	<< toEntity 
		 	<< ". Message: " << msg->toString() << enderr;
	}

	if (msg->getRecverPort() <= 0)
	{
		//
		// Use the comm group IP address and port
		//
		msg->setRecverAddr(remoteIpAddr, remotePort);
	}

	return commGroup->sendMsgTo(msg);
	*/

	OmnNotImplementedYet;
	return false;
}


OmnRslt
OmnNetwork::sendTo(const OmnConnBuffPtr &buff, const int toEntityId)
{
	//
	// 'buff' may contain remote site address information. If
	// it contains valid remote information, use it as the 
	// receiver. Otherwise, retrieve the receiver from 
	// mCommGroupMgr.
	//
	// Note: It is very important for the caller of this function	
	// to make sure the remote address information is set to invalid
	// if it wants the comm group to determine who the receiver is.
	//

/*	OmnIpAddr remoteIpAddr;
	int remotePort;

	OmnCommGroupPtr commGroup = 
        mNetConnMgr->getGroup(buff->getProtocol(), toEntityId,
			remoteIpAddr, remotePort);

    if (commGroup.isNull())
    {
        return OmnAlarm << "No Comm Group To Send to: "
            << toEntityId << enderr;
    }

	if (buff->getRemotePort() <= 0)
	{
		//
		// It is invalid address.
		//
		buff->setRemoteAddr(remoteIpAddr, remotePort);
	}	

    return commGroup->sendTo(buff);
	*/

	OmnNotImplementedYet;
	return false;
}


/*
OmnRslt
OmnNetwork::sendTo(const OmnString &data, 
				   const int toEntityId,
				   const OmnMsgId::E protocol)
{
	OmnCommGroupPtr commGroup =
        mCommGroupMgr->getGroup(protocol, toEntityId);
    if (commGroup.isNull())
    {
        return OmnAlarmor::log(OmnFileLine, OmnAlarmId::eNoCommGroupToSend,
            OmnStrUtil::itoa(toEntityId));
    }

    return commGroup->sendTo(data);
}
*/


OmnRslt             
OmnNetwork::sendTo(const char *data,
                   const int len,
                   const int toEntityId,
                   const OmnMsgId::E protocol)
{
/*	OmnIpAddr remoteIpAddr;
	int remotePort;
    OmnCommGroupPtr commGroup = 
        mNetConnMgr->getGroup(protocol, toEntityId,
			remoteIpAddr, remotePort);
    if (commGroup.isNull())
    {
        return OmnAlarm << "OmnAlarmId::eNoCommGroupToSend: "
            << toEntityId << enderr;
    }

    return commGroup->sendTo(data, len, remoteIpAddr, remotePort);
	*/

	OmnNotImplementedYet;
	return false;
}


OmnRslt
OmnNetwork::startReadingComm(const int commGroupId,
						 const OmnCommListenerPtr &callback)
{
	return mCommGroupMgr->startReadingComm(commGroupId, callback);
}


/*
OmnRslt
OmnNetwork::startReading(const int fromEntityId,
                         const OmnMsgId::E protocol,
                         const OmnCommListenerPtr &callback)
{
    return mNetConnMgr->startReadingEntity(fromEntityId, protocol, callback);
}


OmnRslt
OmnNetwork::stopReading(const int entityId, 
						const OmnMsgId::E protocol,
						const OmnCommListenerPtr &requester)
{
	return mNetConnMgr->stopReadingEntity(entityId, protocol, requester);
}
*/


OmnRslt
OmnNetwork::stopReadingComm(const int groupId,
							const OmnCommListenerPtr &requester)
{
	return mCommGroupMgr->stopReadingComm(groupId, requester);
}


bool            
OmnNetwork::isEntityDefined(const int entityId) const
{
	return mEntityMgr->isEntityDefined(entityId);
}


/*
OmnRslt         
OmnNetwork::getAdjEntities(const OmnMsgId::E protocol, 
				std::vector<OmnNetRoute> &entities)
{
	return mNetConnMgr->getAdjEntities(protocol, entities);
}


OmnRslt         
OmnNetwork::getAdjEntities(const OmnNetEtyType::E type, 
				std::vector<OmnNetRoute> &entities)
{
	return mNetConnMgr->getAdjEntities(type, entities);
}
*/

bool            
OmnNetwork::isCommGroupDefined(const int groupId) const
{
	return mCommGroupMgr->isCommGroupDefined(groupId);
}


bool            
OmnNetwork::usingSameProtocol(const int group1, const int group2) const
{
	return mCommGroupMgr->usingSameProtocol(group1, group2);
}


/*
OmnMsgId::E     
OmnNetwork::getCommGroupProtocol(const int groupId) const
{
	return mCommGroupMgr->getCommGroupProtocol(groupId);
}
*/


OmnNetEntityPtr
OmnNetwork::getNetEntity(const int entityId) const
{
	return mEntityMgr->getNetEntity(entityId);
}


/*
OmnNetEntityPtr
OmnNetwork::getMyself() const
{
	return mSelf;
}
*/


OmnIpBubblePtr
OmnNetwork::getIpBubble(const int bubbleId) const
{
    return mEntityMgr->getIpBubble(bubbleId);
}


OmnNetIfPtr
OmnNetwork::getNetInterface(const int netIfId) const
{
	return mNetIfMgr->getNetInterface(netIfId);
}


/*
OmnRslt
OmnNetwork::getEntityAddr(const int entityId,
						  const OmnMsgId::E protocol,
						  OmnIpAddr &ipAddr,
						  int &port)
{
	//
	// When configurating a network, one may specify an IP 
	// address and port to a pair of (EntityId, Protocol),
	// which means that if one wants to send a message
	// to the entity "entityId" through the protocol 
	// "protocol", one can use the associated IP address
	// and port. 
	// 
	// Note that in PACE, programs shold stay away from 
	// using IP addresses and ports directly. Figuring 
	// out IP addresses and ports should be PACE's job.
	// 
	// For instance, if one wants to send a message to 
	// (Entity, Protocol), it can simply call the member
	// function: 
	//
	//	sendTo(msg, entityId, protocol)
	//
	// We will figure out how to send the message.
	//

	return mNetConnMgr->getEntityAddr(entityId, protocol, ipAddr, port);
}
*/


OmnNetEtyType::E    
OmnNetwork::getNextEntityType(const int netEntityId)
{
	//
	// When SPNR processes an MGCP message, it needs to know whether 
	// the next hop is an ALG in order to reach 'netEntity'. 
	// This function checks the network connectivity to 
	// determine:
	//
	// 1. If netEntity is directly connected to this entity,
	//    it returns the entity's type.
	// 2. Otherwise, it finds the next hop entity and returns
	//    that entity's type.
	//

	//
	// Temporarily, we always return that netEntity's type
	//
	OmnAlarm << "Not fully implemented yet!" << enderr;

	return getEntityType(netEntityId);
}


OmnNetEtyType::E
OmnNetwork::getEntityType(const int netEntityId) const
{
	OmnNetEntityPtr net = getNetEntity(netEntityId);
	if (net.isNull())
	{
		return OmnNetEtyType::eInvalidNetEntity;
	}

	return net->getEntityType();
}


bool
OmnNetwork::isNiidDefined(const int niid) const
{
	return mNetIfMgr->isNiidDefined(niid);
}


int
OmnNetwork::getMyEntityId() const
{
	return mSelfId;
}


int                 
OmnNetwork::getServingSpnrId(const int algId)
{
	//
	// An ALG Server wants to assign an SPNR to an ALG. 
	// It calls this function to determine what is the 
	// best for the given ALG. 
	//

	OmnAlarm << "Not implemented yet" << enderr;
	return -1;
}


/*
OmnString
OmnNetwork::getPID() const
{
	return mThisNode->getPID();
}
*/


/*
OmnIpAddr
OmnNetwork::getAlgDbServerIpAddr() const
{
	if (mAlgDbServer.isNull())
	{
		OmnProgWarn << OmnAlarmId::eWarnProgError
			<< "ALG Database Server is null" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mAlgDbServer->getIpAddr();
}


int
OmnNetwork::getAlgDbServerPort() const
{
	if (mAlgDbServer.isNull())
	{
		OmnProgWarn << OmnAlarmId::eWarnProgError
			<< "ALG Database Server is null" << enderr;
		return -1;
	}

	return mAlgDbServer->getPort();
}
*/


OmnIpAddr
OmnNetwork::getIpAddr(const int niid) const
{
	return mNetIfMgr->getIpAddr(niid);
}

