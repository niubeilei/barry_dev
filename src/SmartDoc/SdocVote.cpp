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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocVote.h"

#include "DbQuery/Query.h"
#include "Actions/ActSeqno.h"
#include "SEInterfaces/DocClientObj.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/ObjMgr.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosSdocVote::AosSdocVote(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_VOTE, AosSdocId::eVote, flag)
{
}


AosSdocVote::~AosSdocVote()
{
}


bool
AosSdocVote::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
 	// <smartdoc 
	//	zky_sdoctp="vote"  //this is sdoc type important!
	//	zky_siteid="100"
	//	zky_otype="zky_smtdoc">
	//	<userconfig>
	//		<time type="interval|cycle" starttime="xxx" num="xxx" hour ="xxx">
	//		</time>
	//	</userconfig>
	//	<docconfig>
	//		<doc total ="xxx" startime="xxx" endtime="xxx">
	//		</doc>
	//	</docconfig>
	//	<logconfig>
	//		<log opr="vote|save" [addAttr="xxxx,xxxx"]></log>
	//	</logconfig>
	//	[<respconfig>
	//		<resp success="xxx" failure="xxx" successe="xxx"/>
	//	</respconfig>]
	//</smartdoc>
	//
	// When 'type' is 'cycle':
	// 	'starttime' format is "yyyy/mm/dd hh:mm:ss";
	// 	'num' is the number of votes one can vote in the cycle; 
	// 	'hour' must be hour (1-24). It is the cycle length. 
	// This means that 'starttime' defines the starting time of the cycle
	// and 'hour' defines the cycle length.
	//
	// When 'type' is 'interval':
	//  'num': the number of votes one can vote within the interval. 
	//  'hour': is the interval length. 
	// This means that the time when the first vote is cast, it starts
	// the interval. 
	//
	// 'docconfig' is used to control how a doc can be voted:
	// 'total': the total number of votes that can be voted for the docs.
	// 'starttime': the start time that the docs can be voted.
	// 'endtime': the end time that the docs can be voted.
	//
	aos_assert_r(rdata, false);
	AosXmlTagPtr request = rdata->getRetrievedDoc();

	if (!sdoc || !request)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!request)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr user_config = sdoc->getFirstChild("userconfig");
	AosXmlTagPtr doc_config = sdoc->getFirstChild("docconfig");
	AosXmlTagPtr log_config = sdoc->getFirstChild("logconfig");
	if ((!log_config) || (!user_config) || (!doc_config))
	{
		rdata->setError() << "Missing Conifg!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//parse config
	AosXmlTagPtr userxml = user_config->getFirstChild("time");
	AosXmlTagPtr docxml = doc_config->getFirstChild("doc");
	AosXmlTagPtr logxml = log_config->getFirstChild("log");
	if ((!userxml) || (!docxml) || (!logxml))
	{
		rdata->setError() << "Missing Xml!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//From userxml get type(interval|cycle)
	OmnString type = userxml->getAttrStr("type"); 
	if (type == "")
	{
		rdata->setError() << "Missing Mode!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//From userxml get num
	int num = userxml->getAttrInt("num", eDefaultNumVotes);
	//From  userxml get hour
	int hour = userxml->getAttrInt("hour", eDefaultNumHours);
	
	//From docxml get start time
	OmnString opr_stime = docxml->getAttrStr("starttime");
	//From  docxml get end time
	OmnString opr_etime= docxml->getAttrStr("endtime"); 

	//From  logxml get Operation (vote|save)
	OmnString opr = logxml->getAttrStr("opr");
	if (opr == "")
	{
		rdata->setError() << "Missing operation!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//get add Attr
	//Additional attributes for each opr
	OmnString addattr = logxml->getAttrStr("addAttr");

	//request
	AosXmlTagPtr objdef;
	if (addattr != "")
	{
		objdef = request->getFirstChild("objdef");
		if (!objdef)
		{
			objdef = request->getFirstChild("command");
			if (!objdef)
			{
				rdata->setError() << "Missing objdef!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}
		objdef = objdef->getFirstChild();
		if (!objdef)
		{
			rdata->setError() << "Missing objdef!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	OmnString objid, collection;
	objid = rdata->getArg1(AOSARG_OBJID);
	collection = rdata->getArg1(AOSARG_COLLECTION);
	if (objid == "")
	{
		rdata->setError() << "Missing Objid!!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!doc)
	{
		rdata->setError() << "Object not found!!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;

	}
	u64 voted_docid = doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(voted_docid, false);
	
	OmnString etime;
	OmnString crttime;
	OmnString cytime = userxml->getAttrStr("starttime");
	if (opr_stime!= "") opr_stime.removeLeadingWhiteSpace();
	if (opr_etime!= "") opr_etime.removeLeadingWhiteSpace();
	type.removeLeadingWhiteSpace();

	//Process Time
	bool rslt = process(crttime, etime, opr_stime, opr_etime, hour, type, cytime, rdata);
	if (!rslt)
	{
		rdata->setError() << "Missing Process!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (etime == "")
	{
		rdata->setError() << "Missing Time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//To get response information from the smartdoc
	OmnString smsg, ferrmsg, smsge;
	AosXmlTagPtr resp_config = sdoc->getFirstChild("respconfig");
	if (resp_config)
	{
		AosXmlTagPtr respxml = resp_config->getFirstChild("resp");
		if (respxml)
		{
			smsg = respxml->getAttrStr("success", "Success!");
			smsge = respxml->getAttrStr("success_end");
			ferrmsg = respxml->getAttrStr("failure", "Failure!");
		}
	}

	//Zky3391, Linda 2011/05/03
	OmnString ctnr_name = AOSTAG_CTNR_LOG;
	ctnr_name << opr << collection;
	OmnString query;
	aos_assert_r(ctnr_name != "", false);
	queryCond(query, etime, crttime, voted_docid, type, collection, ctnr_name, rdata);
	OmnString contents;
	parsequery(query, contents, opr, collection, ctnr_name, rdata);

	AosXmlParser parser;
	AosXmlTagPtr contents1 = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (!contents1)
	{
		rdata->setError() << "Missing Query Results!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	contents1 = contents1->getFirstChild();
	if (!contents1)
    {
	    rdata->setError() << "Missing Query Results!";
        OmnAlarm << rdata->getErrmsg() << enderr;
	    return false;
	}

	int total = contents1->getNumSubtags();
	//int total = contents1->getAttrInt("total", 0);
	if (total >= num) 
	{
		AosSetErrorU(rdata, "internal_error") << ": " << ferrmsg << enderr;
		return false;
	}
	else
	{
		rslt = createLog(voted_docid, crttime, opr, addattr, objdef, collection, ctnr_name, rdata);
		if (!rslt)
		{
	 	  	rdata->setError() << "Failure Create Log!";
       		OmnAlarm << rdata->getErrmsg() << enderr;
	   		return false;
		}
	}
	OmnString content = smsg;
	if (smsge !="") 
	{
		content << (num-total-1) << smsge;
	}
	//rdata->setError(eAosXmlInt_Ok, errmsg);
	rdata->setResults(content);
	rdata->setOk();
	return true;
}


bool
AosSdocVote::queryCond(
		OmnString &query,
		const OmnString &etime,
		const OmnString &crttime,
		const u64 &voted_docid,
		const OmnString &type,
		const OmnString &collection, 
		const OmnString &ctnr_name, 
		const AosRundataPtr &rdata)
{
	//Zky3391, Linda 2011/05/03
	query <<"<cond1 type=\"eq\" odrattr=\"true\" odrctnr=\"" << ctnr_name <<"\">"
		<<"<lhs><![CDATA[userid]]></lhs>"
		<<"<rhs><![CDATA[" << rdata->getUserid() << "]]></rhs>"
		<<"</cond1>"
		<<"<cond1 type=\"eq\" odrattr=\"true\" odrctnr=\"" << ctnr_name <<"\">";
	if (collection!="")
	{
		query<<"<lhs><![CDATA[vcontainer]]></lhs>"
			<<"<rhs><![CDATA[" << collection<< "]]></rhs>";
	}
	else
	{
		query<<"<lhs><![CDATA[vdocid]]></lhs>"
			<<"<rhs><![CDATA[" << voted_docid<< "]]></rhs>";
	}

	query << "</cond1>";

	if (strcmp(type.data(), "cycle") == 0)
	{
		query << "<cond1 type=\"gt\" odrattr=\"true\" odrctnr=\"" << ctnr_name <<"\">";
	}
	else
	{
		query << "<cond1 type=\"ge\" odrattr=\"true\" odrctnr=\"" << ctnr_name <<"\">";
	}
	query<<"<lhs><![CDATA[vtime]]></lhs>"
		<<"<rhs><![CDATA[" << etime<< "]]></rhs>"
		<<"</cond1>"
		<<"<cond1 type=\"le\" odrattr=\"true\"  odrctnr=\"" << ctnr_name <<"\">"
		<<"<lhs><![CDATA[vtime]]></lhs>"
		<<"<rhs><![CDATA[" <<crttime<< "]]></rhs>"
		<<"</cond1>";
	return true;
}

bool
AosSdocVote::createLog(
			const u64 &voted_docid,
			const OmnString &crttime,
			const OmnString &opr,
			const OmnString &addattr,
			const AosXmlTagPtr &objdef,
			const OmnString &collection,
			const OmnString &ctnr_name,
			const AosRundataPtr &rdata)
{
	// Create vote log doc
	// 1. check vote contaner 
	// if not exsit  create .
	
	// <votectnr zky_objid="AOSTAG_CTNR_LOG"+opr AOSTAG_ORDER="voted_docid, userid, time, vote_type" 
	// 			 zky_public_ctnr="true"/>

	// <votelog zky_objid="vote+seqno"  zky_pctrn="AOSTAG_CTNR_LOG"+opr zky_otp = opr
	// 			voted_docid="xxx" 
	// 			vote_uid="xxx" 
	// 			vote_time="xxxx"
	// 			vote_type=""/>
	// 			<contents ></contents>
	// </votelog>
	OmnString cmd, cmd1;	
	OmnString addvalue;
	if (addattr != "")
	{
		aos_assert_r(objdef, false);
		OmnStrParser1 parser(addattr, ",");
		OmnString word;
		while ((word = parser.nextWord())!="")
		{
			addvalue = objdef->getAttrStr(addattr);
			if (addvalue == "")
			{
				rdata->setError() << "Missing objdef!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			cmd<< word<<"=\"" << addvalue<< "\" ";
		}
	}
	
	OmnString sorted = "vdocid,userid,vtime";
	if (collection != "") 
	{
		cmd1 <<"vcontainer" <<"=\"" << collection<<"\" " ;
		sorted << ",vcontainer";
	}
	
	//AosXmlRc errcode = eAosXmlInt_General;
	//OmnString errmsg;
	u32 siteid = rdata->getSiteid();
	
	OmnString ctnrname = AOSTAG_CTNR_LOG;
	ctnrname << opr << collection;
	//Zky3391, Linda 2011/05/03
	aos_assert_r(ctnr_name == ctnrname, false);

	// Chen Ding, 09/02/2011
	// AosXmlTagPtr ctnr = AosDocClient::getSelf()->getDoc(ctnrname, siteid, rdata);
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(ctnrname, rdata);
	AosXmlTagPtr ctnr_xml;
	if (!ctnr)
	{
		OmnString docstr = "<ctnr ";
		docstr << AOSTAG_OBJID <<"=\"" << ctnrname <<"\" "
			<< AOSTAG_SORTED_ATTRS <<"=\"" << sorted <<"\" "
			<< AOSTAG_OTYPE <<"=\"" <<  AOSOTYPE_CONTAINER <<"\" "
			<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_CTNR_LOG << "\" "
			<< AOSTAG_CTNR_PUBLIC <<"=\"" << "true" <<"\" "
			<< AOSTAG_PUBLIC_DOC <<"=\"" << "true" <<"\"/>";
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		ctnr_xml = AosDocClientObj::getDocClient()->createDocSafe1(
					rdata, docstr, "", "", true, true, false, false, true, true);
		rdata->setUserid(userid);
		rdata->setAppname(appname);
		if (!ctnr_xml) 
		{
			rdata->setError() << "Failure Create Ctnr!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	u64 seqno;
	bool rslt = getSeqno(seqno, rdata);
	if (!rslt)
	{
		rdata->setError() << "Log  Seqon!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString logobjid = "log";
	logobjid <<opr << seqno;
	OmnString docstr="<log ";
	docstr <<AOSTAG_OBJID  << "=\"" << logobjid <<"\" "
		<< AOSTAG_SITEID << "=\"" << siteid <<"\" "
		<< AOSTAG_PARENTC << "=\"" << ctnrname <<"\" " 
		<< AOSTAG_CTNR_PUBLIC <<"=\"" << "true" <<"\" "
		<< AOSTAG_PUBLIC_DOC <<"=\"" << "true" <<"\" "
		<< "zky_otp" <<"=\"" << opr <<"\" "
		<< "vdocid" <<"=\"" << voted_docid<<"\" "
		<< "userid" <<"=\"" << rdata->getUserid() << "\" "
		<< "vtime"<<"=\"" << crttime << "\" "
		<< cmd
		<< cmd1
		<< " >"
		<<"<contents></contents>"
		<< "</log>";
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	AosXmlTagPtr log = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, "", "", true, false, false, false, false, false);
	rdata->setUserid(userid);
	rdata->setAppname(appname);
	if(!log) 
	{
		rdata->setError() << "Failure Create Log!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosSdocVote::parsequery(
			const OmnString &query,
			OmnString &contents,
			const OmnString &opr,
			const OmnString &collection,
			const OmnString &ctnr_name,
			const AosRundataPtr &rdata)
{
	OmnString ctnrname = AOSTAG_CTNR_LOG;
	ctnrname << opr << collection;
	//Zky3391, Linda 2011/05/03
	aos_assert_r(ctnrname == ctnr_name, false); 

	OmnString cmd = "<cmd><query ";
	cmd << "get_total" << "=\"" << "true\" "
		<< "order" << "=\"" <<"zky_objid\" "
		<< "tname" << "=\"" <<ctnrname << "\" "
		<< "rmmetadata" << "=\"" <<"false\" "
		<< "psize" << "=\"" <<"1000\" "
		<< "start_idx" << "=\"" << "-1\" "
		<< "reverse" << "=\"" << "true\" "
		<< "opr" << "=\"" << "retlist\" "
		<< "subopr" <<"=\"" <<"none\" "
		<< "fnames" << "=\"" << "vtime|$|vtime|$|1\" >"
		<< "<conds>"
		<< 		"<cond type=\"AND\">"
		<<query
		<<		"</cond>"
		<< "</conds>"
		<< "</query></cmd>";

	AosXmlParser parser;
	AosXmlTagPtr cmd1 = parser.parse(cmd, "" AosMemoryCheckerArgs);
	
	aos_assert_r(cmd1, false);
	AosQueryReqObjPtr qry = AosQuery::parseQuery(cmd1, rdata);
	if (!qry)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	bool rslt = AosQuery::doQuery(qry, cmd1, rdata);
	aos_assert_r(rslt, false);

	contents = rdata->getResults();
	if (contents == "")
	{
		rdata->setError() <<"Missing Query!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosSdocVote::getSeqno(u64 &seqno, const AosRundataPtr &rdata)
{
	// There shall be a subtag:
	// 	<sdoc ...zky_objid = "sdoc_vote_seqno">
	// 		<AOSTAG_OBJID_SEQNO_ACT .../>
	// 		...
	// 	</sdoc>
	OmnString seqnoobjid = "sdoc_vote_seqno";
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(seqnoobjid, rdata);
	if (!sdoc)
	{
		rdata->setError() <<"Missing Seqno Smart Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnNotImplementedYet;
	// ICE_WARN
	// AosActSeqno seqno_action;
	// seqno = 0;
	// bool rslt;// = seqno_action.getSeqno(seqno, sdoc, rdata);
	// aos_assert_r(rslt, false);
	return false;
}


bool
AosSdocVote::process(
		OmnString &crttime,
		OmnString &etime,
		const OmnString &opr_stime,
		const OmnString &opr_etime,
		const int hh,
		const OmnString &type,
		OmnString &cytime,
		const AosRundataPtr &rdata)
{
	ptime pt1;
	try
	{
		pt1 = second_clock::local_time();
		ostringstream os;	
		time_facet *facet2 = OmnNew time_facet("%Y/%m/%d %H:%M:%S");
		// The facet2 pointer will  construct a smartpointer in locale fuction
		os.imbue(locale(cout.getloc(), facet2));
		if (pt1.is_not_a_date_time())
		{
			rdata->setError() << "Missing Time format!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		os << pt1;
		crttime << os.str();
	}
	catch(...)
	{
		rdata->setError() << "Missing Time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (pt1.is_not_a_date_time())
	{
		rdata->setError() << "Missing Time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	aos_assert_r(crttime != "", false);
	crttime.removeLeadingWhiteSpace();
	if (opr_stime!="")
	{
		if (strcmp(crttime.data(), opr_stime.data()) < 0)
		{
			rdata->setError() << "Voting does not start!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	
	if (opr_etime!="")
	{
		if (strcmp(crttime.data(), opr_etime.data()) >0)
		{
			rdata->setError() << "Voting has ended!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	if (strcmp(type.data(), "interval") == 0)
	{
		try
		{
			ptime pt2 = pt1 - hours(hh);
			ostringstream os1;	
			time_facet *facet = OmnNew time_facet("%Y/%m/%d %H:%M:%S");
			os1.imbue(locale(cout.getloc(), facet));
			if (pt2.is_not_a_date_time())
			{
				rdata->setError() << "Missing Time format!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			os1 << pt2;
			etime = "";
			etime << os1.str();
		}
		catch(...)
		{
			rdata->setError() << "Missing Time format!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	if (strcmp(type.data(), "cycle") == 0)
	{
		//start time 
		aos_assert_r(cytime != "", false);	
		cytime.removeLeadingWhiteSpace();		
		const char *data = cytime.data();
		aos_assert_r(!strcmp(data, "") == 0, false);
		try 
		{
			ptime p1 = time_from_string(data);
			if (p1.is_not_a_date_time())
			{
				rdata->setError() << "Missing Time format!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}

			time_duration p3 = pt1-p1;
			if (p3.is_not_a_date_time())
			{
				rdata->setError() << "Missing Time format!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			//int h = p3.hours();
			//int h1 = h % hh;
			//ptime p4 = pt1 - hours(h1);
			int s = p3.total_seconds();
			int s1 = hh*60*60; 
			int s2 = s % s1;
			ptime p4 = pt1 - seconds(s2);

			etime = "";
			ostringstream os2;	
			time_facet *facet1 = OmnNew time_facet("%Y/%m/%d %H:%M:%S");
			os2.imbue(locale(cout.getloc(), facet1));
			if (p4.is_not_a_date_time())
			{
				rdata->setError() << "Missing Time format!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			os2 << p4;
			etime << os2.str();
		}
		catch(...)
		{
			rdata->setError() << "Missing Time format!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		//etime = to_simple_string(p4);
		//etime << to_iso_extended_string(p4);
	}
	return true;
}
