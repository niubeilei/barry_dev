////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IpBubble.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Network/IpBubble.h"

#include "Debug/Debug.h"
#include "Debug/ErrId.h"
#include "Thread/Mutex.h"



OmnIpBubble::OmnIpBubble(const OmnXmlItemPtr &def) 
:
OmnNetEntity(OmnNetEtyType::eIpBubble)
{
	OmnRslt rslt = config(def);
	if (!rslt)
	{
		//
		// Failed to create the object
		//
		OmnExcept e(OmnFileLine, rslt.getErrId(), rslt.toString());
		throw e;
	}
}


OmnIpBubble::~OmnIpBubble()
{
}


OmnRslt
OmnIpBubble::config(const OmnXmlItemPtr &def)
{
    //
    // This function creates a new net entity based on the definition
    // 'def'. This function assumes the following:
    //
    //      <EntityDef>
    //          <EntityId>
    //          <EntityType>
    //          <LocationId>
    //          <LocationDesc>
    //          <EntityDesc>
    //      </EntityDef>
    //

	//
	// Call the parent's config first
	//
	OmnRslt rslt = OmnNetEntity::config(def);
	
	//
	// Currently, that's all we do
	// 
	return rslt;
}


/*
bool
OmnIpBubble::addToTable(const OmnString &termEID, const OmnMgcpEndpointPtr &endpoint)
{
	mTableLock->lock();
	bool res = mSpnrEIDTable.add(termEID, termEID, endpoint);
	mTableLock->unlock();
	if ( !res )
	{
		//
		// error
		//
		int errId = OmnError::log( __FILE__, 
								   __LINE__, mNetId, 
								   OmnErrId::eMgcpSpnrEIDTableAddingError,
		                          "Can not add SpnrEIDTable for SPNRID = " + termEID);
		return false;
	}
	return true;
}




OmnString
OmnIpBubble::removeFromTable(const OmnString &termEID)
{
	bool				res;
	OmnMgcpEndpointPtr	endpoint;

	mTableLock->lock();
	res = mSpnrEIDTable.get(termEID, termEID, endpoint, true);
	mTableLock->unlock();
	if ( !res )
	{
		//
		// error
		//
		int errId = OmnError::log( __FILE__, 
			                       __LINE__, 0, 
								   OmnErrId::eMgcpSpnrEIDNotFound,
			                      "SpnrEID not found when removing for termEP" + termEID);
	}
	return endpoint->getSpnrEID();
}





OmnString
OmnIpBubble::lookupTable(const OmnString &termEID)
{
	bool				res;
	OmnMgcpEndpointPtr	endpoint;

	mTableLock->lock();
	res = mSpnrEIDTable.get(termEID, termEID, endpoint, false);
	mTableLock->unlock();

	if ( !res )
	{
		//
		// error
		//
		int errId = OmnError::log( __FILE__, 
			                       __LINE__, 0, 
								   OmnErrId::eMgcpSpnrEIDNotFound,
			                      "SpnrEID not found when retrieving for termEP" + termEID);
	}
	return endpoint->getSpnrEID();
}


bool
OmnIpBubble::loadLocalSpnrEIDTables()
{
	// 
	// read in endpoints through a database serve, add
	// each endpoint into mEndpointTable by taking spnrEID
	// as the key.
	//
	// At the same time, add each terminating endpoint into
	// mSpnrEIDTable with key = (connected NIID, terminating
	// endpoint Id). A terminating endpoint is the one which 
	// can be directly reached (not through another Spnr proxy)
	// by this Spnr proxy. In database, its isTerm column is
	// marked to be true.
	//
	OmnValList<OmnSoMgcpEndpointPtr>	list;
	OmnSoMgcpEndpointPtr				soEndpoint;
	OmnMgcpEndpointPtr					endpoint;
	
	if ( !OmnDataMgrSelf->getLocalActiveEndpoints(mNetId, list) )
	{
	}
	int mNumOfEntries = list.entries();
	for (int i=0; i<mNumOfEntries; i++)
	{
		soEndpoint = list[i];
		OmnString termEID = soEndpoint->getTermEID();
		endpoint = OmnNew OmnMgcpEndpoint(soEndpoint);
		addToTable(termEID, endpoint);
	}
	return true;
}
*/

