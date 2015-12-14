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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/U64RemoveDescendantTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64RemoveDescendant::AosIILTransU64RemoveDescendant(const bool flag)
:
AosIILTrans(AosTransType::eU64RemoveDescendant, flag AosMemoryCheckerArgs),
mDocid(0)
{
}

AosIILTransU64RemoveDescendant::AosIILTransU64RemoveDescendant(
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		vector<u64> &ancestors,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64RemoveDescendant, iilname, 
		false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mDocid(docid),
mParentDocid(parent_docid)
{
	mAncestors.clear();
	int s = ancestors.size();
	for(int i=0; i<s; i++)
	{
		mAncestors.push_back(ancestors[i]);
	}
}


AosTransPtr 
AosIILTransU64RemoveDescendant::clone()
{
	return OmnNew AosIILTransU64RemoveDescendant(false);
}


bool 
AosIILTransU64RemoveDescendant::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);
	buff->setU64(mParentDocid);
	
	int s = mAncestors.size();
	buff->setInt(s);
	for(int i=0; i<s; i++)
	{
		buff->setU64(mAncestors[i]);
	}
	return true;
}


bool 
AosIILTransU64RemoveDescendant::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	mParentDocid = buff->getU64(0);
	
	int s = buff->getInt(0);
	u64 did = 0;
	for(int i=0; i<s; i++)
	{
		did = buff->getU64(0);
		aos_assert_r(did, false);

		mAncestors.push_back(did);
	}
	return true;
}

	
bool
AosIILTransU64RemoveDescendant::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);
	
	mAncestors.push_back(mParentDocid);
	bool rslt;
	size_t l = mAncestors.size();
	for (size_t i = 0; i < l; i++)
	{
		rslt = iilobj->removeDocSafe(mAncestors[i], mDocid, rdata);
		if (!rslt)
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
	}
	
	return true;
}


AosIILType
AosIILTransU64RemoveDescendant::getIILType() const
{
	return eAosIILType_U64;
}


int
AosIILTransU64RemoveDescendant::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + 
		AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size() + AosBuff::getU64Size()
		+ AosBuff::getU64Size() * mAncestors.size();
}

