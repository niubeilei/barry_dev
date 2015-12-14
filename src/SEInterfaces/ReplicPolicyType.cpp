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
// 09/21/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ReplicPolicyType.h"

#include "Thread/Mutex.h"
#include <map>
using namespace std;

//static AosStr2U32_t         sgMap;   
//map<OmnString, u32>	 sgMap;   

// sgReplicPolicTpMap is defined in AllReplicPolicy.cpp. to make sure sgReplicPolicTpMap
// is inited before gAosAllReplicPolicy. because gAosAllReplicPolicy construct will register
// sgAosReplicOnePlusOne, who will use sgReplicPolicTpMap.
extern map<OmnString, u32>  sgReplicPolicTpMap;
static OmnMutex             sgLock;


AosReplicPolicyType::E
AosReplicPolicyType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;

	sgLock.lock();
	//AosStr2U32Itr_t itr = sgMap.find(id);
	map<OmnString, u32>::iterator itr = sgReplicPolicTpMap.find(id);
	sgLock.unlock();
	if (itr == sgReplicPolicTpMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosReplicPolicyType::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	//AosStr2U32Itr_t itr = sgReplicPolicTpMap.find(name);
	map<OmnString, u32>::iterator itr = sgReplicPolicTpMap.find(name);
	if (itr != sgReplicPolicTpMap.end())
	{
		OmnAlarm << "this name has added." << enderr;
		return false;
	}

	sgReplicPolicTpMap.insert(make_pair(name, eid));
	//sgMap[name] = eid;
	return true;
}


