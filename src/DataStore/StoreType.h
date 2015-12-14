////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StoreType.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataStore_StoreType_h
#define Omn_DataStore_StoreType_h

#include "Util/RCObject.h"

class OmnStoreType
{
public:
	enum E
	{
		eFirstValidStoreType,

		eMySQL		= 0, 
		eSQLite		= 1,
		eFile,

		eLastValidStoreType	// IMPORTANT: eMaxStoreType is used by other 
							// classes in defining array sizes. It is very
							// important not to add anything after it. Also,
							// one should use continuous numbers for StoreType, 
							// i.e., do not use something like:
							//		eTheType = 5000
	};
};
#endif
