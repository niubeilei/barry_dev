////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ManagedObj.h
// Description:
//	This class defines an interface for "Managed Object". A MO can be
//  accessed from outside, such as get the current values of MO, to 
//  call member functions from outside, etc.
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_ManagedObj_h
#define Omn_NMS_ManagedObj_h

#include "Util/Obj.h"

class OmnManagedObj OmnDeriveFromObj
{
public:
	//
	// Put its member data values into "data". "level" is used to indicate 
	// which member data shall be included, which is entirely up to the
	// class to interpret. level=0 means to include everything. 
	//
	virtual bool		serializeTo(OmnString &data, const int level) = 0;

	//
	// Assume an instance is already there. Use this function to instantiate
	// or re-instantiate the object using the data "data". 
	//
	virtual bool		serlalizeFrom(OmnString &data) = 0;

	//
	// Use this member function to invoke certain member functions. It is 
	// up to the class to determine which member functions can be invoked
	// remotely, and how to pass parameters to the member function invocation.
	//
	virtual bool		runMemberFunction(const OmnSoPtr &so) = 0;
};
#endif
