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
#include "IILTrans/U64GetAncestorTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64GetAncestor::AosIILTransU64GetAncestor(const bool flag)
:
AosIILTrans(AosTransType::eU64GetAncestor, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransU64GetAncestor::AosIILTransU64GetAncestor(
		const OmnString &iilname,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64GetAncestor, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
//mIILName(iilname),
mDocid(docid)
{
}


//Ketty 2012/10/24
/*
bool
AosIILTransU64GetAncestor::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(trans_doc, rdata, false);
	mTransId = trans->getTransId();
	bool rslt = AosIILTrans::initTransBaseByName(trans_doc, mIILName, rdata);
	if(!rslt) return rslt;
	
	mDocid = trans_doc->getAttrU64("docid", 0);
	return true;
}
*/

AosTransPtr 
AosIILTransU64GetAncestor::clone()
{
	return OmnNew AosIILTransU64GetAncestor(false);
}


bool 
AosIILTransU64GetAncestor::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	return true;
}


bool 
AosIILTransU64GetAncestor::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	return true;
}

	
bool
AosIILTransU64GetAncestor::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	//1. add docid: parentdocid
	//2. find pparentdocid of the parentdocid from ancesster iil
	//3. add docid:pparentdocid
	//4. loop all parent.
	vector<u64> pids;
	pids.reserve(16);
	u64 pdid = mDocid;
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

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU32(pids.size());
	for (size_t i = 0; i < pids.size(); i++)
	{
		resp_buff->setU64(pids[i]);
	}

	//OmnString str;
	//str << "<rsp><entrys>";
	//for (size_t i = 0; i < pids.size(); i++)
	//{
	//	str <<"<entry>"<< pids[i] << "</entry>";
	//}
	//str << "</entrys></rsp>";

	//rdata->setContents(str);
	return true;
}


/*
u64 
AosIILTransU64GetAncestor::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/

AosIILType 
AosIILTransU64GetAncestor::getIILType() const
{
	return eAosIILType_U64;
}

	
int
AosIILTransU64GetAncestor::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

