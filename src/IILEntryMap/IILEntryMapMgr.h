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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILEntryMap_IILEntryMapMgr_h
#define Aos_IILEntryMap_IILEntryMapMgr_h 

#include "IILEntryMap/IILEntryMap.h"
#include "Thread/ThrdShellProc.h"


class AosIILEntryMapMgr
{

	class queryThrd : public OmnThrdShellProc
	{
		friend class AosIILEntryMapMgr;

		OmnDefineRCObject;

		AosIILEntryMapPtr	mIILMap;

		AosRundataPtr		mRundata;
		
	public:
		queryThrd(
			const AosIILEntryMapPtr &iilmap,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("iilentry_queryThrd"),
		mIILMap(iilmap),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};

public:
	static AosIILEntryMapPtr	retrieveIILEntryMap(
									const OmnString &iilname,
									const AosRundataPtr &rdata);
	static AosIILEntryMapPtr	retrieveIILEntryMap(
									const OmnString &iilname,
									const bool need_swap,
									const AosRundataPtr &rdata);
	static AosIILEntryMapPtr	retrieveIILEntryMap(
									const OmnString &iilname,
									const bool need_split,
									const OmnString &sep,
									const bool need_swap,
									const bool use_key_as_value,
									const AosRundataPtr &rdata);

	static bool					getMapValues(
									const AosIILEntryMapPtr &mp,
									vector<OmnString> &keys,
									vector<OmnString> &values,
									const OmnString &dft_value,
									const bool need_create,
									const AosRundataPtr &rdata);

	static void					clear();
};

#endif
