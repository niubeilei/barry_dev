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
// 10/30/2012 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_UtilData_DfmDocType_h
#define AOS_UtilData_DfmDocType_h

class AosDfmDocType
{
public:
	enum E 
	{
		eInvalid,

		eNormal,
		eIIL,
		eGroupedDoc,
		eBitmap,
		eIILCache,
		eDatalet,		// Chen Ding, 2013/06/12
		
		eMax,
	};

	static bool isValid(const E type)
	{
		return type > eInvalid && type < eMax;
	}
};

#endif

