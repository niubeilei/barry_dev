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
// This file is generated automatically by the ProgramAid facility.   
//
// Modification History:
// 8/18/2010: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UserMgmt_Vote_h
#define Aos_UserMgmt_Vote_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"

class AosVote : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:


public:
    AosVote();
    ~AosVote();

	bool deleteVotes(
			const AosXmlTagPtr &root
			);

	bool siggleVote(
		 const OmnString &siteid,
		 const OmnString &objid,
		 const OmnString &cid,
		 const u64 &num, 
		 const AosRundataPtr &rdata);

	bool dateVote(
		 const OmnString &siteid,
		 const OmnString &objid,
		 const OmnString &cid,
		 const u64 &num, 
		 const AosRundataPtr &rdata);

	bool timeVote(
		 const OmnString &siteid,
		 const OmnString &objid,
		 const OmnString &cid,
		 const u64 &num,
		 const u64 &mhour, 
		 const AosRundataPtr &rdata);

	u64 mergerValue(
			u32 mtime,
			u32 num);

	bool controlOnVote(
		 const OmnString &siteid,
		 const OmnString &objid,
		 const OmnString &cid,
		 const OmnString &mode,
		 const OmnString &num,
		 const OmnString &hour, 
		 const AosRundataPtr &rdata);

private:

private:

};
#endif

