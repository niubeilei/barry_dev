
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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/IILTrans.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TransProcThrd.h"
#include "Util/Buff.h"
#include "WordMgr/WordIdHash.h"

AosTransProcThrdPtr 	AosIILTrans::smIILProcThrd = OmnNew AosTransProcThrd("iilTrans", 32);


AosIILTrans::AosIILTrans(
		const AosTransType::E opr, 
		const bool reg_flag
		AosMemoryCheckDecl)
:
AosCubicTrans(opr, reg_flag),
mFlags(0)
{
	//AosMemoryCheckerObjCreated(AosClassName::eAosIILTrans);
}


AosIILTrans::AosIILTrans(
		const AosTransType::E opr,
		const u64 iilid,
		const bool isPersis,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp
		AosMemoryCheckDecl)
:
AosCubicTrans(opr, iilid, need_save, need_resp),
mIILID(iilid),
mIILName(""),
mFlags(0),
mSnapshotId(snap_id)

{
	//AosMemoryCheckerObjCreated(AosClassName::eAosIILTrans);
	if(isPersis) setPersis();
}


AosIILTrans::AosIILTrans(
		const AosTransType::E opr,
		const OmnString &iilname,
		const bool isPersis,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp
		AosMemoryCheckDecl)
:
AosCubicTrans(opr, AosGetHashKey(iilname), need_save, need_resp),
mIILID(0),
mIILName(iilname),
mFlags(0),
mSnapshotId(snap_id)
{
	//AosMemoryCheckerObjCreated(AosClassName::eAosIILTrans);
	if(isPersis) setPersis();
}

AosIILTrans::AosIILTrans(
		const AosTransType::E opr,
		const u32 vid,
		const bool is_cube,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp
		AosMemoryCheckDecl)
:
AosCubicTrans(opr, vid, is_cube, need_save, need_resp),
mSnapshotId(snap_id)
{
}


AosIILTrans::~AosIILTrans()
{
	//AosMemoryCheckerObjDeleted(AosClassName::eAosIILTrans);
}


bool
AosIILTrans::serializeTo(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeTo(buff);
	
	buff->setU64(mIILID);
	buff->setOmnStr(mIILName);
	buff->setChar(mFlags);
	buff->setU64(mSnapshotId);
	return true;
}


bool
AosIILTrans::serializeFrom(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeFrom(buff);

	mIILID = buff->getU64(0);
	mIILName = buff->getOmnStr("");
	mFlags = buff->getChar(0);
	mSnapshotId = buff->getU64(0);
	return true;
}


u64
AosIILTrans::getIILID(const AosRundataPtr &rdata)
{
	if(mIILID)	return mIILID;	
	
	aos_assert_r(mIILName != "", 0);

	AosIILMgrObjPtr iil_mgr = AosIILMgrObj::getIILMgr();

	aos_assert_r(iil_mgr, 0);
		
	mIILID  = iil_mgr->getIILID(mIILName, needCreateIIL(), rdata);
	if(needCreateIIL() && !mIILID) 
	{
		OmnAlarm << "failed to get iilid, name:" << mIILName << enderr;
		return 0;
	}
	
	return mIILID;	
}


int
AosIILTrans::getSerializeSize() const
{
	return AosBuff::getCharSize() + AosBuff::getU64Size() +
			AosBuff::getU64Size() + AosBuff::getCharSize();
}

	
bool 
AosIILTrans::proc()
{
	AosIILTransPtr thisptr(this, false);
	u64 iilid = getIILID(mRdata);
	if (iilid)
	{
		AosIILMgrObjPtr iil_mgr = AosIILMgrObj::getIILMgr();
		aos_assert_r(iil_mgr, false);
		bool rslt = iil_mgr->addTrans(
			iilid, mRdata->getSiteid(), thisptr, mRdata);
		if (rslt) return true;
	}
	
	sendErrResp();	
	return true;
}


bool 
AosIILTrans::procTrans(const AosIILObjPtr &iil, const AosRundataPtr &rdata)
{
	AosBuffPtr resp_buff;

	u32 siteid_1 = rdata->getSiteid();
	u32 siteid_2 = mRdata->getSiteid();
	if (siteid_1 != siteid_2)
	{
		OmnAlarm << "siteid is not the same" << enderr;
	}

	bool rslt = proc(iil, resp_buff, rdata);
	if (isNeedResp())
	{
		if (resp_buff && rslt)
		{
			// Ketty 2013/07/20
			//if (isAsyncReq())
			//{
			//	AosAsyncReqTransPtr trans = getAsyncReqTransPtr();
			//	if (trans)
			//	{
			//		trans->sendResp(resp_buff);
			//	}
			//	else
			//	{
			//		OmnAlarm << "error!, missing AsyncReqTransPtr" << enderr;
			//	}
			//}
			//else
			//{
			//	AosTransPtr thisptr(this, false);
			//	AosSendResp(thisptr, resp_buff);
			//}
			sendResp(resp_buff);
			return true;
		}

		if (!resp_buff)
		{
			OmnAlarm << "error! miss resp!." << " opr:" << getType() << enderr; 
		}

		sendErrResp();
	}
	
	return true;
}


bool
AosIILTrans::sendErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(false);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}


bool
AosIILTrans::directProc()
{
	u32 idx = mIILID % smIILProcThrd->getNumThrds();
	AosTransPtr thisptr(this, false);
	return smIILProcThrd->addRequest(idx, thisptr);	
}


// Gavin, 2015/08/24
//STORAGE-31
bool
AosIILTrans::sendResp(const AosBuffPtr &buff)
{
	if (!mJimoCall) 
	{
		return AosTrans::sendResp(buff);
	}

	mJimoCall->arg(AosFN::eBuff, buff);
	mJimoCall->setSuccess();
	mJimoCall->sendResp(mRdata.getPtr());
	return true;
}

