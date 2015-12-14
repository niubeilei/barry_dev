////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjNet.h
// Description:
//	A Network Object is an object that can be sent to and received
//  from the network. 
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_NetObj_h
#define Omn_Obj_NetObj_h

#include "Obj/Obj.h"


class OmnNetObj : virtual public OmnObj
{
public:
	OmnNetObj();
	virtual ~OmnNetObj();

    virtual bool	sendTo(const OmnString &url) const = 0;
	virtual bool	sendTo(const OmnAddr &local, const OmnAddr &remote) const = 0;
    virtual bool    recvFrom(const OmnString &url) const = 0;
	virtual bool	recvFrom(const OmnAddr &local, const OmnAddr &remote) const = 0;
};
#endif
