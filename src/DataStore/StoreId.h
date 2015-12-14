////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StoreId.h
// Description:
//	Defines Abstract Store IDs, which contains Store Domain Name, 
//  Store Instance ID, and Store Type. A Store Domain Name is 
//  logical name, similar to domain name. Each instance must have
//  a unique domain name. Store domain name is similar to domain 
//  name, consisting of dotted words, such as:
//		a-to-m.subscribers.msql
//		n-to-z.subscribers.msql   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataStore_StoreId_h
#define Omn_DataStore_StoreId_h



class OmnStoreId
{
public:
	enum E
	{
		eFirstStoreId,

		eAlarmStore,
		eGeneral,
		eTest,

		eLastStoreId
	};
};
#endif
