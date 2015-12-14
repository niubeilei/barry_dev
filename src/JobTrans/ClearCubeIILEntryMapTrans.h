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
// 2015/12/04	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_ClearCubeIILEntryMapTrans_h
#define Aos_JobTrans_ClearCubeIILEntryMapTrans_h

#include "TransUtil/DocTrans.h"


class AosClearCubeIILEntryMapTrans : virtual public AosDocTrans
{
private:
public:
	AosClearCubeIILEntryMapTrans(const bool regflag);
	AosClearCubeIILEntryMapTrans(const u32 cube_id);
	~AosClearCubeIILEntryMapTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif

