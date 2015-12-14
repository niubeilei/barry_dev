////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NmsSvrObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_NmsSvrObj_h
#define Omn_NMS_NmsSvrObj_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"

class OmnSmNmsCmd;
class OmnSmEpCmd;


class OmnNmsSvrObj : public OmnRCObject
{
public:
	virtual bool procNmsCommand(const OmnSmNmsCmd&, 
								const OmnTcpClientPtr &conn,
							    bool &status,
								OmnString &err) = 0;
	virtual bool procEpCommand(const OmnSmEpCmd&, 
								const OmnTcpClientPtr &conn,
								bool &status,
								OmnString &err) = 0;
};
#endif
