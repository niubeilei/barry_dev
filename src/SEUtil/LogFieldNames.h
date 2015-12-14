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
// 2014/11/08 Created by Chen Dign
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SEUtil_LogFieldNames_h
#define AOS_SEUtil_LogFieldNames_h

#include "Util/String.h"

#define AOSLOGFIELDNAME_ERROR_TYPE				"err_type"
#define AOSLOGFIELDNAME_REMOTEENDPOINTID        "remoteendpointid"
#define AOSLOGFIELDNAME_ENDPOINTID              "endpointid"
#define AOSLOGFIELDNAME_CONFIG                  "config"
#define AOSLOGFIELDNAME_ERROR                   "error" 

struct AosFieldName
{
	enum E
	{
		eInvalid,

		eErrType,
		eRemoteEndpointID,	// 2014 11 25 xianghui
		eEndpointId,   // 2014 11 25 xianghui
        eConfig,
		eError,
		eSiteid,
		eFuncName,

		eMaxFieldName
	};
};
#endif

#endif
