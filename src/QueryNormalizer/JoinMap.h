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
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryNormalizer_JoinMap_h
#define Aos_QueryNormalizer_JoinMap_h



class AosJoinMap : public OmnRCObject
{
private:
	vector<OmnString>		mKeys;
	AosJoinType::E			mJoinType;
	vector<AosQueryCondPtr>	mConds;
	
public:
	AosJoinMap();
	~AosJoinMap();
};
#endif

