////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocServer_DocSvrProcId_h
#define Aos_DocServer_DocSvrProcId_h

#include "Util/String.h" 


class AosDocSvrProcId
{
public:
	enum E 
	{
		eInvalid,
		
		eCreateDoc,
		eCreateDocSafe,
		eCreateExcDoc,
		eCheckDocLock,
		eDocLock,
		eDeleteObj,
		eGetDoc,
		eIsDocDeleted,
		eModifyObj,
		eModifyAttrStr,
		eSaveToFile,
		eBatchFixed,
		eBatchVariable,
		eBatchDelDocs,		
		eIncrementValue,
		eBatchGetDocs,
		eCreateBinaryDoc,
		eDeleteBinaryDoc,
		eRetrieveBinaryDoc,
		eBatchSendDocids,
		eBatchSendStatus,
	
		eBatchFixedStart,	// Ken Lee, 2013/01/15
		eBatchFixedFinish,	// Ken Lee, 2013/01/15 
		eBatchFixedClean,	// Ken Lee, 2013/01/17

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};
#endif

