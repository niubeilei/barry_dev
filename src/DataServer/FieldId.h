////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FieldId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_FieldId_h
#define Omn_DataServer_FieldId_h



class OmnFieldId
{
public:
	enum E
	{
		//
		// For MGCP Endpoint Table
		//
		eSpnrEndpointId,
		eMgcpEndpointId,
		eManufacturerId,
		eCallAgentId,

};
#endif