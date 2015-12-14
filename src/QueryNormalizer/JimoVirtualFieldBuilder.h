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
#ifndef Aos_QueryNormalizer_JimoVirtualFieldBuilder_h
#define Aos_QueryNormalizer_JimoVirtualFieldBuilder_h



class AosJimoVirtualFieldBuilder : public OmnRCObject
{
private:
	AosQueryTablePtr		mTable;
	vector<OmnString>		mKeyFields;
	vector<AosQueryCondPtr>	mConds;
	
public:
	AosJimoVirtualFieldBuilder();
	~AosJimoVirtualFieldBuilder();

};
#endif

