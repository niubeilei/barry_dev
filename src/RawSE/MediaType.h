////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014-11-26 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSe_MediaType_H_
#define Aos_RawSe_MediaType_H_

class AosMediaType
{
public:
	enum Type
	{
		eInvalid	= 0,

		eMemory		= 1,
		eNVRAM		= 2,
		eSSD		= 3,
		eDisk		= 4
	};
public:
    AosMediaType();
    virtual ~AosMediaType();
};


#endif /* Aos_RawSe_MediaType_H_ */
