////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GroupRRobin.cpp
// Description:
//	A Round Robin group maintains a group of load balanced comms. Each
//  comm has a reading thread. They read incoming messages and put
//  them into mMsgQueue. When someone reads a message from this 
//  group, it reads from this queue. If no message in the queue, it will
//  wait until it there is something or the timer expires.
//
//  If a comm connection breaks, this class takes the comm out of the 
//  reading and tries to restore the comm.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/GroupRRobin.h"

#include "Alarm/Alarm.h"
#include "Message/Msg.h"
#include "Network/Network.h"
#include "Network/NetIf.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/Comm.h"
#include "UtilComm/CommMgr.h"
#include "UtilComm/CommGroupMgr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/UdpComm.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"


extern OmnNetwork *		OmnNetworkSelf;


// 
// This constructor creates a MGCP comm group
//
OmnGroupRRobin::OmnGroupRRobin(const OmnCommMgrPtr &commMgr,
							   const OmnString &name,
			   				   const OmnCommProt::E protocol,
							   const bool proxySenderAddr,
			   				   const OmnIpAddr &localAddr,
			   				   const int localPort)
:
OmnCommGroup(commMgr),
mCommIndex(0),
mNumComms(0),
mCommLock(OmnNew OmnMutex()),
mIsReading(false)
{
	mProxySenderAddr = proxySenderAddr;
	mName = name;
	mProtocol = protocol;
	mGroupId = OmnCommGroup::getNewGroupId();
    mTargetId = 0;
	mTargetType = OmnNetEtyType::eNoEntityType;
	OmnCommGroupPtr thisPtr(this, false);
    OmnCommPtr comm = OmnNew OmnUdpComm(mName, mProtocol, mProxySenderAddr,
		localAddr, localPort, thisPtr);
	mComms[mNumComms++] = comm;
}


OmnGroupRRobin::OmnGroupRRobin(const OmnCommMgrPtr &commMgr, 
							   const OmnXmlItemPtr &def)
:
OmnCommGroup(commMgr),
mCommIndex(0),
mNumComms(0),
mCommLock(OmnNew OmnMutex()),
mIsReading(false)
{
	OmnRslt rslt = config(def);
	if (!rslt)
	{
		OmnAlarm << "Failed to create CommGroup: " << def->toString() << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eConfigError, rslt.getErrmsg());
		throw e;
	}

	OmnTraceComm << "RoundRobin CommGroup created: " << mGroupId << endl;
}


OmnGroupRRobin::~OmnGroupRRobin()
{
	OmnTrace << "Deleting GroupRRobin" << endl;
}


OmnRslt
OmnGroupRRobin::config(const OmnXmlItemPtr &def)
{
    //
    // It assumes the following:
    // 
    //  <CommGroup>
    //      <Name>
	//		<CommGroupId>
    //      <Protocol>
	//      <ProxySenderAddr>	; Optional
    //      <TargetId>			; Optional
    //      <GroupType> 		; should be RoundRobin 
    //      <Comms>
    //          <Comm>
    //              <CommType>    
    //              <LocalNiid>
    //              <LocalPort>
    //              <RemoteIpAddr>;		optional
    //              <RemotePort>;		optional
    //          </Comm>
    //          ...             ; One per backup
    //      </Comms>           
    //  </CommGroup>
    //  
    // It assumes the type is "RoundRobin".
    // 
    
	mCommLock->lock();

    // Retrieve the Group Name
    mName = def->getStr(OmnXmlItemName::eName, "NoName");
    
    // Retrieve the protocol
    OmnString protocol = def->getStr(OmnXmlItemName::eProtocol, "NoProtocol");
    mProtocol = OmnCommProt::strToEnum(protocol);
    if (mProtocol == OmnCommProt::eInvalid)   
    {
        //
        // It is not a protocol
        // 
		mCommLock->unlock();
        return OmnWarn << OmnErrId::eWarnCommError
			<< "Invalid protocol: " 
			<< def->toString() << enderr;
    }       
    
	mProxySenderAddr = def->getBool(OmnXmlItemName::eProxySenderAddr, false);

	//
	// Retrieve CommGroupId
	//
	mGroupId = def->getInt(OmnXmlItemName::eCommGroupId, -1);

	//
	// It is possible that CommGroupId is not specified. In this case, 
	// we need to allocate an ID for it.
	//
	if (mGroupId == -1)
	{
		//
		// It did not specify comm group. 
		// 
		mGroupId = OmnCommGroup::getNewGroupId();
	}

    //
    // Retrieve Target ID
    // 
    mTargetId = def->getInt(OmnXmlItemName::eTargetId, 0);
	mTargetType = OmnNetworkSelf->getEntityType(mTargetId);

	if (mTargetType == OmnNetEtyType::eInvalidNetEntity)
	{
    	// Retrieve EntityType
    	OmnString entityTypeStr = def->getStr(OmnXmlItemName::eTargetType, "");
    	mTargetType = OmnNetEtyType::nameToEnum(entityTypeStr);
	}

    //
    // Create the connections
    //
	OmnCommGroupPtr thisPtr(this, false);
    OmnXmlItemPtr connDef = def->getItem(OmnXmlItemName::eComms);
    connDef->reset();
    while (connDef->hasMore())
    {
        OmnXmlItemPtr item = connDef->next();
        OmnCommPtr comm = OmnComm::createComm(mProtocol, mProxySenderAddr, 
			item, mTargetId, mTargetType, thisPtr, __FILE__, __LINE__);
        if (comm.isNull())
        {
            //
            // This is not a correct connection definition
            //
			mCommLock->unlock();
            return OmnWarn << OmnErrId::eWarnCommError
				<< "Failed to create comm: " 
				<< item->toString() << enderr;
        }

        //
        // Creation successful.
        //
		if (mNumComms >= eMaxComms)
		{
			mCommLock->unlock();
			return OmnAlarm << "Too many comms for RoundRobin Comm Group: "
				<< toString() << enderr;
		}
		
		mComms[mNumComms++] = comm;
    }

	//
	// Make sure there is at least one comm.
	//
	if (mNumComms <= 0)
	{
		// 
		// Found no comms
		//
		mCommLock->unlock();
		return OmnAlarm << "No comms found: " 
			<< def->toString() << enderr;
	}

	mCommLock->unlock();
    return true;
}


bool
OmnGroupRRobin::addComm(const OmnCommPtr &comm)
{
	mCommLock->lock();
	if (mNumComms >= eMaxComms)
	{
		//
		// Too many comms for a the group
		//
		OmnAlarm << "Configuration Error: "  
			<< ": Too many comms for RoundRobin Group: " 
			<< toString() << enderr;
		return false;
	}

	mComms[mNumComms++] = comm;
	mCommLock->unlock();
	return true;
}


bool
OmnGroupRRobin::removeComm(const int commId)
{
	mCommLock->lock();
	for (int i=0; i<mNumComms; i++)
	{
		if ( mComms[i]->getCommId() == commId )
		{
			//
			// Found it. Stop its reading first. 
			//
			OmnGroupRRobinPtr thisPtr(this, false);

			//
			// If it is the last element, reset the element and return
			//
			if (i == mNumComms-1)
			{
				//
				// It is the last element
				//
				mComms[i] = 0;
				mCommLock->unlock();
				return true;	
			}

			//
			// Otherwise, it is not the last element. Swap the last with it.
			//
			mNumComms--;
			mComms[i] = mComms[mNumComms];
			mComms[mNumComms] = 0;
			mCommLock->unlock();
			return true;
		}
	}

	mCommLock->unlock();

	//
	// Did not find it.
	//
	return false;
}


OmnRslt
OmnGroupRRobin::readFrom(OmnConnBuffPtr &buff, 
						 const int timerSec,
						 const int timerMsec,
						 bool &isTimeout)
{
	//
	// Call this function to read a message. Optionally the caller
	// can specify a timer (timerSec, timerUsec), where 'timerSec' is 
	// the timer second part and 'timeruSec' is the microsecond part.
	// If timer is not specified (i.e., timerSec = -1), it will not 
	// return until a message is read. If timer is specified and the
	// timer expired before it reads anything, it returns false and 
	// reason is "eTimeout", 'msg' points to null. If failed to read, 
	// it returns false.
	//
	// Internally, this class has multiple comms. Each has its own 
	// thread to read. When they read, they will put messages into 
	// mMsgQueue. 
	//

	return OmnAlarm << "Not implemented yet" << enderr;
/*
	while (1)
	{
		mMsgLock->lock();
		if (mMsgQueue.entries() <= 0)
		{
			//
			// There is nothing to read. Need to wait
			//
			bool isTimeout = false;
			if (mMsgCondVar->timedWait(mMsgLock, isTimeout, msec))
			{
				//
				// Check whether it is timeout
				//
				// QQQQ, it should check isTimeout first, then unlock();
				// because isTimeout is a shared variable by multi-threads.
				mMsgLock->unlock();
				if (isTimeout)
				{
					buff = 0;
					return OmnRslt(OmnComm::eTimeout);
				}

				//
				// Otherwise, it read something.
				//
				continue;
			}
			else
			{
				//
				// Wait failed. This should never happen
				//
				mMsgLock->unlock();
				return OmnError::log(OmnFileLine, OmnErrId::eCondVarWaitError);
			}
		}

		//
		// This means that the queue has something. 
		//
		buff = mMsgQueue.popFront();
		mMsgLock->unlock();
		return true;
	}

	//
	// Should never come to this point
	//
	return true;
*/
}


OmnRslt				
OmnGroupRRobin::sendMsgTo(const OmnMsgPtr &msg)
{
	OmnSerialTo s;
	if (!(msg->serializeTo(s)))
	{
		return OmnAlarm << "Failed to serialize the message" << enderr;
	}

	if (mProxySenderAddr)
	{
		char *data = (char *)s.getData();
		uint length = s.getDataLength();

		OmnTraceHB << "Attach sender address: " 
			<< (msg->getRecverAddr()).toString() 
			<< ":" << msg->getRecverPort() << ":" << length << endl;

		OmnConnBuff::attachSenderAddr(data, length, 
			msg->getRecverAddr(), msg->getRecverPort());

		OmnTraceHB << "After attach: " << length << endl;

OmnTrace << "Send through proxy" << endl;
		return sendTo(data, length, OmnIpAddr::eInvalidIpAddr, -1);
	}
	else
	{
		return sendTo((const char *)s.getData(), s.getDataLength(),
				msg->getRecverAddr(), msg->getRecverPort());
	}
}


OmnRslt
OmnGroupRRobin::sendTo(const OmnConnBuffPtr &buff)
{
	if (mProxySenderAddr)
	{
		OmnTraceHB << "Attach sender address: " 
			<< (buff->getRemoteAddr()).toString()
			<< ":" << buff->getRemotePort() << endl;

		buff->attachSenderAddr();
		return sendTo(buff->getBuffer(), buff->getDataLength(),
					OmnIpAddr::eInvalidIpAddr, -1);
	}
	else
	{
		return sendTo(buff->getBuffer(), buff->getDataLength(),
			buff->getRemoteAddr(), buff->getRemotePort());
	}
}


bool
OmnGroupRRobin::forward(const OmnConnBuffPtr &buff)
{
	// 
	// If mProxySenderAddr is true, it needs to attach the sender
	// IP address and port (contained in buff) to the end of 'buff'.
	// It then forwards the message.
	//
	// Note that forwarding means that the comm group knows where
	// the message should go. It will ignore the receiver IP address
	// and port in 'buff' and rely on the comm group to determine
	// the receiver IP address and port.
	//
	if (mProxySenderAddr)
	{
		OmnTraceHB << "Attach sender address" << endl;
		buff->attachSenderAddr();
	}

	return sendTo(buff->getBuffer(), buff->getDataLength(),
					OmnIpAddr::eInvalidIpAddr, -1);
}


OmnRslt 
OmnGroupRRobin::sendTo(const char *data, const int length)
{
	if (mProxySenderAddr)
	{
		OmnTraceHB << "Attach sender address" << endl;
		uint len = length;
		OmnConnBuff::attachSenderAddr((char*)data, len, 
			OmnIpAddr::eInvalidIpAddr, -1);
		return sendTo(data, len, OmnIpAddr::eInvalidIpAddr, -1);
	}

	return sendTo(data, length, OmnIpAddr::eInvalidIpAddr, -1);
}


OmnRslt 
OmnGroupRRobin::sendTo(const char *buff, 
					   const int length,
					   const OmnIpAddr &recvIpAddr,
					   const int recvPort)
{
	//
	// It picks the next comm to send.
	//
	mCommLock->lock();

	OmnTrace << "Send to: " << recvIpAddr.toString() << ":" << recvPort << endl;
	
	//
	// Make sure there is comm
	//
	if (mNumComms == 0)
	{
		mCommLock->unlock();
		return OmnAlarm << "There is no more comms in this comm group: " 
			<< toString() << enderr;
	}

	//
	// Move the index to the next one
	// 
	mCommIndex++;
	if (mCommIndex >= mNumComms)
	{
		mCommIndex = 0;
	}

	//
	// Send the msg
	//
	OmnRslt rslt = 
		mComms[mCommIndex]->sendTo(buff, length, recvIpAddr, recvPort);

	mCommLock->unlock();
	if (rslt)
	{
		//
		// Sending successful, return
		//
		return rslt;
	}
	else
	{
		//
		// Failed to send. Need to remove the comm and try to restore it.
		// 
		return rslt;

		/*
		OmnCommPtr comm = mComms.crtValue();
		mComms.eraseCrt();
		comm->stopReading();
		switch ((OmnComm::E)rslt.getReason())
		{
		case OmnComm::eReconnect:
			 mCommMgr->restoreComm(comm);
			 break;

		default:
			 OmnError::log(OmnFileLine, OmnErrId::eUnrecognizedCommErrReason,
				OmnStrUtil::itoa(rslt.getReason()));
			 break;
		}
		*/
	}
}


bool
OmnGroupRRobin::commRestored(const OmnCommPtr &comm)
{
	//
	// This is because this class experienced some problem with the comm.
	// It asked OmnCommMgr to stored the interface. Now the comm
	// has been restored. This function will add the comm.
	// 

	mCommLock->lock();
	if (mNumComms >= eMaxComms)
	{
		//
		// Too many comms
		//
		OmnAlarm << "Too many comms for this group: " << toString() << enderr;
		return false;
	}

	mComms[mNumComms++];
	mCommLock->unlock();
	return true;
}


OmnRslt     
OmnGroupRRobin::startReading(const OmnCommListenerPtr &requester)
{
	//
	// Call all comms to start reading
	//
	mCommLock->lock();
	for (int i=0; i<mNumComms; i++)
	{
		mComms[i]->startReading(requester);
	}

	mIsReading = true;

	mCommLock->unlock();
	return true;
}


OmnRslt     
OmnGroupRRobin::stopReading(const OmnCommListenerPtr &requester)
{
    //
    // Call all comms to stop reading
    //
    mCommLock->lock();
	for (int i=0; i<mNumComms; i++)
    {
        mComms[i]->stopReading(requester);
    }
	
	mIsReading = false;

    mCommLock->unlock();
    return true;
}

OmnString
OmnGroupRRobin::toString() const
{
	OmnString str = OmnCommGroup::toString()
						<< "\n	CommIndex = " << OmnStrUtil::itoa(mCommIndex)
						<< "\n	NumComms = " << OmnStrUtil::itoa(mNumComms)
						<< "\n	IsReading = " << OmnStrUtil::itoa(mIsReading);
	return str;
}


int
OmnGroupRRobin::getNiid() const
{
	if (mComms[0].isNull())
	{
		OmnAlarm << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return -1;
	}

	return mComms[0]->getNiid();
}


OmnIpAddr
OmnGroupRRobin::getLocalIpAddr() const
{
	if (mComms[0].isNull())
	{
		OmnWarn << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mComms[0]->getLocalIpAddr();
}


int
OmnGroupRRobin::getLocalPort() const
{
	if (mComms[0].isNull())
	{
		OmnWarn << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return -1;
	}

	return mComms[0]->getLocalPort();
}


// 
// Chen Ding, 05/10/2003
//
OmnIpAddr		
OmnGroupRRobin::getRemoteAddr() const
{
	if (mComms[0].isNull())
	{
		OmnWarn << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mComms[0]->getRemoteAddr();
}


// 
// Chen Ding, 05/10/2003
//
int				
OmnGroupRRobin::getRemotePort() const
{
	if (mComms[0].isNull())
	{
		OmnWarn << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return -1;
	}

	return mComms[0]->getRemotePort();
}


