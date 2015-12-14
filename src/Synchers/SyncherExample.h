////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/09/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_SyncherExample_h
#define AOS_Synchers_SyncherExample_h

#include "Synchers/Syncher.h"

class AosSyncherExample : public AosSyncher
{
private:
	i64		mCreateTimeSec;

public:
	AosSyncherExample(const int version);
	virtual ~AosSyncherExample();

	virtual AosSyncherType::E getType() { return AosSyncherType::eExample; }
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }
};

#endif
