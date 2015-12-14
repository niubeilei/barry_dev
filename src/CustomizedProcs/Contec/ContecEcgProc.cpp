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
// The torturer is in SengTorturer/TesterContecEcgProcNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "CustomizedProcs/Contec/ContecEcgProc.h"

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
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#ifdef __amd64__
#include "SeReqProc/Lib.h"
#else
#include "SeReqProc/Lib32.h"
#endif
//#include "SeReqProc/Unpack8000w.h"
#include "API/AosApiC.h"


AosContecEcgProc::AosContecEcgProc(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CONTEC_ECG_PROC, 
		AosSeReqid::eContecEcgProc, rflag)
{
}


bool 
AosContecEcgProc::proc(const AosRundataPtr &rdata)
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

	OmnString mkcmd = "mkdir  -p ";
	mkcmd << "/tmp/" << ssid << "/Data";
	OmnScreen << "---------------- To mkdir file: " << mkcmd << endl;
	system(mkcmd.data());

	char *sor = (char *)path.data();
	OmnString s2;
	s2 << "/tmp/" << ssid << "/Data/";
	char *des = (char *)s2.data();

	UnPackFile(sor, des);

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

	readPres(rdata, presDataPath, pnode);

	readResp(rdata, respDataPath, pnode);

	readTmp(rdata, tmpDataPath, pnode);

	readRr(rdata, rrDataPath, pnode);

	doc->addNode(pnode);
	//save doc
	bool rslt = AosCreateDoc(doc, rdata);
	aos_assert_rr(rslt, rdata, false);
	//3.remove dir

	OmnString rvcmd = "rm -rf ";
	rvcmd << "/tmp/" << ssid;
	OmnScreen << "---------------- To remove file: " << rvcmd << endl;
	system(rvcmd.data());

	AOSLOG_LEAVE(rdata);
	return true;
}

bool
AosContecEcgProc::readPersonDetails(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
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
AosContecEcgProc::readRr(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
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
		int sum =0;
		for (int i=0; i<bytesread;)
		{
			sum += data[i++];
		}
		int avg = sum/bytesread;
		pleth << avg;
		if(bytesread < 0)
		{
			break;
		}
		if((unsigned int)bytesread < sizeof(data))
			break;
	}while(1);
	doc->setAttr("PLETH", pleth<<"%");
	return true;
}

bool
AosContecEcgProc::readTmp(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
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
	doc->setAttr("temp", temp<<" C");
	return true;
}

bool
AosContecEcgProc::readResp(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
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
	doc->setAttr("Breathing", breathing<<"BrPM");
	return true;
}

bool
AosContecEcgProc::readPres(const AosRundataPtr &rdata, OmnString path, AosXmlTagPtr doc)
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
			NIBP_SYS << data[i++];
			NIBP_DIA << data[i++];
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
	doc->setAttr("NIBP_SYS", NIBP_SYS<<"mmHg");
	doc->setAttr("NIBP_DIA", NIBP_DIA<<"mmHg");
	doc->setAttr("NIBP_MAP", NIBP_MAP<<"mmHg");
	doc->setAttr("PR", PR<<"mmHg");
	return true;
}

bool
AosContecEcgProc::readEcg(const AosRundataPtr &rdata, OmnString path, OmnString &ecg)
{
	OmnString s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12;
	short data[2400];
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
			s1 << data[i++] << ",";
			s2 << data[i++] << ",";
			s3 << data[i++] << ",";
			s4 << data[i++] << ",";
			s5 << data[i++] << ",";
			s6 << data[i++] << ",";
			s7 << data[i++] << ",";
			s8 << data[i++] << ",";
			s9 << data[i++] << ",";
			s10 << data[i++] << ",";
			s11 << data[i++] << ",";
			s12 << data[i++] << ",";

		}
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

	ecg << "<AppNode ";
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

