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
// 2015/4/08 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "DataletProcs/ModifyDataletProc.h"

#include "SEInterfaces/DocSvrObj.h"
#include "JimoAPI/JimoDocFunc.h"
#include "TransUtil/TaskTransChecker.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosModifyDataletProc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosModifyDataletProc(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosModifyDataletProc::AosModifyDataletProc(const int version)
:
AosDataletProc(version)
{
}


AosJimoPtr
AosModifyDataletProc::cloneJimo() const
{
	return OmnNew AosModifyDataletProc(mJimoVersion);
}


bool 
AosModifyDataletProc::isValid() const
{
	return true;
}


bool 
AosModifyDataletProc::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case JimoDocFunc::eModifyDatalet:
		return modifyDatalet(rdata, jimo_call);

	case JimoDocFunc::eWriteUnLockDatalet:
		return writeUnLockDatalet(rdata, jimo_call);

	case JimoDocFunc::eWriteLockDatalet:
		return writeLockDatalet(rdata, jimo_call);

	case JimoDocFunc::eMergeSnapshot:
		return mergeSnapshot(rdata, jimo_call);

	case JimoDocFunc::eCommitSnapshot:
		return commitSnapshot(rdata, jimo_call);

	case JimoDocFunc::eRollbackSnapshotOnCube:
		return rollbackSnapshot(rdata, jimo_call);

	case JimoDocFunc::eCreatebackSnapshot:
		return createSnapshot(rdata, jimo_call);

	default:
		 break;
	}

	return false;

}


bool
AosModifyDataletProc::modifyDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function is the server side implementations of AosDocSvr::modifyObj(...)
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);

	OmnString docstr = jimo_call.getOmnStr(rdata, AosFN::eDoc, "");
	AosXmlTagPtr newdoc = AosXmlParser::parse(docstr);
	jimo_call.reset();

	if(newdoc.getPtr() == 0)
	{
		jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	u32 siteid = newdoc->getAttrU32(AOSTAG_SITEID, 0);

	if (siteid != rdata->getSiteid())
	{
		OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
		rdata->setSiteid(siteid);
	}

	AosTransId transid;

	bool rslt = false;
	if (newdoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		OmnString nodename = newdoc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
		AosBuffPtr buff = newdoc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
		//aos_assert_r(buff && buff->dataLen() >0, false);
		if (buff && buff->dataLen() > 0)
		{
			if (nodename == "")
			{
				newdoc->removeNodeTexts();
			}
			else
			{
				newdoc->removeNode(nodename, false, false);
			}
			OmnString signature;
			rslt = AosDocSvrObj::getDocSvr()->saveBinaryDoc(newdoc, buff, signature, rdata, transid, snap_id);
			if( !rslt || signature == "" )
			{
				jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
				jimo_call.setLogicalFail();
				jimo_call.sendResp(rdata);
				return true;
			}
		}
	}

	rslt = AosDocSvrObj::getDocSvr()->modifyObj(rdata, newdoc, docid, transid, snap_id);
	if( !rslt )
	{
		jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosModifyDataletProc::writeLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
#if 0	
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 userid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	int waittimer = jimo_call.getInt(rdata, AosFN::eValue1, 0);
	int holdtimer = jimo_call.getInt(rdata, AosFN::eValue2, 0);
	jimo_call.reset();

	
	AosTransId transid;

	int status;
	bool rslt = AosDocSvrObj::getDocSvr()->writeLockDoc(docid, userid, waittimer, holdtimer, transid, status, rdata);	

	AosBuffPtr resp;
	if(!rslt)
	{
		// 1. There are errors in reading the doc. 
		// 2. It failed locking the doc.
		switch (status)
		{
		case 0:
			 // Need to return an error;
			 resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			 resp->setU8(false);
			 resp->setOmnStr(rdata->getErrmsg());
		OmnScreen << "@@@@@@ WriteLockDocTrans error : " << userid << " errmsg : " << rdata->getErrmsg() << endl;
			 break;

		case 1:
			 // It means it failed obtaining the lock. It needs
			 // to wait until the lock becomes available.
			 // When the lock becomes available, it will call:
		OmnScreen << "@@@@@@ WriteLockDocTrans wait : " << userid << endl;
			 return true;

		default:
			 resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			 resp->setU8(false);
			 OmnString errmsg = __FILE__;
			 errmsg << ":" << __LINE__ 
				 << " Internal Error: " << status;
			 resp->setOmnStr(errmsg);
		OmnScreen << "@@@@@@ WriteLockDocTrans invalid : " << userid << " errmsg : " << errmsg << endl;
			 break;
		}
	}
	else
	{
		resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp->setU8(true);
		OmnScreen << "@@@@@@ WriteLockDocTrans success : " << userid << endl;
	}
	
	jimo_call.arg(AosFN::eBuff, resp);
#endif
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}

bool
AosModifyDataletProc::writeUnLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 userid =  jimo_call.getU64(rdata, AosFN::eUserid, 0);

	OmnString docstr = jimo_call.getOmnStr(rdata, AosFN::eDoc, "");
	AosXmlTagPtr newdoc = AosXmlParser::parse(docstr);
	jimo_call.reset();

	AosTransId transid;

	bool rslt = AosDocSvrObj::getDocSvr()->writeUnLockDoc(docid, userid, newdoc, transid, rdata);

	AosBuffPtr resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	resp->setOmnStr(rdata->getErrmsg());
	
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}
bool 
AosModifyDataletProc::mergeSnapshot(AosRundata *rdata, AosJimoCall &jimo_call)
{
	int virtual_id = jimo_call.getInt(rdata, AosFN::eValue, 0);
	int doctype = jimo_call.getInt(rdata, AosFN::eType, 0);
	u64 merge_snap_id = jimo_call.getU64(rdata, AosFN::eValue2, 0);
	u64 target_snap_id = jimo_call.getU64(rdata, AosFN::eValue1, 0);
	jimo_call.reset();

	AosTransId transid;

	OmnScreen << "jozhi doc_type: " << (AosDocType::E)doctype << endl;
	bool rslt = AosDocSvrObj::getDocSvr()->mergeSnapshot(virtual_id, 
			(AosDocType::E)doctype, merge_snap_id, target_snap_id, transid, rdata);

	AosBuffPtr resp = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool 
AosModifyDataletProc::commitSnapshot(AosRundata *rdata, AosJimoCall &jimo_call)
{
	int virtual_id = jimo_call.getInt(rdata, AosFN::eValue, 0);
	int doctype = jimo_call.getInt(rdata, AosFN::eType, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	u64 taskdocid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	jimo_call.reset();

	AosTransId transid;

	AosDocSvrObj::getDocSvr()->commitSnapshot(
			virtual_id, (AosDocType::E)doctype, snap_id, transid, rdata);

	AosTaskTransChecker::getSelf()->removeEntry(taskdocid, virtual_id, snap_id); 

	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosModifyDataletProc::rollbackSnapshot(AosRundata *rdata, AosJimoCall &jimo_call)
{
	int virtual_id = jimo_call.getInt(rdata, AosFN::eValue, 0);
	int doctype = jimo_call.getInt(rdata, AosFN::eType, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	u64 taskdocid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	jimo_call.reset();

	AosTransId transid;

	AosDocSvrObj::getDocSvr()->rollbackSnapshot(
			virtual_id, (AosDocType::E)doctype, snap_id, transid, rdata);

	AosTaskTransChecker::getSelf()->removeEntry(taskdocid, virtual_id, snap_id); 

	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosModifyDataletProc::createSnapshot(AosRundata *rdata, AosJimoCall &jimo_call)
{
	int virtual_id = jimo_call.getInt(rdata, AosFN::eValue, 0);
	int doctype = jimo_call.getInt(rdata, AosFN::eType, 0);
	u64 taskdocid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	jimo_call.reset();

	AosTransId transid;

	u64 snap_id = AosDocSvrObj::getDocSvr()->createSnapshot(
			virtual_id, snap_id, (AosDocType::E)doctype, transid, rdata);
	
	OmnScreen << "Test-------------- add snaoshpt :trans_id:" << 0
		<< "; mTaskDocid:" << taskdocid
		<< "; mVirtualId:" << virtual_id
		<< "; mSnapshotId:" << snap_id 
		<< endl;

	AosTaskTransChecker::getSelf()->addEntry(taskdocid, virtual_id, snap_id); 

	AosBuffPtr resp = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp->setU64(snap_id);
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}
