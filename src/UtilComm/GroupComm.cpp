////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GroupComm.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/GroupComm.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Message/Msg.h"
#include "Network/Network.h"
#include "Network/NetIf.h"
#include "Porting/Select.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/Comm.h"
#include "UtilComm/CommMgr.h"
#include "UtilComm/CommGroupMgr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommTypes.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"


extern OmnNetwork *		OmnNetworkSelf;


OmnGroupComm::OmnGroupComm(const OmnXmlItemPtr &def)
:
OmnCommGroup(0),
mCommLock(OmnNew OmnMutex())
{
	// 
	// Initialize the bitmap
	//
	FD_ZERO(&mReadFds);
	FD_ZERO(&mWorkingFds);
	mFdscnt = 0;

	OmnRslt rslt = config(def);
	if (!rslt)
	{
		OmnExcept e(OmnFileLine, OmnErrId::eConfigError, "Failed to create");
		throw e;
	}

	OmnTraceComm << "GroupComm CommGroup created: " << mGroupId << endl;
}


OmnGroupComm::~OmnGroupComm()
{
	OmnTrace << "Deleting GroupComm" << endl;
}


OmnRslt
OmnGroupComm::config(const OmnXmlItemPtr &def)
{
    //
    // It assumes the following:
    // 
    //  <CommGroup>
    //      <Name>
	//		<CommGroupId>
    //      <Protocol>
    //      <GroupType> ; should be GroupComm 
    //      <Comms>
    //          <Comm>
    //              <CommType>;		// Should be UDP 
    //              <LocalNiid>
    //              <LocalPort>
    //              <RemoteIpAddr>;		optional
    //              <RemotePort>;		optional
    //          </Comm>
    //          ...            
    //      </Comms>           
    //  </CommGroup>
    //  
    // It assumes the type is "RoundRobin".
    // 
    
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
        return OmnWarn << "Invalid protocol: " 
			<< def->toString() << enderr;
    }       
    
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
	mTargetType = OmnNetEtyType::eNoEntityType;
	mTargetId = -1;

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
            return OmnWarn << OmnErrId::eWarnCommError
				<< "Failed to create comm: " 
				<< item->toString() << enderr;
        }

		if (!addComm(comm))
		{
			// 
			// Failed to add the comm. Probably too many.
			//
			return false;
		}
    }

    return true;
}


bool
OmnGroupComm::addComm(const OmnCommPtr &comm)
{
	mCommLock->lock();
	if (mComms.entries() >= eMaxComms)
	{
		//
		// Too many comms for a the group
		//
		mCommLock->unlock();
		OmnWarn << "Too many comms for GroupComm: " 
			<< mComms.entries() << enderr;
		return false;
	}

	mComms.append(comm);

	int sock = comm->getSock();
	if (sock < 0 || sock >= OmnCommTypes::eMaxSockToSelect)
	{
		mCommLock->unlock();
		OmnAlarm << "Invalid sock: " << sock << enderr;
		return false;
	}

	FD_SET(sock, &mReadFds);
	if (mFdscnt <= sock)
	{
		mFdscnt = sock+1;
	}

	mCommLock->unlock();
	return true;
}


bool
OmnGroupComm::removeComm(const int commId)
{
	mCommLock->lock();
	mComms.reset();
	while (mComms.hasMore())
	{
		if ((mComms.crtValue())->getCommId() == commId)
		{
			// 
			// Clear the bit in mReadFds
			//
			int sock = (mComms.crtValue())->getSock();
			if (sock < 0 || sock >= OmnCommTypes::eMaxSockToSelect)
			{
				OmnAlarm << "Invalid sock: " << sock << enderr;
			}
			else
			{
				FD_CLR(sock, &mReadFds);
			}

			mComms.eraseCrt();
			mCommLock->unlock();
			return true;
		}

		mComms.next();
	}

	mCommLock->unlock();

	//
	// Did not find it.
	//
	return false;
}


bool
OmnGroupComm::getEvent(OmnCommPtr &comm)
{
	OmnCommPtr c;
	mCommLock->lock();
	while (mComms.hasMore())
	{
		c = mComms.crtValue();

		if (c->getSock() <= 0)
		{
			mComms.eraseCrt();
			continue;
		}

		if (FD_ISSET(c->getSock(), &mWorkingFds))
		{
			// 
			// Found event
			//
			comm = c;
			FD_CLR(c->getSock(), &mWorkingFds);
			mCommLock->unlock();
			return true;
		}

		mComms.next();
	}

	// 
	// No event found
	//
	return false;
}


bool
OmnGroupComm::checkReading(OmnCommPtr &comm)
{
	comm = 0;

	if (getEvent(comm))
	{
		// 
		// Found event from the mWorkingFds
		//
		return true;
	}

	// 
	// No event in mWorkingFds
	//
	mWorkingFds = mReadFds;
	if (!OmnSocketSelect(mFdscnt, &mWorkingFds, 0, 0, 0))
	{
		OmnWarn << OmnErrId::eWarnCommError
			<< "Failed to select" << enderr;

		checkConns();
		return false;
	}

	if (getEvent(comm))
	{
		return true;
	}

	return false;
}


void
OmnGroupComm::checkConns()
{
	mComms.reset();
	while (mComms.hasMore())
	{
		if (!(mComms.crtValue())->isConnGood())
		{
			OmnTrace << "Bad connection found: " 
				<< (mComms.crtValue())->getSock() << endl;

			FD_CLR((mComms.crtValue())->getSock(), &mReadFds);
			mComms.eraseCrt();
		}
		else
		{
			mComms.next();
		}
	}
}


OmnRslt
OmnGroupComm::readFrom(OmnConnBuffPtr &buff, OmnCommPtr &comm)
{
	int tries = 0;
	bool isTimeout;
	while (1)
	{
		if (!checkReading(comm))
		{
			if (tries >= 3)
			{
				// 
				// We tried 3 times and still not reading anything.
				// Return faliure.
				//
				return false;
			}

			// 
			// Something is wrong. Will sleep one second and 
			// try it again.
			//
			tries++;
			OmnSleep(1);
			continue;
		}

		// 
		// Found something to read.
		//
		if (!comm->readFrom(buff, 1, 0, isTimeout))
		{
			//
			// Something is wrong with the connection
			//
			tries++;
			continue;
		}
		else
		{
			// 
			// Read something. 
			//
			return true;
		}
	}

	return false;
}


bool
OmnGroupComm::commRestored(const OmnCommPtr &comm)
{
	//
	// This is because this class experienced some problem with the comm.
	// It asked OmnCommMgr to stored the interface. Now the comm
	// has been restored. This function will add the comm.
	// 

	addComm(comm);
	return true;
}


OmnRslt 
OmnGroupComm::startReading(const OmnCommListenerPtr &requester)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


OmnRslt 
OmnGroupComm::stopReading(const OmnCommListenerPtr &requester)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


OmnRslt		
OmnGroupComm::readFrom(OmnConnBuffPtr &buff,
					   const int timerMsec)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


OmnRslt		
OmnGroupComm::sendTo(const OmnMsgPtr &msg)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


OmnRslt		
OmnGroupComm::sendTo(const OmnConnBuffPtr &buff)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


OmnRslt		
OmnGroupComm::sendTo(const char *data, const int len)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


OmnRslt		
OmnGroupComm::sendTo(const char *data, const int len,
					 const OmnIpAddr &recverIpAddr,
					 const int recverPort)
{
	return OmnWarn << "Not implemented yet" << enderr;
}


int			
OmnGroupComm::getNiid() const
{
	OmnWarn << "Not implemented yet" << enderr;
	return -1;
}


OmnIpAddr	
OmnGroupComm::getLocalIpAddr() const
{
	OmnWarn << "Not implemented yet" << enderr;
	return OmnIpAddr::eInvalidIpAddr;
}


int			
OmnGroupComm::getLocalPort() const
{
	OmnWarn << "Not implemented yet" << enderr;
	return 0;
}



