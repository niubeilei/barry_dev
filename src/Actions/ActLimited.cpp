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
// 06/13/2011	Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActLimited.h"

#include "Alarm/Alarm.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "CounterClt/CounterClt.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEModules/ObjMgr.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "SeLogClient/SeLogClient.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

using namespace std;

AosActLimited::AosActLimited(const bool flag)
:
AosSdocAction(AOSACTTYPE_LIMITED, AosActionType::eLimited, flag)
{
}


/*
AosActLimited::AosActLimited(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eLimited, false)
{
	if(!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
				OmnString("Missing the doc tag"));
		throw e;
	}
}
*/


AosActLimited::~AosActLimited()
{
}


/*
AosActionObjPtr
AosActLimited::clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosActLimited(def);
	}
	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
*/


bool
AosActLimited::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function updates a counter based on the smartdoc configuration.
	// sdoc:Action Doc
	// Action Doc format
	// <actions>
	// 		<action>
	//	 		<config type ="interval|cycle" cycle_stime="xxx" num="xxx" hour="xxx" total="xxx" lim_opentime="xxx" lim_closingtime="xxx"/>
	// 			<seqno zky_initvalue="xxx" zky_prefxmd="withprefix" zky_seqnotn="seqid" zky_seqnoan="seqan" zky_seqnopx="LO">
	// 				<seqno seq_idx="1" zky_initvalue="xxx"></seqno>
	// 			</seqno>
	// 			<container_name zky_value_type="const">zky_objid</container_name>
	// 			<container_sorted zky_value_type="const">vdocid,userid</container_sorted>
	// 			<log_type zky_value_type="const">vote</log_type>
	// 			<log_attrs>
	// 				<log_attr>
	// 					<log_attrname>
	// 				  		<zky_valuedef zky_value_type="const">vdocid</zky_valuedef>
	// 				  		...
	// 				  	</log_attrname>
	// 					<log_attrvalue>
	// 				  		<zky_valuedef zky_value_type="const">vdocid</zky_valuedef>
	// 				  	</log_attrvalue>
	// 				  	<log_queryopr zky_value_type="const">eq</log_queryopr>
	// 				 </log_attr>
	// 				 ...
	// 			</log_attrs>
	// 		</action>
	// </actions>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr config = sdoc->getFirstChild("config"); 
	if (!config)
	{
		rdata->setError() << "Missing Conifg!"; 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false; 
	}

	//From docxml get start time and end time
	OmnString vote_stime = config->getAttrStr("lim_opentime");
	OmnString vote_etime = config->getAttrStr("lim_closingtime");

	ptime pcrt_time;
	OmnString crt_time = getCrtTime(rdata, pcrt_time);
	if (vote_stime!="")
	{
		vote_stime.removeLeadingWhiteSpace();
		if (strcmp(crt_time.data(), vote_stime.data()) <0)
		{
			rdata->setError() << "Voting does not start!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	
	if (vote_etime!="")
	{
		vote_etime.removeLeadingWhiteSpace();
		if (strcmp(crt_time.data(), vote_etime.data()) >0)
		{
			rdata->setError() << "Voting has ended!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	//From config get num and hour, type(interval|cycle)
	int num = config->getAttrInt("num", eDefaultNumVotes);
	int hour = config->getAttrInt("hour", eDefaultNumHours);
	OmnString type = config->getAttrStr("type", "cycle");

	//Process Time
	OmnString endtime;
	OmnString cytime = config->getAttrStr("cycle_stime"); 
	if (cytime != "") cytime.removeLeadingWhiteSpace();

	bool rslt = process(rdata, pcrt_time, type, cytime, hour, endtime);

	//get Container 
	OmnString ctnr_objid = getContainerObjid(rdata, sdoc);
	aos_assert_rr(ctnr_objid != "", rdata, false);
	
	int total;
	rslt = check(rdata, sdoc, ctnr_objid, num, total, crt_time, endtime, type);
	aos_assert_rr(rslt, rdata, false);
	if (rslt)
	{
		if (total >= num)
		{
			//rdata->setError(eAosXmlInt_General, ferrmsg);
			return false;
		}
		else
		{
			rslt = createLog(rdata, sdoc, ctnr_objid, crt_time);
			if (!rslt)
			{
				rdata->setError() << "Failure Create Log!";
				return false;
			}	
		}
	}
	return true;
}

OmnString
AosActLimited::getCrtTime(
			const AosRundataPtr &rdata,
			ptime &pt1)
{
	OmnString crttime;
	//ptime pt1;
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
			return NULL;
		}
		os << pt1;
		crttime << os.str();
	}
	catch(...)
	{
		rdata->setError() << "Missing Time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return NULL;
	}

	if (pt1.is_not_a_date_time())
	{
		rdata->setError() << "Missing Time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return NULL;
	}
	
	aos_assert_r(crttime != "", NULL);
	crttime.removeLeadingWhiteSpace();
	return crttime;
}

bool
AosActLimited::process(
		const AosRundataPtr &rdata,
		const ptime &pcrt_time,
		const OmnString &type,
		const OmnString &cycletime,
		const int hh,
		OmnString &etime)

{
	if (strcmp(type.data(), "interval") == 0)
	{
		try
		{
			ptime pt2 = pcrt_time- hours(hh);
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
		const char *data = cycletime.data();
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

			time_duration p3 = pcrt_time - p1;
			if (p3.is_not_a_date_time())
			{
				rdata->setError() << "Missing Time format!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			//int h = p3.hours();
			//int h1 = h % hh;
			//ptime p4 = pcrt_time - hours(h1);
			int s = p3.total_seconds();
			int s1 = hh*60*60; 
			int s2 = s % s1;
			ptime p4 = pcrt_time - seconds(s2);

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

bool
AosActLimited::runQuery(
			const AosRundataPtr &rdata,
			const OmnString &ctnr_objid,
			const OmnString *anames,
			const OmnString *values,
			const AosOpr *opr,
			const int num_conds,
			AosQueryRsltObjPtr &query_rslt)
{
	// Never finished. Originally developed by Linda.
	OmnNotImplementedYet;
	return false;
	/*
	bool order = true;

//	fname = "vtime|$|vtime|$|1";
//	AosQueryTermPtr term = OmnNew AosTermIIL(ctnr_objid, aname, value, opr, true, order, rdata);

	AosTermAndPtr cond = OmnNew AosTermAnd();

	for (int i=0; i< num_conds; i++)
	{
		AosQueryTermPtr term = OmnNew AosTermIIL(ctnr_objid, anames[i],
					values[i], opr[i], true, order, rdata);
		cond->addTerm(term, rdata);
	}
	bool rslt = cond->loadData(rdata);
	aos_assert_rr(rslt, rdata, false);

	query_rslt = cond->getQueryData();
	return true;
	*/
}

bool
AosActLimited::check(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &sdoc,
			const OmnString &ctnr_objid,
			const int &num,
			int &total,
			const OmnString &crt_time,
			const OmnString &endtime,
			const OmnString &type)
{
	OmnString userid;
	userid << rdata->getUserid();
	OmnString anames[eMaxValues];
	OmnString values[eMaxValues];
	AosOpr opr[eMaxValues];
	int num_conds = 0;
	bool rslt = parseTerms(rdata, sdoc, anames, values, opr, num_conds);
	aos_assert_rr(rslt, rdata, false);
	//add cond
	if (crt_time != "" && endtime != "")
	{
		anames[num_conds] = "userid";
		values[num_conds] = userid; 
		opr[num_conds] = AosOpr_toEnum("ge");
		num_conds ++;
		anames[num_conds] = "vtime";
		values[num_conds] = endtime;
		if (type == "cycle")
		{	
			opr[num_conds] = AosOpr_toEnum("gt");
		}
		else 
		{
			opr[num_conds] = AosOpr_toEnum("ge");
		}
		num_conds ++;
		anames[num_conds] = "vtime";
		values[num_conds] = crt_time;
		opr[num_conds] = AosOpr_toEnum("le");
		num_conds ++;
	}

	AosQueryRsltObjPtr query_rslt;
	rslt = runQuery(rdata, ctnr_objid, anames, values, opr, num_conds, query_rslt);
	aos_assert_rr(rslt, rdata, false);
	u64 logid, value;
	AosXmlTagPtr logdoc;
	bool finished = false;
	while (!finished)
	{
		if (!query_rslt->nextDocid(logid, finished, rdata) || finished) break;
		aos_assert_rr(logid, rdata, false);
		logdoc = AosDocClientObj::getDocClient()->getDocByDocid(logid, rdata);
		aos_assert_rr(logdoc, rdata, false);
		value = logdoc->getAttrU64("AOSTAG_NUM", 0);
		total = value + total;
	}
	return true;
	
}

bool
AosActLimited::createLog(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &ctnr_objid,
		const OmnString &crttime) 
{
	// Create vote log doc
	// 1. check vote contaner 
	// if not exsit  create .
	// <votelog zky_objid="vote+seqno"  zky_pctrn="AOSTAG_CTNR_LOG"+opr zky_otp = opr
	// 			voted_docid="xxx" 
	// 			vote_uid="xxx" 
	// 			vote_time="xxxx"
	// 			vote_type=""/>
	// 			<contents ></contents>
	// </votelog>
	//
	bool rslt = createLogContainer(rdata, sdoc, ctnr_objid);
	aos_assert_r(rslt, false);
	// get Seqno
	u64 seqno;
	AosXmlTagPtr seqnotag = sdoc->getFirstChild("seqno");
	AosValueRslt valueRslt;
	//if (AosValueSel::getValue(valueRslt, seqnotag, rdata))
	//{
	//	seqno = valueRslt.getU64();
	//}
	seqno = 1;

	OmnString logtype;
	AosXmlTagPtr logtypetag = sdoc->getFirstChild("log_type");
	AosValueRslt valueRslt1;
	if (logtypetag && AosValueSel::getValueStatic(valueRslt1, logtypetag, rdata))
	{
		logtype = valueRslt1.getStr();
	}

	OmnString logvalue = getLogAttribute(rdata, sdoc);
	aos_assert_rr(logvalue != "", rdata, false);

	OmnString logobjid = "log";
	logobjid << logtype << seqno;
	OmnString docstr="<log ";
	docstr <<AOSTAG_OBJID  << "=\"" << logobjid <<"\" "
		<< AOSTAG_SITEID << "=\"" << rdata->getSiteid() <<"\" "
		<< AOSTAG_PARENTC << "=\"" << ctnr_objid<<"\" " 
		<< AOSTAG_CTNR_PUBLIC <<"=\"" << "true" <<"\" "
		<< AOSTAG_PUBLIC_DOC <<"=\"" << "true" <<"\" "
		<< "zky_otp" <<"=\"" << logtype <<"\" "
		<< logvalue
		<< "ctime"<<"=\"" << crttime << "\" "
		<< "userid" << "=\"" << rdata->getUserid() << "\" "
		<< " >"
		<<"<contents></contents>"
		<< "</log>";

	//rslt = AosSeLogClient::getSelf()->addLog(ctnr_objid, docstr, rdata);
	rslt = true;
	return rslt;
}


bool
AosActLimited::createLogContainer(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr_objid)

{
	// <votectnr zky_objid="AOSTAG_CTNR_LOG_..." AOSTAG_ORDER="....." zky_public_ctnr="true"/>
	//get container sorted
	OmnString sorted;
	AosXmlTagPtr ctnr_sortedtag = sdoc->getFirstChild("container_sorted");
	AosValueRslt valueRslt;
	if (ctnr_sortedtag && AosValueSel::getValueStatic(valueRslt, ctnr_sortedtag, rdata))
	{
		sorted = valueRslt.getStr();
	}
	aos_assert_rr(sorted != "", rdata, false);

	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);
	AosXmlTagPtr ctnr_xml;
	if (!ctnr)
	{
		OmnString docstr = "<ctnr ";
		docstr << AOSTAG_OBJID <<"=\"" << ctnr_objid<<"\" "
			<< AOSTAG_SORTED_ATTRS <<"=\"" << sorted <<"\" "
			<< AOSTAG_OTYPE <<"=\"" <<  AOSOTYPE_CONTAINER <<"\" "
			<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_CTNR_LOG << "\" "
			<< AOSTAG_CTNR_PUBLIC <<"=\"" << "true" <<"\" "
			<< AOSTAG_PUBLIC_DOC <<"=\"" << "true" <<"\"/>";
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));
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
	return true;
}

OmnString 
AosActLimited::getContainerObjid(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &sdoc)
{
	//get container objid
	OmnString ctnrname;
	AosXmlTagPtr ctnr_nametag = sdoc ->getFirstChild("container_name");
	AosValueRslt valueRslt;
	if (ctnr_nametag && AosValueSel::getValueStatic(valueRslt, ctnr_nametag, rdata))
	{
		ctnrname = valueRslt.getStr();
	}
	aos_assert_rr(ctnrname != "", rdata, "");
	OmnString ctnr_objid = AOSTAG_CTNR_LOG;
	ctnr_objid << ctnrname;
	return ctnr_objid;
}

OmnString
AosActLimited::getLogAttribute(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &sdoc)
{
	OmnString logvalue;
	//get Log attrname and attrvalue
	AosXmlTagPtr logattrstag = sdoc->getFirstChild("log_attrs");
	if (logattrstag)
	{
		//get Log attr
		AosXmlTagPtr attrtag = logattrstag->getFirstChild("log_attr");
		while (attrtag)
		{
			//get log attrname
			AosXmlTagPtr attrnametag = attrtag->getFirstChild("log_attrname");
			AosValueRslt valueRslt;
			OmnString attrname;
			if (attrnametag && AosValueSel::getValueStatic(valueRslt, attrnametag, rdata))
			{
				attrname = valueRslt.getStr();
			}

			// get log attrvalue
			AosXmlTagPtr attrvaluetag = attrtag->getFirstChild("log_attrvalue");
			AosValueRslt valueRslt1;
			OmnString attrvalue;
			if (attrvaluetag && AosValueSel::getValueStatic(valueRslt1, attrvaluetag, rdata))
			{
				attrvalue = valueRslt1.getStr();
			}

			if (attrname != "" && attrvalue != "" )
			{
				logvalue << attrname << "=\"" << attrvalue << "\" ";
			}
			attrtag = logattrstag->getNextChild();
		}
	}
	return logvalue;
}


bool
AosActLimited::parseTerms(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &sdoc,
			OmnString *anames,
			OmnString *values,
			AosOpr *opr,
			int &num_conds)
{
	num_conds = 0;	
	AosXmlTagPtr querytag = sdoc->getFirstChild("query");
	if (querytag)
	{
		//get Log attr
		AosXmlTagPtr termtag = querytag->getFirstChild("term");
		while (termtag)
		{
			//get log attrname
			AosXmlTagPtr lhstag= termtag->getFirstChild("lhs");
			AosValueRslt valueRslt;
			OmnString attrname;
			if (lhstag && AosValueSel::getValueStatic(valueRslt, lhstag, rdata))
			{
				attrname = valueRslt.getStr();
			}

			// get log attrvalue
			AosXmlTagPtr rhstag = termtag->getFirstChild("rhs");
			AosValueRslt valueRslt1;
			OmnString attrvalue;
			if (rhstag && AosValueSel::getValueStatic(valueRslt1, rhstag, rdata))
			{
				attrvalue = valueRslt1.getStr();
			}

			// get log query opr
			AosXmlTagPtr typetag = termtag->getFirstChild("type");
			AosValueRslt valueRslt2;
			OmnString oprvalue;
			if (typetag && AosValueSel::getValueStatic(valueRslt2, typetag, rdata))
			{
				oprvalue = valueRslt2.getStr();
			}

			if (attrname != "" && attrvalue != "" && oprvalue != "")
			{
				anames[num_conds] = attrname;
				values[num_conds] = attrvalue;
				opr[num_conds] = AosOpr_toEnum(oprvalue);
				aos_assert_rr(opr[num_conds], rdata, false);
				num_conds++;
			}
			termtag = querytag->getNextChild();
		}
	}
	return true;
}


AosActionObjPtr
AosActLimited::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActLimited(false);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


