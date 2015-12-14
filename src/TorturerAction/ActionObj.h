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
//
// Modification History:
// 11/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerAction_ActionObj_h
#define Omn_TorturerAction_ActionObj_h

#include "Util/RCObject.h"

class OmnString;


class AosActionObj : virtual public OmnRCObject
{
   
public:	
	virtual ~AosActionObj() {}

    virtual bool    setParm(const OmnString &name,
			 				const OmnString &value,
			  				OmnString &errmsg) = 0;
	virtual bool    setVar(const OmnString &name,
			  				const OmnString &value,
			   				OmnString &errmsg) = 0;
	virtual bool    setCrtValue(const OmnString &name,
			   				const OmnString &value,
			    			OmnString &errmsg) = 0;
};
#endif

