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
#include "IILTrans/U64AddAncestorTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64AddAncestor::AosIILTransU64AddAncestor(const bool flag)
:
AosIILTrans(AosTransType::eU64AddAncestor, flag AosMemoryCheckerArgs),
mDocid(0),
mParentDocid(0)
{
}


AosIILTransU64AddAncestor::AosIILTransU64AddAncestor(
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64AddAncestor, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
//mIILName(iilname),
mDocid(docid),
mParentDocid(parent_docid)
{
}


//Ketty 2012/10/24
/*
bool
AosIILTransU64AddAncestor::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(trans_doc, rdata, false);
	mTransId = trans->getTransId();
	bool rslt = AosIILTrans::initTransBaseByName(trans_doc, mIILName, rdata);
	if(!rslt) return rslt;
	
	mDocid = trans_doc->getAttrU64("docid", 0);
	mParentDocid = trans_doc->getAttrU64("parent_docid", 0);
	return true;
}
*/

AosTransPtr 
AosIILTransU64AddAncestor::clone()
{
	return OmnNew AosIILTransU64AddAncestor(false);
}


bool 
AosIILTransU64AddAncestor::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	//buff->setOmnStr(mIILName);
	buff->setU64(mDocid);
	buff->setU64(mParentDocid);
	return true;
}


bool 
AosIILTransU64AddAncestor::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	//mIILName = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	mParentDocid = buff->getU64(0);
	return true;
}

	
bool
AosIILTransU64AddAncestor::proc(
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
			rslt = iilobj->addDocSafe(mDocid, pids[i], false, false, rdata);
			if (!rslt)
			{
				OmnAlarm << enderr;
				break;
			}
		}
	}
	return true;
}


/*
u64
AosIILTransU64AddAncestor::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/

AosIILType
AosIILTransU64AddAncestor::getIILType() const
{
	return eAosIILType_U64;
}
	

int
AosIILTransU64AddAncestor::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + 
		AosBuff::getStrSize(mIILName.length()) +
		AosBuff::getU64Size() +
		AosBuff::getU64Size();
}

