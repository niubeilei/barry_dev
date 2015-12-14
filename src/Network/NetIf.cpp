////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetIf.cpp
// Description:
//	Each SPNR has multiple network interface. This class is used to define
//  these network interfaces.
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Network/NetIf.h"

#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "UtilComm/CommGroup.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"



OmnNetIf::OmnNetIf(const OmnXmlItemPtr &def) 
:
mNiid(0),
mBandwidth(0),
mCommLock(OmnNew OmnMutex())
{
	OmnRslt rslt = config(def);
	if (!rslt)
	{
		OmnExcept e(OmnFileLine, rslt.getErrId(), rslt.toString());
		throw e;
	}
}


OmnNetIf::~OmnNetIf()
{
}


OmnRslt
OmnNetIf::config(const OmnXmlItemPtr &def)
{
	//
	// It assumes the following:
	//
	//	<NetInterface>
	//		<NIID>
	//		<IpAddr>
	//		<InterfaceName>
	//		<Description>
	//		<Bandwidth>
	//	</NetInterface>
	//

	mNiid = def->getInt(OmnXmlItemName::eNIID);
	// mEntityId = def->getInt(OmnXmlItemName::eEntityId, -1);
	mIpAddr = def->getIpAddr(OmnXmlItemName::eIpAddr);
	mDesc = def->getStr(OmnXmlItemName::eDescription, "NoDescription");
	mInterfaceName = def->getStr(OmnXmlItemName::eInterfaceName, "NoName");
	mBandwidth = def->getInt(OmnXmlItemName::eBandwidth, 100);
	
	OmnTrace << "Created Interface: " << mNiid 
		<< ":" << mIpAddr.toString() << endl;
	return true;
}


void
OmnNetIf::addCommGroup(const OmnCommGroupPtr &comm)
{
	mCommLock->lock();
	mComms.append(comm);
	mCommLock->unlock();
	return;
}


OmnCommGroupPtr
OmnNetIf::getCommGroup(const OmnCommProt::E prot)
{
	//
	// Each network interface has many communicators, one for a specific 
	// purpose (i.e., message category). All the comms are stored in mComms.
	// This function checks whether there is a comm that handles the 
	// message category 'msgCat'. If yes, it returns it.
	//
	mCommLock->lock();
	const uint s = mComms.entries();
	mComms.reset();
	for (uint i=0; i<s; i++, mComms.next())
	{
		if ((mComms.crtValue())->getProtocol() == prot)
		{
			//
			// Found it.
			//
			OmnCommGroupPtr comm = mComms.crtValue();
			mCommLock->unlock();

			return comm;
		}
	}

	//
	// Did not find it.
	//
	mCommLock->unlock();
	return 0;
}

