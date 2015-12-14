////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	
// Modification History:
// 2014/11/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Replicator_Replicator_h
#define AOS_Replicator_Replicator_h


class AosReplicator : public OmnCommListener
{
	OmnDefineRCObject;
	
private:
	AosReplicPolicyPtr	mPolicy;

	u64						mReplicatorID;
	vector<OmnUdpCommPtr>	mCubeComms;
	vector<OmnUdpComm*>		mCubeCommsRaw;
	
public:
	AosReplicator();
	~AosReplicator();

	// OmnCommListener interface
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;
	virtual void 		readingFailed();
};
#endif
