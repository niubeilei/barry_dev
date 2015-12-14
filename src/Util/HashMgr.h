////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashMgr.h
// Description:
// 	This class is used to manage all hash tables. It's mainly
// 	designed to manage memory.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_HashMgr_h
#define Snt_Util_HashMgr_h

#include "Util/ValList.h"


class OmnHashTable;

class OmnHashMgr 
{
private:
	static OmnVList<OmnHashTable*>		mHashTables;

	OmnHashMgr();
	~OmnHashMgr();

public:
	static bool			addHashTable(OmnHashTable *table);
	static bool			removeHashTable(OmnHashTable *table);

	static int			getHashedObj();
	static int			getNumHashTables();
};

#endif

