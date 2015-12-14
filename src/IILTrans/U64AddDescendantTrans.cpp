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
#include "IILTrans/U64AddDescendantTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64AddDescendant::AosIILTransU64AddDescendant(const bool flag)
:
AosIILTrans(AosTransType::eU64AddDescendant, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransU64AddDescendant::AosIILTransU64AddDescendant(
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		vector<u64> &ancestors,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64AddDescendant, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
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


/*
bool
AosIILTransU64AddDescendant::initTrans(
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

	mAncestors.clear();
	AosXmlTagPtr entrys = trans_doc->getFirstChild("entrys");
	aos_assert_r(entrys, false);
	AosXmlTagPtr entry = entrys->getFirstChild();
	u64 did = 0;
	while(entry)
	{
		did = atoll(entry->getNodeText().data());
		mAncestors.push_back(did);
		entry = entrys->getNextChild();
	}
	return true;
}
*/

AosTransPtr 
AosIILTransU64AddDescendant::clone()
{
	return OmnNew AosIILTransU64AddDescendant(false);
}


bool 
AosIILTransU64AddDescendant::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	//buff->setOmnStr(mIILName);
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
AosIILTransU64AddDescendant::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	//mIILName = buff->getOmnStr("");
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
AosIILTransU64AddDescendant::proc(
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
		rslt = iilobj->addDocSafe(mAncestors[i], mDocid, false, false, rdata);
		if (!rslt)
		{
			OmnAlarm << enderr;
			break;
		}
	}
	
	return true;
}


/*
u64
AosIILTransU64AddDescendant::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/


AosIILType
AosIILTransU64AddDescendant::getIILType() const
{
	return eAosIILType_U64;
}


int
AosIILTransU64AddDescendant::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + 
		AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size() + AosBuff::getU64Size()
		+ AosBuff::getIntSize()
		+ AosBuff::getU64Size() * mAncestors.size();
}

