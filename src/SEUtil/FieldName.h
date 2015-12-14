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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_FieldName_h
#define AOS_SEUtil_FieldName_h

#include "Util/String.h"


struct AosFieldName
{
	enum E
	{
		eInvalidFieldName,

		eErrorType,
		eValue,
		eValue1,
		eValue2,
		eValue3,
		eDocid,
		eSnapshot,
		eUserid,
		eForturerName,					// Levi, 2014/11/13
		eSiteid,						// Chen Ding, 2014/11/24
		eFuncName,						// Chen Ding, 2014/11/24
		eConfig,				
		eFieldName,				
		eMaxLength,
		eExpectedLength,				//White, 2014-12-23
		eActualLength,				
		eErrorMsg,				
		eRemoteEndpointID,				// Chen Ding, 2014/12/06
		eEndpointID,					// Chen Ding, 2014/12/06
		eError,							// Chen Ding, 2014/12/06
		eMessage,						// Chen Ding, 2014/12/06
		eContainer,						// Chen Ding, 2014/12/06
		eDataletFormat,					// Chen Ding, 2014/12/06
		eData,							// Chen Ding, 2014/12/06
		eReturnValue,					//White, 2014-12-24
		//BlobSE
		eMaxSize,
		eHeaderStatus,
		eRawFileId,
		ePackage,						// Chen Ding, 2014/12/13
		eFunc,							// Chen Ding, 2014/12/13
		eDialer,						// Chen Ding, 2014/12/13
		eFromEPID,						// Chen Ding, 2014/12/13
		eToEPID,						// Chen Ding, 2014/12/13
		eJimoCallID,					// Chen Ding, 2014/12/13
		eStartTime,						// Chen Ding, 2014/12/13
		eStatus,						// Chen Ding, 2014/12/13
		eLength,						// Chen Ding, 2014/12/13
        eResults,
	    eLoginStatus,
		eObjid,
		eDoc,
		eIILName,
		eIILID,
		eKey,
		eKeyUnique,
		eValueUnique,
		ePersisFalg,
		eAddFlag,
		eOpr,
		eReverse,
		eDft,
		eFound,
		eCloudid,
		eSeqid,
		eStartid,
		eFlag,
		eDelta,
		eType,
		eTag,
		eBuff,
		eSize,

		eMaxFieldName
	};

	static OmnString toStr(const E code);
};
#endif
