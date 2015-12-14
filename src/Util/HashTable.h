////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashTable.h
// Description:
// 	This class is the super class for all hash tables.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_HashTable_h
#define Snt_Util_HashTable_h

#include "Util/RCObject.h"


class OmnHashTable : public OmnRCObject 
{
public:
	virtual int getHashedObj() const = 0;
};

#endif

