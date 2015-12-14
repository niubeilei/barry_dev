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
// 2013/03/28 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Datalet_SName_h
#define Aos_Datalet_SName_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/HashUtil.h"



class AosSName : virtual public OmnRCObject
{
public:
	enum
	{
		eInvalidNameId = 0,
		eUserDefinedId = 100,
		eMaxFileLength = 1000 * 1000 * 1000
	};

private:
	typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutexPtr	mLock;
	u32			mCrtId;
	map_t 		mMap;
	OmnString	mObjid;
	bool		mIsLocal;

public:
	AosSName();
	AosSName(
		const AosRundataPtr &rdata, 
		const OmnString &objid, 
		const bool is_local);
	~AosSName();

	u32  addName(const OmnString &name);
	u32  getNameIdx(const OmnString &name);
	static bool isValidIdx(const u32 idx) {return idx != eInvalidNameId;}

private:
	bool loadNames(const AosRundataPtr &rdata);
	bool loadLocalNamesLocked(const AosRundataPtr &rdata);
	bool loadDbNamesLocked(const AosRundataPtr &rdata);
};
#endif
