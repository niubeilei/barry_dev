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
// 08/14/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_DfmFileType_h
#define AOS_SEInterfaces_DfmFileType_h

class AosDfmFileType 
{
public:
	enum E 
	{
		eInvalid,

		eNormHeadFile,
		eDeltaHeadFile,
		
		eDiskFile,
		eSlabFile,
		eReadOnlyFile,
		
		eMax,
	};
	
	static bool isValid(const E tp)
	{
		return tp > eInvalid && tp < eMax;
	}

};
#endif
