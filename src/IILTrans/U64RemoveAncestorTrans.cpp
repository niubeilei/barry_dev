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
#include "IILTrans/U64RemoveAncestorTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64RemoveAncestor::AosIILTransU64RemoveAncestor(const bool flag)
:
AosIILTrans(AosTransType::eU64RemoveAncestor, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransU64RemoveAncestor::AosIILTransU64RemoveAncestor(
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64RemoveAncestor,
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mDocid(docid),
mParentDocid(parent_docid)
{
}


AosTransPtr 
AosIILTransU64RemoveAncestor::clone()
{
	return OmnNew AosIILTransU64RemoveAncestor(false);
}


bool 
AosIILTransU64RemoveAncestor::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);
	buff->setU64(mParentDocid);
	return true;
}


bool 
AosIILTransU64RemoveAncestor::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	mParentDocid = buff->getU64(0);
	return true;
}

	
bool
AosIILTransU64RemoveAncestor::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	//1. add docid : parentdocid
	//2. find pparentdocid of the parentdocid from ancesster iil
	//3. add docid : pparentdocid
	//4. loop all parent.
	//vector<u64> pids(16);
	vector<u64> pids;
	pids.reserve(16);
	pids.push_back(mParentDocid);
	u64 pdid = mParentDocid;
	int64_t idx = -10;
	int64_t iilidx = -10;
	bool rslt = false;
	bool unique;
	while(1)
	{
		u64 did = 0;
		rslt = iilobj->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, pdid, did, unique, rdata);
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
		if (idx == -5  || iilidx == -5 || !did)
		{
			break;
		}
		pids.push_back(did);
	}

	if (rslt)
	{
		size_t l = pids.size();
		for (size_t i = 0; i < l; i++)
		{
			rslt = iilobj->removeDocSafe(mDocid, pids[i], rdata);
			if (!rslt)
			{
				OmnAlarm << enderr;
				break;
			}
		}
	}
	return true;
}


AosIILType
AosIILTransU64RemoveAncestor::getIILType() const
{
	return eAosIILType_U64;
}


int
AosIILTransU64RemoveAncestor::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + 
		AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size() + AosBuff::getU64Size();
}

