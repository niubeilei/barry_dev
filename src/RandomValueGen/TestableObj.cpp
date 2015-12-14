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
// This class serves as the super class for all classes that want to
// use the torturer facilities. This class is very critical to 
// automate class testing. It implements many testing features. 
//
// 1. Memory Check
// This class may add some additional memory at the beginning of 
// the class. The size of this added memory is tunable: the larger, 
// the most chances to detect memory corruption. This memory holds
// specific values. Every time the class does correctness check, 
// it checks whether the memory holds the contents as expected. If
// not, it means someone has corrupted the memory. 
//
// 2. Object Management
// All instances of this class are maintained in a hash table. 
// Whenever an instance is created, it checks whether the memory 
// is still in the hash table. If yes, it means the object was not
// deleted but somehow, the same memory is re-used. This is an error.
// If the memory is not on the hash table, it adds to the hash table. 
// When an instance is deleted, it checks whether the object 
// is in the table. If not, it means one tries to delete an object
// that is no longer in the system. If yes, the object is removed
// from the table. 
//
// Modification History:
// 03/13/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Parms/TestableObj.h"


AosTestableObj::AosTestableObj()
{
	// 
	// 1. Initialize the attached memory
	// 2. Add the object into the hashtable
	//
}


AosTestableObj::~AosTestableObj()
{
}


// 
// Each class may define a set of invariants. An invariant is a checking
// on an instance of a class. No matter how an object is manipulated, 
// it must ensure the invariant to be true. Otherwise, it is an error.
// This is a virtual function. One should override this function. 
// By default, this function does nothing.
//
bool 
AosTestableObj::checkInvariants()
{
	// 
	// Check the attached memory
	// (TBD)
	//
	return true;
}


