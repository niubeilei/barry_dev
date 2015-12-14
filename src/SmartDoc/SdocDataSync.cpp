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
// 03/03/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocDataSync.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/File.h"

AosSdocDataSync::AosSdocDataSync(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_DATASYNC, AosSdocId::eDataSync, flag)
{
}


AosSdocDataSync::~AosSdocDataSync()
{
}


bool
AosSdocDataSync::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString remoteAddr = sdoc->getAttrStr("remote_addr", "");
	int remotePort = sdoc->getAttrInt("remote_port", -1);
	if(remoteAddr == "" || remotePort == -1)
	{
		rdata->setError() << "Missing remoteAddr or remotePort!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString checkCtnrs = sdoc->getAttrStr("zky_sync_ctnrs", "");
	if(checkCtnrs == "")
	{
		rdata->setError() << "Missing sync ctnrs!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//felicia,2012/04/06
	OmnString attrname = sdoc->getAttrStr("zky_attrname", "zky_pctrs");
	if (attrname == "")
	{
		rdata->setError() << "Missing sync attribute name!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString save = sdoc->getAttrStr("zky_sync_save", "false");
	rdata->setArg1("zky_sync_save", save);
	if (save == "true")
	{
		OmnString dirname = sdoc->getAttrStr("zky_sync_dir", "datasync");
		OmnString cmd;
		cmd << "mkdir -p ";
		cmd << dirname; 
		system(cmd.data());
		rdata->setArg1("zky_sync_dir", dirname);
	}

	OmnTcpClientPtr conn = OmnNew OmnTcpClient("DataSync", remoteAddr, remotePort, 1, eAosTLT_FirstFourHigh);
	OmnString err;
	u32 times = 0;
	while( !conn->connect(err))
	{
		OmnSleep(eReConnTime);
		times++;
		if(times >= eMaxTries)
		{
			rdata->setError() << "Can't connect! remoteAddr:" << remoteAddr << "; remotePort:" << remotePort;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	
	// 1. login get the ssid.
	// 2. get the server time. to add into the last_sync_time. 
	
	OmnString server_crt_time = "";
	//server_crt_time << OmnGetSecond();
	bool rslt = getServerCrtTime(conn, server_crt_time, rdata);
	aos_assert_r(rslt, false);

	//felicia,2012/03/26
	bool uselst = sdoc->getAttrBool("zky_use_stime", true);
	OmnString lastSyncTime = sdoc->getAttrStr("last_sync_time", "");
	if (!uselst)
	{
		lastSyncTime = "";
	}

	u32 eSyncNum = sdoc->getAttrU32("zky_sync_num", 50);	

	bool canModifySdoc = sdoc->getAttrBool("zky_modify_sdoc", true);
	OmnString ctnrname = sdoc->getAttrStr("zky_ctnr_name", "");
	if (attrname == "zky_pctrs")
	{
		OmnStrParser1 parser(checkCtnrs, ", ");
		OmnString ctnr;
		rslt = false;
		while((ctnr = parser.nextWord()) != "")
		{
			rslt = procEachCtnr(conn, attrname, ctnr, eSyncNum, lastSyncTime, rdata, ctnrname);		
			if(!rslt)
			{
				canModifySdoc = false;
				rdata->setError() << "sync Error!";
				OmnAlarm << rdata->getErrmsg() << enderr;
			}
		}
	}
	else
	{
		rslt = procEachCtnr(conn, attrname, checkCtnrs, eSyncNum, lastSyncTime, rdata, ctnrname);		
		if(!rslt)
		{
			canModifySdoc = false;
			rdata->setError() << "sync Error!";
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
	}
	if(!canModifySdoc)	return false;
	
	// 3. modify the last_sync_time;
	//sdoc->setAttr("last_sync_time", server_crt_time);
	// rslt = AosDocClient::getSelf()->modifyAttr(sdoc, 
	rslt = AosModifyDocAttrStr(rdata, sdoc, "last_sync_time", server_crt_time,
				false, false);
	if(!rslt)
	{
		rdata->setError() << "Modify Sdoc Error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}

bool
AosSdocDataSync::procEachCtnr(
		const OmnTcpClientPtr &conn, 
		const OmnString &attrname,
		const OmnString &ctnr, 
		const u32 &eSyncNum,
		const OmnString &lastSyncTime,
		const AosRundataPtr &rdata,
		const OmnString &ctnrname)
{
	//	the response msg.
	// 		<Contents total="..." start_idx="..." crt_idx="..." num="..." queriid="...">
	//			<vpd />
	//			...
	// 		</Contents>
	
	// 1. check whether this ctnr is exist.
	
	//felicia,2012/03/26
	/*bool exist = checkCtnr(ctnr, rdata);	
	if(!exist)
	{
		bool rslt = retrieveCtnr(conn, ctnr, rdata);
		aos_assert_r(rslt, false);
	}*/

	// 2. proc created and modified doc.
	OmnString sendMsg = createSendMsg(attrname, ctnr, eSyncNum, lastSyncTime, 0, rdata, ctnrname);

	AosXmlTagPtr rootResp, rootChild;
	bool rslt = send(conn, sendMsg, rootResp, rootChild, rdata);
	aos_assert_r(rslt && rootResp && rootChild, false);
	AosXmlTagPtr contents = rootChild->getNextChild("Contents");
	if(!contents)
	{
		OmnAlarm << "missing response:" << rootResp->toString()
			<< enderr;
		return false;
	}
//cout << "query rslt === " << contents->toString().data() << endl;

	u32 totalNum = contents->getAttrU32("total", 0);
	if(totalNum == 0)	return true;

	rslt = procModified(conn, contents, rdata);
	aos_assert_r(rslt, false);

	u32 i=1;
	u32 procedNum = (totalNum > eSyncNum ? eSyncNum : totalNum);	
	while(procedNum < totalNum)
	{
		u32 startIdx = i * eSyncNum;
//cout << "startIdx ===== " << startIdx << endl;
		sendMsg = createSendMsg(attrname, ctnr, eSyncNum, lastSyncTime, startIdx, rdata, ctnrname);
		rslt = send(conn, sendMsg, rootResp, rootChild, rdata);
		aos_assert_r(rslt && rootResp && rootChild, false);
		contents = rootChild->getNextChild("Contents");
		if(!contents)
		{
			OmnAlarm << "missing response:" << rootResp->toString()
				<< enderr;
			return false;
		}
//cout << "query rslt === " << contents->toString().data() << endl;

		rslt = procModified(conn, contents, rdata);
		aos_assert_r(rslt, false);
	
		i++;
		procedNum += eSyncNum;
	}

	// 2. proc deleted doc.	
	return true;
}


bool
AosSdocDataSync::procModified(
		const OmnTcpClientPtr &conn, 
		const AosXmlTagPtr &contents,
		const AosRundataPtr &rdata)
{
	aos_assert_r(contents, false);

	AosXmlTagPtr newdoc, doc = contents->getFirstChild();
	vector<OmnString> old_docids, new_docids;
	OmnString docid, objid;
	while(doc)
	{
		docid = doc->getAttrStr(AOSTAG_DOCID, "");
		objid = doc->getAttrStr(AOSTAG_OBJID, "");
		
		doc->removeAttr(AOSTAG_DOCID);
		doc->removeAttr(AOSTAG_CREATOR);
		doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		doc->setAttr(AOSTAG_PUBLIC_DOC, "true");

		OmnString save = rdata->getArg1("zky_sync_save");
		if (save == "true")
		{
			OmnString dirname = rdata->getArg1("zky_sync_dir");
			aos_assert_r(dirname != "", false);
			OmnString fname;
			fname << dirname << "/" << objid << ".txt";
			doc->removeAttr(AOSTAG_SYSACCT_SIGNATURE);
			OmnString data = doc->toString();
			OmnFile ff(fname, OmnFile::eCreate AosMemoryCheckerArgs);
			ff.append(data, true);
		}
		else
		{
			bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, "false", false);
			if (rslt)
			{
				newdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
				if (newdoc)
				{
					old_docids.push_back(docid);	
					docid = newdoc->getAttrStr(AOSTAG_DOCID, "");
					new_docids.push_back(docid);
				}
			}
		}
		
		doc = contents->getNextChild();
	}
	
	int num = old_docids.size();
	if (num == 0) return true;
	
	//sync accessrecord
	OmnString args = "docid=";
	for (int i=0; i<num-1; i++)
	{
		args << old_docids[i] << "/";
	}
	args << old_docids[num-1] << ",datasync=true";
	OmnString msg = "<request>";
	msg << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">arcd_get</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << rdata->getSiteid() << "</item>"
		<< "<item name=\"args\">" << args << "</item>"
		<< "</request>";
	
	AosXmlTagPtr rootResp, rootChild;
	bool rslt = send(conn, msg, rootResp, rootChild, rdata);
	aos_assert_r(rslt && rootResp && rootChild, false);
	AosXmlTagPtr accr_contents = rootChild->getNextChild("Contents");
	if(!accr_contents)
	{
		OmnAlarm << "missing response:" << rootResp->toString()
			<< enderr;
		return false;
	}

	rslt = procModifyAccr(accr_contents, old_docids, new_docids, rdata);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosSdocDataSync::procModifyAccr(
		const AosXmlTagPtr &contents,
		const vector<OmnString> &old_docids,
		const vector<OmnString> &new_docids,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = contents->getFirstChild();
	OmnString old_docid, docidstr;
	int index = -1;
	while(doc)
	{
		doc->removeAttr(AOSTAG_DOCID);
		old_docid = doc->getAttrStr(AOSTAG_OWNER_DOCID, "");
		for (u32 i=0; i<old_docids.size(); i++)
		{
			if (old_docid == old_docids[i])
			{
				index = i;
				break;
			}
		}
		if (index == -1)
		{
			OmnAlarm << "missing AccessRecord" << enderr;
			return false;
		}
		docidstr = new_docids[index];
		doc->setAttr(AOSTAG_OWNER_DOCID, docidstr);
		u64 docid = atoll(docidstr.data());
		bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(docid, doc, rdata);
		aos_assert_r(rslt, false);
		
		doc = contents->getNextChild();
	}
	return true;
}

OmnString
AosSdocDataSync::createSendMsg(
		const OmnString &attrname,
		const OmnString &ctnr,
		const u32 &eSyncNum,
		const OmnString &lastSyncTime,
		const u32 startIdx,
		const AosRundataPtr &rdata,
		const OmnString &ctnrname)
{
	OmnString conds = "<conds>";
	/*conds << "<cond type=\"AND\">" 
		<< "<term type=\"arith\" zky_ctobjid=\"giccreators_querynew_term_default_h\" order=\"false\" reverse=\"false\">"
			<< "<selector type=\"attr\" aname=\"zky_pctrs\"/><cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[" << ctnr << "]]></cond></term>";*/
	
	conds << "<cond type=\"AND\">" 
		<< "<term type=\"arith\" zky_ctobjid=\"giccreators_querynew_term_default_h\" order=\"false\" reverse=\"false\">"
			<< "<selector type=\"attr\" ";
	if (ctnrname != "")
	{
		conds << "container=\"" << ctnrname << "\" ";
	}
	conds << "aname=\"" << attrname << 
			"\"/><cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[" << ctnr << "]]></cond></term>";

	if (lastSyncTime != "")
	{
		conds << "<term type=\"arith\" zky_ctobjid=\"giccreators_querynew_term_default_h\" order=\"false\" reverse=\"false\">"
			<< "<selector type=\"attr\" aname=\"zky_mtmepo__d\"/><cond type=\"arith\" ctype=\"const\" zky_opr=\"ge\"><![CDATA[" << lastSyncTime << "]]></cond></term>";
	}
	conds << "</cond>"
		<< "</conds>"
		<< "<fnames><fname type=\"x\" zky_ctobjid=\"giccreators_query_fname_h\"><oname>xml</oname><cname>xml</cname></fname></fnames>";

	OmnString cmd = "<cmd ";
	cmd << "opr=\"retlist\" "
		<< "psize=\"" << eSyncNum << "\" "
		<< "get_total=\"true\" "
		//<< "start_idx=\"0\" >"
		<< "start_idx=\"" << startIdx << "\" >"
		<< conds
		<< "</cmd>";

//cout << "query_cmd ====  " << cmd.data() << endl;
	OmnString msg = "<request>";
	msg << "<item name=\"operation\">serverCmd</item>"
		//<< "<item name=\"trans_id\">" << trans_id << "</item>";
	    //<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << rdata->getSiteid() << "</item>"
		<< "<command>" << cmd << "</command>"
		<< "</request>";
	
	return msg;	
}


bool
AosSdocDataSync::send(
		const OmnTcpClientPtr &conn,
		const OmnString &req, 
		AosXmlTagPtr &root,
		AosXmlTagPtr &rootChild,
		const AosRundataPtr &rdata)
{
	bool rslt = conn->smartSend(req.data(), req.length());
	if(!rslt)
	{
		rdata->setError() << "Can't send! ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnConnBuffPtr buff;
	rslt = conn->smartRead(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(buff, false);

	OmnString respMsg = "<response zky_status = \"";
	respMsg << "true\">";
	respMsg << buff->getData();
	respMsg << "</response>";

	// response format
	// <response trans_id="..">
	// 		<status error="false" code="200" proctime="...">
	// 		<Contents /> 
	// </response>
	
	AosXmlParser parser;
	//AosXmlTagPtr root = parser.parse(buff, "" AosMemoryCheckerArgs);
	root = parser.parse(respMsg, "" AosMemoryCheckerArgs);

	if(!root || !(rootChild = root->getFirstChild()) )    
	{
		OmnAlarm << "response format is wrong:" << enderr;
		return false;
	}

	AosXmlTagPtr status = rootChild->getFirstChild();
	OmnString error = status->getAttrStr("error");
	if(error == "true")
	{
		OmnAlarm << "It's Server Error, errorMsg:"
				  << status->getNodeText();
		return false;
	}

	return true;	
}


bool
AosSdocDataSync::getServerCrtTime(
		const OmnTcpClientPtr &conn,
		OmnString &server_crt_time,
		const AosRundataPtr	&rdata)
{
	server_crt_time = "";
	server_crt_time << (u64)OmnGetSecond(); 
	return true;

	OmnString msg = "<request>";
	msg << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << rdata->getSiteid() << "</item>"
		<< "<command>"
		<< "<cmd opr=\"servertime\" />"
		<< "</command>"
		<< "</request>";
	
	AosXmlTagPtr rootResp, rootChild;
	bool rslt = send(conn, msg, rootResp, rootChild, rdata);
	aos_assert_r(rslt && rootResp && rootChild, false);
	AosXmlTagPtr contents = rootChild->getNextChild("Contents");
	if(!contents)
	{
		OmnAlarm << "missing response:" << rootResp->toString()
			<< enderr;
		return false;
	}
	
	server_crt_time = contents->getAttrStr(AOSTAG_EPOTIME, "");
	return true;
}


bool
AosSdocDataSync::checkCtnr(const OmnString &ctnr, const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(ctnr, rdata); 
	return (doc ? true : false);	
}


bool
AosSdocDataSync::retrieveCtnr(
		const OmnTcpClientPtr &conn,
		const OmnString &ctnr,
		const AosRundataPtr &rdata)
{
	OmnAlarm << "not complete yet!" << enderr;
	return false;

	OmnString msg = "<request >";
	msg << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << rdata->getSiteid() << "</item>"
		<< "<item name=\"objid\">" << ctnr << "</item>"
		<< "</request>";
	
	AosXmlTagPtr rootResp, rootChild;
	bool rslt = send(conn, msg, rootResp, rootChild, rdata);
	aos_assert_r(rslt && rootResp && rootChild, false);
	AosXmlTagPtr contents = rootChild->getNextChild("Contents");
	if(!contents)
	{
		OmnAlarm << "missing response:" << rootResp->toString()
			<< enderr;
		return false;
	}
	
	return true;
}


