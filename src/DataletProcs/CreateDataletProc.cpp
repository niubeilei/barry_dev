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
#include "DataletProcs/CreateDataletProc.h"

#include "SEInterfaces/DocSvrObj.h"
#include "SEInterfaces/StatSvrObj.h"
#include "JimoAPI/JimoDocFunc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "StatUtil/StatModifyInfo.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCreateDataletProc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCreateDataletProc(version);
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


AosCreateDataletProc::AosCreateDataletProc(const int version)
:
AosDataletProc(version)
{
}


AosJimoPtr
AosCreateDataletProc::cloneJimo() const
{
	return OmnNew AosCreateDataletProc(mJimoVersion);
}


bool 
AosCreateDataletProc::isValid() const
{
	return true;
}


bool 
AosCreateDataletProc::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case JimoDocFunc::eCreateDatalet:
		return createDatalet(rdata, jimo_call);

	case JimoDocFunc::eCreateDataletSafe:
		return createDataletSafe(rdata, jimo_call);

	case JimoDocFunc::eWriteAccessDatalet:
		return saveToFile(rdata, jimo_call);

	case JimoDocFunc::eBatchSaveStatDatalets:
		return batchSaveStatDatalets(rdata, jimo_call);

	case JimoDocFunc::eRetrieveBinaryDatalet:
		return retrieveBinaryDatalet(rdata, jimo_call);

	default:
		 break;
	}

	return false;

}


bool
AosCreateDataletProc::createDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function is the server side implementations of AosDocSvr::createDoc(...)
	// It assumes:
	// 	<request ...>
	// 		<doc>
	// 			<the_doc .../>
	// 		</doc>
	// 	</request>
	
	bool isXml = jimo_call.getBool(rdata, AosFN::eFlag, false);
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	OmnString docstr = jimo_call.getOmnStr(rdata, AosFN::eDoc, "");
	jimo_call.reset();

	//u64 opr_id = jimo_call.getU64(rdata, AosFN::eOprID, 0);
	
	if (!isXml)
	{
		bool rslt = AosDocSvrObj::getDocSvr()->createDoc(rdata, docid, docstr.data(), docstr.length(), AosTransId::Invalid, snap_id);
		if( !rslt )
		{
			jimo_call.arg(AosFN::eErrmsg, "createDoc_failed");
			jimo_call.setLogicalFail();
			jimo_call.sendResp(rdata);
			return true;
		}

		jimo_call.setSuccess();
		jimo_call.sendResp(rdata);
		return true;
	}

	AosXmlTagPtr newdoc = AosXmlParser::parse(docstr);
	if(newdoc.getPtr() == 0)
	{
		jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	u32 siteid = newdoc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
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
			//aos_assert_r(rslt, false);
			//aos_assert_r(signature != "", false);
			//newdoc->setAttr(AOSTAG_BINARY_DOC_SIGNATURE, signature);
			if (!rslt || signature == "")
			{
				jimo_call.arg(AosFN::eErrmsg, "createDoc_failed");
				jimo_call.setLogicalFail();
				jimo_call.sendResp(rdata);
				return true;
			}
		}
	}

	rslt = AosDocSvrObj::getDocSvr()->createDoc(rdata, newdoc, transid, snap_id);
	if (!rslt)
	{
		jimo_call.arg(AosFN::eErrmsg, "createDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosCreateDataletProc::createDataletSafe(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function is the server side implementations of AosDocSvr::createDocSafe(...)
	
	OmnString docstr = jimo_call.getOmnStr(rdata, AosFN::eDoc);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID);
	AosXmlTagPtr newdoc = AosXmlParser::parse(docstr);
	bool saveflag = jimo_call.getBool(rdata, AosFN::eFlag);
	//bool need_save = jimo_call.getBool(rdata, AosFN::eValue1);
	//bool need_resp = jimo_call.getBool(rdata, AosFN::eValue2);
	jimo_call.reset();

	if(newdoc.getPtr() == 0)
	{
		jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	u32 siteid = newdoc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	   	OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}
	
	bool rslt = false;

	AosTransId transid;

	if (newdoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		OmnString nodename = newdoc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
		AosBuffPtr buff = newdoc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
		aos_assert_r(buff && buff->dataLen() >0, false);
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
		//aos_assert_r(rslt, false);
		//aos_assert_r(signature != "", false);
		//newdoc->setAttr(AOSTAG_BINARY_DOC_SIGNATURE, signature);
		if ( !rslt || signature == "" )
		{
			jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
			jimo_call.setLogicalFail();
			jimo_call.sendResp(rdata);
			return true;
		}
	}

	rslt = AosDocSvrObj::getDocSvr()->createDocSafe(rdata, newdoc, saveflag, transid, snap_id);
	if (!rslt)
	{
		jimo_call.arg(AosFN::eErrmsg, "createDocSafe_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosCreateDataletProc::saveToFile(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	//bool need_save = jimo_call.getBool(rdata, AosFN::eValue1);
	//bool need_resp = jimo_call.getBool(rdata, AosFN::eValue2);

	OmnString docstr = jimo_call.getOmnStr(rdata, AosFN::eDoc, "");
	AosXmlTagPtr arcd = AosXmlParser::parse(docstr);
	jimo_call.reset();

	if(arcd.getPtr() == 0)
	{
		jimo_call.arg(AosFN::eErrmsg, "saveBinaryDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	AosTransId transid;

	bool rslt;
	rslt = AosDocSvrObj::getDocSvr()->saveToFile(docid, arcd, rdata, transid, snap_id);
	if (!rslt)
	{
		jimo_call.arg(AosFN::eErrmsg, "save_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosCreateDataletProc::batchSaveStatDatalets(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString statcubestr = jimo_call.getOmnStr(rdata, AosFN::eValue, "");
	AosXmlTagPtr statcubeconf = AosXmlParser::parse(statcubestr);

	AosBuffPtr inputdata = jimo_call.getBuff(rdata, AosFN::eBuff, 0);
	AosBuffPtr statbuff = jimo_call.getBuff(rdata, AosFN::eData, 0);

	AosStatModifyInfo statmdfinfo;
	statmdfinfo.serializeFrom(rdata, statbuff);
	jimo_call.reset();

	AosBuffPtr resp;
	AosStatSvrObjPtr stat_svr = AosStatSvrObj::getStatSvr();
	if(!stat_svr)
	{
		resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
		resp->setU8(false);	
		jimo_call.arg(AosFN::eBuff, resp);
		jimo_call.arg(AosFN::eErrmsg, "get_StatSvr_failed");
		jimo_call.sendResp(rdata);
		return true;
	}

	bool rslt = stat_svr->modify(rdata, statcubeconf, inputdata, statmdfinfo);
	if(!rslt)
	{
		resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
		resp->setU8(false);	
		jimo_call.arg(AosFN::eBuff, resp);
		jimo_call.arg(AosFN::eErrmsg, "modify_failed");
		jimo_call.sendResp(rdata);
		return true;
	}

	resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp->setU8(true);
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool
AosCreateDataletProc::retrieveBinaryDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnAlarm << enderr;
	return false;
}
