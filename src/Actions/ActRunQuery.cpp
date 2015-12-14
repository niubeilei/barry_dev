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
// This action sets a value to rundata:
//
// Modification History:
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActRunQuery.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelector.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "Util/Buff.h"
#include "Util/CodeConvertion.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>

#include <boost/regex.hpp> 
using namespace std;
using namespace boost;
#if 0

AosActRunQuery::AosActRunQuery(const bool flag)
:
AosSdocAction(AOSACTTYPE_RUNQUERY, AosActionType::eRunQuery, flag)
{
}


AosActRunQuery::~AosActRunQuery()
{
}


bool	
AosActRunQuery::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//format <action zky_type="runquery" zky_run_type="loop|random|batchquery">
	//			<batch queryobjid="" max_of_record="2000" result="sendtofront|savefile" whereisvalue="attribute|node">
	//				<query_template>
	//				</query_template>
	//				<replace_entries>
	//					<entry>query_template_node_path_attribute</entry>
	//					<entry>query_template_node_path_text</entry>
	//				</replace_entries>
	//				<createdoc_action>
	//				</createdoc_action>
	//			</batch_query>
	//			<cmd pagesize="20">
	//			<conds>
	//				<cond .../>
	//				<cond .../>
	//				...
	//			</conds>
	//			<fnames>
	//				<fname>
	//					<oname>xxxx</oname>
	//					<cname>xxxx</cname>
	//					...
	//			</fnames>
	//			</cmd>
	//		 </action>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString run_type = sdoc->getAttrStr("zky_run_type", "random");
OmnScreen << "************************" << run_type << endl;


	if (run_type == "random")
	{
		//random
		AosXmlTagPtr cmd = sdoc->getFirstChild();
		aos_assert_rr(cmd, rdata, false);

		AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(cmd, rdata);
		aos_assert_rr(query && query->isGood(), rdata, false);

		OmnString contents;
		bool rslt = query->procPublic(cmd, contents, rdata);
		aos_assert_rr(rslt, rdata ,false);

		AosXmlParser parser;
		AosXmlTagPtr data = parser.parse(contents, "" AosMemoryCheckerArgs);
		aos_assert_rr(data, rdata, false);

		OmnString varname = sdoc->getAttrStr(AOSTAG_VARNAME);
		aos_assert_rr(varname != "", rdata, false);

		int num = data->getNumSubtags();
		if (num>0)
		{
			int idx = random() % num;
			AosXmlTagPtr randdoc = data->getChild(idx);
			rdata->setDocByVarWithRet(varname, randdoc, false);
		}
		else
		{
			OmnAlarm << "Query record is 0" << enderr;
			return false;
		}
	}
	else if (run_type == "batchquery")
	{
OmnScreen << "------------start batch query----------" << endl;
		struct batchquery bq; 
		bool gRSLT = parseConfig(sdoc, bq, rdata);
		aos_assert_r(gRSLT, false);

		if (bq.result == "savefile")
		{
			OmnString filepath;
			OmnString filename;
			OmnFilePtr file;
			bool r = openUserFile(bq, filepath, filename, file, rdata);
			aos_assert_r(r, false);
	
			// create doc start
			OmnString docstr = "<queryresultdoc ";
			docstr 	<< AOSTAG_RSCDIR << "=\"" << filepath << "\" "
			<< AOSTAG_RSC_FNAME << "=\"" << filename << "\" "
			<< AOSTAG_OTYPE << "=\"userfile\" "
			<< "querystatus=\"starting\" rate=\"0\"/>";
	
			AosXmlTagPtr workdoc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
			aos_assert_r(workdoc, false);
	
			rdata->setWorkingDoc(workdoc, false);
			AosXmlTagPtr doc;
			r = createDoc(bq, doc, rdata);
			aos_assert_r(r, false);
	
			OmnScreen << "run batch query : " << doc->toString() << endl;

			//create thread
			OmnThrdShellProcPtr runner = OmnNew BatchQuery(this, bq, file, doc, rdata);
			addThreadShellProc(runner, rdata);

			r = sendMsgToFront(doc, eSendWait, rdata);
			aos_assert_r(r, false);
			return true;
		}

		bool tryquery = true;
		OmnFilePtr file;
		AosXmlTagPtr tmpdoc;
		AosXmlTagPtr doc = procQuery(bq, file, tmpdoc, rdata, tryquery);

		if (tryquery && bq.result=="sendtofront")
		{
			aos_assert_r(doc, false);
			gRSLT = sendMsgToFront(doc, eSendResult, rdata);
			aos_assert_r(gRSLT, false);
			return true;
		}
			
		return false;
	}
	else if (run_type == "loop")
	{
		//loop
		OmnString argname = sdoc->getAttrStr(AOSTAG_ARGNAME, "");
		aos_assert_rr(argname != "", rdata, false);
		AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
		if (!actions)
		{
			AosSetError(rdata, AosErrmsgId::eMissingTags);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosValueRslt valueRslt;
		bool rslt = AosValueSel::getValueStatic(valueRslt, sdoc, AOSTAG_VALUEDEF, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eFailedGetValue);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		map<u64, AosXmlTagPtr> docs;
		while(valueRslt.hasMore())
		{
			u64 docid;
			if (!valueRslt.getU64()) return false;
			if (docid != 0)
			{
				AosXmlTagPtr ddoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
				aos_assert_rr(ddoc, rdata, false);
				docs.insert(pair<u64, AosXmlTagPtr>(docid, ddoc));
			}
		}
		int numfield = sdoc->getAttrInt("zky_numfield", 0);
		aos_assert_rr(numfield != 0, rdata, false);
		for(int i=0; i<numfield; i++)
		{
			map<u64, AosXmlTagPtr>::iterator pos;
			for(pos = docs.begin(); pos != docs.end(); pos++)
			{
				AosXmlTagPtr doc = pos->second;	
				aos_assert_rr(doc, rdata, false);

				time_t dctime = doc->getAttrU64("zky_ctmepo__d", 0);
				int dday = (localtime(&dctime))->tm_mday;
				int dmon = (localtime(&dctime))->tm_mon;
				int dyear = (localtime(&dctime))->tm_year;

				time_t crttime = time(0);
				int cday = (localtime(&crttime))->tm_mday;
				int cmon = (localtime(&crttime))->tm_mon;
				int cyear = (localtime(&crttime))->tm_year;

				if (dyear == cyear && dmon == cmon)
				{
					int delt = (cday - dday);
					if (delt == (i+1))
					{
						rdata->setReceivedDoc(doc, true);
						rdata->setArg1(argname, i);
						bool rslt = AosSdocAction::runActions(actions, rdata);
						if (!rslt)
						{
							OmnAlarm << "Failed docid :" << pos->first << enderr;
						}
						if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
						{
							break;
						}
						break;
					}
				}
			}
		}
		/*
		int i=0; 
		while(valueRslt.hasMore())
		{
			u64 docid = valueRslt.getU64();
			if (docid != 0)
			{
				AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
				aos_assert_rr(doc, rdata, false);

				u32 crttime = OmnGetSecond();


				rdata->setReceivedDoc(doc, true);
				rdata->setArg1(argname, i++);
				bool rslt = AosSdocAction::runActions(actions, rdata);
				if (!rslt)
				{
					OmnAlarm << "Failed docid :" << docid << enderr;
				}
				if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
				{
					break;
				}
			}
		}
		*/
	}
	rdata->setOk();
	return true;
}


bool
AosActRunQuery::BatchQuery::run()
{
	bool tryquery = false;
	mActQuery->procQuery(mBq, mFile, mDoc, mRdata, tryquery);
	mActQuery->closeUserFile(mBq, mFile, mRdata);
	return true;
}


bool
AosActRunQuery::BatchQuery::procFinished()
{
	return true;
}


bool	
AosActRunQuery::createDoc(
		const struct batchquery &bq, 
		AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	//run createdoc action 
	aos_assert_r(bq.createdoc_action, false);
	AosActionType::E type = AosActionType::toEnum("createdoc");
	if (!AosActionType:.isNull(type))
	{
		type = AosActionType::toEnum(bq.createdoc_action->getAttrStr("type"));
		if (!AosActionType:.isNull(type))
		{
			AosSetErrorU(rdata, "invalid_action:") << bq.createdoc_action->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	// Chen Ding, 2013/05/05
	// AosActionObjPtr action = AosActionObj::getAction(type, rdata);
	AosActionObjPtr action = AosActionObj::getAction(type, bq.createdoc_action, rdata);
	aos_assert_r(action, 0);
	AosActionObjPtr act = action->clone(bq.createdoc_action, rdata);
	aos_assert_r(act, false);
	bool rslt = act->run(bq.createdoc_action,rdata);
	aos_assert_r(rslt, 0);
	OmnString contents = rdata->getResults();
	AosXmlParser parser1;
	AosXmlTagPtr newdoc = parser1.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_rr(newdoc, rdata, 0);
	AosXmlTagPtr xmlroot = newdoc;
	if (newdoc->isRootTag()) xmlroot = newdoc->getFirstChild();
	AosXmlTagPtr element = xmlroot->getFirstChild();
	aos_assert_r(element, 0);
	doc = element;
	return true;
}

bool	
AosActRunQuery::parseConfig(const AosXmlTagPtr &sdoc, struct batchquery &bq, const AosRundataPtr &rdata)
{
	AosXmlTagPtr batch_query = sdoc->getFirstChild("batch_query");
	aos_assert_rr(batch_query, rdata, false);
	int max_of_record = batch_query->getAttrInt("max_of_record", 2000);
	OmnString filetype = batch_query->getAttrStr("file_type", "csv");
	OmnString codingType = batch_query->getAttrStr("coding_type", "UTF-8");
	OmnString objid = batch_query->getAttrStr("queryobjid");
	AosXmlTagPtr querydoc;
	bool hasquerydoc = false;
	if(objid != "")
	{
		querydoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
		aos_assert_rr(querydoc, rdata, false);
		hasquerydoc = true;
	}
	else
	{
		querydoc = AosRunDocSelector(rdata, batch_query, "query_obj");
		if (querydoc)
		{
			hasquerydoc = true;
		}
		else
		{
			hasquerydoc = false;
		}
	}
	OmnString whereisvalue = batch_query->getAttrStr("whereisvalue", "attribute");
	bool hasdocid = batch_query->getAttrBool("hasdocid", false);
	bool firstway = batch_query->getAttrBool("firstway", false);
	AosXmlTagPtr replace_entries = batch_query->getFirstChild("replace_entries");
	aos_assert_rr(replace_entries, rdata, false);
	AosXmlTagPtr createdoc_action = batch_query->getFirstChild("createdoc_action");
	aos_assert_rr(createdoc_action, rdata, false);
	bool is_set = createdoc_action->getAttrBool(AOSTAG_SET_CONTENTS, false);
	aos_assert_rr(is_set, rdata, false);
	OmnString result = batch_query->getAttrStr("result", "savefile");

	AosXmlTagPtr query_template = batch_query->getFirstChild("query_template");
	aos_assert_rr(query_template, rdata, false);
	AosXmlTagPtr fnames = query_template->getFirstChild("fnames");
	aos_assert_rr(fnames, rdata, false);
	AosXmlTagPtr fname = fnames->getFirstChild("fname");
	int index = 0;
	while(fname)
	{
		OmnString type = fname->getAttrStr("type");
		aos_assert_rr(type != "x", rdata, false);

		OmnString columnname;
		if (type == "h")
		{
			columnname = fname->xpathQuery("oname/_$text");
		}
		else
		{
			columnname = fname->xpathQuery("cname/_$text");
		}
		aos_assert_rr(columnname != "", rdata, false);

		pair<OmnString, OmnString> pair = make_pair(columnname, "");
		bq.fields[index] = pair;
		fname = fnames->getNextChild();
		index++;
	}

	bq.config = batch_query;
	bq.querydoc = querydoc;
	bq.whereisvalue = whereisvalue;
	bq.result = result;
	bq.hasdocid = hasdocid;
	bq.firstway = firstway;
	bq.query_template = query_template;
	bq.replace_entries = replace_entries;
	bq.createdoc_action = createdoc_action;
	bq.max_of_record = max_of_record;
	bq.hasquerydoc = hasquerydoc;
	bq.filetype = filetype;
	bq.codingType = codingType;
	return true;
}

AosXmlTagPtr	
AosActRunQuery::procQuery(	struct batchquery &bq, 
							OmnFilePtr &file, 
							AosXmlTagPtr &doc,
						  	const AosRundataPtr &rdata, 
							bool &tryquery)
{
	bool firstline = true;
	AosXmlTagPtr root = bq.querydoc;
	AosXmlTagPtr entry;
	int TOTAL = 100;
	int PROCNUM = 0;
	OmnString insertto;
	sregex_iterator it1;
	sregex_iterator end;
	regex reg("(<record.*?></record>)|(<record.*?/>)|(&lt;record.*?/&gt;)|(&lt;record.*?&gt;&lt;/record&gt;)");
	string records;
	if(bq.hasquerydoc)
	{
		if (bq.querydoc->isRootTag()) root = bq.querydoc->getFirstChild();
		insertto = root->getAttrStr("insertto", "");
		if(insertto == "cdata")
		{
			OmnString s = root->getNodeText(); 
			string s_tmp(s.data());
			records = s_tmp; 
			sregex_iterator it(records.begin(),records.end(),reg);
			if(it == end)
			{
				OmnAlarm << "not match!!! " << enderr;
				return NULL;
			}
			it1 = it;

			OmnString rcd((*it1)[0]);
			int f1 = rcd.findSubString("&lt;", 0);
			int f2 = rcd.findSubString("&gt;", 0);
			if(f1 != -1){
				rcd.replace("&lt;", "<", true);
			}
			if(f2 != -1){
				rcd.replace("&gt;", ">", true);
			}
			AosXmlParser rcdparser;
			entry = rcdparser.parse(rcd, "" AosMemoryCheckerArgs);
			TOTAL = distance(it1,end) * 100;
		}
		else
		{
			entry = root->getFirstChild();
			TOTAL = root->getNumSubtags() * 100;
		}
	}
	OmnString str = "<Contents/>";
	AosXmlParser parser1;
	AosXmlTagPtr result = parser1.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_rr(result, rdata, NULL);
	int query_num = 0;
	while(entry || !bq.hasquerydoc)
	{
		int q_num = 0;
		AosXmlTagPtr tempdoc = bq.query_template->clone(AosMemoryCheckerArgsBegin);
		if(bq.hasquerydoc)
		{
			AosXmlTagPtr e = bq.replace_entries->getFirstChild();
			while(e)
			{
				//modify template
				OmnString pathname = e->getNodeText(); 
				aos_assert_rr(pathname != "", rdata, NULL);
				OmnString aname = tempdoc->xpathQuery(pathname);
				aos_assert_rr(aname != "", rdata, NULL);
				OmnString avalue = entry->getAttrStr(aname);
				aos_assert_rr(avalue != "", rdata, NULL);
				tempdoc->xpathSetAttr(pathname, avalue);
				e = bq.replace_entries->getNextChild();
			}
		}

		AosXmlParser parser;
		int total = 1;
		int start_idx = 0;
		AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(tempdoc, rdata);
		aos_assert_rr(query && query->isGood(), rdata, NULL);
		while(!query->finished())
		{
			//query start
OmnScreen << "batch query startidx : " << start_idx << endl;

			OmnString contents;
			bool rslt = query->procPublic(tempdoc, contents, rdata);
			aos_assert_rr(rslt, rdata , NULL);
if(!bq.firstway)
{
	total = query->getTotal();	
	start_idx += tempdoc->getAttrInt("psize", 0);
	query->setStartIdx(start_idx);
	continue;
}
			AosXmlParser pageparse;
			AosXmlTagPtr page = pageparse.parse(contents, "" AosMemoryCheckerArgs);
			aos_assert_rr(page, rdata, NULL);
			AosXmlTagPtr pageroot = page;
			if (page->isRootTag()) pageroot = page->getFirstChild();
			AosXmlTagPtr pagechild = pageroot->getFirstChild();	

			total = pageroot->getAttrInt("total", 0);	
			if(result->toString().length()>=5000000&& tryquery)
			{
				tryquery = false;
				return NULL;
			}

			while(pagechild)
			{
				q_num++;
				//filter start
				pagechild->removeAttr(AOSTAG_PARTIAL_DOC_FNAMES);
				pagechild->removeAttr(AOSTAG_PARTIAL_DOC_SIGNATURE);
				if(!bq.hasdocid)
				{
					pagechild->removeAttr(AOSTAG_DOCID);
				}
				//filter end
				result->addNode(pagechild);
				pagechild = pageroot->getNextChild();
				//save file start
				if(!tryquery)
				{
					if(result->toString().length()>60000)
					{
OmnScreen << "save file size : " << result->toString().length() << endl;
						//update process
						if(total > 0 && TOTAL > 0)
						{
							PROCNUM = query_num * 100 + q_num * 100 / total;
							int rate = PROCNUM * 100 / TOTAL;
							OmnString value;
							value << rate;
							bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
								doc->getAttrU64(AOSTAG_DOCID, 0), 
								doc->getAttrStr(AOSTAG_OBJID), 
								"rate", value, "", true, false, true);
							if (!rslt)
							{
								OmnAlarm << "modify percent failed!" << enderr;
							}
						}
						saveQueryToFile(bq, result, file, true, firstline, rdata);
						firstline = false;
						//remove the elements of result
						result->removeAllChildren();
					}
				}
			//save file end 
			}
OmnScreen << "page total: " << q_num << endl;

			int n = pageroot->getAttrInt("num", 0);
			if(n==0)
			{
				break;
			}
			start_idx += pageroot->getAttrInt("num", 0);
//			tempdoc->setAttr("start_idx", start_idx);
			query->setStartIdx(start_idx);
			//query end 
		}

if(!bq.firstway)
{
	return result;
}
			
		//save tail of result start
		if(!tryquery)
		{
			if(result->getFirstChild())
			{
OmnScreen << "save the tail size of the file : " << result->toString().length() << endl;
				saveQueryToFile(bq, result, file, true, firstline, rdata);
				firstline = false;
				//remove the elements of result
				result->removeAllChildren();
			}

			//update process
			if(total > 0 && TOTAL > 0)
			{
				PROCNUM = query_num * 100 + q_num * 100 / total;
				int rate = PROCNUM * 100 / TOTAL;
				OmnString value;
				value << rate;
				bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
					doc->getAttrU64(AOSTAG_DOCID, 0), 
					doc->getAttrStr(AOSTAG_OBJID), 
					"rate", value, "", true, false, true);
				if (!rslt)
				{
					OmnAlarm << "modify percent failed!" << enderr;
				}
			}
		}
		//save tail of result end 
		if(bq.hasquerydoc)
		{
			if(insertto == "cdata")
			{
				it1++;
				if(it1 == end)
				{
					break;
				}
				OmnString rcd((*it1)[0]);
				int f1 = rcd.findSubString("&lt;", 0);
				int f2 = rcd.findSubString("&gt;", 0);
				if(f1 != -1){
					rcd.replace("&lt;", "<", true);
				}
				if(f2 != -1){
					rcd.replace("&gt;", ">", true);
				}
				AosXmlParser rcdparser;
				entry = rcdparser.parse(rcd, "" AosMemoryCheckerArgs);
			}
			else
			{
				entry = root->getNextChild();
			}
		}
		else
		{
			break;
		}
		query_num++;
	}

	OmnString value = "100";
	bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
		doc->getAttrU64(AOSTAG_DOCID, 0), 
		doc->getAttrStr(AOSTAG_OBJID), 
		"rate", value, "", true, false, true);

	rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
		doc->getAttrU64(AOSTAG_DOCID, 0), 
		doc->getAttrStr(AOSTAG_OBJID), 
		"querystatus", "finish", "", true, false, true);

	if (!rslt)
	{
		OmnAlarm << "modify percent failed!" << enderr;
	}
	return result;
}


bool
AosActRunQuery::openUserFile(const struct batchquery &bq, OmnString &filepath, OmnString &filename, OmnFilePtr &file, const AosRundataPtr &rdata)
{
	u64 userid = rdata->getUserid();
	aos_assert_rr(userid != 0, rdata ,false);
	AosUserAcctObjPtr userdoc = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
	aos_assert_rr(userdoc, rdata ,false);
	OmnString cid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	OmnString zky_rscfn = "act_run_query_";
	zky_rscfn << OmnGetTime("YYYYMMDDhhmmss") << "_"
		<< (unsigned int)OmnGetCurrentThreadId() << "_"
		<< OmnRandom::letterDigitStr(6,6)
		<< "." << bq.filetype;

	OmnString usedir = userdoc->getAttrStr(AOSTAG_USER_RSCDIR);
	OmnString dir = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");
	OmnString fdir;
	if (dir[0] == '/')
	{
		fdir = dir;
		fdir << "/" << usedir;
	}
	else
	{
		fdir = OmnApp::getAppBaseDir();
		fdir << "/" << dir << "/" << usedir;
	}

	OmnString cmmd = "mkdir -p ";
	cmmd << fdir;
	system(cmmd.data());

	fdir << "/" << zky_rscfn;
OmnScreen << "file name:" << fdir << endl;
	file = OmnNew OmnFile(fdir, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_rr(file->isGood(), rdata ,false);
	filepath = usedir;
	filename = zky_rscfn;
	return true;
}

bool
AosActRunQuery::closeUserFile(const struct batchquery &bq, OmnFilePtr &file, const AosRundataPtr &rdata)
{
	// Ken Lee, 2013/05/29
	//file->closeFile();
	file = 0;
	return true;
}

bool	
AosActRunQuery::saveQueryToFile(struct batchquery &bq, 
								AosXmlTagPtr &doc, 
								OmnFilePtr &file, 
								const bool savefile,
								bool firstline,
								const AosRundataPtr &rdata)
{
	OmnString addcontents;
	if (firstline)
	{
		OmnString cellname;
		map<int, pair<OmnString, OmnString> >::iterator it;
		for ( it=bq.fields.begin() ; it != bq.fields.end(); it++ )
		{
			cellname = (*it).second.first;
			if (it != bq.fields.begin())
			{
				addcontents << ",";
			}
			addcontents << cellname;
		}

		addcontents << "\r\n";
	}

	//change format start
	AosXmlTagPtr xmlroot;
	if (doc->isRootTag()) xmlroot = doc->getFirstChild();
	AosXmlTagPtr element = xmlroot->getFirstChild();

	OmnString line, cellname, rowvalue;

	while(element)
	{
		line = "";

		map<int, pair<OmnString, OmnString> >::iterator it;
		for ( it=bq.fields.begin() ; it != bq.fields.end(); it++ )
		{
			cellname = (*it).second.first;
			rowvalue = "";
			if (bq.whereisvalue == "node")
			{
				rowvalue = element->getNodeText(cellname);
			}
			else
			{
				rowvalue = element->getAttrStr(cellname);
			}

			if (it != bq.fields.begin())
			{
				line << ",";
			}

			if (bq.filetype == "csv")
			{
				line << "\"" << rowvalue << "\"";
			}
			else
			{
				line << rowvalue;
			}
		}

		addcontents << line << "\r\n";

		element = xmlroot->getNextChild();
	}

	//change format end
	if (bq.codingType != "UTF-8")                                                       
	{
		u64 bufflen = addcontents.length() * 2;
		AosBuffPtr buff = OmnNew AosBuff(bufflen AosMemoryCheckerArgs);

		u64 descLen = CodeConvertion::convert("UTF-8", bq.codingType.getBuffer(),
				addcontents.getBuffer(), addcontents.length(),
				buff->data(), bufflen);

		buff->setDataLen(descLen);
		addcontents.setLength(0);
		addcontents << buff->data();
	}

	file->append(addcontents);
	return true;
}


bool	
AosActRunQuery::sendMsgToFront(
		const AosXmlTagPtr &doc,
		const int method,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);

	switch(method)
	{
	case eSendResult:
		 {
			doc->setAttr("zky_result", "doc");
			rdata->setResults(doc->toString());
		 }
		 break;
	case eSendWait:
		 {
			OmnString rslts;
			rslts << "<Contents zky_result=\"wait\" zky_docid=\"" <<
				doc->getAttrStr(AOSTAG_DOCID) << "\"></Contents>";
			rdata->setResults(rslts);
		 }
		 break;
	}
	return true;
}


AosActionObjPtr
AosActRunQuery::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRunQuery(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
#endif
