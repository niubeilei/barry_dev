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
// Modification History:
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MsgProc/GareaMsgProc.h"

#include "API/AosApi.h"
#include "Base64/Base64.h"
#include "DbQuery/Query.h"
#include "Rundata/Rundata.h"
#include "Security/SecurityMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/File.h"
#include "Util/StrUtil.h"


static 	u32 sgSessionId					 = 10000;
#define AOSTAG_VERSION_R 				 "version"
#define AOSSOAPDOC_GAREA_SMARTDOC    "garea_smtdoc_1.txt"
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::gregorian;
using namespace boost::posix_time;


const OmnString sgErrmgType_InvalidRequest 				= "Invalid request";
const OmnString sgErrmgType_FailedToQuery  				= "Query failed";
const OmnString sgErrmgType_InvalidDoctorId  			= "Invalid doctor id";
const OmnString sgErrmgType_UndefinedCommand			= "Command is undefined";
const OmnString sgErrmgType_NotFoundDeviceDoc			= "Device doc not found"; 
const OmnString sgErrmgType_PatientAttrIsEmpty			= "Patient's attribute is empty";
const OmnString sgErrmgType_PatientValueIsEmpty			= "Patient's value is empty";
const OmnString sgErrmgType_SessionNotFound				= "session not found"; 
const OmnString sgErrmgType_DataIncorrect				= "Data Incorrect";
const OmnString sgErrmgType_TooBigData					= "Data Too Big";
const OmnString sgErrmgType_UndefinedItemCode			= "Undefined Item Code";
const OmnString sgErrmgType_UndefinedUploadRequest		= "Undefined Upload Request";



const OmnString sgRespType_DataIsEmpty     = "The requested data is empty";
const OmnString sgRespType_AccessDenied    = "Access Denied";

AosGareaMsgProc::AosGareaMsgProc(const bool flag)
:
AosMsgProc(AOSMSGPROCTYPE_RECEIVER_PROC, AosMsgProcType::eReceiverProc, flag),
mMax_xindian(0),
mMin_xindian(0),
mMax_xueyangwave(0),
mMin_xueyangwave(0),
mMax_spo2(0),
mMin_spo2(0),
mMax_glu(0),
mMin_glu(0),
mMax_nibp_sys(0),
mMin_nibp_sys(0),
mMax_nibp_dia(0),
mMin_nibp_dia(0),
mMax_nibp_map(0),
mMin_nibp_map(0)
{
	//aos_assert(init());
//	initParam();
}


bool
AosGareaMsgProc::initParam()
{
	mDocClientObj = AosDocClientObj::getDocClient();
	mDoc = readSmartDoc();
	aos_assert_r(mDoc, false);
	AosXmlTagPtr data_control = mDoc->getFirstChild("data_control");
	mMax_xindian     = data_control->getAttrInt("max_xindian",0);
	mMin_xindian     = data_control->getAttrInt("min_xindian",0);
	mMax_xueyangwave = data_control->getAttrInt("max_xueyangwave",0);
	mMin_xueyangwave = data_control->getAttrInt("min_xueyangwave",0);
	mMax_spo2        = data_control->getAttrInt("max_spo2",0);
	mMin_spo2        = data_control->getAttrInt("min_spo2",0);
	mMax_nibp_sys    = data_control->getAttrInt("max_nibp_sys",0);
	mMin_nibp_sys    = data_control->getAttrInt("min_nibp_sys",0);
	mMax_nibp_dia    = data_control->getAttrInt("max_nibp_dia",0);
	mMin_nibp_dia    = data_control->getAttrInt("min_nibp_dia",0);
	mMax_nibp_map    = data_control->getAttrInt("max_nibp_map",0);
	mMin_nibp_map    = data_control->getAttrInt("min_nibp_map",0);

	OmnString max_glu_str = data_control->getAttrStr("max_glu");
	OmnString min_glu_str = data_control->getAttrStr("min_glu");
	mMax_glu         = atof(max_glu_str.data());
	mMin_glu         = atof(min_glu_str.data());
	return true;
}

bool
AosGareaMsgProc::init()
{
	mDocClientObj = AosDocClientObj::getDocClient();
	mDoc = readSmartDoc();
	aos_assert_r(mDoc, false);
	mPageSize = mDoc->getAttrInt(AOSTAG_PAGESIZE, 0);
	aos_assert_r(mPageSize>0, false);
	mDeviceCtnr = mDoc->getAttrStr(AOSTAG_DEVICE_CTNR);
	aos_assert_r(mDeviceCtnr != "", false);
	mPatientCtnr = mDoc->getAttrStr(AOSTAG_PATIENT_CTNR);
	aos_assert_r(mPatientCtnr != "", false);
	mMaxDataSize = mDoc->getAttrU64("data_max_size", 0);
	aos_assert_r(mMaxDataSize, false);
	
	AosXmlTagPtr data_control = mDoc->getFirstChild("data_control");
	aos_assert_r(data_control, false);	
	mMax_xindian     = data_control->getAttrInt("max_xindian",0);
	mMin_xindian     = data_control->getAttrInt("min_xindian",0);
	mMax_xueyangwave = data_control->getAttrInt("max_xueyangwave",0);
	mMin_xueyangwave = data_control->getAttrInt("min_xueyangwave",0);
	mMax_spo2        = data_control->getAttrInt("max_spo2",0);
	mMin_spo2        = data_control->getAttrInt("min_spo2",0);
	mMax_nibp_sys    = data_control->getAttrInt("max_nibp_sys",0);
	mMin_nibp_sys    = data_control->getAttrInt("min_nibp_sys",0);
	mMax_nibp_dia    = data_control->getAttrInt("max_nibp_dia",0);
	mMin_nibp_dia    = data_control->getAttrInt("min_nibp_dia",0);
	mMax_nibp_map    = data_control->getAttrInt("max_nibp_map",0);
	mMin_nibp_map    = data_control->getAttrInt("min_nibp_map",0);

	OmnString max_glu_str = data_control->getAttrStr("max_glu");
	OmnString min_glu_str = data_control->getAttrStr("min_glu");
	mMax_glu         = atof(max_glu_str.data());
	mMin_glu         = atof(min_glu_str.data());
	// mDoc should be this format:
	// <doc zky_doctor_attr="xxx" zky_doctor_value="xxx"  zky_patient_attr="xxx" zky_patient_value="xxx"_>
	// 	 <doctor>
	// 	 	<attributes>
	// 	 		<fullname />
	// 	 		<password />
	// 	 		<userId	  />
	// 	 		......
	// 	 	</attributes>
	// 	 </doctor>
	// 	 <patient>
	// 	 	<attributes>
	// 	 		<fullname />
	// 	 		<password />
	// 	 		<userId	  />
	// 	 		.....
	// 	 	</attributes>
	// 	 </patient>
	// </doc>
	// The tag attributes should contain all the infomation the response should have,
	// if the coustomer ask to add or delete the information of request, we just to 
	// modify the mDoc in our system.
	return true;
}


AosXmlTagPtr 
AosGareaMsgProc::readSmartDoc()
{
 	OmnString fname = AOSSOAPDOC_GAREA_SMARTDOC;
	OmnFile f(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
 	if (!f.isGood())
 	{
   	  	OmnAlarm << "Failed to open the config file: " << fname << enderr;
		return 0; 	
	}

 	OmnString contents;
 	if (!f.readToString(contents))
	{
   		OmnAlarm << "Failed to read config: " << fname << enderr;
     	return 0;
	}

 	AosXmlParser parser;
 	AosXmlTagPtr sdoc = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (!sdoc)
 	{
   		OmnAlarm << "Configuration file incorrect: " << fname << enderr;
     	return 0;
 	}
	return sdoc;
}


AosGareaMsgProc::~AosGareaMsgProc()
{
}


bool
AosGareaMsgProc::msgReceived(
		const OmnString &appid,
		const AosXmlTagPtr &msg, 
		const AosRundataPtr &rdata)
{
	// msg should be this format:
	// <Request>
	//   <header>
	//   	<version>1.0</version>
	//   	<deviceId>{2013E03B7065,  这个是变量}</deviceId>
	//   	<command>1</command>
	//   </header>
	//   <body>
	//   </body>
	// </Request>
	//
	// where 'command' can be one of the following:
	aos_assert_r(msg, false);
	
	AosXmlTagPtr header = msg->getFirstChild(AOSTAG_HEADER);
	if (!header)
	{
		OmnAlarm << "Invalid request" << enderr;
		//sendResponse(req, sgErrmgType_InvalidRequest, rdata);
		return false;
	}
	AosSoapRequest req;
	aos_assert_r(parseCmd(req, header, rdata), false);
	AosXmlTagPtr body = msg->getFirstChild(AOSTAG_BODY);
	if (!body)
	{
		OmnAlarm << "Invalid request" << enderr;
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		return false;
	}

	if (req.getCmd() == eInValid)
	{
		OmnAlarm << sgErrmgType_UndefinedCommand << enderr;
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_UndefinedCommand);
		sendResponse(req, "", -1, rdata);
		return false;
	}

	switch (req.getCmd())
	{
	case eLogin:
		 return login(req, body, rdata);

	case eSyncPtntInfo:
		 return syncPatientInfo(req, body, rdata);

	case eSetAdroidPara:
		 return setAndroidPara(req, body, rdata);
		 
	case eUpdHisData:
		 return uploadHistoryData(req, body, rdata);

	case eUpdRtimeData:
		 return uploadRtimeData(req, body, rdata);

	case eSyncDctrInfo:
		 return syncDoctorInfo(req, body, rdata);

	default:
		 OmnAlarm << sgErrmgType_UndefinedCommand << enderr;
		 req.setErrCode(1);
		 req.setErrmsg(sgErrmgType_UndefinedCommand);
		 sendResponse(req, "", -1, rdata);
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosGareaMsgProc::login(
		AosSoapRequest &req,
		const AosXmlTagPtr &body, 
		const AosRundataPtr &rdata)
{
	// This function login to system.
	aos_assert_r(body, false);

	////////////////////////////////////////////////////////////
	// Now, we just send a string to client for sync.
	// ////////////////////////////////////////////////////////
	OmnString resp;
	resp << "<response>"
		<< "login success!"
		<< "</response>";
	AosXmlParser parser;
	AosXmlTagPtr response = parser.parse(resp, "" AosMemoryCheckerArgs);
	rdata->setResults(resp);
	return true;
	/////////////////////////////////////////////////////////////
}


bool
AosGareaMsgProc::syncPatientInfo(
		AosSoapRequest &req,
		const AosXmlTagPtr &body, 
		const AosRundataPtr &rdata)
{
	// 1. Retrieve the device id
	// 2. Use device id to retrieve the doc (Device Doc) for the device id
	// 3. From the Dovice doc, retrieve the Location info
	// 4. Using the "Location Info" to determine which patients to download
	// 5. Download the patients based on 'page number'.
	//
	// This function sync the patient's information.
	// The body should be this format:
	// <request>
	//    <updateTime>2012/02/02 13:20:03</updateTime>
	//    <page>0</page>
	// </request>
	aos_assert_r(body, false);
	AosXmlTagPtr request = body->getFirstChild(AOSTAG_REQUEST);
	if (!request)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}
	
	AosXmlTagPtr updatetime = request->getFirstChild(AOSTAG_UPDATETIME);
	if (!updatetime)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr;
		return false;
	}

	int64_t last_time = 0;
	OmnString timestr = updatetime->getNodeText();
	if (timestr != "" && timestr != "0")
	{
		last_time = timeStr2Int(timestr);
	}

	AosXmlTagPtr page = request->getFirstChild(AOSTAG_PAGE);
	if (!page)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr;
		return false;
	}
	int pnum = atoi(page->getNodeText().data());
	req.setPage(pnum);

	// Retrieve device doc
	bool isunique;
	u64 did = AosQuery::getSelf()->getMember(mDeviceCtnr, 
			AOSTAG_OBJID, req.getDvcId(), isunique, rdata);
	if (did == 0)
	{
		req.setErrCode(3);
		req.setErrmsg(sgErrmgType_NotFoundDeviceDoc);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_NotFoundDeviceDoc 
			<< ":" << mDeviceCtnr 
			<< ":" << mDoc->getAttrStr(AOSTAG_DEVIECE_ATTR)
			<< ":" << req.getDvcId() << enderr;
		return false;
	}
	
	AosXmlTagPtr doc = mDocClientObj->getDocByDocid(did, rdata);
	if (!doc)
	{
		req.setErrCode(3);
		req.setErrmsg(sgErrmgType_NotFoundDeviceDoc);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_NotFoundDeviceDoc << enderr;
		return false;
	}
	
	AosQueryRsltObjPtr query_rslt = 0;
	OmnString objids[eMaxConds];
	OmnString attrs[eMaxConds];
	AosValueRslt valueRslts[eMaxConds];
	AosOpr oprs[eMaxConds];
	bool reverses[eMaxConds];
	bool orders[eMaxConds];
	
	// Need to retrieve the district value based on the device id.
	// Set the condition: 
	// objids[0] = req.getDvcId();
	OmnString aname = doc->getAttrStr(AOSTAG_PATIENT_ATTR);
	if (aname == "")
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_PatientAttrIsEmpty);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_PatientAttrIsEmpty << enderr;
		return false;
	}
	OmnString value = doc->getAttrStr(AOSTAG_PATIENT_VALUE);
	if (value == "")
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_PatientValueIsEmpty);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_PatientValueIsEmpty << enderr; 
		return false;
	}
	
	objids[0] = "";
	attrs[0] = aname; 
	valueRslts[0].setStr(value);
	oprs[0] = eAosOpr_eq;
	reverses[0] = false;
	orders[0] = false;
	
	// 		AOSTAG_OTYPE == "garea_patient"
	objids[1] = ""; 
	attrs[1] = AOSTAG_OTYPE;
	valueRslts[1].setStr("garea_patient");
	oprs[1] = eAosOpr_eq;
	reverses[1] = false;
	orders[1] = false;

	// Set the modification time condition:
	// 		AOSTAG_MT_EPOCH > last_time
	objids[2] = ""; 
	attrs[2] = AOSTAG_MT_EPOCH;
	valueRslts[2].setI64(last_time);
	oprs[2] = eAosOpr_gt;
	reverses[2] = false;
	orders[2] = false;
	
	int startidx = mPageSize * pnum;
	bool rslt = AosQuery::getSelf()->runQuery(startidx, mPageSize, objids, attrs, 
					valueRslts, oprs, reverses, orders, 3, query_rslt, rdata);  
	if (!rslt)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_FailedToQuery);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_FailedToQuery << enderr;
		return false;
	}
	
	aos_assert_r(query_rslt,0);
	if (query_rslt->isEmpty())
	{
		req.setPage(-1);
		req.setErrCode(0);
		req.setErrmsg("");
		sendResponse(req, "", -1, rdata);
		return true;
	}

	bool finished = false;
	u64 docid = 0;
	query_rslt->reset();
	for (int i=0; i<startidx; i++)
	{
		query_rslt->nextDocid(docid, finished, rdata);
		if (finished) 
		{
			req.setPage(-1);
			break;
		}
	}

	if (finished)
	{
		req.setErrCode(0);
		req.setErrmsg("");
		req.setPage(-1);
		sendResponse(req, "", -1, rdata);
		return true;
	}

	OmnString response = parseQueryRslt(req, "patient", query_rslt, rdata);
	if (response == "")
	{
		req.setErrCode(0);
		req.setErrmsg("");
		sendResponse(req, "", -1, rdata);
		return true;
	}
	
	req.setErrCode(0);
	req.setErrmsg("");
	aos_assert_r(sendResponse(req, response, -1, rdata), false);
	return true;
}


bool
AosGareaMsgProc::syncDoctorInfo(
		AosSoapRequest &req, 
		const AosXmlTagPtr &body, 
		const AosRundataPtr &rdata)
{
	// This function sync the doctor's information.
	// The body should be this format:
	// <request>
	//    <updateTime>2012/02/02 13:20:03</updateTime>
	//    <page>0</page>
	// </request>
	aos_assert_r(body, false);
	AosXmlTagPtr request = body->getFirstChild(AOSTAG_REQUEST);
	if (!request)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}
	
	AosXmlTagPtr updatetime = request->getFirstChild(AOSTAG_UPDATETIME);
	if (!updatetime)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}
	int64_t last_time;
	OmnString timestr = updatetime->getNodeText();
	if (timestr != "" && timestr != "0")
	{
		last_time = timeStr2Int(timestr);
	}


	AosXmlTagPtr page = request->getFirstChild(AOSTAG_PAGE);
	if (!page)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}
	int pnum = atoi(page->getNodeText().data());
	req.setPage(pnum);

	// Retrieve device doc
	bool isunique;
	u64 did = AosQuery::getSelf()->getMember(mDeviceCtnr, 
			AOSTAG_OBJID, req.getDvcId(), isunique, rdata);
	if (did == 0)
	{
		req.setErrCode(3);
		req.setErrmsg(sgErrmgType_NotFoundDeviceDoc);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_NotFoundDeviceDoc << enderr;
		return false;
	}
	 
	AosXmlTagPtr device_doc = mDocClientObj->getDocByDocid(did, rdata);
	if (!device_doc)
	{
		req.setErrCode(3);
		req.setErrmsg(sgErrmgType_NotFoundDeviceDoc);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_NotFoundDeviceDoc << enderr;
		return false;
	}
	
	AosQueryRsltObjPtr query_rslt = 0;
	OmnString objids[eMaxConds];
	OmnString attrs[eMaxConds];
	AosValueRslt valueRslts[eMaxConds];
	AosOpr oprs[eMaxConds];
	bool reverses[eMaxConds];
	bool orders[eMaxConds];
	
	// Need to retrieve the district value based on the device id.
	// Set the condition: 
	// objids[0] = req.getDvcId();
	OmnString aname = device_doc->getAttrStr(AOSTAG_DOCTOR_ATTR);
	if (aname == "")
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_PatientAttrIsEmpty);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_PatientAttrIsEmpty << enderr;
		return false;
	}
	OmnString value = device_doc->getAttrStr(AOSTAG_DOCTOR_VALUE);
	if (value == "")
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_PatientValueIsEmpty);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_PatientValueIsEmpty << enderr; 
		return false;
	}
	
	// objids[0] = mPatientCtnr;
	objids[0] = "";
	attrs[0] = aname; 
	valueRslts[0].setStr(value);
	oprs[0] = eAosOpr_eq;
	reverses[0] = false;
	orders[0] = false;

	// Set the modification time condition:
	// 		AOSTAG_MT_EPOCH > last_time
	objids[1] = ""; 
	attrs[1] =  AOSTAG_MT_EPOCH;
	valueRslts[1].setI64(last_time);
	oprs[1] = eAosOpr_gt;
	reverses[1] = false;
	orders[1] = false;
	
	// 		AOSTAG_OTYPE == "garea_patient"
	objids[2] = ""; 
	attrs[2] = AOSTAG_OTYPE;
	valueRslts[2].setStr("garea_patient");
	oprs[2] = eAosOpr_eq;
	reverses[2] = false;
	orders[2] = false;
	
	int startidx = mPageSize * pnum;
	bool rslt = AosQuery::getSelf()->runQuery(startidx, mPageSize, objids, attrs, 
					valueRslts, oprs, reverses, orders, 3, query_rslt, rdata);  
	if (!rslt)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_FailedToQuery);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_FailedToQuery << enderr;
		return false;
	}

	aos_assert_r(query_rslt,false);
	if (query_rslt->isEmpty())
	{
		req.setErrCode(0);
		req.setErrmsg("");
		sendResponse(req, "", -1, rdata);
		return true;
	}
	
	// we need to response from startidx.
	bool finished;
	u64 docid;
	for (int i=0; i<startidx; i++)
	{
		query_rslt->nextDocid(docid, finished, rdata);
		if (finished) break;
	}

	if (finished)
	{
		req.setErrCode(0);
		req.setErrmsg("");
		sendResponse(req, "", -1, rdata);
		return true;
	}
	
	
	OmnString response = parseQueryRslt(req, "doctor", query_rslt, rdata);
	if (response == "")
	{
		req.setErrCode(0);
		req.setErrmsg("");
		sendResponse(req, "", -1, rdata);
		return true;
	}

	req.setErrCode(0);
	req.setErrmsg("");
	aos_assert_r(sendResponse(req, response, -1, rdata), false);
	return true;
}


OmnString 
AosGareaMsgProc::parseQueryRslt(
		AosSoapRequest &req,
		const OmnString &tagname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	// This function will parse the data queryed into the 
	// the response xml.
	aos_assert_r(mDoc, "");
	aos_assert_r(query_rslt, "");

	// AosXmlTagPtr doc = mDoc->getFirstChild(tagname);
	// aos_assert_r(doc, "");
	// AosXmlTagPtr attributes = doc->getFirstChild("attributes");
	// aos_assert_r(attributes, "");
	// u64 docid;
	// bool finished;
	// query_rslt->nextDocid(docid, finished, rdata);

	query_rslt->reset();
	AosXmlTagPtr attributes = mDoc->getFirstChild(tagname);
	aos_assert_r(attributes, "");
	u64 docid;
	bool finished;
	query_rslt->nextDocid(docid, finished, rdata);
	
	OmnString response;
	response << "<updateTime>" << getTime() << "</updateTime>";
	OmnString page = "<page>";
	OmnString userinfo = "<users>";
	while (docid>0)
	{
		AosXmlTagPtr dd = mDocClientObj->getDocByDocid(docid, rdata);
		if (!dd)
		{
			rdata->setError() << "Missing doc: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return "";
		}
		userinfo << "<user>";
		AosXmlTagPtr attr = attributes->getFirstChild();
		while (attr)
		{
			OmnString tname = attr->getTagname();
			OmnString value = dd->getAttrStr(tname);

			userinfo << "<" << tname << ">" << value << "</" << tname << ">";
			attr = attributes->getNextChild();
		}
		userinfo << "</user>";
		query_rslt->nextDocid(docid, finished, rdata);
		if (finished)
		{
			req.setPage(-1);
			break;
		}
	}
	page << req.getPage() << "</page>";
	response << page << userinfo << "</users>";
	return response; 
}


bool
AosGareaMsgProc::sendResponse(
		AosSoapRequest &req,
		const OmnString &response,
		const int &interval,
		const AosRundataPtr &rdata)
{
	//<Response>
	//  <header>
	//     <version>1.0</version>
	//     <command>1</command>
	//     <errorCode>0</errorCode>
	//     <errorMsg></errorMsg>
	//  </header>
	//  <body>
	//  </body>
	//</Response>
	
	// First, we retrieve the template of response from mDoc.
	aos_assert_r(mDoc, false);
	AosXmlTagPtr resp_tag = mDoc->getFirstChild(AOSTAG_RESPONSE);
	aos_assert_r(resp_tag, false);

	resp_tag = resp_tag->getFirstChild("Response");
	aos_assert_r(resp_tag, false);

	AosXmlTagPtr response_tag = resp_tag->clone(AosMemoryCheckerArgsBegin);
	
	// Second, we set contents to header
	AosXmlTagPtr header = response_tag->getFirstChild(AOSTAG_HEADER);
	aos_assert_r(header, false);

	AosXmlTagPtr version = header->getFirstChild(AOSTAG_VERSION_R);
	aos_assert_r(version, false);
	version->setNodeText(req.getVersion(), false);

	AosXmlTagPtr command = header->getFirstChild(AOSTAG_COMMAND);
	aos_assert_r(command, false);
	OmnString cmd;
	cmd << req.getCmd();
	command->setNodeText(cmd, false);
	
	AosXmlTagPtr errorCode = header->getFirstChild("errorCode");
	aos_assert_r(errorCode, false);
	OmnString errcode;
	errcode << req.getErrCode();
	errorCode->setNodeText(errcode, false);
	
	AosXmlTagPtr errmsg = header->getFirstChild("errorMsg");
	aos_assert_r(errmsg, false);
	errmsg->setNodeText(req.getErrmsg(), false);

	// Then we will set the contents of body to template.
	// The response should be in the form:
	// 	<body>
	// 		<response>
	// 			response
	// 		</response
	// 	</body>
	OmnString respstr = "<response>";
	respstr << response;
	if(interval >0)
	{
		respstr << "<interval>" << interval << "</interval>";
	}
	respstr << "</response>";

	AosXmlParser parser;
	AosXmlTagPtr node = parser.parse(respstr, "" AosMemoryCheckerArgs);
	aos_assert_r(node, false);

	AosXmlTagPtr body = response_tag->getFirstChild("body");
	aos_assert_r(body, false);

	body->addNode(node);

OmnString ss = response_tag->toString();
OmnScreen << "Response: " << ss << endl;
	rdata->setResults(response_tag->toString());
	return true;
}


bool
AosGareaMsgProc::setAndroidPara(
		AosSoapRequest &req, 
		const AosXmlTagPtr &body, 
		const AosRundataPtr &rdata)
{
	OmnString objid = mDoc->getAttrStr(AOSTAG_ANDROID_PARA);
	aos_assert_r(objid != "", false);
	AosXmlTagPtr doc = mDocClientObj->getDocByObjid(objid, rdata);
	aos_assert_r(doc, false);
	OmnString syncInterval = doc->getAttrStr(mDoc->getAttrStr(AOSTAG_SYNCINTERVAL));
	aos_assert_r(syncInterval != "", false);

	OmnString response;
	response << "<updateTime>" << getTime() << "</updateTime>"
		<< "<param>"
		<< "<syncInterval>" << syncInterval << "</syncInterval>"
		<< "</param>";
	aos_assert_r(sendResponse(req, response, -1, rdata), false);
	return true;
}


bool
AosGareaMsgProc::uploadHistoryData(
		AosSoapRequest &req, 
		const AosXmlTagPtr &body, 
		const AosRundataPtr &rdata)
{
	// This function uploads history data to the server. 
	// The message format is:
	// 	<request>
	// 		<userId>112233</userId>
	// 		<doctorId>556677</doctorId>
	// 		<itemcode>1</itemcode>
	// 		<testtime>2012/02/02 13:20:03</testtime>
	// 	</request>
	//
	// 1. If there is no session id, it is a request for uploading
	//    history data. We need to create a session id and send
	//    a response back.
	//
	// 2. If there is a session id, it is for uploading the history
	//    data. We need to retrieve the data and save the data 
	//    into the database.
	AosXmlTagPtr request = body->getFirstChild("request");
	if (!request)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	// 1. Retrieve the session ID
	AosXmlTagPtr ssid_tag = request->getFirstChild("sessionId");
	OmnString ssid = request->xpathQuery("sessionId/_#text");;
	if (ssid == "")
	{
		// 1. Retrieve 'userId'
		OmnString user_cloudid = request->xpathQuery("userId/_#text");
		if (user_cloudid == "")
		{
			req.setErrCode(1);
			req.setErrmsg("Missing UserID");
			sendResponse(req, "", -1, rdata);
			return true;
		}

		OmnString user_ctnr = mDoc->getAttrStr("user_ctnr");
		aos_assert_rr(user_ctnr != "", rdata, 0);
		AosXmlTagPtr userdoc = getUserDoc(user_cloudid, user_ctnr, rdata);
		if (!userdoc)
		{
			req.setErrCode(1);
			req.setErrmsg("UserID Incorrect");
			sendResponse(req, "", -1, rdata);
			return true;
		}

		OmnString doctor_id = request->xpathQuery("doctorId/_#text");
		if (doctor_id == "")
		{
			req.setErrCode(1);
			req.setErrmsg("Missing Doctor ID");
			sendResponse(req, "", -1, rdata);
			return true;
		}

		OmnString doctor_ctnr = mDoc->getAttrStr("doctor_ctnr");
		aos_assert_rr(doctor_ctnr != "", rdata, 0);
		AosXmlTagPtr doctor_doc = getUserDoc(doctor_id, doctor_ctnr, rdata);
		if (!doctor_doc)
		{
			req.setErrCode(1);
			req.setErrmsg("DoctorID Incorrect");
			sendResponse(req, "", -1, rdata);
			return true;
		}
		u64 doctor_docid = doctor_doc->getAttrU64(AOSTAG_DOCID, 0);
		if (doctor_docid == 0)
		{
			req.setErrCode(1);
			req.setErrmsg("Doctor Docid Incorrect");
			sendResponse(req, "", -1, rdata);
			return true;
		}

		// No session id yet. Need to create one and send a response back.
		ssid = getSsid();

		OmnString card_num  = userdoc->getAttrStr("zky_family_member_code"); 
		OmnString name      = userdoc->getAttrStr("zky_name");
		OmnString id_number = userdoc->getAttrStr("zky_id_number");
		OmnString sex       = userdoc->getAttrStr("zky_sex");

		UploadInfo info(user_cloudid, 
						doctor_id, 
						doctor_docid, 
						"",
						card_num,
						name,
						id_number,
						sex);
		mUpdHtryDataSMap[ssid] = info;

		OmnString response;
		response << "<sessionId>" << ssid << "</sessionId>";
		aos_assert_r(sendResponse(req, response, -1, rdata), false);
		return true;
	}

	// There is session ID. Need to check whether it is in the map.
	AosUpdHtryDataSMapItr itr = mUpdHtryDataSMap.find(ssid);
	if (itr == mUpdHtryDataSMap.end())
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_SessionNotFound);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_SessionNotFound << enderr; 
		return false;
	}
	
	rdata->setUserid(itr->second.doctor_docid);

	// if page equel -1, remove session from map.
	AosXmlTagPtr page = request->getFirstChild(AOSTAG_PAGE);
	if (!page)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	AosXmlTagPtr data_tag = request->getFirstChild("data");
	if (!data_tag)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	// Data is in ASCII Hex format. Need to convert 
	OmnString data = data_tag->getNodeText();
	if(data.length() > 0)
	{
		int strlen = DecodeBase64((unsigned char*)data.getBuffer(),
								  (unsigned char*)data.getBuffer(),
								  data.length());
		data.setLength(strlen);
	}
	itr->second.data << data;

	if (page->getNodeText() != "-1")
	{
		// It is not the last page. 
		if (itr->second.data.length() > eMaxDataLen)
		{
			req.setErrCode(1);
			req.setErrmsg(sgErrmgType_TooBigData);
			sendResponse(req, "", -1, rdata);
			OmnAlarm << sgErrmgType_TooBigData << enderr; 
			return false;
		}
		
		req.setErrCode(0);
		req.setErrmsg("");
		OmnString response;
		response << "<sessionId>" << ssid << "</sessionId>";
		aos_assert_r(sendResponse(req, response, -1, rdata), false);
		return true;
	}

	// This is the last page. 
	OmnString user_cloudid = itr->second.user_cloudid;
	OmnString card_num		= itr->second.card_num;
	OmnString name			= itr->second.name;
	OmnString id_number		= itr->second.id_number;
	OmnString sex			= itr->second.sex;

	OmnString doctor_id = itr->second.doctor_id;
	data = itr->second.data;

	mUpdHtryDataSMap.erase(ssid);

	// Received the last page.
	int itemcode = atoi(request->xpathQuery("itemcode/_#text").data());
	if (itemcode != eXindian && 
		itemcode != eXueya &&
		itemcode != eXuetang && 
		itemcode != eXueyang)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", -1, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}
	
	// Convert from base64 to binary
//	if(data.length() > 0)
//	{
//		int strlen = DecodeBase64((unsigned char*)data.getBuffer(),
//								  (unsigned char*)data.getBuffer(),
//								  data.length());
//		data.setLength(strlen);
//	}
	
	OmnString errmsg;
	int length = data.length();
	OmnString value;

	OmnString ch1; 
	OmnString ch2; 
	OmnString ch3; 
	OmnString ch4; 
	OmnString ch5; 
	OmnString ch6; 
	OmnString ch7; 
	OmnString ch8; 
	OmnString flags; 
	OmnString xueyang;

	ShengliInfo shengli;
	
	if (length > 0) 
	{
		u8 *data_mem = (u8*)data.data();

		int idx = 0;
		int prev_idx = -1;
		while (idx < length)
		{
			// Byte 1:		0x55
			// Byte 2:		0xAA
			// Byte 3:		Version
			// Byte 4:		ID
			// Byte 5-8:	Reserve
			// Byte 9-10:	Len
			// Byte 11-12:	Error Checking Codes
			// ...			Msg type dependent.
			
			aos_assert_r(prev_idx < idx, false);
			prev_idx = idx;

//			int v1 = AosStrUtil::singleHexToInt(data_mem + idx);
			if (data_mem[idx] != 0x55)
			{
				req.setErrCode(1);
				req.setErrmsg(sgErrmgType_DataIncorrect);
				sendResponse(req, "", -1, rdata);
				OmnAlarm << sgErrmgType_DataIncorrect << enderr; 
				return false;
			}
			idx ++;

//			int v2 = AosStrUtil::singleHexToInt(data_mem + idx+2);
			if (data_mem[idx] != 0xAA)
			{
				req.setErrCode(1);
				req.setErrmsg(sgErrmgType_DataIncorrect);
				sendResponse(req, "", -1, rdata);
				OmnAlarm << sgErrmgType_DataIncorrect << enderr; 
				return false;
			}
			idx ++;

			// jump version
			idx ++;
			
			int id = (int)(data_mem[idx]);
//			int id = AosStrUtil::singleHexToInt(data_mem + idx+6);
			idx += 9;
			switch (id)
			{
			case 0:
				 if (itemcode == eXindian)
				 {
				 	uploadCardioElect(req, 
							          ch1, 
									  ch2, 
					 				  ch3, 
									  ch4, 
									  ch5, 
									  ch6, 
									  ch7, 
									  ch8, 
									  flags, 
									  idx, 
									  data_mem, 
									  length);
				 }
				 else
				 {
					idx += eCardioElectSize;
				 }
				 break;

			case 2:
				 if (itemcode == eXueyang)
				 {
				 	uploadXueYang(xueyang, idx, data_mem, length);
				 }
				 else
				 {
					idx += eXueYangDataSize;
				 }
				 break;

			case 3:
				 if (itemcode == eXueya ||
					 itemcode == eXuetang ||
					 itemcode == eXueyang)
				 {
					 // Upload Shengli
				 	uploadShengli(shengli, idx, data_mem, length);
				 }
				 else
				 {
					 idx += eShengliDataSize;
				 }
				 break;

			case 4:
				 idx += eStatusDataSize; 	// 24
				 break;

			case 5:
				 idx += eEthernetDataSize;	// 32
				 break;

			case 6:
				 idx += eWifiDataSize;		// 104
				 break;

			default:
				 req.setErrCode(1);
				 req.setErrmsg(sgErrmgType_UndefinedUploadRequest);
				 sendResponse(req, "", -1, rdata);
				 OmnAlarm << sgErrmgType_UndefinedUploadRequest << enderr; 
				 return false;	 
			}
		}
	}
	
	switch(itemcode)
	{
		case eXindian:
			createDocForXindian(req, 
					            user_cloudid, 
								doctor_id, 
								card_num,
								name,
								id_number,
								sex,
								ch1, 
								ch2, 
								ch3, 
								ch4, 
								ch5, 
								ch6, 
								ch7, 
								ch8, 
								flags, 
								rdata);	
			break;
		case eXueya:
			createDocForXueya(req, 
							  user_cloudid, 
							  doctor_id, 
							  card_num,
							  name,
							  id_number,
							  sex,
							  shengli, 
							  rdata);
			break;
		case eXuetang:
			createDocForXuetang(req, 
								user_cloudid, 
								doctor_id, 
							    card_num,
							    name,
							    id_number,
							    sex,
								shengli, 
								rdata);
			break;
		case eXueyang:
			createDocForXueyang(req, 
								user_cloudid, 
								doctor_id, 
							    card_num,
							    name,
							    id_number,
							    sex,
								xueyang, 
								shengli, 
								rdata);
			break;
		default:
			req.setErrCode(1);
			req.setErrmsg(sgErrmgType_UndefinedUploadRequest);
			sendResponse(req, "", -1, rdata);
			OmnAlarm << sgErrmgType_UndefinedUploadRequest << enderr; 
			return false;	 
	}

	OmnString response;
	response << "<sessionId>" << ssid << "</sessionId>";
	aos_assert_r(sendResponse(req, response, -1, rdata), false);
	return true;
}


bool
AosGareaMsgProc::uploadCardioElect(
		AosSoapRequest &req, 
		OmnString &ch1,
		OmnString &ch2,
		OmnString &ch3,
		OmnString &ch4,
		OmnString &ch5,
		OmnString &ch6,
		OmnString &ch7,
		OmnString &ch8,
		OmnString &flags,
		int &idx, 
		const u8 *data, 
		const int data_len)
{
	// The format is:
	// 	ECG_CH1: 500 u16 (1000 bytes)
	// 	ECG_CH2: 500 u16 (1000 bytes)
	// 	ECG_CH3: 500 u16 (1000 bytes)
	// 	FLAGs:   500 u8
	//
	// 	<data>
	//		<ddd>11,11,11,11 ...</ddd> 
	//		<ddd>11,11,11,11 ...</ddd> 
	//		<ddd>11,11,11,11 ...</ddd> 
	//		<ddd>11,11,11,11 ...</ddd> 
	//	</data>
	const u8* data1 = data + idx;
	if (!data)
	{
		OmnAlarm << "data is null" << enderr;
		idx += eCardioElectSize;
		return false;
	}

	if (idx < 0 || idx + eCardioElectSize > data_len)
	{
		OmnAlarm << "Data too short: " << idx << ":" << data_len << enderr;
		idx += eCardioElectSize;
		return false;
	}
	
	// 1. Create ECG1 data
	for (int i=0; i<500; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
//		int vv = AosStrUtil::doubleHexToInt(data1 + i*4);
		if (ch1.length() > 0) ch1 << ",";
		ch1 << vv;
	}
	
	// 2. Create ECG2 data
	for (int i=500; i<1000; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch2.length() > 0) ch2 << ",";
		ch2 << vv;
	}
	
	// 3. Create ECG3 data
	for (int i=1000; i<1500; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch3.length() > 0) ch3 << ",";
		ch3 << vv;
	}
	
	// 4. Create ECG4 data
	for (int i=1500; i<2000; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch4.length() > 0) ch4 << ",";
		ch4 << vv;
	}
	
	// 5. Create ECG5 data
	for (int i=2000; i<2500; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch5.length() > 0) ch5 << ",";
		ch5 << vv;
	}
	
	// 6. Create ECG6 data
	for (int i=2500; i<3000; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch6.length() > 0) ch6 << ",";
		ch6 << vv;
	}
	
	// 7. Create ECG7 data
	for (int i=3000; i<3500; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch7.length() > 0) ch7 << ",";
		ch7 << vv;
	}
	
	// 3. Create ECG3 data
	for (int i=3500; i<4000; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 8);
		if(vv > mMax_xindian || vv < mMin_xindian) continue;
		if (ch8.length() > 0) ch8 << ",";
		ch8 << vv;
	}
	
	// 4. Create Flags
	for (int i=8000; i<8500; i++)
	{
		int vv = (data1[2*i]) + 
		         ((data1[2*i+1]) << 4);
		if (flags.length() > 0) flags << ",";
		flags << vv;
	}

	idx += eCardioElectSize;
	return true;
}


bool
AosGareaMsgProc::createDocForXueyangShengli(
		AosSoapRequest &req,
		const OmnString &user_cloudid,
		const OmnString &doctor_id,
		const OmnString &card_num,
		const OmnString &name,
		const OmnString &id_number,
		const OmnString &sex,
		const OmnString &xueyang,
		const ShengliInfo &shengli,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
		
	return true;
}

//	Here is the full format of garea doc below:
// 	<AppNode 
// 		zky_public_ctnr="true|false"
// 		AOSTAG_PRENTC="xxx"
// 		datatype="xindian,xueyang_wave,xueya,xueyang,xuetang,shengli"
// 		userid="xxx"
// 		doctor_id="xxx"
// 		device_id="xxx"
// 		AOSTAG_OTYPE="xxx"
// 		AOSTAG_STYPE="xxx">
//		<SW_LEAD_I__n>ddd,ddd,ddd</SW_LEAD_I__n>
//		<SW_LEAD_II__n>ddd,ddd,ddd</SW_LEAD_II__n>
//		<SW_LEAD_V1__n>ddd,ddd,ddd</SW_LEAD_V1__n>
//		<SW_LEAD_V2__n>ddd,ddd,ddd</SW_LEAD_V2__n>
//		<SW_LEAD_V3__n>ddd,ddd,ddd</SW_LEAD_V3__n>
//		<SW_LEAD_V4__n>ddd,ddd,ddd</SW_LEAD_V4__n>
//		<SW_LEAD_V5__n>ddd,ddd,ddd</SW_LEAD_V5__n>
//		<SW_LEAD_V6__n>ddd,ddd,ddd</SW_LEAD_V6__n>
//		<FLAG__n>ddd,ddd,</FLAG__n>
// 		<PLETH__n>ddd,dddd,</PLETH__n>
// 		<NIBP_SYS__n>ddd,ddd,...</NIBP_SYS__n>
// 		<NIBP_DIA__n>ddd,ddd,...</NIBP_DIA__n>
// 		<NIBP_MAP__n>ddd,ddd,...</NIBP_MAP__n>
// 		<SPO2__n>ddd,ddd,...</SPO2__n>
// 		<GLU__n>ddd,ddd,...</GLU__n>
//		<PHYSIOLOGY NIBP_SYS="last_value"
//					NIBP_DIA="last_value" 
//					NIBP_MAP="last_value"
//					SPO2="last_value"
//					GLU="last_value"/> 
//		<AVG NIBP_SYS="average"
//			 NIBP_DIA="average" 
//			 NIBP_MAP="average"
//			 SPO2="average"
//			 GLU="average"/> 
//	</AppNode>

bool
AosGareaMsgProc::createDocForXueya(
		AosSoapRequest &req,
		const OmnString &user_cloudid,
		const OmnString &doctor_id,
		const OmnString &card_num,
		const OmnString &name,
		const OmnString &id_number,
		const OmnString &sex,
		const ShengliInfo &shengli,
		const AosRundataPtr &rdata)
{
	// This doc format should be:
	// 	<AppNode 
	// 		zky_public_ctnr="true|false"
	// 		AOSTAG_PRENTC="xxx"
	// 		datatype="xueya"
	// 		userid="xxx"
	// 		doctor_id="xxx"
	// 		device_id="xxx"
	// 		AOSTAG_OTYPE="xxx"
	// 		AOSTAG_STYPE="xxx">
	// 		<NIBP_SYS__n>ddd,ddd,...</NIBP_SYS__n>
	// 		<NIBP_DIA__n>ddd,ddd,...</NIBP_DIA__n>
	// 		<NIBP_MAP__n>ddd,ddd,...</NIBP_MAP__n>
	//		<PHYSIOLOGY NIBP_SYS="last_value"
	//					NIBP_DIA="last_value" 
	//					NIBP_MAP="last_value"/> 
	//		<AVG NIBP_SYS="average"
	//			 NIBP_DIA="average" 
	//			 NIBP_MAP="average"/> 
	//	</AppNode>
	if (shengli.NIBP_SYS.size() <= 0) return true;
	
	u32 size = shengli.NIBP_SYS.size();

	// Create Shengli
	OmnString docstr = "<AppNode ";
	docstr << "zky_public_ctnr=\"true\" " 
		<< "zky_public_doc=\"true\" " 
		<< AOSTAG_PARENTC << "=\"" << mDoc->getAttrStr("data_container") 
		<< "\" datatype=\"xueya" 
		<< "\" userid=\"" << user_cloudid
		<< "\" zky_pctrs=\"garea_检测"
		<< "\" doctor_id=\"" << doctor_id 
		<< "\" zky_card_number=\"" << card_num 
		<< "\" zky_name=\"" << name 
		<< "\" zky_id_number=\"" << id_number 
		<< "\" zky_sex=\"" << sex 
		<< "\">";

	// Crfeate NIBP_SYS data
	aos_assert_r(shengli.NIBP_SYS.size() == size, false);
	docstr << "<NIBP_SYS__n id=\"NIBP_SYS\"><![CDATA[";
	int sys_average = 0;
	int sys_last = 0;
	int sys_counter = 0;
	for (u32 i=0; i<shengli.NIBP_SYS.size(); i++)
	{
		if (i != 0) docstr << ",";
		docstr << shengli.NIBP_SYS[i];
		sys_average += shengli.NIBP_SYS[i];
		sys_last = shengli.NIBP_SYS[i];
		sys_counter ++;
	}
	docstr << "]]></NIBP_SYS__n>";
	if(sys_counter == 0)
	{
		sys_average = 0;
	}
	else
	{
		sys_average = sys_average / sys_counter;
	}

	// Crfeate NIBP_DIA data
	docstr << "<NIBP_DIA__n id=\"NIBP_DIA\"><![CDATA[";
	aos_assert_r(shengli.NIBP_DIA.size() == size, false);
	int dia_average = 0;
	int dia_last = 0;
	int dia_counter = 0;
	for (u32 i=0; i<shengli.NIBP_DIA.size(); i++)
	{
		if (i != 0) docstr << ",";
		docstr << shengli.NIBP_DIA[i];
		dia_average += shengli.NIBP_DIA[i];
		dia_last = shengli.NIBP_DIA[i];
		dia_counter ++;
	}
	docstr << "]]></NIBP_DIA__n>";
	if(dia_counter == 0)
	{
		dia_average = 0;
	}
	else
	{
		dia_average = dia_average / dia_counter;
	}

	// Crfeate NIBP_MAP data
	docstr << "<NIBP_MAP__n id=\"NIBP_MAP\"><![CDATA[";
	aos_assert_r(shengli.NIBP_MAP.size() == size, false);
	int map_average = 0;
	int map_last = 0;
	int map_counter = 0;
	for (u32 i=0; i<shengli.NIBP_MAP.size(); i++)
	{
		if (i != 0) docstr << ",";
		docstr << shengli.NIBP_MAP[i];
		map_average += shengli.NIBP_MAP[i];
		map_last = shengli.NIBP_MAP[i];
		map_counter ++;
	}
	docstr << "]]></NIBP_MAP__n>";
	if(map_counter == 0)
	{
		map_average = 0;
	}
	else
	{
		map_average = map_average / map_counter;
	}

	docstr  << "<PHYSIOLOGY "
		    << "NIBP_SYS=\"" << sys_last << "\" "
			<< "NIBP_DIA=\"" << dia_last << "\" "
			<< "NIBP_MAP=\"" << map_last << "\"/> ";
			
	docstr  << "<AVG "
		    << "NIBP_SYS=\"" << sys_average << "\" "
			<< "NIBP_DIA=\"" << dia_average  << "\" "
			<< "NIBP_MAP=\"" << map_average << "\"/>";

	docstr << "</AppNode>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	bool rslt = AosCreateDoc(doc, rdata);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool
AosGareaMsgProc::createDocForXuetang(
		AosSoapRequest &req,
		const OmnString &user_cloudid,
		const OmnString &doctor_id,
		const OmnString &card_num,
		const OmnString &name,
		const OmnString &id_number,
		const OmnString &sex,
		const ShengliInfo &shengli,
		const AosRundataPtr &rdata)
{
	// This doc format should be:

	// 	<AppNode 
	// 		zky_public_ctnr="true|false"
	// 		AOSTAG_PRENTC="xxx"
	// 		datatype="xueyang"
	// 		userid="xxx"
	// 		doctor_id="xxx"
	// 		device_id="xxx"
	// 		AOSTAG_OTYPE="xxx"
	// 		AOSTAG_STYPE="xxx">
	// 		<GLU__n>ddd,ddd,...</GLU__n>
	//		<PHYSIOLOGY GLU="last_value"/> 
	//		<AVG GLU="average"/> 
	//	</AppNode>

	if (shengli.GLU.size() <= 0) return true;
	u32 size = shengli.GLU.size();

	// Create Shengli
	OmnString docstr = "<AppNode ";
	docstr << "zky_public_ctnr=\"true\" " 
		<< "zky_public_doc=\"true\" " 
		<< AOSTAG_PARENTC << "=\"" << mDoc->getAttrStr("data_container") 
		<< "\" datatype=\"xuetang" 
		<< "\" userid=\"" << user_cloudid
		<< "\" zky_pctrs=\"garea_检测"
		<< "\" doctor_id=\"" << doctor_id 
		<< "\" zky_card_number=\"" << card_num 
		<< "\" zky_name=\"" << name 
		<< "\" zky_id_number=\"" << id_number 
		<< "\" zky_sex=\"" << sex 
		<< "\">";

	// Crfeate GLU data
	docstr << "<GLU__n id=\"GLU\"><![CDATA[";
	aos_assert_r(shengli.GLU.size() == size, false);
	float glu_average = 0;
	float glu_last = 0;
	int glu_counter = 0;
	for (u32 i=0; i<shengli.GLU.size(); i++)
	{
		if (i != 0) docstr << ",";
		docstr << shengli.GLU[i];
		glu_average += shengli.GLU[i];
		glu_last = shengli.GLU[i];
		glu_counter ++;
	}
	docstr << "]]></GLU__n>";
	if(glu_counter == 0)
	{
		glu_average = 0;
	}
	else
	{
		glu_average = glu_average / glu_counter;
	}

	// Create the last data record
	docstr << "<PHYSIOLOGY "
		   << "GLU=\"" << glu_last << "\" "
		   << "/>";

	docstr << "<AVG "
		   << "GLU=\"" << glu_average << "\" "
		   << "/>";
	docstr << "</AppNode>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	bool rslt = AosCreateDoc(doc, rdata);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool
AosGareaMsgProc::createDocForXueyang(
		AosSoapRequest &req,
		const OmnString &user_cloudid,
		const OmnString &doctor_id,
		const OmnString &card_num,
		const OmnString &name,
		const OmnString &id_number,
		const OmnString &sex,
		const OmnString &xueyang,
		const ShengliInfo &shengli,
		const AosRundataPtr &rdata)
{
	// This doc format should be:
	// 	<AppNode 
	// 		zky_public_ctnr="true|false"
	// 		AOSTAG_PRENTC="xxx"
	// 		datatype="xueyang_wave,xueyang"
	// 		userid="xxx"
	// 		doctor_id="xxx"
	// 		device_id="xxx"
	// 		AOSTAG_OTYPE="xxx"
	// 		AOSTAG_STYPE="xxx">
	// 		<PLETH__n>ddd,dddd,</PLETH__n>
	// 		<SPO2__n>ddd,ddd,...</SPO2__n>
	//		<PHYSIOLOGY SPO2="last_value"/> 
	//		<AVG SPO2="average"/> 
	//	</AppNode>
	
	if (xueyang.length() <= 0 && shengli.SPO2.size() <= 0) return true;
		
	// 1. Build the doc header (AppNode header)
	OmnString docstr = "<AppNode ";
	docstr << "zky_public_ctnr=\"true\" " 
		<< "zky_public_doc=\"true\" " 
		<< AOSTAG_PARENTC << "=\"" << mDoc->getAttrStr("data_container") 
		<< "\" datatype=\"xueyang_wave,xueyang" 
		<< "\" userid=\"" << user_cloudid
		<< "\" doctor_id=\"" << doctor_id 
		<< "\" zky_pctrs=\"garea_检测"
		<< "\" zky_card_number=\"" << card_num 
		<< "\" zky_name=\"" << name 
		<< "\" zky_id_number=\"" << id_number 
		<< "\" zky_sex=\"" << sex 
		<< "\">";

	if(shengli.SPO2.size() > 0)
	{
		u32 size = shengli.SPO2.size();

		// Crfeate SPO2 data
		docstr << "<SPO2__n id=\"SPO2\"><![CDATA[";
		aos_assert_r(shengli.SPO2.size() == size, false);
		int spo2_average = 0;
		int spo2_last = 0;
		int spo2_counter = 0;
		for (u32 i=0; i<shengli.SPO2.size(); i++)
		{
			if (i != 0) docstr << ",";
			docstr << shengli.SPO2[i];
			spo2_average += shengli.SPO2[i];
			spo2_last = shengli.SPO2[i];
			spo2_counter ++;
		}
		docstr << "]]></SPO2__n>";
		if(spo2_counter == 0)
		{
			spo2_average = 0;
		}
		else
		{
			spo2_average = spo2_average / spo2_counter;
		}


		// Create the last data record
		docstr << "<PHYSIOLOGY "
			   << "SPO2=\"" << spo2_last << "\"/> ";
		docstr << "<AVG "
			   << "SPO2=\"" << spo2_average << "\"/> ";
	}
	
	if(xueyang.length() > 0)
	{
		docstr << "<PLETH__n><![CDATA["
			   << xueyang << "]]></PLETH__n>";
	}

	docstr << "</AppNode>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	bool rslt = AosCreateDoc(doc, rdata);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool
AosGareaMsgProc::createDocForXindian(
		AosSoapRequest &req,
		const OmnString &user_cloudid,
		const OmnString &doctor_id,
		const OmnString &card_num,
		const OmnString &name,
		const OmnString &id_number,
		const OmnString &sex,
		const OmnString &ch1,
		const OmnString &ch2,
		const OmnString &ch3,
		const OmnString &ch4,
		const OmnString &ch5,
		const OmnString &ch6,
		const OmnString &ch7,
		const OmnString &ch8,
		const OmnString &flags,
		const AosRundataPtr &rdata)
{
	// This function create doc for xindian.
	// 	<AppNode 
	// 		zky_public_ctnr="true|false"
	// 		AOSTAG_PRENTC="xxx"
	// 		datatype="xindian"
	// 		userid="xxx"
	// 		doctor_id="xxx"
	// 		device_id="xxx"
	// 		AOSTAG_OTYPE="xxx"
	// 		AOSTAG_STYPE="xxx">
	//		<SW_LEAD_I__n>ddd,ddd,ddd</SW_LEAD_I__n>
	//		<SW_LEAD_II__n>ddd,ddd,ddd</SW_LEAD_II__n>
	//		<SW_LEAD_V1__n>ddd,ddd,ddd</SW_LEAD_V1__n>
	//		<SW_LEAD_V2__n>ddd,ddd,ddd</SW_LEAD_V2__n>
	//		<SW_LEAD_V3__n>ddd,ddd,ddd</SW_LEAD_V3__n>
	//		<SW_LEAD_V4__n>ddd,ddd,ddd</SW_LEAD_V4__n>
	//		<SW_LEAD_V5__n>ddd,ddd,ddd</SW_LEAD_V5__n>
	//		<SW_LEAD_V6__n>ddd,ddd,ddd</SW_LEAD_V6__n>
	//		<FLAG__n>ddd,ddd,</FLAG__n>
	//	</AppNode>
	
	OmnString docstr = "<AppNode ";
	docstr << "zky_public_ctnr=\"true\" " 
		<< "zky_public_doc=\"true\" " 
		<< AOSTAG_PARENTC << "=\"" << mDoc->getAttrStr("data_container") 
		<< "\" datatype=\"xindian" 
		<< "\" zky_pctrs=\"garea_检测"
		<< "\" userid=\"" << user_cloudid
		<< "\" zky_card_number=\"" << card_num 
		<< "\" zky_name=\"" << name 
		<< "\" zky_id_number=\"" << id_number 
		<< "\" zky_sex=\"" << sex 
		<< "\" " << AOSTAG_SITEID << "=\"100"
		<< "\" doctor_id=\"" << doctor_id 
		<< "\">"
		<< "<SW_LEAD_I__n><![CDATA["
		<< ch1 << "]]></SW_LEAD_I__n>"
		<< "<SW_LEAD_II__n><![CDATA["
		<< ch2 << "]]></SW_LEAD_II__n>"
		<< "<SW_LEAD_V1__n><![CDATA["
		<< ch3 << "]]></SW_LEAD_V1__n>"
		<< "<SW_LEAD_V2__n><![CDATA["
		<< ch4 << "]]></SW_LEAD_V2__n>"
		<< "<SW_LEAD_V3__n><![CDATA["
		<< ch5 << "]]></SW_LEAD_V3__n>"
		<< "<SW_LEAD_V4__n><![CDATA["
		<< ch6 << "]]></SW_LEAD_V4__n>"
		<< "<SW_LEAD_V5__n><![CDATA["
		<< ch7 << "]]></SW_LEAD_V5__n>"
		<< "<SW_LEAD_V6__n><![CDATA["
		<< ch8 << "]]></SW_LEAD_V6__n>"
		<< "<FLAG__n><![CDATA["
		<< flags << "]]></FLAG__n>"
		<< "</AppNode>";

	// AosXmlTagPtr sdoc = mDoc->getFirstChild(AOSTAG_XINDIAN_SDOC);
	// aos_assert_r(sdoc, false);
	
	// AosActionObjPtr actobj = AosActionObj::getActionObj();
	// aos_assert_r(actobj, false);
	//	
	// aos_assert_r(actobj->runActions(sdoc, rdata), false);

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	bool rslt = AosCreateDoc(doc, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosGareaMsgProc::uploadRtimeData(
		AosSoapRequest &req,
		const AosXmlTagPtr &body, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr request = body->getFirstChild("request");
	if (!request)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", eInterval, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	// 1. Retrieve the session ID
	AosXmlTagPtr ssid_tag = request->getFirstChild("sessionId");
	OmnString ssid = request->xpathQuery("sessionId/_#text");;
	if (ssid == "")
	{
		// 1. Retrieve 'userId'
		OmnString user_cloudid = request->xpathQuery("userId/_#text");
		if (user_cloudid == "")
		{
			req.setErrCode(1);
			req.setErrmsg("Missing UserID");
			sendResponse(req, "", eInterval, rdata);
			return true;
		}

		OmnString user_ctnr = mDoc->getAttrStr("user_ctnr");
		aos_assert_rr(user_ctnr != "", rdata, 0);
		AosXmlTagPtr userdoc = getUserDoc(user_cloudid, user_ctnr, rdata);
		if (!userdoc)
		{
			req.setErrCode(1);
			req.setErrmsg("UserID Incorrect");
			sendResponse(req, "", eInterval, rdata);
			return true;
		}

		OmnString doctor_id = request->xpathQuery("doctorId/_#text");
		if (doctor_id == "")
		{
			req.setErrCode(1);
			req.setErrmsg("Missing Doctor ID");
			sendResponse(req, "", eInterval, rdata);
			return true;
		}

		OmnString doctor_ctnr = mDoc->getAttrStr("doctor_ctnr");
		aos_assert_rr(doctor_ctnr != "", rdata, 0);
		AosXmlTagPtr doctor_doc = getUserDoc(doctor_id, doctor_ctnr, rdata);
		if (!doctor_doc)
		{
			req.setErrCode(1);
			req.setErrmsg("DoctorID Incorrect");
			sendResponse(req, "", eInterval, rdata);
			return true;
		}
		
		u64 doctor_docid = doctor_doc->getAttrU64(AOSTAG_DOCID, 0);
		if (doctor_docid == 0)
		{
			req.setErrCode(1);
			req.setErrmsg("Doctor Docid Incorrect");
			sendResponse(req, "", eInterval, rdata);
			return true;
		}

		// No session id yet. Need to create one and send a response back.
		ssid = getSsid();
		
		OmnString card_num  = userdoc->getAttrStr("zky_family_member_code"); 
		OmnString name      = userdoc->getAttrStr("zky_name");
		OmnString id_number = userdoc->getAttrStr("zky_id_number");
		OmnString sex       = userdoc->getAttrStr("zky_sex");
		
		UploadInfo info(user_cloudid, 
						doctor_id, 
						doctor_docid, 
						"",
						card_num,
						name,
						id_number,
						sex);

		//mUpdHtryDataSMap[ssid] = info;
		mUpdRtDataSMap[ssid] = info;
		//AosUpdRtDataSMapItr itr = mUpdRtDataSMap.find(ssid);

		OmnString response;
		response << "<sessionId>" << ssid << "</sessionId>";
		aos_assert_r(sendResponse(req, response, eInterval, rdata), false);
		return true;
	}

	// There is session ID. Need to check whether it is in the map.
	AosUpdRtDataSMapItr itr = mUpdRtDataSMap.find(ssid);
	if (itr == mUpdRtDataSMap.end())
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_SessionNotFound);
		sendResponse(req, "", eInterval, rdata);
		OmnAlarm << sgErrmgType_SessionNotFound << enderr; 
		return false;
	}

	rdata->setUserid(itr->second.doctor_docid);
	
	// if page equel -1, remove session from map.
	AosXmlTagPtr page = request->getFirstChild(AOSTAG_PAGE);
	if (!page)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", eInterval, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	AosXmlTagPtr data_tag = request->getFirstChild("data");
	if (!data_tag)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", eInterval, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	// Data is in ASCII Hex format. Need to convert 
	OmnString data = data_tag->getNodeText();
	if(data.length() > 0)
	{
		int strlen = DecodeBase64((unsigned char*)data.getBuffer(),
								  (unsigned char*)data.getBuffer(),
								  data.length());
		data.setLength(strlen);
	}

	itr->second.data << data;

	if (page->getNodeText() != "-1")
	{
		// It is not the last page. 
		if (itr->second.data.length() > eMaxDataLen)
		{
			req.setErrCode(1);
			req.setErrmsg(sgErrmgType_TooBigData);
			sendResponse(req, "", eInterval, rdata);
			OmnAlarm << sgErrmgType_TooBigData << enderr; 
			return false;
		}
		
		req.setErrCode(0);
		req.setErrmsg("");
		OmnString response;
		response << "<sessionId>" << ssid << "</sessionId>";
		aos_assert_r(sendResponse(req, response, eInterval, rdata), false);
		return true;
	}

	OmnString user_cloudid  = itr->second.user_cloudid;
	OmnString card_num		= itr->second.card_num;
	OmnString name			= itr->second.name;
	OmnString id_number		= itr->second.id_number;
	OmnString sex			= itr->second.sex;

	OmnString doctor_id = itr->second.doctor_id;
	data = itr->second.data;

	mUpdRtDataSMap.erase(ssid);

	// Received the last page.
	int itemcode = atoi(request->xpathQuery("itemcode/_#text").data());

	if (itemcode != eXindian && itemcode != eXueya)
	{
		req.setErrCode(1);
		req.setErrmsg(sgErrmgType_InvalidRequest);
		sendResponse(req, "", eInterval, rdata);
		OmnAlarm << sgErrmgType_InvalidRequest << enderr; 
		return false;
	}

	// Convert from base64 to binary
//	if(data.length() > 0)
//	{
//		int strlen = DecodeBase64((unsigned char*)data.getBuffer(),
//				                  (unsigned char*)data.getBuffer(),
//								  data.length());
//		data.setLength(strlen);
//	}
	
	OmnString errmsg;
	int length = data.length();
	OmnString value;

	OmnString ch1; 
	OmnString ch2; 
	OmnString ch3; 
	OmnString ch4; 
	OmnString ch5; 
	OmnString ch6; 
	OmnString ch7; 
	OmnString ch8; 
	OmnString flags; 
	OmnString xueyang;

	ShengliInfo shengli;
	
	if (length > 0) 
	{
		u8 *data_mem = (u8*)data.data();

		int idx = 0;
		int prev_idx = -1;
		while (idx < length)
		{
			// Byte 1:		0x55
			// Byte 2:		0xAA
			// Byte 3:		Version
			// Byte 4:		ID
			// Byte 5-8:	Reserve
			// Byte 9-10:	Len
			// Byte 11-12:	Error Checking Codes
			// ...			Msg type dependent.
			
			aos_assert_r(prev_idx < idx, false);
			prev_idx = idx;

//			int v1 = AosStrUtil::singleHexToInt(data_mem + idx);
			if (((unsigned char*)data_mem)[idx] != 0x55)
			{
				req.setErrCode(1);
				req.setErrmsg(sgErrmgType_DataIncorrect);
				sendResponse(req, "", eInterval, rdata);
				OmnAlarm << sgErrmgType_DataIncorrect << enderr; 
				return false;
			}
			idx ++;

//			int v2 = AosStrUtil::singleHexToInt(data_mem + idx+2);
			if (((unsigned char*)data_mem)[idx] != 0xAA)
			{
				req.setErrCode(1);
				req.setErrmsg(sgErrmgType_DataIncorrect);
				sendResponse(req, "", eInterval, rdata);
				OmnAlarm << sgErrmgType_DataIncorrect << enderr; 
				return false;
			}
			idx ++;

			// Version
			idx ++;
			
			// ID
//			int id = AosStrUtil::singleHexToInt(data_mem + idx+6);
			int id = (int)(data_mem[idx]);
			idx += 9;
			switch (id)
			{
			case 0:
				 if (itemcode == eXindian)
				 {
				 	uploadCardioElect(req, 
							          ch1, 
									  ch2, 
									  ch3, 
									  ch4, 
									  ch5, 
									  ch6, 
									  ch7, 
									  ch8, 
									  flags, 
									  idx, 
									  data_mem, 
									  length);
				 }
				 else
				 {
					idx += eCardioElectSize;
				 }
				 break;

			case 2:
				 if (itemcode == eXueyang)
				 {
				 	uploadXueYang(xueyang, idx, data_mem, length);
				 }
				 else
				 {
					idx += eXueYangDataSize;
				 }
				 break;

			case 3:
				 if (itemcode == eXueya ||
					 itemcode == eXuetang ||
					 itemcode == eXueyang)
				 {
					 // Upload Shengli
				 	uploadShengli(shengli, idx, data_mem, length);
				 }
				 else
				 {
					 idx += eShengliDataSize;
				 }
				 break;

			case 4:
				 idx += eStatusDataSize; 	// 24
				 break;

			case 5:
				 idx += eEthernetDataSize;	// 32
				 break;

			case 6:
				 idx += eWifiDataSize;		// 104
				 break;

			default:
				 req.setErrCode(1);
				 req.setErrmsg(sgErrmgType_UndefinedUploadRequest);
				 sendResponse(req, "", eInterval, rdata);
				 OmnAlarm << sgErrmgType_UndefinedUploadRequest << enderr; 
				 return false;	 
			}
		}
	}
	
	switch(itemcode)
	{
		case eXindian:
			createDocForXindian(req, 
								user_cloudid, 
								doctor_id, 
								card_num,
								name,
								id_number,
								sex,
								ch1, 
								ch2, 
								ch3, 
								ch4, 
								ch5, 
								ch6, 
								ch7, 
								ch8, 
								flags, 
								rdata);	
			break;
		case eXueya:
			createDocForXueya(req, 
							  user_cloudid, 
							  doctor_id, 
							  card_num,
							  name,
							  id_number,
							  sex,
							  shengli, 
							  rdata);
			break;
		case eXuetang:
			createDocForXuetang(req, 
								user_cloudid, 
								doctor_id, 
								card_num,
								name,
								id_number,
								sex,
								shengli, 
								rdata);
			break;
		case eXueyang:
			createDocForXueyang(req, 
								user_cloudid, 
								doctor_id, 
								card_num,
								name,
								id_number,
								sex,
								xueyang, 
								shengli, 
								rdata);
			break;
		default:
			req.setErrCode(1);
			req.setErrmsg(sgErrmgType_UndefinedUploadRequest);
			sendResponse(req, "", eInterval, rdata);
			OmnAlarm << sgErrmgType_UndefinedUploadRequest << enderr; 
			return false;	 
	}

	OmnString response;
	response << "<sessionId>" << ssid << "</sessionId>";
	aos_assert_r(sendResponse(req, response, eInterval, rdata), false);
	
	return true;
}


OmnString 
AosGareaMsgProc::getTime()
{
	ptime p6 = second_clock::local_time();
	ostringstream os2;
	time_facet *facet2 = OmnNew time_facet("%Y/%m/%d %H:%M:%S");
	os2.imbue(locale(cout.getloc(), facet2));
	os2 << p6;
	return OmnString(os2.str());
}


u32
AosGareaMsgProc::timeStr2Int(const OmnString &timestr)
{
	// The time string should be this format:
	// xxxx/xx/xx xx:xx:xx
	aos_assert_r(timestr != "", 0);
	OmnString part1[3];
	OmnString part2[4];
	OmnString part3[4];

	AosStrSplit split;
	int num = split.splitStr(timestr.data(), " ", part1, 3);
	aos_assert_r(num == 2, 0);

	num = split.splitStr(part1[0].data(), "/", part2, 4);
	aos_assert_r(num == 3, 0);
	
	num = split.splitStr(part1[1].data(), "/", part3, 4);
	aos_assert_r(num == 3, 0);

	OmnString errmsg;
	u32 second = AosGetEpoch(part1[0].data(), part1[1].data(), part1[2].data(), part2[0].data(), part2[1].data(), part2[3].data(), errmsg);
	if (errmsg != "")
	{
		OmnAlarm << "parse timestr error"  << enderr;
		return 0;
	}
	return second;
}


bool
AosGareaMsgProc::parseCmd(
		AosSoapRequest &req, 
		const AosXmlTagPtr &header, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(header, false);
	
	AosXmlTagPtr version = header->getFirstChild(AOSTAG_VERSION_R);
	if (!version)
	{
		rdata->setError() << "Missing version";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	req.setVersion(version->getNodeText());

	AosXmlTagPtr deviceId = header->getFirstChild(AOSTAG_DEVICEID);
	if (!deviceId)
	{
		rdata->setError() << "Missing deviceId";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	req.setDvcId(deviceId->getNodeText());
	

	AosXmlTagPtr cmd = header->getFirstChild(AOSTAG_COMMAND);
	if (!cmd)
	{
		rdata->setError() << "Missing command";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	req.setCmd(req.toCmd(cmd->getNodeText()));

	req.setErrCode(0);
	req.setErrmsg("");
	return true;
}


OmnString
AosGareaMsgProc::getSsid()
{
	OmnString ssid;
	ssid << ++ sgSessionId;
	return ssid;
}


AosXmlTagPtr
AosGareaMsgProc::getUserDoc(
		const OmnString &user_cloudid, 
		const OmnString &account_ctnr, 
		const AosRundataPtr &rdata)
{
	AosQueryRsltObjPtr query_rslt = 0;
	OmnString objids[eMaxConds];
	OmnString attrs[eMaxConds];
	AosValueRslt valueRslts[eMaxConds];
	AosOpr oprs[eMaxConds];
	bool reverses[eMaxConds];
	bool orders[eMaxConds];
	
	// Set the condition:
	// 		AOSTAG_PARENTC = "garea_account"
	objids[0] = "";
	attrs[0] = AOSTAG_PARENTC; 
	
	valueRslts[0].setStr(account_ctnr);
	oprs[0] = eAosOpr_eq;
	reverses[0] = false;
	orders[0] = true;
	
	// Set the condition
	// 		zky_cloudid__a = user_cloudid
	objids[1] = ""; 
	attrs[1] = AOSTAG_CLOUDID;
	valueRslts[1].setStr(user_cloudid);
	oprs[1] = eAosOpr_eq;
	reverses[1] = false;
	orders[1] = false;

	bool rslt = AosQuery::getSelf()->runQuery(0, 10, objids, attrs, 
					valueRslts, oprs, reverses, orders, 2, query_rslt, rdata);  
	if (!rslt)
	{
		OmnAlarm << "Doc not found!" << enderr;
		return 0;
	}

	u64 docid = 0;
	bool finished;
	aos_assert_r(query_rslt,0);
	query_rslt->reset();
	query_rslt->nextDocid(docid, finished, rdata);
	if (!docid)
	{
		OmnAlarm << "Doc not found!" << enderr;
		return 0;
	}

	return AosGetDocByDocid(docid, rdata);
}


bool
AosGareaMsgProc::uploadXueYang(
		OmnString &xueyang, 
		int &idx, 
		const u8 *data_mem, 
		const int data_len)
{
	const u8* data1 = data_mem + idx;
	
	if (!data_mem)
	{
		OmnAlarm << "data is null" << enderr;
		idx += eXueYangDataSize;
		return false;
	}

	if (idx < 0 || idx + eXueYangDataSize > data_len)
	{
		OmnAlarm << "Data too short: " << idx << ":" << data_len << enderr;
		idx += eXueYangDataSize;
		return false;
	}
	
	// 1. Create data
	for (int i=0; i<60; i++)
	{
		//int vv = AosStrUtil::singleHexToInt(data1 + i*2);
		int vv = (int)(data1[i]);
		if(vv > mMax_xueyangwave || vv < mMin_xueyangwave) continue;

		if (xueyang.length() > 0) xueyang << ",";
		xueyang << vv;
	}
	// xueyang.assign(data1, eXueYangDataSize);
	idx += eXueYangDataSize;
	return true;
}


bool
AosGareaMsgProc::uploadShengli(
		ShengliInfo &shengli, 
		int &idx, 
		const u8 *data_mem, 
		const int data_len)
{
	const u8* data1 = data_mem + idx;
	if (!data_mem)
	{
		OmnAlarm << "data is null" << enderr;
		idx += eShengliDataSize;
		return false;
	}

	if (idx < 0 || idx + eShengliDataSize > data_len)
	{
		OmnAlarm << "Data too short: " << idx << ":" << data_len << enderr;
		idx += eShengliDataSize;
		return false;
	}
	
	// 1. Create data
//	int hr = AosStrUtil::doubleHexToInt(data1);
//	int spo2 = AosStrUtil::singleHexToInt(data1+4);
//	int res1 = AosStrUtil::singleHexToInt(data1+6);
//	int pr = AosStrUtil::doubleHexToInt(data1+8);
//	int res2 = AosStrUtil::doubleHexToInt(data1 + 12);
//	int nibp_sys = AosStrUtil::doubleHexToInt(data1+16);
//	int nibp_dia = AosStrUtil::doubleHexToInt(data1+20);
//	int nibp_map = AosStrUtil::doubleHexToInt(data1+24);
//	int res3 = AosStrUtil::doubleHexToInt(data1+28);
//	float glu = AosStrUtil::doubleHexToInt(data1+32) + 
//				(AosStrUtil::doubleHexToInt(data1+36)<<16);

	int hr = (data1[0]) + ((data1[1]) << 8);
	int spo2 = data1[2];
	int res1 = data1[3];
	int pr = (data1[4]) + ((data1[5]) << 8);
	int res2 = (data1[6]) + ((data1[7]) << 8);
	int nibp_sys = (data1[8]) + ((data1[9]) << 8);
	int nibp_dia = (data1[10]) + ((data1[11]) << 8);
	int nibp_map = (data1[12]) + ((data1[13]) << 8);
	int res3 = (data1[14]) + ((data1[15]) << 8);
	float glu = 0;
	if(sizeof(float)!= 4)
	{
   	  	OmnAlarm << "float size error" << enderr;
	}
//	memcpy(glu, &(data1[16]) , sizeof(float));
	memcpy(&glu, &(data1[16]) , 4);

	shengli.HR.push_back(hr);
	
	if(spo2 <= mMax_spo2 && spo2 >= mMin_spo2)	shengli.SPO2.push_back(spo2);
	shengli.PR.push_back(pr);
	if(nibp_sys <= mMax_nibp_sys && nibp_sys >= mMin_nibp_sys)  shengli.NIBP_SYS.push_back(nibp_sys);
	if(nibp_dia <= mMax_nibp_dia && nibp_dia >= mMin_nibp_dia)  shengli.NIBP_DIA.push_back(nibp_dia);
	if(nibp_map <= mMax_nibp_map && nibp_map >= mMin_nibp_map)  shengli.NIBP_MAP.push_back(nibp_map);
	if(glu <= mMax_glu && glu >= mMin_glu)  shengli.GLU.push_back(glu);
	shengli.RES1.push_back(res1);
	shengli.RES2.push_back(res2);
	shengli.RES3.push_back(res3);
	
	
	// The process of RES4 may not work now
	char cur_char;
	for(int i = 20;i < 40;i+=2)
	{ 
		cur_char = data1[i];
		if(i > 20) shengli.RES4 << ","; 
		shengli.RES4 << (int)cur_char;
	}
	idx += eShengliDataSize;
	return true;
}

