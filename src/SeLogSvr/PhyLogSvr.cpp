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
// 12/15/2011 Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/PhyLogSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "QueryUtil/QrUtil.h"
#include "SeLogUtil/LogUtil.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "SeLogSvr/Ptrs.h"
#include "SeLogSvr/MdlLogSvr.h"
#include "SeLogUtil/LogId.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/ThreadPool.h"
#include "TransServer/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include <vector>


OmnSingletonImpl(AosPhyLogSvrSingleton,
		AosPhyLogSvr,
		AosPhyLogSvrSelf,
		"AosPhyLogSvr");

static AosMdlLogSvrPtr			sgMdlSvrVect[AosLogUtil::eMaxModules];
bool AosPhyLogSvr::mIsChecking = false;
extern int gAosLogLevel;
static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("PhyLogSvr", __FILE__, __LINE__);


//AosPhyLogSvr::AosPhyLogSvr(const AosXmlTagPtr &config)
//{
//	aos_assert(init(config));
//}
	
AosPhyLogSvr::AosPhyLogSvr()
{
}


AosPhyLogSvr::~AosPhyLogSvr()
{
}


bool
AosPhyLogSvr::start()
{
	return true;
}
	

bool
AosPhyLogSvr::config(const AosXmlTagPtr &config)
{
	bool rslt = init(config);	
	aos_assert_r(rslt, false);
	return true;
}


bool
AosPhyLogSvr::init(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr selogsvr = config->getFirstChild("SeLogSvr");
	aos_assert_r(selogsvr, false);
		
	mIsChecking = selogsvr->getAttrBool(AOSTAG_ISCHECKING, false);

	return true;
}


/*
bool
AosPhyLogSvr::proc(
		const AosBigTransPtr &trans, 
		const AosBuffPtr &content,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans && content, false);
	
	AosXmlParser parser;
	AosXmlTagPtr trans_doc = parser.parse(content->data(), content->dataLen(),
			        "" AosMemoryCheckerArgs);
	aos_assert_r(trans_doc, false);

	trans->setFinishLater();
	AosPhyLogSvrPtr thisptr(this, false);
	OmnThrdShellProcPtr runner = OmnNew procThrd(thisptr, trans, trans_doc, rdata);
//	return OmnThreadShellMgr::getSelf()->proc(runner);
}
*/

bool
AosPhyLogSvr::addTrans(const AosLogTransPtr &trans)
{
	OmnThrdShellProcPtr runner = OmnNew procThrd(trans);
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->proc(runner);
}


bool
AosPhyLogSvr::addLog(
		//const AosXmlTagPtr &trans_doc,
		const AosLogOpr::E opr,
		const u64 ctnrdocid,
		const OmnString &pctr_objid,
		const OmnString logname,
		const AosXmlTagPtr &loginfo, 
		u64 &logid,
		const AosRundataPtr &rdata)
{
	//u64  ctnrdocid = trans_doc->getAttrU64(AOSTAG_CTNRDOCID, 0);
	aos_assert_r(ctnrdocid>0, false);
	int idx = ctnrdocid % AosGetNumCubes();
	AosMdlLogSvrPtr logsvr = getMdlLogSvr((int)idx);
	if (!logsvr)
	{
		AosSetError(rdata, AOSLT_MODULE_LOG_SERVER_NOT_FOUND);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//if (!logsvr->addLog(trans_doc, rdata))
	if (!logsvr->addLog(opr, pctr_objid, logname, loginfo, logid, rdata))
	{
		AosSetError(rdata, AOSLT_FAILED_ADD_LOG);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


AosMdlLogSvrPtr
AosPhyLogSvr::getMdlLogSvr(const int idx) 
{
	// This function retrieve the ctnrdocid % mNumVirSvr virtual 
	// server. if not exist in the virtual server map, put it into
	// the map.
	aos_assert_r(AosLogUtil::isValidModuleId(idx), 0);
	if (sgMdlSvrVect[idx]) return sgMdlSvrVect[idx];

	// The virtual server is not in the map
	AosMdlLogSvrPtr mlogsvr = OmnNew AosMdlLogSvr(idx);
	aos_assert_r(mlogsvr, 0);
	sgMdlSvrVect[idx] = mlogsvr;
	return mlogsvr;	
}


/*
bool
AosPhyLogSvr::retrieveLog(
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);
	OmnString logidstr = trans_doc->getNodeText();
	if (logidstr == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingLogid);
		OmnAlarm << rdata->getErrmsg() << ". Logid: " << logidstr << enderr;
		return false;
	}

	u64 logid = (u64)atoll(logidstr.data());
	aos_assert_r(logid>0, false);

	AosXmlTagPtr log = retrieveLog(logid, rdata);
	if (!log)
	{
		AosSetError(rdata, AOSLT_FAILED_RETRIEVE_LOG);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	OmnString contents = "<Contents>";
	contents << log->toString() << "</Contents>";
	rdata->setContents(contents);
	return true;
}
*/


AosXmlTagPtr
AosPhyLogSvr::retrieveLog(
		const u64 &logid,
		const AosRundataPtr &rdata)
{
	u32 seqno, offset, moduleid;
	AosLogId::decomposeLogid(logid, seqno, offset, moduleid);
	aos_assert_r(AosLogUtil::isValidModuleId(moduleid), 0);
	
	// When receive a request to retrieve a log, we compose the logid
	// to get the virid, the virtual server should be in this phy server,
	// if not, it is a error.
	AosMdlLogSvrPtr logsvr = sgMdlSvrVect[moduleid];
	if (!logsvr)
	{
		logsvr = getMdlLogSvr(moduleid);
		aos_assert_r(logsvr, 0);
	}
	aos_assert_r(logsvr, 0);	
	AosXmlTagPtr log = logsvr->retrieveLog(logid, rdata);
	if (!log)
	{
		AosSetError(rdata, AOSLT_FAILED_RETRIEVE_LOG);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Ketty 2013/03/27
	/*
	if (log->isRootTag())
	{
		log = log->getFirstChild();
	}
	else
	{
		log = log;
	}
	AosXmlTagPtr body = log->getFirstChild();
	aos_assert_r(body, 0);
	return body;
	*/
	return log;
}


OmnString
AosPhyLogSvr::retrieveLogs(
		//const AosXmlTagPtr &trans,
		const AosXmlTagPtr &req,
		const AosRundataPtr &rdata)
{
	// trans_doc should be this format:
	// <Contents fields="zky_objid|$|zky_objid|$$|...">
	// 		logid, logid ...
	// </Contents>
	aos_assert_rr(req, rdata, "");

	//AosXmlTagPtr transdoc;
	//if (trans->isRootTag()) 
	//{
	//	transdoc = trans->getFirstChild();	
	//}
	//else
	//{
	//	transdoc = trans;
	//}

	OmnString fields_str = req->getAttrStr(AOSTAG_FIELDS);
	aos_assert_r(fields_str != "", "");
	
	vector<AosQrUtil::FieldDef> fields;
	bool rslt = AosQrUtil::parseFnames(rdata, fields_str, fields, "|$$|", "|$|");
	if (!rslt)
	{
		AosSetError(rdata, AOSLT_FAILED_PARSING_FNAMES);
		OmnAlarm << rdata->getErrmsg() << ": " << fields_str << enderr;
		return "";
	}

	// Retrieve log
	OmnString lid, resp="<Contents>";
	AosXmlTagPtr log;
	OmnString logidstr = req->getNodeText();
	if (logidstr == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingLogid);
		OmnAlarm << rdata->getErrmsg() << ". Logid: " << logidstr << enderr;
		return "";
	}
	
	OmnStrParser1 parser(logidstr, ",", false, false);
	while ((lid = parser.nextWord()) != "")
	{
		u64 id = (u64)atoll(lid.data());
		log = retrieveLog(id, rdata);
		if (!log)
		{
			AosSetError(rdata, AosErrmsgId::eFailedRetrieveLog);
			OmnAlarm << rdata->getErrmsg() << ". Logid: " << id << enderr;
		}
		else
		{
			OmnString errmsg;
			bool rslt = AosQrUtil::createRecord(rdata, resp, log, fields);
			if (!rslt)
			{
				OmnAlarm << "Failed create record: " << errmsg << enderr;
			}
		}
	}
	resp << "</Contents>";
	//rdata->setContents(resp);
	return resp;
}


/*
void
AosPhyLogSvr::sendResp(
		const AosBigTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	AosXmlRc errcode = rdata->getErrcode();
	OmnString errmsg = rdata->getErrmsg(); 
	OmnString resp = "<response><status error=\"";
	if (errcode == eAosXmlInt_Ok)
	{
		resp << "false\" code=\"200\" />";
	}
	else
	{
		resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";
	}

	if (rdata->getContents() != "")
	{
		resp << rdata->getContents();
	}
	resp << "</response>";
	if (gAosLogLevel >=3)
	{
		OmnScreen << "Send response: "<< resp << endl;
	}

	//rdata->setContents(resp);
	
	// Ketty 2012/11/12
	aos_assert(mTransServer); 
	AosBuffPtr resp_buff = OmnNew AosBuff(resp.length() + 100, 0 AosMemoryCheckerArgs);	
	resp_buff->setBuff(resp.data(), resp.length());
	mTransServer->sendResp((AosTrans *)trans.getPtr(), resp_buff);	
	mTransServer->finishTrans(trans);
}


bool
AosPhyLogSvr::procThrd::run()
{
	bool rslt;
	//AosRundataPtr rdata = mTrans->getRdata();
	AosRundataPtr rdata = mTrans->getRdata();
	OmnString opr = mTransDoc->getAttrStr(AOSTAG_LOG_OPR);
	if (opr == "")
	{
		AosSetError(rdata, AOSLT_MISSING_OPERATION);
		OmnAlarm << "InValid Operation: " << opr << enderr;
		mPhyLogSvr->sendResp(mTrans, rdata);
		return false;
	}
	if (opr == AOSLOGOPR_RETRIEVE)
	{
		rslt = mPhyLogSvr->retrieveLog(mTransDoc, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AOSLT_FAILED_RETRIEVE_LOG);
			OmnAlarm << rdata->getErrmsg() << enderr;
			mPhyLogSvr->sendResp(mTrans, rdata);
			return false;
		}
		mPhyLogSvr->sendResp(mTrans, rdata);
		return true;
	}
	if (opr == AOSLOGOPR_RETR_LOGS) 
	{
		rslt = mPhyLogSvr->retrieveLogs(mTransDoc, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AOSLT_FAILED_RETRIEVE_LOG);
			OmnAlarm << rdata->getErrmsg() << enderr;
			mPhyLogSvr->sendResp(mTrans, rdata);
			return false;
		}
		mPhyLogSvr->sendResp(mTrans, rdata);
		return true;
	}
	else
	{
		rslt = mPhyLogSvr->addLog(mTransDoc, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AOSLT_FAILED_ADD_LOG);
			OmnAlarm << rdata->getErrmsg() << enderr;
			mPhyLogSvr->sendResp(mTrans, rdata);
			return false;
		}
		mPhyLogSvr->sendResp(mTrans, rdata);
		return true;
	}

	OmnShouldNeverComeHere;
	mPhyLogSvr->sendResp(mTrans, rdata);
	return false;
}

bool
AosPhyLogSvr::procThrd::procFinished()
{
	return true;
}
*/


bool
AosPhyLogSvr::procThrd::run()
{
	return mTrans->procLog();
}


bool
AosPhyLogSvr::procThrd::procFinished()
{
	return true;
}

