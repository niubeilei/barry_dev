////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GroupReliable.cpp
// Description:
//	A Reliable Comm has one Primary Comm and one or more backup
//  comms to a remote site. All traffic goes through the 
//  primary comm. If the primary comm fails, it checks
//  whether there are any backup comms. If yes, it uses one of 
//  them to restoer the connection. Meanwhile, it tries to restore
//  the primary comm. If the primary comm comes back, 
//  it switches back. 
//
//  The primary comm is considered the best path to the site. 
//  So if it is possible, use the primary comm.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/GroupReliable.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Message/Msg.h"
#include "Network/Network.h"
#include "Thread/Mutex.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "UtilComm/Comm.h"
#include "UtilComm/CommMgr.h"
#include "UtilComm/CommGroupMgr.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"




OmnGroupReliable::OmnGroupReliable(const OmnCommMgrPtr &commMgr, 
								   const OmnXmlItemPtr &def)
:
OmnCommGroup(commMgr)
{
    OmnRslt rslt = config(def);
    if (!rslt)
    {
        OmnExcept e(OmnFileLine, OmnErrId::eConfigError, rslt.getErrmsg());
        throw e;
    }

	mBackupLock = OmnNew OmnMutex();
}


OmnGroupReliable::~OmnGroupReliable()
{
}


OmnRslt
OmnGroupReliable::config(const OmnXmlItemPtr &def)
{
	//
	// It assumes the following:
	//
	//	<CommGroup>
	//		<GroupName>
	//		<Protocol>
	//		<ProxySenderAddr>
	//		<TargetId>
	//		<TargetType>
	//		<GroupType>	; should be ReliableGroup
	//		<PrimaryConn>
	//			<LocalNiid>
	//			<LocalPort>
	//			<RemoteIpAddr>
	//			<RemotePort>
	//			<IpType>
	//		</PrimaryConn>
	//		<Backups>
	//			<Backup>
	//				<LocalNiid>
    //          	<LocalPort>
    //          	<RemoteIpAddr>
    //          	<RemotePort>
    //          	<IpType>	
	//			</Backup>
	//			...				; One per backup
	//		</Backup>
	//	</CommGroup>
	//
	// It assumes the type is "ReliableGroup". 
	//

	// Retrieve the Group Name
	mName = def->getStr(OmnXmlItemName::eName, "NoName");
	
	// Retrieve the protocol
	OmnString protocol = def->getStr(OmnXmlItemName::eProtocol);
	mProtocol = OmnCommProt::strToEnum(protocol);
	if (mProtocol == OmnCommProt::eInvalid)	
	{
		// 
		// It is not a protocol
		//
		return OmnAlarm << "Invalid Protocol: " << def->toString() << enderr;
	}

	mProxySenderAddr = def->getBool(OmnXmlItemName::eProxySenderAddr, false);

	//
	// Retrieve Target ID
	//
	mTargetId = def->getInt(OmnXmlItemName::eTargetId, -1);
    OmnString targetType = def->getStr(OmnXmlItemName::eTargetType, "None");
    mTargetType = OmnNetEtyType::nameToEnum(targetType);

	// 
	// Create the primary connection
	//
	OmnCommGroupPtr thisPtr(this, false);
	OmnXmlItemPtr primaryDef = def->getItem(OmnXmlItemName::ePrimaryConn);
	mPrimary = OmnComm::createComm(mProtocol, mProxySenderAddr, primaryDef, 
		mTargetId, mTargetType, thisPtr, __FILE__, __LINE__);
	if (mPrimary.isNull())
	{
		//
		// Failed to create the connection
		//
		return OmnAlarm << "Failed To Create Connection: " << def->toString() << enderr;
	}

	//
	// Create the backup (note that backups are optional)
	//
	OmnXmlItemPtr backupDef = def->tryItem(OmnXmlItemName::eBackups);
	if (backupDef.isNull())
	{
		//
		// There are no backups.
		//
		return true;
	}

	//
	// There are backups. Process them
	//
	backupDef->reset();
	while (backupDef->hasMore())
	{
		OmnXmlItemPtr item = backupDef->next();
		OmnCommPtr comm = OmnComm::createComm(mProtocol, mProxySenderAddr, 
			item, mTargetId, mTargetType, thisPtr, __FILE__, __LINE__);
		if (comm.isNull())
		{
			//
			// This is not a correct connection definition
			//
			return OmnAlarm << "Incorrect Conn Def: " << item->toString() << enderr;
		}

		//
		// Creation successful.
		//
		mBackup.append(comm);
	}

	return true;
}


bool
OmnGroupReliable::setPrimaryComm(const OmnCommPtr &ni)
{
	mPrimary = ni;
	return true;
}


bool
OmnGroupReliable::addBackupComm(const OmnCommPtr &ni)
{
	mBackupLock->lock();
	mBackup.append(ni);
	mBackupLock->unlock();
	return true;
}


bool
OmnGroupReliable::removeBackupComm(const int commId)
{
	mBackupLock->lock();
	int len = mBackup.entries();
	mBackup.reset();
	for (int i=0; i<len; i++)
	{
		if ( (mBackup.crtValue())->getCommId() == commId )
		{
			mBackup.eraseCrt();
			mBackupLock->unlock();
			return true;
		}
	}

	mBackupLock->unlock();
	return false;
}


OmnRslt				
OmnGroupReliable::readFrom(OmnConnBuffPtr &buff,
						   const int timerSec,
						   const int timerMsec, 
						   bool &isTimeout)
{
	//
	// If there is no primary, it can't read. 
	// 
	// If the primary is bad, check whether there is backup. If yes,
	// use one of them.
	//
	// If the primary is good, read from it.
	//
	if (mPrimary.isNull())
	{
		return OmnAlarm << "No Primary To Read From" << enderr;
	}

	OmnRslt rslt = mPrimary->readFrom(buff, timerSec, timerMsec, isTimeout);
	if (!rslt)
	{
		//
		// Send the primary to the NetInterfaceMgr to restore the interface. 
		// When the class restored the interface, it will call this class to 
		// switch backup primary back.
		//
		mCommMgr->restoreComm(mPrimary);

		//
		// Failed to read. Need to switch to one of the backup. 
		//
		mBackupLock->lock();
		mBackup.reset();
		while (mBackup.hasMore())
		{
			//
			// Switch to the next backup
			//
			mPrimary = mBackup.crtValue();

			//
			// Otherwise, read from the new 'primary'.
			//
			rslt = mPrimary->readFrom(buff, timerSec, timerMsec, isTimeout);
			if (rslt)
			{
				//
				// Read successfully. 
				//
				return true;
			}

			mBackup.next();
		}
	}

	//
	// There is no more backup.
	//
	mBackupLock->unlock();
	return OmnAlarm << "Failed To Read Reliable Comm: "
				<< "No primary and no backup: " << mName << enderr;
}


OmnRslt				
OmnGroupReliable::sendMsgTo(const OmnMsgPtr &msg)
{
	OmnSerialTo s;
	if (!msg->serializeTo(s))
	{
		return OmnAlarm << "Failed to serialize the message" << enderr;
	}

    OmnTraceCP << "\n\n    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"
        << "    Send To: " << (msg->getRecverAddr()).toString()
        << ":" << msg->getRecverPort() << "\n    "
        << s.getData()
        << "\n    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << endl;

    return sendTo((const char *)s.getData(), s.getDataLength(),
			msg->getRecverAddr(), msg->getRecverPort());
}


OmnRslt
OmnGroupReliable::sendTo(const OmnConnBuffPtr &buff)
{
	return sendTo(buff->getBuffer(), buff->getDataLength(),
			buff->getRemoteAddr(), buff->getRemotePort());
}


OmnRslt
OmnGroupReliable::sendTo(const char *data, const int length)
{
	return sendTo(data, length, OmnIpAddr::eInvalidIpAddr, -1);
}


OmnRslt
OmnGroupReliable::sendTo(const char *data, 
						 const int length,
						 const OmnIpAddr &recvIpAddr,
						 const int recvPort)
{
	//
	// If there is no primary, it can't send.
	//
	// If the primary is bad, switch to one of the backup. If failed to switch
	// can't send.
	//
	// Otherwise, send.
	//
	if (mPrimary.isNull())
	{
		//
		// No primary
		//
		return OmnAlarm << "No Primary To Send To" << enderr;
	}

	OmnRslt rslt = mPrimary->sendTo(data, length, recvIpAddr, recvPort);
	if (!rslt)
	{
		//
		// Send the primary to the NetInterfaceMgr to restore the interface. 
		// When the class restored the interface, it will call this class to 
		// switch backup primary back.
		//
		mCommMgr->restoreComm(mPrimary);

		mBackupLock->lock();
		mBackup.reset();
		while (mBackup.hasMore())
		{
			//
			// Switch to the next backup
			//
			mPrimary = mBackup.crtValue();

			//
			// Otherwise, read from the new 'primary'.
			//
			rslt = mPrimary->sendTo(data, length, recvIpAddr, recvPort);
			if (rslt)
			{
				//
				// Read successfully. 
				//
				return true;
			}

			mBackup.next();
		}
	}

	//
	// There is no more backup.
	//
	mBackupLock->unlock();
	return OmnAlarm << "Failed To Send Thru Reliable Comm. "
			<< "No primary and no backup: " << mName << enderr;
}


/*
OmnRslt				
OmnGroupReliable::sendTo(const char *data, const int length)
{
	//
	// If there is no primary, it can't send.
	//
	// If the primary is bad, switch to one of the backup. If failed to switch
	// can't send.
	//
	// Otherwise, send.
	//
	if (mPrimary.isNull())
	{
		//
		// No primary
		//
		return OmnError::log(OmnFileLine, OmnErrId::eNoPrimaryToSendTo);
	}

	OmnRslt rslt = mPrimary->sendTo(data, length);
	if (rslt.failed())
	{
		//
		// Send the primary to the NetInterfaceMgr to restore the interface. 
		// When the class restored the interface, it will call this class to 
		// switch backup primary back.
		//
		mCommMgr->restoreComm(mPrimary);

		//
		// Failed to send. Switch to one of the backup.
		//
		uint index = 0;
		while (1)
		{
			//
			// Switch to the next backup
			//
			mBackupLock->lock();
			if (index < mBackup.entries())
			{
				mPrimary = mBackup.valueAt(index++);
			}
			else
			{
				//
				// There is no more backup.
				//
				mBackupLock->unlock();
				return OmnError::log(OmnFileLine, OmnErrId::eFailedToSendThruReliableComm,
					"No primary and no backup: " + mName);
			}
			mBackupLock->unlock();

			//
			// Otherwise, read from the new 'primary'.
			//
			rslt = mPrimary->sendTo(data, length);
			if (!rslt.failed())
			{
				//
				// Read successfully. 
				//
				break;
			}
		}
	}

	//
	// Read successful. 
	//
	return rslt;
}
*/


bool
OmnGroupReliable::commRestored(const OmnCommPtr &comm)
{
	//
	// This is because this class experienced some problem with the mPrimary. 
	// It asked OmnCommMgr to stored the interface. Now the comm
	// has been restored. This function will set the mPrimary comm
	// 
	// We should use a mutex to protect the operation. However, I (Chen Ding)
	// believe without a mutex protection, it should still work.
	//
	mPrimary = comm;
	return true;
}


OmnRslt 
OmnGroupReliable::startReading(const OmnCommListenerPtr &callback)
{
	if (!mPrimary.isNull())
	{
		return mPrimary->startReading(callback);
	}

	return true;
}


OmnRslt
OmnGroupReliable::stopReading(const OmnCommListenerPtr &requester)
{
	if (!mPrimary.isNull())
	{
		return mPrimary->stopReading(requester);
	}

	return true;
}


int
OmnGroupReliable::getNiid() const
{
	if (mPrimary.isNull())
	{
		OmnAlarm << "Reliable Group: mPrimary is null!" << enderr;
		return -1;
	}

	return mPrimary->getNiid();
}


OmnIpAddr
OmnGroupReliable::getLocalIpAddr() const
{
	if (mPrimary.isNull())
	{
		OmnAlarm << "Reliable Group: mPrimary is null!" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mPrimary->getLocalIpAddr();
}


int
OmnGroupReliable::getLocalPort() const
{
	if (mPrimary.isNull())
	{
		OmnAlarm << "Reliable Group: mPrimary is null!" << enderr;
		return -1;
	}

	return mPrimary->getLocalPort();
}


bool			
OmnGroupReliable::forward(const OmnConnBuffPtr &buff)
{
	OmnWarn << "Not implemented yet" << enderr;
	return false;
}


// 
// Chen Ding, 05/10/2003
//
OmnIpAddr		
OmnGroupReliable::getRemoteAddr() const
{
	if (mPrimary.isNull())
	{
		OmnAlarm << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mPrimary->getRemoteAddr();
}


// 
// Chen Ding, 05/10/2003
//
int				
OmnGroupReliable::getRemotePort() const
{
	if (mPrimary.isNull())
	{
		OmnAlarm << "Comm group: " << mName 
			<< " does not have any comm!" << enderr;
		return -1;
	}

	return mPrimary->getRemotePort();
}



