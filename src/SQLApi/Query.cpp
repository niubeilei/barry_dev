////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/29/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SQLApi/SqlApi.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



AosXmlTagPtr
AosSqlApi::queryDocsByAttrs(
		const u32 siteid, 
		const OmnString &attr1, 
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2, 
		const AosOpr opr2,
		const OmnString &value2, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"";
	query << AosOpr_toStr(opr1) << "\">"
		<< "<lhs><![CDATA[" << attr1 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value1 <<"]]></rhs>"
		<<"</cond>"
		<<"<cond type=\""
		<< AosOpr_toStr(opr2) << "\">"
		<< "<lhs><![CDATA[" << attr2 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value2 <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = "false";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocsByAttrs(
		const u32 siteid, 
		const OmnString &attr, 
		const AosOpr opr,
		const OmnString &value, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"";
	query << AosOpr_toStr(opr) << "\">"
		<< "<lhs><![CDATA[" << attr <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = "";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocsByAttrs(
		const u32 siteid, 
		const OmnString &attr1, 
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2, 
		const AosOpr opr2,
		const OmnString &value2, 
		const OmnString &attr3, 
		const AosOpr opr3,
		const OmnString &value3, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"";
	query << AosOpr_toStr(opr1) << "\">"
		<< "<lhs><![CDATA[" << attr1 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value1 <<"]]></rhs>"
		<<"</cond>"
		<<"<cond type=\""
		<< AosOpr_toStr(opr2) << "\">"
		<< "<lhs><![CDATA[" << attr2 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value2 <<"]]></rhs>"
		<<"</cond>"
		<<"<cond type=\""
		<< AosOpr_toStr(opr3) << "\">"
		<< "<lhs><![CDATA[" << attr3 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value3 <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = "false";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query, queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocsByContainer(
		const u32 siteid, 
		const OmnString &container, 
		const int startidx,
		const int queryid)
{
	const int psize = 20;
	const OmnString order = "";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, container, order, fnames, "",queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocByObjid(
		const u32 siteid, 
		const OmnString &objid,
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"eq\">";
	query<<"<lhs>zky_objid</lhs>"
		<<"<rhs><![CDATA["<< objid <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames<<AOSTAG_OBJID << "|$$|" << AOSTAG_DOCID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocByDocid(
		const u32 siteid, 
		const OmnString &docid,
		const int startidx,
		const int queryid)
{

	OmnString query = "<cond type=\"eq\">";
	query<< "<lhs>zky_docid</lhs>"
		<< "<rhs><![CDATA[" << docid << "]]></rhs>"
		<< "</cond>";

	const int psize = 5;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize,"", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocsByWords(
		const u32 siteid, 
		const OmnString &words, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<words type=\"wd\">";
	query << words << "</words>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	AosXmlTagPtr xml = queryDocs(siteid,  startidx, psize, "", order, fnames, query,queryid);
	return xml;
}


AosXmlTagPtr
AosSqlApi::queryDocsByTags(
		const u32 siteid, 
		const OmnString &tags, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<tags type=\"tg\">";
	query << tags << "</tags>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSqlApi::queryDocs(
		const u32 siteid, 
		const int startidx,
		const int psize,
		const OmnString &query,
		const OmnString &fields,
		const int queryid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name=\"" << AOSTAG_SITEID << "\">" 
		<< "<![CDATA["<< siteid <<"]]>"<< "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<command>"
		<< "<cmd start_idx=\"" << startidx << "\""
		<< " psize=\"" << psize << "\"";

	if(queryid >= 0)
	{
		req << " queryid=\"" << queryid<< "\"";
	}
	req << " opr=\"" << "retlist" << "\">"
		<< "<conds>"
		<< 	  "<cond type=\"AND\">"
		<< query
		<<    "</cond>"
		<< "</conds>"
		<< "<fnames>"
		<< fields
		<< "</fnames>"
		<< "</cmd></command>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	// The response should be in the form:
	// 	<response>
	// 		<status .../>
	// 		<transid .../>
	// 		<Contents total="xxx"
	// 			queryid="xxx"
	// 			num="xxx">
	// 			<record .../>
	// 			<record .../>
	// 			...
	// 		</Contents>
	// 	</response>
	//
	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}


AosXmlTagPtr
AosSqlApi::queryDocs(
		const u32 siteid, 
		const int startidx,
		const int psize,
		const OmnString &ctnrs,
		const OmnString &order,
		const OmnString &fields,
		const OmnString &query,
		const int queryid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name=\"" << AOSTAG_SITEID << "\">" 
		<< "<![CDATA["<< siteid <<"]]>"<< "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<command>"
		<< "<cmd start_idx=\"" << startidx << "\"";
	if(order != "")
	{
		req << " order=" << "\"" << order << "\"";
	}

	req << " psize=\"" << psize << "\"";

	if(queryid >= 0)
	{
		req << " queryid=\"" << queryid<< "\"";
	}
	req	<< " ctnrs=\"" << ctnrs << "\""
		<< " fnames=\"" << fields << "\""
		<< " opr=\"" << "retlist" << "\">"
		<< "<conds>"
		<< 	  "<cond type=\"AND\">"
		<< query
		<<    "</cond>"
		<< "</conds>"
		<< "</cmd></command>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	// The response should be in the form:
	// 	<response>
	// 		<status .../>
	// 		<transid .../>
	// 		<Contents total="xxx"
	// 			queryid="xxx"
	// 			num="xxx">
	// 			<record .../>
	// 			<record .../>
	// 			...
	// 		</Contents>
	// 	</response>
	//
	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}


AosXmlTagPtr
AosSqlApi::queryDocByWords(
		const u32 siteid, 
		const OmnString &words, 
		const u64 &docid)
{
	int queryid = -1;
	int startidx = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByWords(siteid, words, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr<<docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		int num = contents->getAttrInt("num", -1);
		queryid = contents->getAttrInt("queryid",-1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;

}


AosXmlTagPtr
AosSqlApi::queryDocByContainer(
		const u32 siteid, 
		const OmnString &container, 
		const u64 &docid)
{
	int startidx = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	int queryid = -1;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByContainer(siteid, container, startidx,queryid);
		if (!rslts) return 0;

		// The results are in the form:
		// 	<response>
		// 		<status .../>
		// 		<transid .../>
		// 		<Contents total="xxx"
		// 			queryid="xxx"
		// 			num="xxx">
		// 			<record .../>
		// 			<record .../>
		// 			...
		// 		</Contents>
		// 	
		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr<<docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr
AosSqlApi::queryDocByTags(
		const u32 siteid, 
		const OmnString &tags, 
		const u64 &docid)
{
	int startidx = -1;
	int guard = 0;
	int queryid = -1;
	u64 curdocid = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByTags(siteid, tags, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			curdocid = child->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
//cout << "docid: " << curdocid << endl;
			if (curdocid == docid)
			{
				docidstr<<docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1,queryid);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;

}


AosXmlTagPtr
AosSqlApi::queryDocByAttrs(
		const u32 siteid, 
		const OmnString &attr,
		const AosOpr opr,
		const OmnString &value, 
		const u64 &docid)
{
	int startidx = -1;
	int queryid = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByAttrs(siteid, attr, opr, value, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr << docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr
AosSqlApi::queryDocByAttrs(
		const u32 siteid, 
		const OmnString &attr1,
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2,
		const AosOpr opr2,
		const OmnString &value2, 
		const u64 &docid)
{
	int startidx = -1;
	int queryid = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByAttrs(siteid, attr1, opr1, value1, attr2, opr2, value2, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr << docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr
AosSqlApi::queryDocByAttrs(
		const u32 siteid, 
		const OmnString &attr1,
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2,
		const AosOpr opr2,
		const OmnString &value2, 
		const OmnString &attr3,
		const AosOpr opr3,
		const OmnString &value3, 
		const u64 &docid)
{
	int startidx = -1;
	int queryid = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByAttrs(siteid, attr1, opr1, value1, 
					attr2, opr2, value2, attr3, opr3, value3, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr << docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	return 0;
}


// This function used to be:
// AosSqlApi::readDocByDocid(const u32 siteid, const u64 &docid)
// Its name is changed.
AosXmlTagPtr 
AosSqlApi::queryAndRetrieveByDocid(const u32 siteid, const u64 &docid)
{
	// This function does the following:
	// 1. Using query-by-docid to retrieve the doc from the server.
	// 2. After retrieved the doc, get the objid. 
	// 3. Use the objid to retrieve the doc from the server again.
	const int startidx = -1;
	OmnString doci;
	doci << docid;
	AosXmlTagPtr resproot = queryDocByDocid(siteid, doci, startidx);
	AosXmlTagPtr child = resproot->getFirstChild("Contents");
	AosXmlTagPtr record = child->getFirstChild("record");

	if (!record) return 0;

	OmnString objid = record->getAttrStr(AOSTAG_OBJID);
	AosXmlTagPtr doc = retrieveDoc(siteid, objid, "", 0, "", 0);
	aos_assert_r(doc, 0);
	AosXmlTagPtr child1 = doc ->getFirstChild("Contents");
	AosXmlTagPtr child2 = child1 ->getFirstChild();

	OmnString data = child2->toString();
	AosXmlParser parser;
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


bool
AosSqlApi::query(
		const AosXmlTagPtr &doc,
		const u32 siteid,
		const OmnString &ssid,
		const u64 &urldocid,
		const u64 &docid)
{
	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);
	if (otype == "")
	{
		OmnAlarm << "Doc missing otype " << enderr;
		return false;
	}
	OmnString ctime = doc->getAttrStr(AOSTAG_CTIME);
	if (ctime == "")
	{
		OmnAlarm << "Doc missing ctime " << enderr;
		return false;
	}

    OmnString creator = doc->getAttrStr(AOSTAG_CREATOR);
	if (creator == "")
	{
		OmnAlarm << "Doc missing creator " << enderr;
		return false;
	}

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	if (objid == "")
	{
		OmnAlarm << "Doc missing objid " << enderr;
		return false;
	}
	if (docid == 90059)
		OmnMark;
		
	AosXmlTagPtr xml;
	//int i = rand()%4;
	int i = rand()%4;
	OmnScreen << "-------        i: " << i << endl;
	if(i == 3)
	{
	   sgFlag ++ ;
	   if(sgFlag == 10)
	   {
	       xml = QueryNe(otype, ctime, docid);
		   sgFlag = 0 ;
	   }
	   else
	   {
	       return true;
	   }
	}
	switch (i)
	{
	case 0://==
			xml = QueryEq(otype, ctime, creator, docid);
			break;
	case 1: //<=
		    xml = QueryLe(otype, ctime, docid);
			break;
	case 2: //>=
			xml = QueryGe(otype, ctime, docid);
			break;
	/*
	case 3: //!=
			//xml = QueryNe(otype, ctime, docid);
			break;
			*/
	}
		
	OmnScreen <<"-------Success : " <<" docid: "<< docid <<" objid: "<< objid <<"  i:" << i << endl ;
	if (i== 3)
	{
		if (xml) 
		{
			OmnAlarm << "Doc missing docid/i " << docid << ":" << i << enderr;
			return false;
		}
		//aos_assert_r(!xml, false);
	}
	if (i != 3)
	{
		if (!xml)
		{
			OmnAlarm << "Doc missing docid/i " << docid << ":" << i << enderr;
			return false;
		}
		aos_assert_r (xml, false);
		AosXmlTagPtr contents = xml->getFirstChild("Contents");
		aos_assert_r (contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		aos_assert_r (child,  false);
		aos_assert_r (child->getAttrStr(AOSTAG_OBJID) == objid, false);
		aos_assert_r (child->getAttrU64(AOSTAG_DOCID,0) == docid, false);
		AosXmlTagPtr xmldoc = retrieveDocByObjid(siteid, ssid, urldocid, objid);
		aos_assert_r (xmldoc->getAttrStr(AOSTAG_OTYPE) == otype, false);
		aos_assert_r (xmldoc->getAttrStr(AOSTAG_CTIME) == ctime, false);
		aos_assert_r (xmldoc->getAttrStr(AOSTAG_CREATOR) == creator, false);

	}
	return true;
}


AosXmlTagPtr
AosSqlApi::QueryEq(
			const OmnString &otype,
			const OmnString &ctime,
			const OmnString &creator,
			const u64 &docid)
{
	//==
	u32 siteid = mSiteid;
	aos_assert_r(otype != "" && ctime != "" && creator != "", 0); 
	AosOpr opr = AosOpr_toEnum("==");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr, otype,
			AOSTAG_CTIME, opr, ctime, AOSTAG_CREATOR, opr, creator, docid);
	return xmldoc;
}


AosXmlTagPtr
AosSqlApi::QueryLe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid)
{
	//<=
	aos_assert_r(otype != "" && ctime != "", 0); 
	u32 siteid = mSiteid;
	AosOpr opr = AosOpr_toEnum("<=");
	AosOpr opr1 = AosOpr_toEnum("==");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr1, otype,
			AOSTAG_CTIME, opr, ctime, docid);
	return xmldoc;
}


AosXmlTagPtr
AosSqlApi::QueryGe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid)
{
	//>=
	aos_assert_r(otype != "" && ctime != "", 0); 
	u32 siteid = mSiteid;
	AosOpr opr = AosOpr_toEnum(">=");
	AosOpr opr1 = AosOpr_toEnum("==");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr1, otype,
				AOSTAG_CTIME, opr, ctime, docid);
	return xmldoc;
}


AosXmlTagPtr
AosSqlApi::QueryNe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid)
{
	//!=
	aos_assert_r(otype != "" && ctime != "" , 0); 
	u32 siteid = mSiteid;
	AosOpr opr = AosOpr_toEnum("!=");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr, otype,
				AOSTAG_CTIME, opr, ctime, docid);
	return xmldoc;
}


bool
AosSqlApi::isValidCloudid(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &cid)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">checkcid</item>"
		<< "<item name=\"args\">cloudid=" << cid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") return "";

	child = resproot->getNextChild();
	if (!child) return false;
	OmnString rslt = child->getAttrStr("rslt");
	return (rslt == "true");
}


bool
AosSqlApi::checkCloudid(
		const u32 siteid,
		const OmnString &cloudid,
		bool	&exist) 
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">checkcid</item>"
		<< "<item name=\"args\">cloudid::" << cloudid << "</item>";
	req << "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	
	AosXmlTagPtr contents = child->getFirstChild("Contents");
	aos_assert_r(contents, false);
	
	OmnString rslt = contents->getAttrStr("rslt", "");
	if(rslt == "true")	exist = true;
	if(rslt == "false")	exist = false;
	return true;
}


AosXmlTagPtr
AosSqlApi::queryDocsByAttrs(
		const u32 siteid,
		const int &numconds, 
		const AosSqlApi::QueryType *types,
		const OmnString *attrs, 
		const AosOpr *oprs,
		const OmnString *values,
		const bool *orders,
		const bool *reverses,
		const int psize,
		const bool reverse,
		const int startidx,
		const int queryid)
{
	aos_assert_r(numconds>0 && numconds <= eMaxQueryCond,0);
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name=\"" << AOSTAG_SITEID << "\">" 
		<< "<![CDATA["<< siteid <<"]]>"<< "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<command>"
		<< "<cmd "
		<< "psize='" << psize << "' ";
	 
	if(reverse)
	{
		req << "reverse='false' ";
	}
	else
	{
		req << "reverse='true' ";
	}
	if(queryid)
	{
		req << "queryid='" << queryid << "' ";
	}
	req << "sortflag='false' increase='false' opr='retlist' ";
	req << "start_idx='" << startidx << "'> ";
	
	req << "<conds><cond type=\"AND\">";
	for(int i= 0;i < numconds;i++)
	{
		switch(types[i])
		{
			case eQueryHit:
				req << "<term type=\"wordand\" reserse =\"";
				if(reverses[i]){
					req << "true";
				}else{
					req << "false";
				}
				req << "\" order=\"";
				if(orders[i]){
					req << "true";
				}else{
					req << "false";
				}
//				req << "\" zky_ctobjid=\"giccreators_querynew_term_h\">";
//				req << "<selector  aname=\"" << attrs[i] << "\"/>";
//				req << "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[";
//				req << attrs[i] << "]]>";
//				req << "</cond></term>";

				req << "\" >";
				req << "<selector type=\"keywords\" aname=\"" << attrs[i] << "\"/>";
				req << "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[";
				req << attrs[i] << "]]>";
				req << "</cond></term>";
				break;
			case eQueryStr:
			case eQueryU64:
				req << "<term type=\"arith\" reserse =\"";
				if(reverses[i]){
					req << "true";
				}else{
					req << "false";
				}
				req << "\" order=\"";
				if(orders[i]){
					req << "true";
				}else{
					req << "false";
				}
				req << "\" zky_ctobjid=\"giccreators_querynew_term_h\">";
				req << "<selector type=\"attr\" aname=\"";
				req << attrs[i];
				req << "\"/>";
				req << "<cond type=\"arith\" zky_opr=\"";
				req << AosOpr_toStr(oprs[i]);
				req << "\"><![CDATA[";
				req << values[i] << "]]>";
				req << "</cond></term>";
				break;
			default:
				aos_assert_r(0,0);
		}
	}
	req << "</cond></conds>";
//	req << "<fnames>"
//	req << "	<fname type="1" zky_ctobjid="giccreators_query_fname_h">
//	req << "		<oname><![CDATA[realname]]></oname>
//	req << "		<cname><![CDATA[realname]]></cname>
//	req << "	</fname>
//	req << "
//	req << "	<fname type="1" zky_ctobjid="giccreators_query_fname_h">
//	req << "		<oname><![CDATA[stime]]></oname>
//	req << "		<cname><![CDATA[stime]]></cname>
//	req << "	</fname>
//	req << "	
//	req << "	<fname type="1" zky_ctobjid="giccreators_query_fname_h">
//	req << "		<oname><![CDATA[checked]]></oname>
//	req << "		<cname><![CDATA[checked]]></cname>
//	req << "	</fname>
//	req << "</fnames>
	req << "<fnames>"
		<< "<fname type=\"1\" zky_ctobjid=\"zky_docid\">"
		<< "<oname><![CDATA[realname]]></oname>"
		<< "<cname><![CDATA[realname]]></cname>"
		<< "</fname>"
		<< "</fnames>";

	req << "</cmd>";
	req << "</command>" << "</request>";
	
	//send request
	OmnString errmsg;
	OmnString resp;


	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	//receive response
	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}		 
#endif
