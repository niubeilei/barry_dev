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
// 2015/02/11: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Sequence/SequenceMgr.h"

#include "API/AosApiG.h"
#include "SEUtil/SeTypes.h"


AosSequenceMgr::AosSequenceMgr()
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr())
{
}


AosSequenceMgr::~AosSequenceMgr()
{
}


AosSequencePtr
AosSequenceMgr::getSequence(
		const AosRundataPtr &rdata,
		const OmnString &name) 
{
	aos_assert_r(name != "", 0);

	AosSequencePtr sequence;
	mLockRaw->lock();
	map<OmnString, AosSequencePtr>::iterator itr = mSequences.find(name);
	if (itr == mSequences.end())
	{
		//get sequence name
		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eSequenceDoc, name);
		AosXmlTagPtr xml = AosGetDocByObjid(objid, rdata);
		if (!xml) 
		{
			mLockRaw->unlock();
			return 0;
		}
		
		sequence = AosSequence::create(rdata, xml);
		aos_assert_r(sequence, 0);

		mSequences[name] = sequence;
	}
	else
	{
		sequence = itr->second;	
	}
	mLockRaw->unlock();

	return sequence;
}


bool
AosSequenceMgr::getNextValue(
		const AosRundataPtr &rdata,
		const OmnString &name, 
		int &value)
{
	AosSequencePtr sequence = getSequence(rdata, name);
	aos_assert_r(sequence, false);

	bool rslt = sequence->getNextValue(rdata, value);
	aos_assert_r(rslt, false);
	
	return true;
}

	
bool
AosSequenceMgr::getCrtValue(
		const AosRundataPtr &rdata,
		const OmnString &name, 
		int &value)
{
	AosSequencePtr sequence = getSequence(rdata, name);
	aos_assert_r(sequence, false);

	bool rslt = sequence->getCrtValue(rdata, value);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosSequenceMgr::checkSequenceExist(
		const AosRundataPtr &rdata,
		const OmnString &name,
		const OmnString &func) 
{
	AosSequencePtr sequence = getSequence(rdata, name);
	if (!sequence) return false;

	OmnString new_func = func;
	new_func.toLower();
	if (new_func == "currval" || new_func == "nextval") return true;

	return false;
}

