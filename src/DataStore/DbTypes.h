////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbTypes.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataStore_DbTypes_h
#define Omn_DataStore_DbTypes_h

class OmnDbStatus
{
public:
	enum E
	{
		eUnknown,
		eActive,
		eDeleted,
		ePending,
		eCommitted,
		eLocked
	};
};
#endif
