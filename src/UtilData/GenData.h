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
// Modification History:
// 2015/03/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilData_GenData_h
#define Aos_UtilData_GenData_h

class AosGenData
{
public:
	virtual getType() = 0;
	virtual serializeFrom() = 0;
	virtual serializeTo() = 0;
};
#endif

