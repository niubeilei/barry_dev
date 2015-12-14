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
//	12/07/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "StoreQuery/StoreQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"
#include "SEInterfaces/QueryReqObj.h"
#include "API/AosApiC.h"
#include "API/AosApiM.h"
#include "API/AosApiR.h"
#include "API/AosApiG.h"

#include "Util/File.h"
#include "Random/RandomUtil.h"
#include "Util/CodeConvertion.h"
#include "SEInterfaces/DocClientObj.h"

#include <boost/regex.hpp> 
using boost::regex;

AosStoreQuery::AosStoreQuery(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
mBlockIdx(0),
mAllRecords(0),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	setConfig(config, mRundata);
}

AosStoreQuery::~AosStoreQuery()
{
//	OmnScreen << "destruct : address " << (long)this << endl;
}

bool
AosStoreQuery::setConfig(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	//config :
	//<stored_query zky_rsltdoc_ctnr=”xxx” zky_max_records=”total records of blocks ” zky_blocksize=”xxx”>   //zky_blocksize is total records of one block
	//		<query>
	//			<conds>
	//				<cond  type="AND">
	//					<term>
	//						<selector type="iilname"><![CDATA[]]></selector>
	//						<cond zky_opr="r1"><![CDATA[]]></cond>
	//					</term>
	//				</cond>
	//
	//				...
	//				<cond  type="AND">
	//					<term/>
	//				</cond>
	//			</conds>
	//			<fnames>
	//				<fname type="1">
	//					<oname><![CDATA[realname]]></oname>
	//					<cname><![CDATA[realname]]></cname>
	//				</fname>
	//				...
	//				<fname type="1">
	//					<oname><![CDATA[checked]]></oname>
	//					<cname><![CDATA[checked]]></cname>
	//				</fname>
	//			</fnames>
	//		</query>
//			<replace_entries>
//					<entry><![CDATA[conds/cond/term/selector]]></entry>
//					...
//					<entry><![CDATA[conds/cond/term/selector]]></entry>
//			</replace_entries>
//			<querydoc>
//				<entry attr1="xxx" attr2="xxx" />
//				...
//				<entry attr1="xxx" attr2="xxx" />
//			</querydoc>
	//		<schedule zky_scheduletype=”time_int|timeofday|dayofweek|dayofmonth” zky_queryapp_id=”xxx” zky_priority=”xxx” time="xxx"/>  //ignore this node
	//		<results .../>   //attributes for stored query result doc
	//</stored_query>
	// 
	//query result doc format:
	//
	//<result zky_pctrs="xxx" zky_scheduletype="xxx" time="xxx" finishedtime="xxx" querystatus="xxx"
	//		total_record="xxx" resultsize="xxx" starttime="xxx" docids="xxx" ...>
	//</result>
	aos_assert_r(config, false);
	mConfig.rsltdoc_ctnr = config->getAttrStr("zky_rsltdoc_ctnr", "");
	aos_assert_r(mConfig.rsltdoc_ctnr != "", false);
	mConfig.binaryctnr = config->getAttrStr("zky_binarydoc_ctnr", "");
	aos_assert_r(mConfig.binaryctnr != "", false);
	mConfig.savetype = config->getAttrStr("savetype", "savefile");
	aos_assert_r(mConfig.savetype != "", false);
	mConfig.querynode = config->getFirstChild("query");
	aos_assert_r(mConfig.querynode, false);
	mConfig.replace_entries = config->getFirstChild("replace_entries");
	mConfig.querydoc = config->getFirstChild("querydoc");
	bool hasquerydoc = false;
	if(mConfig.querydoc)
	{
		AosXmlTagPtr ele = mConfig.querydoc->getFirstChild();
		if(ele)
		{
			hasquerydoc = true;
		}
	}
	else
	{
		hasquerydoc = false;
	}
	mConfig.hasquerydoc = hasquerydoc;
	mConfig.schedulenode = config->getFirstChild("schedule");
	mConfig.downloaddoc = config->getFirstChild("downloaddoc");
	aos_assert_r(mConfig.schedulenode, false);

	mConfig.resultnode = config->getFirstChild("results");

	OmnString doc = "<queryresultdoc><docids/></queryresultdoc>";
	AosXmlParser parser;
	mQueryResultDoc = parser.parse(doc, "" AosMemoryCheckerArgs); 
	aos_assert_r(mQueryResultDoc, false);
	mQueryResultDoc->setAttr(AOSTAG_HPCONTAINER, mConfig.rsltdoc_ctnr);
	mQueryResultDoc->setAttr("zky_scheduletype", mConfig.schedulenode->getAttrStr("zky_scheduletype", ""));
	mQueryResultDoc->setAttr("time", mConfig.schedulenode->getAttrStr("time", ""));
	mQueryResultDoc->setAttr("querystatus", "starting");
	mQueryResultDoc->setAttr("rate", "0");
	AosXmlTagPtr attrnode = mConfig.resultnode;	
	if(attrnode)
	{
		for(int i=0; i<attrnode->getNumAttrs(); i++)
		{
			OmnString name;
			OmnString value;
			attrnode->getAttr(i, name, value);
			mQueryResultDoc->setAttr(name, value);
		}
	}
	
	OmnString objid;
	objid << "home." << mRundata->getCid();
	AosXmlTagPtr homedoc = AosGetDocByObjid(objid, mRundata);
	if(!homedoc)
	{
		OmnString hdstr;
		hdstr << "<container zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_objid=\"" << objid << "\" zky_otype=\"zky_ctnr\" zky_pctrs=\"_zt3e\" zky_crtor=\"" << mRundata->getCid() << "\"/>";
		AosXmlParser hdparser;
		AosXmlTagPtr homedoc = hdparser.parse(hdstr, "" AosMemoryCheckerArgs); 
		AosXmlTagPtr r = AosCreateDoc(homedoc, mRundata);
		aos_assert_r(r,  false);
	}

	AosXmlTagPtr fnames = mConfig.querynode->getFirstChild("fnames");
	bool rslt = AosQrUtil::parseFnames(mRundata, fnames, mFieldDef);
	aos_assert_rr(rslt, mRundata, NULL);
	mQueryResultDoc->addNode(mConfig.querynode);	

	AosXmlTagPtr clonexml = mQueryResultDoc->clone(AosMemoryCheckerArgsBegin);
	mQueryResultDoc= AosCreateDoc(clonexml, mRundata);
	aos_assert_r(mQueryResultDoc, false);

	//create user file
	int type = sSaveType::toEnum(mConfig.savetype);
	if(type == sSaveType::eSaveFile)
	{
		aos_assert_r(createUserFile(), false);
	}
	
	return true;
}

bool
AosStoreQuery::runQuery()
{

	OmnString starttime = OmnGetTime(AosLocale::getDftLocale());
	aos_assert_r(mQueryResultDoc, false);
	mQueryResultDoc->setAttr("starttime", starttime);

	AosXmlTagPtr entry;
	int TOTAL = 100;
	OmnString insertto;
	boost::sregex_iterator it1;
	boost::sregex_iterator end;
	regex reg("(<record.*?></record>)|(<record.*?/>)|(&lt;record.*?/&gt;)|(&lt;record.*?&gt;&lt;/record&gt;)");
	string records;
	if(mConfig.hasquerydoc)
	{
		insertto = mConfig.querydoc->getAttrStr("insertto", "");
		if(insertto == "cdata")
		{
			OmnString s = mConfig.querydoc->getNodeText(); 
			string s_tmp(s.data());
			records = s_tmp; 
			boost::sregex_iterator it(records.begin(),records.end(),reg);
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
			entry = mConfig.querydoc->getFirstChild();
			TOTAL = mConfig.querydoc->getNumSubtags() * 100;
		}
	}
	int PROCNUM=0;
	int query_num = 0;
	while(entry || !mConfig.hasquerydoc)
	{
		AosXmlTagPtr tempdoc = mConfig.querynode->clone(AosMemoryCheckerArgsBegin);
		if(mConfig.hasquerydoc)
		{
			//modify query doc template
			AosXmlTagPtr e = mConfig.replace_entries->getFirstChild();
			while(e)
			{
				OmnString pathname = e->getNodeText(); 
				aos_assert_rr(pathname != "", mRundata, NULL);
				OmnString aname = tempdoc->xpathQuery(pathname);
				aos_assert_rr(aname != "", mRundata, NULL);
				OmnString avalue = entry->getAttrStr(aname);
				aos_assert_rr(avalue != "", mRundata, NULL);
				tempdoc->xpathSetAttr(pathname, avalue);
				e = mConfig.replace_entries->getNextChild();
			}
		}

		//query
		int total = 1;
		int start_idx = 0;
		int psize = tempdoc->getAttrInt("psize", 0);
		tempdoc->setAttr("zky_batch_query_flag", "true");
		AosQueryReqObjPtr query = AosQueryReqObj::createQueryStatic(tempdoc, mRundata);
		aos_assert_rr(query && query->isGood(), mRundata, NULL);
		while(!query->finished())
		{
			OmnString contents;
			bool rslt = query->procPublic(tempdoc, contents, mRundata);
			aos_assert_rr(rslt, mRundata, NULL);

			vector<AosBuffPtr> batchdatas = query->getBatchData();
			query->setNoMoreDocs();
			total = query->getTotal();
			aos_assert_r(total >0 , false);
			int	batchnum = query->getBatchNum();
			
			int fieldnums = mFieldDef.size();
			bool r = saveBlock(batchdatas, batchnum, fieldnums);
			aos_assert_r(r, false);

			PROCNUM = query_num * 100 + batchnum * 100 / total;
			r = updateRate(TOTAL, PROCNUM);
			aos_assert_r(r, false);

			start_idx += psize;
			query->setStartIdx(start_idx);
		}

		if(mConfig.hasquerydoc)
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
				entry =  mConfig.querydoc->getNextChild();
			}
		}
		else
		{
			break;
		}
		query_num++;
	}

	OmnString finishedtime = OmnGetTime(AosLocale::getDftLocale());
	mQueryResultDoc->setAttr("finishedtime", finishedtime);
	mQueryResultDoc->setAttr("querystatus", "finish");
	mQueryResultDoc->setAttr("rate", "100");
	saveQueryResultDoc();
	return true;
}

AosXmlTagPtr	
AosStoreQuery::getQueryResultDoc(const AosRundataPtr &rdata)
{
	aos_assert_r(mQueryResultDoc, 0);
	return mQueryResultDoc->clone(AosMemoryCheckerArgsBegin);
}

bool
AosStoreQuery::saveQueryResultDoc()
{
	AosXmlTagPtr clonexml = mQueryResultDoc->clone(AosMemoryCheckerArgsBegin);
	bool rslt = AosModifyDoc(clonexml, mRundata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosStoreQuery::saveHeadToUserFile()
{
	OmnString head;
	for(u32 i=0; i<mFieldDef.size(); i++)
	{
		head << "\"" << mFieldDef[i].oname << "\"";
		if (i != (mFieldDef.size() - 1))
		{
			head << ",";
		}
	}
	head << "\r\n";
	mUserFile->append(head);
	return true;
}

bool
AosStoreQuery::saveBlockToUserFile(const vector<AosBuffPtr> &batchdatas, int &records, int &fieldnums)
{
	if(mBlockIdx == 0)
	{
		saveHeadToUserFile();
	}
	OmnString line;
	for(int i=0; i<records; i++)
	{
		for(int j=0; j<fieldnums; j++)
		{
			AosBuffPtr column = batchdatas[j];
			if(i==0)
			{
				column->reset();
			}
			aos_assert_r(column, false);
			line << "\"" << column->getOmnStr("") << "\"";
			if (j != (fieldnums- 1))
			{
				line << ",";
			}
		}
		line << "\r\n";
	}
	mUserFile->append(line);
	return true;
}

bool
AosStoreQuery::saveBlock(const vector<AosBuffPtr> &batchdatas, int &records, int &fieldnums)
{
	mAllRecords += records;
	int type = sSaveType::toEnum(mConfig.savetype);
	switch(type)
	{
		case sSaveType::eSaveFile : 
			return saveBlockToUserFile(batchdatas, records, fieldnums);

		case sSaveType::eSaveBinaryDoc : 
			return saveBlockToBinaryDoc(batchdatas, records, fieldnums);

		default:
			 return false;
		
	}
	return true;
}

bool
AosStoreQuery::updateRate(int &total, int &procnum)
{
	aos_assert_r(procnum <= total, false);
	int rate = procnum * 100 / total;
	OmnString value;
	value << rate;
	mQueryResultDoc->setAttr("rate", value);
	saveQueryResultDoc();
	return true;
}

bool
AosStoreQuery::openUserFile( OmnString &filepath, 
							 OmnString &filename)
{
	u64 userid = mRundata->getUserid();
	aos_assert_rr(userid != 0, mRundata,false);
	AosUserAcctObjPtr userdoc = AosDocClientObj::getDocClient()->getUserAcct(userid, mRundata);
	aos_assert_rr(userdoc, mRundata,false);
	OmnString cid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	OmnString zky_rscfn = "store_query_";
	zky_rscfn << OmnGetTime("YYYYMMDDhhmmss") << "_"
		<< (unsigned int)OmnGetCurrentThreadId() << "_"
		<< OmnRandom::letterDigitStr(6,6) << ".csv";

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
	mUserFile = OmnNew OmnFile(fdir, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_rr(mUserFile->isGood(), mRundata,false);
	filepath = usedir;
	filename = zky_rscfn;
	return true;
}

bool
AosStoreQuery::createUserFile()
{
	OmnString filepath;
	OmnString filename;
	OmnFilePtr file;
	openUserFile(filepath, filename);
	OmnString docstr = "<usedoc ";
	docstr 	<< AOSTAG_RSCDIR << "=\"" << filepath << "\" "
	<< AOSTAG_RSC_FNAME << "=\"" << filename << "\" "
	<< AOSTAG_OTYPE << "=\"userfile\" "
	<< "/>";
	AosXmlParser parserdoc;
	AosXmlTagPtr downloaddoc = parserdoc.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(downloaddoc, false);
	AosXmlTagPtr downloadnode = mConfig.downloaddoc;
	if(downloadnode)
	{
		AosXmlTagPtr node = downloadnode->getFirstChild("attribute");
		while(node)
		{
			OmnString name = node->getAttrStr("name"); 
			OmnString value = node->getNodeText(); 
			downloaddoc->setAttr(name, value);
			node = downloadnode->getNextChild();
		}
	}
	AosXmlTagPtr rsltdoc = AosCreateDoc(downloaddoc, mRundata);
	aos_assert_r(rsltdoc, false);
	mQueryResultDoc->addNode(rsltdoc);
	return true;
}

bool
AosStoreQuery::saveBlockToBinaryDoc(const vector<AosBuffPtr> &batchdatas, int &records, int &fieldnums)
{

	AosBuffPtr binarybuff = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
	AosBuffPtr head = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
	AosBuffPtr body = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
	for(int i=0; i<records; i++)
	{
		const u32 offset = body->getCrtIdx();
		head->setU32(offset);
		body->setU32(0);//set record length

		int data_len = 0;

		for(int j=0; j<fieldnums; j++)
		{
			AosBuffPtr column = batchdatas[j];
			if(i==0)
			{
				column->reset();
			}
			aos_assert_r(column, false);
			OmnString fieldvalue = column->getOmnStr("");
			body->setOmnStr(fieldvalue);
			data_len += fieldvalue.length() + sizeof(int);
		}

		// set fields len
		int crt_idx = body->getCrtIdx();
		body->setCrtIdx(offset);
		body->setU32(data_len);
		body->setCrtIdx(crt_idx);
	}

	binarybuff->setBuff(head);
	binarybuff->setBuff(body);

    const bool is_public = true;
    OmnString docstr = "";
    docstr << "<querybinarydoc " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_BINARYDOC << "\" "
        << AOSTAG_PARENTC << "=\"" << mConfig.binaryctnr << "_blockcontainer\" "
        << AOSTAG_PUBLIC_DOC << "=\"true\" "
        << AOSTAG_CTNR_PUBLIC << "=\"true\" >"
        << "</querybinarydoc>";
	AosXmlTagPtr newdoc = AosCreateBinaryDoc(-1, docstr, is_public, binarybuff, mRundata);
	aos_assert_r(newdoc, false);	
	u64 docid = newdoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid > 0, false);
	AosXmlTagPtr node = mQueryResultDoc->getFirstChild("docids");

	OmnString nodestr = "<docid index=\"";
	nodestr << mBlockIdx << "\" existdocs=\"" << records << "\"></docid>";
	AosXmlParser parser;
	AosXmlTagPtr docidtag = parser.parse(nodestr, "" AosMemoryCheckerArgs);

	OmnString did;
	did << docid;
	docidtag->setNodeText(did, true);

	node->addNode(docidtag);

	mQueryResultDoc->setAttr("exist_docs_all", mAllRecords);

	saveQueryResultDoc();
	mBlockIdx++;


	return true;
}
