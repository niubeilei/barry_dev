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
// 2013/07/16 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DfmSnapshotType_h
#define Aos_SEInterfaces_DfmSnapshotType_h


class AosDfmSnapshotType 
{

public:
	enum E 
	{
		eInvalid,

		eBigData,
		
		eMax,
	};

	static bool isValid(const E type)
	{
		return type > eInvalid && type < eMax;
	}
};
#endif
