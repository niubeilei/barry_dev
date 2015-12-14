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
// The torturer is in SengTorturer/TesterProcEcgNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "CustomizedProcs/Contec/ProcEcg.h"

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
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
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


AosProcEcg::AosProcEcg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_PROCECG, 
		AosSeReqid::eProcEcg, rflag)
{
}


bool 
AosProcEcg::proc(const AosRundataPtr &rdata)
{
	// 'root' should be in the form:
	// 	<request>
	// 		<command>
	// 			<cmd opr="procecg"/>
	// 		</command>
	// 		<item name="siteid">100</item>
	// 		<item name="operation">serverCmd</item>
	// 		<item name="trans_id">10000</item>
	// 		<item name="urldocid">313</item>
	// 		<item name="ecg_data">/tmp/sessionid123/userbase.ecgbas</item>
	// 		<item name="zky_ssid_313">sessionid123</item>
	// 	</request>
	//
	// If success, it returns the following:
	// 	<Contents objid="xxx"/>
	//
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr cmd = rootchild->getFirstChild("command");
	if (cmd)
	{
		cmd = cmd->getFirstChild();
	}

	OmnString errmsg;

	if (AosSeReqProc::getIsStopping())
	{
		rdata->setError() << "Server is stopping";
		AOSLOG_LEAVE(rdata);
		return true;
	}


	OmnString path = root->getChildTextByAttr("name", "ecg_data");
	if (path == "") 
	{
		errmsg = "Missing the file path";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString ssid = rdata->getSsid();
	if (ssid == "") 
	{
		errmsg = "Missing the session";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//make dir
	OmnString mkcmd = "mkdir  -p ";
	mkcmd << "/tmp/" << ssid << "/Data";
	OmnScreen << "---------------- To mkdir file: " << mkcmd << endl;
	system(mkcmd.data());

	OmnString mksourcefile = "mkdir  -p /ECGSOURCEFILE";
	OmnScreen << "---------------- To mkdir source file: " << mksourcefile << endl;
	system(mksourcefile.data());

	char *sor = (char *)path.data();
	OmnString s2;
	s2 << "/tmp/" << ssid << "/Data/";
	char *des = (char *)s2.data();

	//get account
	AosXmlTagPtr usr_doc = AosDocClient::getSelf()->getDocByCloudid(rdata->getCid(), rdata);
	aos_assert_rr(usr_doc, rdata, false);
	usr_doc = usr_doc->clone(AosMemoryCheckerArgsBegin);
	OmnString zky_xindian_yuyue = usr_doc->getAttrStr("zky_xindian_yuyue", "");
	OmnString zky_yuyue_id = usr_doc->getAttrStr("zky_yuyue_id", "");
	if(zky_xindian_yuyue == "old" || zky_yuyue_id == "")
	{
		bool msg_flag = sendFailedMsgToFront(usr_doc, rdata);
		if(!msg_flag)
		{
			errmsg = "Failed to send msg! ";
			OmnAlarm << errmsg << sor << enderr;
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		//remove dir
		OmnString rvcmd = "mv ";
		rvcmd << "/tmp/" << ssid << " /ECGSOURCEFILE";
		OmnScreen << "---------------- To remove file: " << rvcmd << ", cloudid:" << rdata->getCid() << ", sessionid:" << ssid << endl;
		system(rvcmd.data());

		rdata->setError() << "zky_xindian_yuyue: " << zky_xindian_yuyue << ", zky_yuyue_id: " << zky_yuyue_id << ", cloudid:" << rdata->getCid() << ", sessionid:" << ssid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false; 
	}
	OmnString zky_cloudid = usr_doc->getAttrStr("zky_cloudid__a", "");
	aos_assert_rr(zky_cloudid!="", rdata, false);
	OmnString userid = zky_cloudid;


	//unpack the file
	OmnString unpackcmd;
	unpackcmd << "./ecg.exe " << sor << " " << des;
	int r = system(unpackcmd.data());
	if(r != 0)
	{
		OmnScreen << "---------------- ecg parse data error!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << endl;
		bool msg_flag = sendFailedMsgToFront(usr_doc, rdata);
		if(!msg_flag)
		{
			errmsg = "Failed to send msg! ";
			OmnAlarm << errmsg << sor << enderr;
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		errmsg = "Failed to unpack this file: ";
		OmnAlarm << errmsg << sor << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnScreen << "---------------- ecg parse data success!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << endl;

	//1.parse data
	//2.save data to db
	OmnString ecgDataPath = "";
	ecgDataPath << s2 << "/6500.ecg";
	OmnString presDataPath = "";
	presDataPath << s2 << "/6500.pres";
	OmnString respDataPath = "";
	respDataPath << s2 << "/6500.resp";
	OmnString tmpDataPath = "";
	tmpDataPath << s2 << "/6500.tmp";
	OmnString rrDataPath = "";
	rrDataPath << s2 << "/6500.rr";

	AosXmlTagPtr doc;
	OmnString ecg="";
	readEcg(rdata, ecgDataPath, ecg);
	doc = AosXmlParser::parse(ecg AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	OmnString pdpath;
	pdpath << "/tmp/" << ssid << "/update.xml";
	//add person node 
	AosXmlTagPtr pnode;
	OmnString pstr;
	pstr << "<PersonInfo/>";
	pnode = AosXmlParser::parse(pstr AosMemoryCheckerArgs);
	readPersonDetails(rdata, pdpath, pnode);

	OmnString test = "";
	readPres(rdata, presDataPath, pnode);
	test << pnode->getAttrStr("NIBP_SYS", "");
	if(test != "-100mmHg" && test != "")
	{
		doc->setAttr("zky_NIBP_test", "NIBP");
	}
	test = "";

	readResp(rdata, respDataPath, pnode);
	test << pnode->getAttrStr("Breathing", "");
	if(test != "-100BrPM" && test != "")
	{
		doc->setAttr("zky_Breathing_test", "Breathing");
	}
	test = "";

	readTmp(rdata, tmpDataPath, pnode);
	test << pnode->getAttrStr("temp", "");
	if(test != "-100 C" && test != "")
	{
		doc->setAttr("zky_temp_test", "temp");
	}
	test = "";

	readRr(rdata, rrDataPath, pnode);
	test << pnode->getAttrStr("PLETH", "");
	if(test != "-100%" && test != "")
	{
		doc->setAttr("zky_PLETH_test", "PLETH");
	}
	test = "";

	doc->addNode(pnode);

	//add attribute zky_instdate zky_instdate__d  
	OmnString time = OmnGetTime(AosLocale::eChina);
	doc->setAttr("zky_instdate", time);
	u32 systemsec = OmnGetSecond();
	doc->setAttr("zky_instdate__d", systemsec);
	

	//save doc
	AosXmlTagPtr rsltdoc = AosCreateDoc(doc, rdata);
	aos_assert_rr(rsltdoc, rdata, false);

	//save objid to account
	OmnString zky_objid = rsltdoc->getAttrStr("zky_objid", "");
	usr_doc->setAttr("zky_report", zky_yuyue_id);
	usr_doc->setAttr("zky_xindian_yuyue", "old");
	usr_doc->setAttr("zky_session_id", ssid);
	usr_doc->removeAttr("zky_yuyue_id");
	usr_doc->setAttr("zky_checktime", time);
	bool mrslt = AosDocClient::getSelf()->modifyObj(rdata, usr_doc, "", false);
	if (!mrslt)
	{
OmnScreen << "---------------- modify user account error!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << ", time:" << time << endl;
		rdata->setError() << "modify error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
OmnScreen << "---------------- modify user account success!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << ", time:" << time << endl;

	//save attributes to zky_yuyue_id object
	
	AosXmlTagPtr yy = AosDocClient::getSelf()->getDocByObjid(zky_yuyue_id, rdata);
	aos_assert_rr(yy, rdata, false);
	yy = yy->clone(AosMemoryCheckerArgsBegin);
	yy->setAttr("zky_xindian_data_id", zky_objid);
	yy->setAttr("zky_pass", "true");
	yy->setAttr("zky_instdate", time);
	yy->setAttr("zky_instdate__d", systemsec);
	yy->setAttr("zky_xindian_ostatus", "true");
	yy->setAttr("zky_status", "诊断完成");
	yy->setAttr("zky_checktime", time);
	yy->setAttr("zky_session_id", ssid);
	yy->setAttr("zky_check_cloudid", rdata->getCid());
	yy->addNode(pnode);
	bool mrslt2 = AosDocClient::getSelf()->modifyObj(rdata, yy, "", false);
	if (!mrslt2)
	{
OmnScreen << "---------------- modify yuyue doc error!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << ", time:" << time << endl;
		rdata->setError() << "modify error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
OmnScreen << "---------------- modify yuyue doc success!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << ", time:" << time << endl;
	OmnString zky_service_id = yy->getAttrStr("zky_service_id", "");
	aos_assert_rr(zky_service_id!="", rdata, false);
	OmnString zky_doctor_id = yy->getAttrStr("zky_doctor_id", "");

	//3.remove dir

	OmnString rvcmd = "mv ";
	rvcmd << "/tmp/" << ssid << " /ECGSOURCEFILE";
	OmnScreen << "---------------- To remove file: " << rvcmd << endl;
	system(rvcmd.data());

	//send msg to msgserver
	OmnString msgok;
	msgok <<  "<Contents zky_report=\"" << zky_yuyue_id << "\" zky_status=\"1\" from=\"10000075\" to=\"" << userid << "\" zky_port=\"aos_deal2\"><msg>have logined</msg></Contents>"; 
	AosXmlTagPtr contents_ok = AosXmlParser::parse(msgok AosMemoryCheckerArgs);
	AosMsgService::getSelf()->sendMsgToUser(contents_ok, rdata);

	OmnString msgok2;
	msgok2 <<  "<Contents zky_report=\"" << zky_yuyue_id << "\" zky_status=\"1\" zky_device=\"6500\" from=\"10000075\" to=\"" << zky_service_id << "\" zky_port=\"aos_deal2\"><msg>have logined</msg></Contents>"; 
	AosXmlTagPtr contents_ok2 = AosXmlParser::parse(msgok2 AosMemoryCheckerArgs);
	AosMsgService::getSelf()->sendMsgToUser(contents_ok2, rdata);

	if(zky_doctor_id != "")
	{
		OmnString msgok3;
		msgok3 <<  "<Contents zky_report=\"" << zky_yuyue_id << "\" zky_status=\"1\" from=\"10000075\" to=\"" << zky_doctor_id << "\" zky_port=\"aos_deal2\"><msg>have logined</msg></Contents>"; 
		AosXmlTagPtr contents_ok2 = AosXmlParser::parse(msgok3 AosMemoryCheckerArgs);
		AosMsgService::getSelf()->sendMsgToUser(contents_ok2, rdata);
	}
	AOSLOG_LEAVE(rdata);
	rdata->setOk();
OmnScreen << "---------------- proc ecg data all finished!  cloudid:" << rdata->getCid() << ", sessionid:" << ssid << ", time:" << time << endl;
	return true;
}

bool
AosProcEcg::sendFailedMsgToFront(const AosXmlTagPtr usr_doc, const AosRundataPtr &rdata)
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
			OmnAlarm << "there is no person on line 1" << enderr;
			break;
		}
		if (docid != 0)
		{
			AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
			if (!doc)
			{
				rdata->setError() << "Inter Error";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
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
	OmnString userid = cid;
	aos_assert_rr(userid != "", rdata, false);
	//send msg to msgserver
	OmnString msg;
	msg << "<Contents zky_status=\"0\" zky_device=\"6500\" from=\"10000075\" to=\"" << userid << "\" zky_port=\"aos_deal2\"><msg>have not logined</msg></Contents>";
	AosXmlTagPtr contents = AosXmlParser::parse(msg AosMemoryCheckerArgs);
	AosMsgService::getSelf()->sendMsgToUser(contents, rdata);
	return true;

}

bool
AosProcEcg::readPersonDetails(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
{
	char data[1000];
	int pos = 0;
	int bytesread = 0;
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	OmnString pd;
	do
	{
		bytesread = file->readToBuff(pos, sizeof(data), data);
		pos += sizeof(data);
		pd << &data[6];
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);

	OmnStrParser1 parser1(pd, ";");
	OmnString infomap;
	while((infomap = parser1.nextWord()) != "")
	{
		OmnStrParser1 parser2(infomap, "=");
		OmnString name, value;
		name = parser2.nextWord();
		aos_assert_rr(name != "", rdata, false);
		value = parser2.nextWord();
		doc->setAttr(name, value);
	}
	return true;
}
bool
AosProcEcg::readRr(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
{
	char data[100];
	int pos = 0;
	int bytesread = 0;
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	OmnString pleth;
	do
	{
		bytesread = file->readToBuff(pos, sizeof(data), data);
		pos += sizeof(data);
		int max = 0;
		for (int i=0; i<bytesread;)
		{
			int tmp = data[i++];
			if(tmp >= max)
			{
				max = tmp;
			}
		}
		pleth << max;
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);
	if(pleth == "-100")
	{
		pleth = "0";
	}
	OmnString  PLETH_rt; 
	int p = pleth.toInt();
	if(0<p && p<90)
	{
		 PLETH_rt = "低血氧症";
	}
	else if(90 <= p && p <94)
	{
		 PLETH_rt = "低血氧";
	}
	else if(94 <= p)
	{
		 PLETH_rt = "正常血氧";
	}
	if(p>0)
	{
		doc->setAttr("PLETH_rt", PLETH_rt);
	}
	doc->setAttr("PLETH", pleth);
	doc->setAttr("PLETH_Y", pleth<<"%");
	return true;
}

bool
AosProcEcg::readTmp(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
{
	short data[100];
	int pos = 0;
	int bytesread = 0;
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	OmnString temp;
	do
	{
		bytesread = file->readToBuff(pos, sizeof(data), (char *)data);
		pos += sizeof(data);
		int sum = 0;
		for (int i=0; i<bytesread/2;)
		{
			sum += data[i++];
		}
		int avg = sum/bytesread*2;
		temp << avg;
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);
	if(temp == "-100")
	{
		temp = "0";
	}
	doc->setAttr("temp", temp);
	doc->setAttr("temp_Y", temp<<" C");
	return true;
}

bool
AosProcEcg::readResp(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
{
	short data[100];
	int pos = 0;
	int bytesread = 0;
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	OmnString breathing;
	do
	{
		bytesread = file->readToBuff(pos, sizeof(data), (char *)data);
		pos += sizeof(data);
		int sum = 0;
		for (int i=0; i<bytesread/2;)
		{
			sum += data[i++];
		}
		int avg = sum/bytesread*2;
		breathing << avg;
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);
	if(breathing == "-100")
	{
		breathing = "0";
	}
	doc->setAttr("Breathing", breathing);
	doc->setAttr("Breathing_Y", breathing<<"BrPM");
	return true;
}

bool
AosProcEcg::readPres(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
{
	OmnString NIBP_SYS = "";
	OmnString NIBP_DIA = "";
	OmnString NIBP_MAP = "";
	OmnString PR = "";
	short data[100];
	int pos = 0;
	int bytesread = 0;
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	do
	{
		bytesread = file->readToBuff(pos, sizeof(data), (char *)data);
		pos += sizeof(data);
		for (int i=0; i<bytesread/2;)
		{
			NIBP_DIA << data[i++];
			NIBP_SYS << data[i++];
			NIBP_MAP << data[i++];
			PR << data[i++];
		}
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);
	if(NIBP_SYS == "-100")
	{
		NIBP_SYS = "0";
	}
	if(NIBP_DIA == "-100")
	{
		NIBP_DIA = "0";
	}
	if(NIBP_MAP == "-100")
	{
		NIBP_MAP  = "0";
	}
	if(PR == "-100")
	{
		PR = "0";
	}

	int nibp_sys = NIBP_SYS.toInt();
	int nibp_dia = NIBP_DIA.toInt();

	OmnString NIBP_rt;
	if(nibp_sys<=90 && nibp_dia<=60)
	{
		NIBP_rt = "低血压";
	}
	if(nibp_sys>90 && nibp_sys<140 && nibp_dia>60 && nibp_dia<90)
	{
		NIBP_rt = "正常血压";
	}
	if(nibp_sys>=140 || nibp_dia>=90)
	{
		NIBP_rt = "高血压";
	}
	doc->setAttr("NIBP_rt", NIBP_rt);

	doc->setAttr("NIBP_SYS", NIBP_SYS);
	doc->setAttr("NIBP_DIA", NIBP_DIA);
	doc->setAttr("NIBP_MAP", NIBP_MAP);
	doc->setAttr("PR", PR);
	doc->setAttr("NIBP_SYS_Y", NIBP_SYS<<"mmHg");
	doc->setAttr("NIBP_DIA_Y", NIBP_DIA<<"mmHg");
	doc->setAttr("NIBP_MAP_Y", NIBP_MAP<<"mmHg");
	doc->setAttr("PR_Y", PR<<"mmHg");
	return true;
}

bool
AosProcEcg::readEcg(const AosRundataPtr &rdata, OmnString path, OmnString &ecg)
{
	OmnString s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12;
	short data[2400];
	int pos = 0;
	int bytesread = 0;
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	bool invalid = true;
	int total = 0;
	int avg = 0;
	int num = 0;
	do
	{
		bytesread = file->readToBuff(pos, sizeof(data), (char *)data);
		pos += sizeof(data);
		for (int i=0; i<bytesread/2;)
		{
			short tmp1 = data[i++];
			s1 << tmp1 << ",";
			short tmp2 = data[i++];
			s2 << tmp2 << ",";
			short tmp3 = data[i++];
			s3 << tmp3 << ",";
			short tmp4 = data[i++];
			s4 << tmp4 << ",";
			short tmp5 = data[i++];
			s5 << tmp5 << ",";
			short tmp6 = data[i++];
			s6 << tmp6 << ",";
			short tmp7 = data[i++];
			s7 << tmp7 << ",";
			short tmp8 = data[i++];
			s8 << tmp8 << ",";
			short tmp9 = data[i++];
			s9 << tmp9 << ",";
			short tmp10 = data[i++];
			s10 << tmp10 << ",";
			short tmp11 = data[i++];
			s11 << tmp11 << ",";
			short tmp12 = data[i++];
			s12 << tmp12 << ",";
			total += tmp1+tmp2+tmp3+tmp4+tmp5+tmp6+tmp7+tmp8+tmp9+tmp10+tmp11+tmp12;
		}
		num += bytesread/2;
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);
	s1.trim(1);
	s2.trim(1);
	s3.trim(1);
	s4.trim(1);
	s5.trim(1);
	s6.trim(1);
	s7.trim(1);
	s8.trim(1);
	s9.trim(1);
	s10.trim(1);
	s11.trim(1);
	s12.trim(1);

	avg = total/num;
	if(avg != 1264 )
	{
		invalid = false;
	}

	ecg << "<AppNode ";
	if(!invalid)
	{
		ecg << "zky_ECG_test=\"ECG\" ";
	}
	ecg << "zky_public_ctnr=\"true\" " 
		<< "zky_public_doc=\"true\" " 
		<< " datatype=\"xindian" 
		<< "\" zky_pctrs=\"shanghai_检测"
		<< "\" zky_cloudid=\"" << rdata->getCid() 
		<< "\" zky_userid=\"" << rdata->getUserid() 
		<< "\" zky_status=\"" << "待诊断" 
		<< "\" " << AOSTAG_SITEID << "=\"100"
		<< "\">"
		<< "<SW_LEAD_I__n><![CDATA["
		<< s1 << "]]></SW_LEAD_I__n>"
		<< "<SW_LEAD_II__n><![CDATA["
		<< s2 << "]]></SW_LEAD_II__n>"
		<< "<SW_LEAD_III__n><![CDATA["
		<< s3 << "]]></SW_LEAD_III__n>"
		<< "<SW_LEAD_aVR__n><![CDATA["
		<< s4 << "]]></SW_LEAD_aVR__n>"
		<< "<SW_LEAD_aVL__n><![CDATA["
		<< s5 << "]]></SW_LEAD_aVL__n>"
		<< "<SW_LEAD_aVF__n><![CDATA["
		<< s6 << "]]></SW_LEAD_aVF__n>"
		<< "<SW_LEAD_V1__n><![CDATA["
		<< s7 << "]]></SW_LEAD_V1__n>"
		<< "<SW_LEAD_V2__n><![CDATA["
		<< s8 << "]]></SW_LEAD_V2__n>"
		<< "<SW_LEAD_V3__n><![CDATA["
		<< s9 << "]]></SW_LEAD_V3__n>"
		<< "<SW_LEAD_V4__n><![CDATA["
		<< s10 << "]]></SW_LEAD_V4__n>"
		<< "<SW_LEAD_V5__n><![CDATA["
		<< s11 << "]]></SW_LEAD_V5__n>"
		<< "<SW_LEAD_V6__n><![CDATA["
		<< s12 << "]]></SW_LEAD_V6__n>"
		<< "</AppNode>";
	return true;
}

