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
// 2015/3/31 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "DataletProcs/DeleteDataletProc.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoDocFunc.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/IILMgrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDeleteDataletProc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDeleteDataletProc(version);
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


AosDeleteDataletProc::AosDeleteDataletProc(const int version)
:
AosDataletProc(version)
{
}


AosJimoPtr
AosDeleteDataletProc::cloneJimo() const
{
	return OmnNew AosDeleteDataletProc(mJimoVersion);
}


bool 
AosDeleteDataletProc::isValid() const
{
	return true;
}


bool 
AosDeleteDataletProc::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case JimoDocFunc::eDeleteDatalet:
		return deleteDatalet(rdata, jimo_call);

	default:
		 break;
	}

	return false;

}


bool
AosDeleteDataletProc::deleteDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	//u32 siteid = rdata->getSiteid();
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	//u64 oprid = jimo_call.getU64(rdata, AosFN::eOprID, 0);
	jimo_call.reset();
	

	AosTransId transid;
#if 0	
	bool rslt = false;
	bool binary_doc = jimo_call.getBool(rdata, AosFN::eBinaryDoc, false);
	if (binary_doc)
	{
		rslt = AosDocSvrObj::getDocSvr()->deleteBinaryDoc(
			docid, rdata, oprid, snap_id);	
		if (!rslt)
		{
			rdata->setError() << "Failed to remove binary doc!" << docid;
			jimo_call.arg(AosFN::eErrmsg, "Failed to remove binary doc!");
			jimo_call.setLogicalFail();
			jimo_call.sendResp(rdata);
			return false;
		}
	}

	/*
	 * This is not done here.
	bool rslt = true;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{
		if (AosGetCubeId(objid) == AosGetCubeId(docid))
		{
			AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
			rslt = iilmgr->unbindObjid(objid, docid, mRdata);
			if (!rslt)
			{
				mRdata->setError() << "failed to unbind objid:"
					<< objid << ":" << docid;
				return false;
			}
		}
	}
	*/
#endif
	
	bool rslt = AosDocSvrObj::getDocSvr()->deleteObj(rdata, docid, transid, snap_id);
	if (!rslt)
	{
		//OmnAlarm << enderr;
		//rdata->setError() << "Failed to remove doc!" << docid;
		jimo_call.arg(AosFN::eErrmsg, "Failed to remove doc!");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return false;
	}
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}

