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
// 3/30/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmUtil_AmRequestTypeId_h
#define Aos_AmUtil_AmRequestTypeId_h

class AosAmRequestSvrTypeId
{
public:
	enum E
	{
		eSvrTypeLocal = 1,
		eSvrTypeLdap,
		eSvrTypeRadius,
		eSvrTypeAd,
		eSvrTypeExternal,
		eSvrTypeReserved,
		eMaxSvrTypeId				//  =< 256
	};
	static bool isValid(const int type_id)
	{
		return (type_id >= AosAmRequestSvrTypeId::eSvrTypeLocal 
			 && type_id < AosAmRequestSvrTypeId::eMaxSvrTypeId);
	};
};


#endif // Aos_AmUtil_AmRequestTypeId_h

