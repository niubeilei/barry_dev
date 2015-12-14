////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommUsrData.h
// Description:
//	This class defines an interface for OmnComm user data. OmnComm 
//  user data is used by OmnComm caller to store transient data.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_CommUsrData_h
#define Omn_UtilComm_CommUsrData_h

#include "Util/RCObject.h"


class OmnCommUsrData : public virtual OmnRCObject 
{

private:
	enum UserDataType
	{

	};


public:
	virtual UserDataType	getCommUserDataType() const = 0;
	virtual OmnString		getCommUserDataName() const = 0;
};
#endif
