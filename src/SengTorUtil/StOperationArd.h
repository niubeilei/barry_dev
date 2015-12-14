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
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AosSengTorUtil_StOperationArd_h
#define AosSengTorUtil_StOperationArd_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"


class AosStOperationArd : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosStAccessPtr, Omn_Str_hash, compare_str> OprMap_t;
	typedef hash_map<const OmnString, AosStAccessPtr, Omn_Str_hash, compare_str>::iterator OprMapItr_t;

	OprMap_t		mOperations;
	bool			mIsDomainOprArd;
	bool			mAllowOverride;

public:
	AosStOperationArd() {}
	~AosStOperationArd() {}

	bool checkAccess(
					const AosStDocPtr &local_doc,
					const OmnString &opr_id, 
					bool &granted, 
					bool &denied,
					const AosSengTestThrdPtr &thread);
};
#endif

#endif
