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
// 03/17/2015 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_StreamSyncer_h
#define AOS_StreamEngine_StreamSyncer_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/Ptrs.h"
#include <map>
#include <vector>

using namespace std;

//
//This class helps to mutex control streaming data 
//flow through different services
//
class AosStreamSyncer
{
private:
	OmnMutex				*mLock;

	//how many relevant stream data controlled by the syncer
	vector<OmnString>		mStreamList;

	//current stream data hold the syncer hold
	OmnString				mStream; 

	//all the RDDs running in the service holding the hold
	vector<u64>				mRDDList;			

	//syncer name
	OmnString				mName;

public:
	AosStreamSyncer();
	~AosStreamSyncer();

	static bool holdByStream(
			OmnString stream,
			u64 rddId);

	static bool releaseByStream(
			OmnString stream,
			u64 rddId);

	bool hold(
			OmnString stream,
			u64 rddId);

	bool release(
			OmnString stream,
			u64	rddId);

	bool canHold(OmnString stream);

	bool hasStream(OmnString stream);

	bool setName(OmnString);

	bool addToSyncerList(OmnString syncer_name);

	bool addToStreamList(OmnString name);

	bool dropSyncer(OmnString name);

	OmnString getName();
	bool dump() {return false;}
};

#endif
