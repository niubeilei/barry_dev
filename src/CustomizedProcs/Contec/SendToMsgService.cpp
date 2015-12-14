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
// The torturer is in SengTorturer/TesterSendToMsgServiceNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "CustomizedProcs/Contec/SendToMsgService.h"
#include "CustomizedProcs/Contec/Ptrs.h"

#include "Actions/ActSeqno.h"
#include "DocClient/DocClient.h"
#include "SearchEngine/DocServerCb.h"
#include "EventMgr/EventMgr.h"
//#include "IILClient/IILClient.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/ImgProc.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "MsgService/MsgService.h"
#include "API/AosApiD.h"
#ifdef __amd64__
#include "SeReqProc/Lib.h"
#else
#include "SeReqProc/Lib32.h"
#endif
//#include "SeReqProc/Unpack8000w.h"
#include "API/AosApiC.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/QueryReqObj.h"
#include "Query/QueryReq.h"
#include "QueryRslt/QueryRslt.h"


AosSendToMsgService::AosSendToMsgService(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SENDTOMSGSERVICE, 
		AosSeReqid::eSendToMsgService, rflag)
{
}


bool 
AosSendToMsgService::proc(const AosRundataPtr &rdata)
{
	// 'root' should be in the form:
	// 	<request>
	// 		<command>
	// 			<cmd opr="sendtomsgservice" subopr="batch|rebatch" type="ioi" garbage="xxx"/>
	// 		</command>
	// 		<objdef>
	// 			<embedobj " +useinfo + " zky_pctrs=\"sh_bmd2\" zky_public_ctnr=\"true\" zky_otype=\"bmd\" 
	// 			 fname=\"a.png\" zky_imgdir=\"" + this.uploadPath1 +  "\" >
	// 			 	<BMD " + datafield + "/>
	// 			 	<imgs>
	// 			 		<img>this.uploadPath1</img>
	// 			 		<img>this.uploadPath2</img>
	// 			 	</imgs>
	// 			 </embedobj>
	// 			 ...
	// 			<embedobj " +useinfo + " zky_pctrs=\"sh_bmd2\" zky_public_ctnr=\"true\" zky_otype=\"bmd\" 
	// 			 fname=\"a.png\" zky_imgdir=\"" + this.uploadPath1 +  "\" >
	// 			 	<BMD " + datafield + "/>
	// 			 	<imgs>
	// 			 		<img>this.uploadPath1</img>
	// 			 		<img>this.uploadPath2</img>
	// 			 	</imgs>
	// 			 </embedobj>
	// 		</objdef>
	// 		<item name="siteid">100</item>
	// 		<item name="operation">serverCmd</item>
	// 		<item name="trans_id">10000</item>
	// 		<item name="urldocid">313</item>
	// 		<item name="img2address">this.uploadPath2</item>
	// 		<item name="userid">10011270</item>
	// 		<item name="zky_ssid_313">sessionid123</item>
	// 	</request>
	//
	//1.check the person information has the neccessary attribute
	//2.send message to msgservice
	//
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing root";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr request = root->getFirstChild();
	if (!request)
	{
		rdata->setError() << "Missing request!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (AosSeReqProc::getIsStopping())
	{
		rdata->setError() << "Server is stopping";
		AOSLOG_LEAVE(rdata);
		return true;
	}
	

	AosXmlTagPtr objdef = request->getFirstChild("objdef");
	OmnString errmsg;
	if (!objdef)
	{
		errmsg = "Missing the object to be created !";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr imgdoc = objdef->getFirstChild();
	if (!imgdoc)
	{
		errmsg = "Missing the object to be created(1)";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr cmd = request->getFirstChild("command");
	if (cmd)
	{
		cmd = cmd->getFirstChild();
	}
	else
	{
		errmsg = "Missing the command!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}


	//proc batch bmd | prod one bmd
	OmnString batch = cmd->getAttrStr("subopr", "");
	OmnString type = cmd->getAttrStr("type", "");
	if(batch == "batch")
	{
		OmnString ToS = "bmd";
		OmnString method = "batch";
		bool r = procBatch(request, objdef, ToS, method, rdata);
		aos_assert_r(r, false);
		OmnScreen << "---------------- save batch bmd success -------------"  << endl;
		AOSLOG_LEAVE(rdata);
		rdata->setOk();
		return true;
	}
	else if(batch == "rebatch")
	{
		OmnString ToS = "bmd";
		OmnString method = "rebatch";
		OmnString garbageid = cmd->getAttrStr("garbage", "");
		aos_assert_r(garbageid != "",  false);
		AosXmlTagPtr garbage = getGarbage(garbageid, rdata); 
		bool r = procBatch(request, garbage, ToS, method, rdata);
		aos_assert_r(r, false);
		OmnScreen << "---------------- save batch bmd success -------------"  << endl;
		AOSLOG_LEAVE(rdata);
		rdata->setOk();
		return true;
	}	
	else if(batch == "ioibatch")
	{
		OmnString ToS = "ioi";
		OmnString method = "batch";
		bool r = procBatch(request, objdef, ToS, method, rdata);
		aos_assert_r(r, false);
		OmnScreen << "---------------- save batch ioi success -------------"  << endl;
		AOSLOG_LEAVE(rdata);
		rdata->setOk();
		return true;
	}
	else if(batch == "ioirebatch")
	{
		OmnString ToS = "ioi";
		OmnString method = "rebatch";
		OmnString garbageid = cmd->getAttrStr("garbage", "");
		aos_assert_r(garbageid != "",  false);
		AosXmlTagPtr garbage = getGarbage(garbageid, rdata); 
		bool r = procBatch(request, garbage, ToS, method, rdata);
		aos_assert_r(r, false);
		OmnScreen << "---------------- save batch ioi success -------------"  << endl;
		AOSLOG_LEAVE(rdata);
		rdata->setOk();
		return true;
	}	
	else if(batch == "ioisingle")
	{
		aos_assert_r(procSingleIOI(root, rdata), false);
		AOSLOG_LEAVE(rdata);
		rdata->setOk();
		OmnScreen << "---------------- save ioi success! -------------"  << endl;
		return true;
	}

	OmnString cloudid = root->getChildTextByAttr("name", "userid");
	if (cloudid == "") 
	{
		errmsg = "Missing the cloudid!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}


	//get account
	AosXmlTagPtr usr_doc = AosDocClient::getSelf()->getDocByCloudid(cloudid, rdata);
	aos_assert_rr(usr_doc, rdata, false);
	usr_doc = usr_doc->clone(AosMemoryCheckerArgsBegin);
	
	OmnString zky_bmd_yuyue = usr_doc->getAttrStr("zky_bmd_yuyue", "");
	OmnString zky_bmd_yuyue_id = usr_doc->getAttrStr("zky_bmd_yuyue_id", "");
	OmnString userid;
	if(zky_bmd_yuyue == "old" || zky_bmd_yuyue_id == "")
	{
		OmnString zky_monitor_name = usr_doc->getAttrStr("zky_monitor_name", "");
		aos_assert_rr(zky_monitor_name!="", rdata, false);

		OmnString objids[2];
		OmnString attrs[2];
		OmnString values[2];
		AosOpr oprs[2];
		bool reverses[2];
		bool orders[2];
		AosValueRslt valueRslts[2];	
		
		objids[0] = "";
		attrs[0] = "zky_pctrs";
		values[0] = "sh_account_cs";
		valueRslts[0].setValue(values[0]);
		oprs[0] = eAosOpr_eq;
		reverses[0] = false;
		orders[0] = false;

		objids[1] = "";
		attrs[1] = "zky_monitor_name";
		values[1] = zky_monitor_name;
		valueRslts[1].setValue(values[1]);
		oprs[1] = eAosOpr_eq;
		reverses[1] = false;
		orders[1] = false;

		AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		aos_assert_r(queryRslt, false);

		bool rslt = AosDbRunQuery(
			-10,1,
			objids,attrs, valueRslts, oprs, reverses, 
			orders, 2, queryRslt, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eQueryFailed);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		u64 docid;
		OmnString cid;
		while(queryRslt->nextDocid(docid, rslt, rdata))
		{
			if(rslt)
			{
				break;
			}
			if (docid != 0)
			{
				AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
				if (!doc)
				{
					rdata->setError() << "Inter Error";
					OmnAlarm << rdata->getErrmsg() << enderr;
				}
				int status;
				cid = doc->getAttrStr(AOSTAG_CLOUDID, "");
				aos_assert_rr(cid != "", rdata, false);
				AosMsgService::getSelf()->getUserOnlineStatus(status, cid, rdata);
				if(status)
				{
					break;
				}
			}
		}
		userid << cid;
		//send msg to msgserver
		sendFailedMsg(userid, "bmd", rdata);
		rdata->setError() << "zky_bmd_yuyue: " << zky_bmd_yuyue << ", zky_bmd_yuyue_id: " << zky_bmd_yuyue_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		OmnScreen << "---------------- failed to save bmd  -------------"  << endl;
		return true; 
	}

	userid << cloudid;

	//save img doc
	AosImgProcPtr mImgProc = AosSeReqProc::getImgProc();
	if (!mImgProc) mImgProc = OmnNew AosImgProc();
	OmnString img_rslt;
	//add attributes
	addAttributes(imgdoc, usr_doc, "", rdata);
	
	//upload image1
	bool rslt = mImgProc->uploadImage(rdata, imgdoc, cmd, img_rslt);
	if(!rslt) 
	{
		rdata->setError() << "Failed to save image doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	AosXmlTagPtr img_tag  = AosXmlParser::parse(img_rslt AosMemoryCheckerArgs);
	aos_assert_rr(img_tag, rdata, false);
	OmnString img_objid = img_tag->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_rr(img_objid != "", rdata, false);

	OmnString img2 = root->getChildTextByAttr("name", "img2address");
	OmnString img2_tnail;
	if (img2 != "") 
	{
		//upload image2
		imgdoc->setAttr("zky_imgdir", img2);
		imgdoc->setAttr("zky_public_ctnr", "true");
		imgdoc->setAttr("zky_pctrs", "sh_bmd3");
		OmnString img_rslt2;
		bool r = mImgProc->uploadImage(rdata, imgdoc, cmd, img_rslt2);
		if(!r) 
		{
			rdata->setError() << "Failed to save image2 doc!";
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
		AosXmlTagPtr img_tag2  = AosXmlParser::parse(img_rslt2 AosMemoryCheckerArgs);
		aos_assert_rr(img_tag2, rdata, false);
		OmnString img_objid2 = img_tag2->getAttrStr(AOSTAG_OBJID, "");
		AosXmlTagPtr doc2 = AosDocClient::getSelf()->getDocByObjid(img_objid2, rdata);
		aos_assert_rr(doc2, rdata, false);
		img2_tnail = doc2->getAttrStr("zky_tnail", "");
		aos_assert_rr(img2_tnail != "", rdata, false);
		AosXmlTagPtr doc1 = AosDocClient::getSelf()->getDocByObjid(img_objid, rdata);
		aos_assert_rr(doc1, rdata, false);
		doc1 = doc1->clone(AosMemoryCheckerArgsBegin);
		doc1->setAttr("zky_tnail2", img2_tnail);
		bool mimg = AosDocClient::getSelf()->modifyObj(rdata, doc1, "", false);
		if (!mimg)
		{
			rdata->setError() << "modify image doc error";
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
	}


	//save attributes to zky_bmd_yuyue_id object
	AosXmlTagPtr yy = AosDocClient::getSelf()->getDocByObjid(zky_bmd_yuyue_id, rdata);
	yy = yy->clone(AosMemoryCheckerArgsBegin);
	aos_assert_rr(yy, rdata, false);
	OmnString zky_service_id = yy->getAttrStr("zky_service_id", "");
	aos_assert_rr(zky_service_id!="", rdata, false);
	yy->setAttr("zky_bmd_id", img_objid);
	yy->setAttr("zky_status", "诊断完成");
	bool mrslt2 = AosDocClient::getSelf()->modifyObj(rdata, yy, "", false);
	if (!mrslt2)
	{
		rdata->setError() << "modify error";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	//modify user account
	usr_doc->setAttr("zky_bmd_yuyue", "old");
	usr_doc->removeAttr("zky_bmd_yuyue_id");
	bool mrslt = AosDocClient::getSelf()->modifyObj(rdata, usr_doc, "", false);
	if (!mrslt)
	{
		rdata->setError() << "modify error";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}


	//send msg to msgserver

	sendSuccessMsg(userid, zky_bmd_yuyue_id, "zky_bmd_objid", img_objid, "bmd", rdata);
	sendSuccessMsg(zky_service_id, zky_bmd_yuyue_id, "zky_bmd_objid", img_objid, "bmd", rdata);

	AOSLOG_LEAVE(rdata);
	rdata->setOk();
	OmnScreen << "---------------- save bmd success! -------------"  << endl;
	return true;
}

bool 
AosSendToMsgService::procSingleIOI(AosXmlTagPtr &root, const AosRundataPtr &rdata)
{
	AosXmlTagPtr request = root->getFirstChild();
	if (!request)
	{
		rdata->setError() << "Missing request!";
		return false;
	}
	AosXmlTagPtr objdef = request->getFirstChild("objdef");
	OmnString errmsg;
	if (!objdef)
	{
		errmsg = "Missing the object to be created !";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}

	AosXmlTagPtr ioidoc = objdef->getFirstChild();
	if (!ioidoc)
	{
		errmsg = "Missing the object to be created(1)";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}
	OmnString cloudid = root->getChildTextByAttr("name", "userid");
	if (cloudid == "") 
	{
		errmsg = "Missing the cloudid!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}


	//get account
	AosXmlTagPtr usr_doc = AosDocClient::getSelf()->getDocByCloudid(cloudid, rdata);
	aos_assert_rr(usr_doc, rdata, false);
	usr_doc = usr_doc->clone(AosMemoryCheckerArgsBegin);
	
	OmnString zky_ioi_yuyue = usr_doc->getAttrStr("zky_ioi_yuyue", "");
	OmnString zky_ioi_yuyue_id = usr_doc->getAttrStr("zky_ioi_yuyue_id", "");
	OmnString userid;
	if(zky_ioi_yuyue == "old" || zky_ioi_yuyue_id == "")
	{
		OmnString zky_monitor_name = usr_doc->getAttrStr("zky_monitor_name", "");
		aos_assert_rr(zky_monitor_name!="", rdata, false);

		OmnString objids[2];
		OmnString attrs[2];
		OmnString values[2];
		AosOpr oprs[2];
		bool reverses[2];
		bool orders[2];
		AosValueRslt valueRslts[2];	
		
		objids[0] = "";
		attrs[0] = "zky_pctrs";
		values[0] = "sh_account_cs";
		valueRslts[0].setValue(values[0]);
		oprs[0] = eAosOpr_eq;
		reverses[0] = false;
		orders[0] = false;

		objids[1] = "";
		attrs[1] = "zky_monitor_name";
		values[1] = zky_monitor_name;
		valueRslts[1].setValue(values[1]);
		oprs[1] = eAosOpr_eq;
		reverses[1] = false;
		orders[1] = false;

		AosQueryRsltPtr queryRslt = OmnNew AosQueryRslt();
		aos_assert_r(queryRslt, false);

		bool rslt = AosDbRunQuery(
			-10,1,
			objids,attrs, valueRslts, oprs, reverses, 
			orders, 2, queryRslt, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eQueryFailed);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		u64 docid;
		OmnString cid;
		while(queryRslt->nextDocid(docid, rslt, rdata))
		{
			if(rslt)
			{
				break;
			}
			if (docid != 0)
			{
				AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
				if (!doc)
				{
					rdata->setError() << "Inter Error";
					OmnAlarm << rdata->getErrmsg() << enderr;
				}
				int status;
				cid = doc->getAttrStr(AOSTAG_CLOUDID, "");
				aos_assert_rr(cid != "", rdata, false);
				AosMsgService::getSelf()->getUserOnlineStatus(status, cid, rdata);
				if(status)
				{
					break;
				}
			}
		}
		userid << cid;
		//send msg to msgserver
		sendFailedMsg(userid, "ioi", rdata);
		rdata->setError() << "zky_ioi_yuyue: " << zky_ioi_yuyue << ", zky_ioi_yuyue_id: " << zky_ioi_yuyue_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		OmnScreen << "---------------- failed to save ioi  -------------"  << endl;
		return true; 
	}

	userid << cloudid;

	//add attributes
	addAttributes(ioidoc, usr_doc, "", rdata);

	OmnString ioiobjid = saveOneIOI(request, ioidoc, rdata);
	aos_assert_r(ioiobjid != "", false);
	
	//save attributes to zky_ioi_yuyue_id object
	AosXmlTagPtr yy = AosDocClient::getSelf()->getDocByObjid(zky_ioi_yuyue_id, rdata);
	yy = yy->clone(AosMemoryCheckerArgsBegin);
	aos_assert_rr(yy, rdata, false);
	OmnString zky_service_id = yy->getAttrStr("zky_service_id", "");
	aos_assert_rr(zky_service_id!="", rdata, false);
	yy->setAttr("zky_ioi_id", ioiobjid);
	yy->setAttr("zky_status", "诊断完成");
	bool mrslt2 = AosDocClient::getSelf()->modifyObj(rdata, yy, "", false);
	if (!mrslt2)
	{
		rdata->setError() << "modify error";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	//modify user account
	usr_doc->setAttr("zky_ioi_yuyue", "old");
	usr_doc->removeAttr("zky_ioi_yuyue_id");
	bool mrslt = AosDocClient::getSelf()->modifyObj(rdata, usr_doc, "", false);
	if (!mrslt)
	{
		rdata->setError() << "modify error";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	//send msg to msgserver
	sendSuccessMsg(userid, zky_ioi_yuyue_id, "zky_ioi_objid", ioiobjid, "ioi", rdata);
	sendSuccessMsg(zky_service_id, zky_ioi_yuyue_id, "zky_ioi_objid", ioiobjid, "ioi", rdata);
	return true;
}

bool 
AosSendToMsgService::sendSuccessMsg(OmnString &userid, OmnString &yuyue_id, const OmnString &objidnmae, OmnString &objidvalue, const OmnString &device, const AosRundataPtr &rdata)
{
	OmnString msgok;
	msgok <<  "<Contents zky_report=\"" << yuyue_id << "\" " <<  objidnmae << "=\"" << objidvalue << "\" zky_status=\"1\" zky_device=\"" << device << "\" from=\"10000075\" to=\"" << userid << "\" zky_port=\"aos_deal2\"><msg>have logined</msg></Contents>"; 
	AosXmlTagPtr contents_ok = AosXmlParser::parse(msgok AosMemoryCheckerArgs);
	AosMsgService::getSelf()->sendMsgToUser(contents_ok, rdata);
	return true;
}

bool 
AosSendToMsgService::sendFailedMsg(OmnString &userid, const OmnString &device, const AosRundataPtr &rdata)
{
	OmnString msg;
	msg << "<Contents zky_status=\"0\" zky_device=\"" << device << "\" from=\"10000075\" to=\"" << userid << "\" zky_port=\"aos_deal2\"><msg>have not logined</msg></Contents>";
	AosXmlTagPtr contents = AosXmlParser::parse(msg AosMemoryCheckerArgs);
	AosMsgService::getSelf()->sendMsgToUser(contents, rdata);
	return true;
}

bool 
AosSendToMsgService::procBatch(	AosXmlTagPtr &request, 
								AosXmlTagPtr &objdef,
								const OmnString &ToS,
								const OmnString &method,
								const AosRundataPtr &rdata)
{
	//proc one bmd or more bmd
	AosXmlTagPtr cmd = request->getFirstChild("command");
	if (cmd)
	{
		cmd = cmd->getFirstChild();
	}
	else
	{
		OmnAlarm << "Missing the command!" << enderr;
		return false;
	}
	OmnString zky_service_id; 
	AosXmlTagPtr entrydoc = objdef->getFirstChild(true);
	while(entrydoc)
	{
		AosXmlTagPtr entrynode = entrydoc->getFirstChild(true);
		if(!entrynode)
		{
			OmnAlarm << "bmd node is not exist!" << enderr;
			entrydoc = objdef->getNextChild();
			continue;
		}
		OmnString peopleid = entrynode->getAttrStr("zky_name", "");
		if(peopleid == "")
		{
			OmnAlarm << "peopleid is not exist!" << enderr;
			entrydoc = objdef->getNextChild();
			continue;
		}
		AosXmlTagPtr account = getUserAccount(peopleid, rdata);
		if(account)
		{
			if(zky_service_id == "")
			{
				zky_service_id = account->getAttrStr("zky_service_id", "");
			}
		}
		if(method == "rebatch")
		{
			reBatch(request, account, entrydoc, ToS, rdata);
		}
		else if(method == "batch")
		{
			batch(request, account, entrydoc, ToS, rdata);
		}
		else
		{
			return false;
		}
		entrydoc = objdef->getNextChild();
	}
	if(zky_service_id == "")
	{
		OmnAlarm << "service id is not exist!" << enderr;
		return true;
	}
	OmnString msgok;
	if(ToS == "bmd")
	{
		msgok <<  "<Contents zky_status=\"1\" zky_device=\"bmds\" from=\"10000075\" to=\"" << zky_service_id << "\" zky_port=\"aos_deal2\"><msg>have logined</msg></Contents>"; 
	}
	else
	{
		msgok <<  "<Contents zky_status=\"1\" zky_device=\"iois\" from=\"10000075\" to=\"" << zky_service_id << "\" zky_port=\"aos_deal2\"><msg>have logined</msg></Contents>"; 
	}
	AosXmlTagPtr contents_ok = AosXmlParser::parse(msgok AosMemoryCheckerArgs);
	AosMsgService::getSelf()->sendMsgToUser(contents_ok, rdata);
	return true;
	
}

bool
AosSendToMsgService::modifyYuyueDoc(AosXmlTagPtr &yuyuedoc, 
									OmnString &objid, 
									const OmnString &ToS,
									const AosRundataPtr &rdata)
{
	//3.modify yuyue doc
	if(ToS == "bmd")
	{
		yuyuedoc->setAttr("zky_bmd_id", objid);
	}
	else
	{
		yuyuedoc->setAttr("zky_ioi_id", objid);
	}
	yuyuedoc->setAttr("zky_status", "诊断完成");
	bool r = AosModifyDoc(yuyuedoc, rdata);
	aos_assert_r(r, false);
	return true;
}

AosXmlTagPtr
AosSendToMsgService::createYuyueDoc(AosXmlTagPtr &account, 
									AosXmlTagPtr &entrydoc, 
									const OmnString &ToS,
									const AosRundataPtr &rdata)
{
	//1.create yuyue doc
	AosXmlTagPtr entrynode = entrydoc->getFirstChild(true);
	if(!entrynode)
	{
		OmnAlarm << "bmd chile node is not exist!" << enderr;
	}
	OmnString yuyuestr = "<yueyue zky_pctrs=\"sh_病历\" zky_public_ctnr=\"true\"";
		yuyuestr << " zky_name=\"" << account->getAttrStr("zky_name","") <<"\"" ;
		yuyuestr << " zky_sex=\"" << account->getAttrStr("zky_sex","") <<"\"" ;
		yuyuestr << " zky_age=\"" << account->getAttrStr("zky_age","") <<"\"" ;
		yuyuestr << " zky_tel=\"" << account->getAttrStr("zky_tel","") <<"\"" ;
		yuyuestr << " zky_cloudid=\"" << account->getAttrStr(AOSTAG_CLOUDID,"") <<"\"" ;
		yuyuestr << " zky_uname=\"" << account->getAttrStr("zky_uname","") <<"\"" ;
		yuyuestr << " zky_account_id=\"" << account->getAttrStr(AOSTAG_OBJID,"") <<"\"" ;
		if(ToS == "bmd")
		{
			yuyuestr << " zky_bmd_yuyue=\"骨密度\"" ;
			yuyuestr << " zky_instdate=\"" << entrynode->getAttrStr("zky_createdate","") <<"\"" ;
		}
		else
		{
			yuyuestr << " zky_ioi_yuyue=\"身体成分\"" ;
			yuyuestr << " zky_instdate=\"" << entrynode->getAttrStr("zky_instdate","") <<"\"" ;
		}
		yuyuestr << " zky_status=\"审核已通过\"" ;
		yuyuestr << " zky_service=\"" << account->getAttrStr("zky_service","") <<"\"" ;
		yuyuestr << " zky_service_id=\"" << account->getAttrStr("zky_service_id","") <<"\"" ;
		yuyuestr << " zky_address=\"" << account->getAttrStr("zky_address","") <<"\"" ;
		yuyuestr << " zky_monitor_num=\"" << account->getAttrStr("zky_monitor_num","") <<"\"" ;
		yuyuestr << " zky_monitor_name=\"" << account->getAttrStr("zky_monitor_name","") <<"\"" ;
		yuyuestr << " zky_organ_num=\"" << account->getAttrStr("zky_organ_num","") <<"\"" ;
		yuyuestr << " zky_organ_name=\"" << account->getAttrStr("zky_organ_name","") <<"\"" ;
		yuyuestr << " />";
	AosXmlParser parser;
	AosXmlTagPtr yuyuexml = parser.parse(yuyuestr, "" AosMemoryCheckerArgs);
	AosXmlTagPtr yuyuedoc = AosCreateDoc(yuyuexml, rdata);
	aos_assert_r(yuyuedoc, NULL);
	return yuyuedoc;
}

OmnString
AosSendToMsgService::saveOneIOI(AosXmlTagPtr &request, AosXmlTagPtr &ioidoc, const AosRundataPtr &rdata)
{
	OmnString objid;
	AosXmlParser parser2;
	AosXmlTagPtr ioiclone = parser2.parse(ioidoc->toString(), "" AosMemoryCheckerArgs);
	AosXmlTagPtr ioi = AosCreateDoc(ioiclone, rdata);
	aos_assert_r(ioi, "");
	objid = ioi->getAttrStr(AOSTAG_OBJID, "");
	return objid;
}

OmnString
AosSendToMsgService::saveOneBmd(AosXmlTagPtr &request, AosXmlTagPtr &bmddoc, const AosRundataPtr &rdata)
{
	//has img or not
	OmnString objid;
	AosXmlTagPtr imgs = bmddoc->getFirstChild("imgs");
	if(imgs)
	{
		AosXmlTagPtr cmd = request->getFirstChild("command");
		if (cmd)
		{
			cmd = cmd->getFirstChild();
		}
		else
		{
			OmnAlarm << "Missing the command!" << enderr;
			return "";
		}
		AosImgProcPtr imgProc = AosSeReqProc::getImgProc();
		if (!imgProc) imgProc = OmnNew AosImgProc();
		int index = 0;
		AosXmlTagPtr img = imgs->getFirstChild(true);
		while(img)
		{
			OmnString imgpath = img->getNodeText();
			aos_assert_r(imgpath != "", "");
			bmddoc->setAttr("zky_imgdir", imgpath);
			bmddoc->setAttr("zky_public_ctnr", "true");
			if(index != 0)
			{
				bmddoc->setAttr("zky_pctrs", "sh_bmd3");
			}
			OmnString img_rslt;
			bool r = imgProc->uploadImage(rdata, bmddoc, cmd, img_rslt);
			if(!r) 
			{
				rdata->setError() << "Failed to save image doc!";
				OmnAlarm << rdata->getErrmsg() << enderr;
			}
			AosXmlTagPtr img_tag  = AosXmlParser::parse(img_rslt AosMemoryCheckerArgs);
			aos_assert_rr(img_tag, rdata, "");
			if(index == 0)
			{
				objid = img_tag->getAttrStr(AOSTAG_OBJID, "");
			}
			else
			{
				OmnString img_objid = img_tag->getAttrStr(AOSTAG_OBJID, "");
				AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid(img_objid, rdata);
				aos_assert_rr(doc, rdata, "");
				OmnString img_tnail = doc->getAttrStr("zky_tnail", "");
				aos_assert_rr(img_tnail != "", rdata, "");
				AosXmlTagPtr doc1 = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
				aos_assert_rr(doc1, rdata, "");
				doc1 = doc1->clone(AosMemoryCheckerArgsBegin);
				doc1->setAttr("zky_tnail2", img_tnail);
				bool mimg = AosDocClient::getSelf()->modifyObj(rdata, doc1, "", false);
				if (!mimg)
				{
					rdata->setError() << "modify image doc error";
					OmnAlarm << rdata->getErrmsg() << enderr;
					return "";
				}
			}
			img = imgs->getNextChild();
			index++;
		}
	}
	else
	{
		AosXmlParser parser2;
		AosXmlTagPtr bmdclone = parser2.parse(bmddoc->toString(), "" AosMemoryCheckerArgs);
		AosXmlTagPtr bmd = AosCreateDoc(bmdclone, rdata);
		aos_assert_r(bmd, "");
		objid = bmd->getAttrStr(AOSTAG_OBJID, "");
	}
	return objid;
}

bool
AosSendToMsgService::updateAccountByIOI(AosXmlTagPtr &entrydoc, 
										AosXmlTagPtr &account, 
										const AosRundataPtr &rdata)
{
	AosXmlTagPtr IOI = entrydoc->getFirstChild("IOI");
	OmnString height = IOI->getAttrStr("zky_height", "");
	OmnString weight = IOI->getAttrStr("zky_weight", "");
	OmnString bmi = IOI->getAttrStr("zky_bmi", "");
	OmnString bmi_rt = IOI->getAttrStr("zky_bmi_rt", "");
	AosXmlTagPtr account_c = account->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(account_c, false);
	bool hasattr = false;
	if(height!="") 	
	{
		account_c->setAttr("zky_height", height);
		hasattr = true;
	}
	if(weight!="") 
	{
		account_c->setAttr("zky_weight", weight);
		hasattr = true;
	}
	if(bmi!="") 
	{
		account_c->setAttr("zky_bmi", bmi);
		hasattr = true;
	}
	if(bmi_rt!="") 
	{
		account_c->setAttr("zky_bmi_rt", bmi_rt);
		hasattr = true;
	}
	if(hasattr)
	{
		bool rslt = AosDocClient::getSelf()->modifyObj(rdata, account_c, "", false);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosSendToMsgService::reBatch(	AosXmlTagPtr &request, 
								AosXmlTagPtr &account, 
								AosXmlTagPtr &entrydoc, 
								const OmnString &ToS,
								const AosRundataPtr &rdata)
{
	//1.create yuyue doc
	//2.modify yuyue doc
	
	aos_assert_r(account, false);
	AosXmlTagPtr entryclone = entrydoc->clone(AosMemoryCheckerArgsBegin);
	addAttributes(entryclone, account, ToS, rdata);
	if(ToS == "bmd")
	{
		entryclone->setAttr(AOSTAG_HPCONTAINER, "sh_bmd2");
	}
	else
	{
		entryclone->setAttr(AOSTAG_HPCONTAINER, "sh_ioi");
		updateAccountByIOI(entryclone, account, rdata);
	}
	bool r = AosModifyDoc(entryclone, rdata);
	aos_assert_r(r, false);
	OmnString objid = entryclone->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(objid != "", false);
	AosXmlTagPtr yuyuedoc = createYuyueDoc(account, entryclone, ToS, rdata);
	aos_assert_r(yuyuedoc, false);
	yuyuedoc = yuyuedoc->clone(AosMemoryCheckerArgsBegin);
	return modifyYuyueDoc(yuyuedoc, objid, ToS, rdata);
}

bool
AosSendToMsgService::addAttributes(AosXmlTagPtr &entry, AosXmlTagPtr &account, const OmnString &ToS, const AosRundataPtr &rdata)
{
	if(ToS == "ioi")
	{
		entry->setAttr("zky_whr_leve", account->getAttrStr("zky_whr_leve", ""));
	}
	entry->setAttr("zky_organ_name", account->getAttrStr("zky_organ_name", ""));
	entry->setAttr("zky_organ_num", account->getAttrStr("zky_organ_num", ""));
	entry->setAttr("zky_monitor_name", account->getAttrStr("zky_monitor_name", ""));
	entry->setAttr("zky_monitor_num", account->getAttrStr("zky_monitor_num", ""));
    entry->setAttr("zky_name", account->getAttrStr("zky_name", ""));
    entry->setAttr("zky_uname", account->getAttrStr("zky_uname", ""));
    entry->setAttr("zky_sex", account->getAttrStr("zky_sex", ""));
    entry->setAttr("zky_age", account->getAttrStr("zky_age", ""));
    entry->setAttr("zky_cloudid", account->getAttrStr(AOSTAG_CLOUDID, ""));
    entry->setAttr("zky_tel", account->getAttrStr("zky_tel", ""));
    entry->setAttr("zky_public_ctnr", "true");
    entry->setAttr("zky_service", account->getAttrStr("zky_service", ""));
    entry->setAttr("zky_service_id", account->getAttrStr("zky_service_id", ""));
    return true;
}

bool
AosSendToMsgService::batch(	AosXmlTagPtr &request, 
							AosXmlTagPtr &account, 
							AosXmlTagPtr &entrydoc, 
							const OmnString &ToS,
							const AosRundataPtr &rdata)
{
	//1.save bmd
	//2.create yuyue doc
	//3.modify yuyue doc
	if(!account)
	{
		//save bmd to tmp file
		if(ToS == "bmd")
		{
			entrydoc->setAttr(AOSTAG_PUB_CONTAINER, "sh_bmd_nouser");
			OmnString objid = saveOneBmd(request, entrydoc, rdata);
			aos_assert_r(objid != "", false);
		}
		else
		{
			entrydoc->setAttr(AOSTAG_PUB_CONTAINER, "sh_ioi_nouser");
			OmnString objid = saveOneIOI(request, entrydoc, rdata);
			aos_assert_r(objid != "", false);
		}
		return true;
	}
	else
	{
		if(ToS == "bmd")
		{
			entrydoc->setAttr(AOSTAG_PUB_CONTAINER, "sh_bmd2");
		}
		else
		{
			entrydoc->setAttr(AOSTAG_PUB_CONTAINER, "sh_ioi");
			updateAccountByIOI(entrydoc, account, rdata);
		}
	}
	addAttributes(entrydoc, account, ToS, rdata);
	OmnString objid;
	if(ToS == "bmd")
	{
		objid = saveOneBmd(request, entrydoc, rdata);
	}
	else
	{
		objid = saveOneIOI(request, entrydoc, rdata);
	}
	aos_assert_r(objid != "", false);
	AosXmlTagPtr yuyuedoc = createYuyueDoc(account, entrydoc, ToS, rdata);
	aos_assert_r(yuyuedoc, false);
	yuyuedoc = yuyuedoc->clone(AosMemoryCheckerArgsBegin);
	return modifyYuyueDoc(yuyuedoc, objid, ToS, rdata);
}

AosXmlTagPtr
AosSendToMsgService::getGarbage(OmnString &container, const AosRundataPtr &rdata)
{
	OmnString querydoc = "<cmd psize=\"500\" opr=\"retlist\" order=\"zky_ctmepo__d\" start_idx=\"0\">";
		querydoc << "<conds><cond type=\"AND\">";
		querydoc << "<term type=\"arith\"  order=\"false\" reverse=\"false\"><selector type=\"attr\" aname=\"zky_pctrs\"/><cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[" << container << "]]></cond></term>";
		querydoc << "</cond></conds>";
		querydoc << "<fnames><fname type=\"x\" ><oname>xml</oname><cname>xml</cname></fname></fnames></cmd>";
	AosXmlParser parser;
	AosXmlTagPtr queryxml = parser.parse(querydoc, "" AosMemoryCheckerArgs);
	OmnString contents;
	AosQueryReqObjPtr query = OmnNew AosQueryReq(queryxml, rdata);
	aos_assert_rr(query && query->isGood(), rdata, NULL);
	bool rslt = query->procPublic(queryxml, contents, rdata);
	aos_assert_rr(rslt, rdata , NULL);
	AosXmlParser pageparse;
	AosXmlTagPtr page = pageparse.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_rr(page, rdata, NULL);
	return page;
}

AosXmlTagPtr 
AosSendToMsgService::getUserAccount(OmnString &peopleid, const AosRundataPtr &rdata)
{
	OmnString querydoc = "<cmd psize=\"20\" opr=\"retlist\" order=\"zky_ctmepo__d\" start_idx=\"0\">";
		querydoc << "<conds><cond type=\"AND\">";
		querydoc << "<term type=\"arith\" reverse=\"false\" order=\"false\" ><selector type=\"attr\" aname=\"zky_pctrs\"/><cond type=\"arith\" zky_opr=\"eq\"><![CDATA[sh_account_p]]></cond></term>";
		querydoc << "<term type=\"arith\"  order=\"false\" reverse=\"false\"><selector type=\"attr\" aname=\"zky_uname\"/><cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[" << peopleid << "]]></cond></term>";
		querydoc << "</cond></conds>";
		querydoc << "<fnames><fname type=\"x\" ><oname>xml</oname><cname>xml</cname></fname></fnames></cmd>";
	AosXmlParser parser;
	AosXmlTagPtr queryxml = parser.parse(querydoc, "" AosMemoryCheckerArgs);
	OmnString contents;
	AosQueryReqObjPtr query = OmnNew AosQueryReq(queryxml, rdata);
	aos_assert_rr(query && query->isGood(), rdata, NULL);
	bool rslt = query->procPublic(queryxml, contents, rdata);
	aos_assert_rr(rslt, rdata , NULL);
	AosXmlParser pageparse;
	AosXmlTagPtr page = pageparse.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_rr(page, rdata, NULL);
	AosXmlTagPtr pageroot = page;
	if (page->isRootTag()) pageroot = page->getFirstChild();
	AosXmlTagPtr account = pageroot->getFirstChild();	
	return account;
}
