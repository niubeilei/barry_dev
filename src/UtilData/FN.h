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
// 2015/03/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilData_FN_h
#define Aos_UtilData_FN_h

#include "Util/String.h"


class AosFN
{
public:
	enum E
	{
		eInvalidFieldName,

		ePackage,
		eFunc,
		eStatus,
		eJimoCallID,
		eStartTime,
		eLength,
		eLeader,
		eMessage,
		eEndpointID,
		eEncryptType,
		eDocid,
		eCubeID,
		eReplicPolicy,
		eReturnValue,
		eFlag,
		eObjName,
		eSiteid,
		eErrmsg,
		eValue,
		eValue1,
		eValue2,
		eValue3,
		eDoc,
		eErrorMsg,
		eFile,
		eLine,
		eBuff,
		eObjid,
		eName,
		eVersion,
		eSnapshot,
		eUserid,
		eContainer,
		eSnapID,
		eOprID,
		eBinaryDoc,
		eType,
		eCloudid,
		eKeyUnique,
		eValueUnique,
		eData,
		eSave,
		eReverse,
		eIDOName,
		eCreateAsNeeded,
		eClassname,
		eMethod,
		eOperation,
		eIILName,
		eIILID,
		eSize,
		eDft,
		eKey,
		eAddFlag,
		eSeqid,
		ePersisFalg,
		eOpr,
		eFound,
		eDelta,
		eStartid,
		eTag,
		eIncrementValue,
		eIDOFunc,
		eDocType,
		eKeepObjid,
		eParserVersion,
		eProcVersion,
		eDocGroupDocid,
		eJimoCallSignature,
		eDocGroupSP,
		eJimoName,
		eContainerDocid,
		eStoreEngineType,
		eHeaderCustomData,
		eAseID,
		eSyncherName,
		eRunSyncher,

		eMaxFieldName
	};

public:
	OmnString		fname;

public:
	AosFN()
	{
	}

	AosFN(const OmnString &f)
	:
	fname(f)
	{
	}

	void reset() {fname = "";}
	static OmnString toStr(const E code) ;
};
#endif

