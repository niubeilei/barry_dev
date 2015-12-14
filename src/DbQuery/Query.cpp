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
// 07/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DbQuery/Query.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/Sleep.h"
#include "Query/QueryReq.h"
#include "Query/TermAnd.h"
#include "Query/TermIIL.h"
#include "QueryClient/QueryClient.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/Opr.h"
#include "Util/StrSplit.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosQuerySingleton,
                 AosQuery,
                 AosQuerySelf,
                "AosQuery");

const i64 sgMaxAttrs = 50;

AosQuery::AosQuery()
:
mRemotePort(-1),
mTransId(100)
{
}


AosQuery::~AosQuery()
{
}


bool      	
AosQuery::start()
{
	return true;
}


bool        
AosQuery::stop()
{
	return true;
}


bool
AosQuery::config(const AosXmlTagPtr	&def)
{
	return true;
}


bool
AosQuery::start(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr conf = config->getFirstChild("query_interface");
	if (!conf) return true;

	mIsLocal = conf->getAttrBool("is_local", true);
	if (!mIsLocal)
	{
		mRemoteAddr = conf->getAttrStr(AOSCONFIG_REMOTE_ADDR);
		mRemotePort = conf->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
		if (mRemotePort <= 0)
		{
			OmnAlarm << "Query remote port is invalid: " << mRemotePort << enderr;
			exit(-1);
		}
		//mConn = OmnNew OmnTcpClient(mRemoteAddr, mRemotePort, 1, eAosTLT_FirstFourHigh);
	}
	return true;
}


bool    
AosQuery::signal(const int threadLogicId)
{
	return true;
}


bool    
AosQuery::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosQuery::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1000);
	}

	return true;
}


bool
AosQuery::runQuery(
		const AosXmlTagPtr &query, 
		AosXmlTagPtr &results, 
		const AosRundataPtr &rdata)
{
	// This function runs the query 'query', which is the XML representation
	// of a query. If success, it returns the results through 'results', 
	// which is an XML in the format:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	//		...
	//	</Contents>
	aos_assert_r(query, false); 
	OmnString cid = rdata->getCid();
	AosQueryReqObjPtr qq = AosQueryClient::getSelf()->createQuery(query, rdata);

	if (cid == "")
	{
		rdata->setError() << "Missing Cloud id";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	bool rslt = qq->procPublic(query, rdata->getResults(), rdata);
	aos_assert_r(rslt, false);
	OmnString contents = rdata->getResults();
	AosXmlParser parser;
	results = parser.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_r(results, false);

	return true;
}


bool
AosQuery::runQuery(
		const OmnString &query, 
		AosXmlTagPtr &results,
		const AosRundataPtr &rdata)
{
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(query, "" AosMemoryCheckerArgs);
	aos_assert_rr(xml, rdata, false);
	return runQuery(xml, results, rdata);
}


bool 
AosQuery::runQuery(
		const OmnString &ctnr_objid, 
		const OmnString &aname,
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &bitmap,
		const AosRundataPtr &rdata)
{
	// This is to query the condition:
	// 	<conds>
	// 		<cond type="AND" ...>
	// 			<term type=opr .../>
	// 		</cond>
	// 	</conds>
	
//shawn only query one page
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setReverse(reverse);
	query_context->setOpr(opr);
	query_context->setStrValue(value);
	query_context->setBlockSize(0);
	
	AosQueryTermObjPtr term = OmnNew AosTermIIL(ctnr_objid, 
			aname, value, opr, reverse, false, rdata);
	term->getDocidsFromIIL(query_rslt, bitmap, query_context, rdata);
	return true;
}


bool 
AosQuery::runQuery(
		const i64 &startidx, 				// Chen Ding, 05/16/2012
		const i64 &psize,					// Chen Ding, 05/16/2012
		const OmnString *ctnr_objids, 
		const OmnString *anames, 
		const AosValueRslt *value, 
		const AosOpr *opr, 
		const bool *reverse,
		const bool *order,
		const i64 &num_conds,
		AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	AosQueryTermObjPtr cond = OmnNew AosTermAnd();
	cond->setPagesize(psize);
	cond->setStartIdx(startidx);

	OmnString vv;
	for(i64 i=0; i<num_conds; i++)
	{
		bool rslt;
		vv = value[i].getStr();
		if (!rslt)
		{
			OmnAlarm << "failed retrieve string" << enderr;
		}
		else
		{
			AosQueryTermObjPtr term = OmnNew AosTermIIL(ctnr_objids[i], anames[i], 
					vv, opr[i], reverse[i], order[i], rdata);
			cond->addTerm(term, rdata);
		}
	}

	bool rslt = cond->loadData(rdata);
	aos_assert_r(rslt, false);
	query_rslt = cond->getQueryData();
	return true;
}


bool 
AosQuery::runQuery(
		const i64 &startidx, 						// Chen Ding, 05/16/2012
		const i64 &psize,							// Chen Ding, 05/16/2012
		const vector<AosQueryCondInfo> &conds,
		AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	aos_assert_r(conds.size() > 0, false);
	AosQueryTermObjPtr cond = OmnNew AosTermAnd();
	cond->setPagesize(psize);
	cond->setStartIdx(startidx);

	OmnString vv;
	for(u32 i=0; i<conds.size(); i++)
	{
		AosQueryTermObjPtr term = OmnNew AosTermIIL(conds[i], rdata);
		cond->addTerm(term, rdata);
	}

	// Skip to 'startidx'.
	// bool finished = false;
	// bool rslt;
	// u64 docid;
	// for (i64 i=0; i<startidx; i++)
	// {
	// 	rslt = cond->nextDocid(0, docid, finished, rdata);
	// 	aos_assert_rr(rslt, rdata, false);
	// 	if (finished)
	// 	{
	// 		// No more contents.
	// 		return true;
	// 	}
	// }

	bool rslt = cond->loadData(rdata);
	aos_assert_rr(rslt, rdata, false);
	query_rslt = cond->getQueryData();
	return true;
}


AosQueryReqObjPtr
AosQuery::parseQuery(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) 
{
	// This function parses the query portion of a smart doc
	// The smartdoc format should be in the form:
	//  <doc AOSTAG_OTYPE=AOSOTYPE_SMARTDOC 
	//  	 AOSTAG_SMARTDOC_OPR="query|simulate|run">
	// 		<query ...>
	// 		<actions ...>
	// 			<action type="xxx">
	// 				<attr namebd="xxx" attrname=" " />
	// 			<action ...>
	// 			...
	// 			<action ...>
	// 		</actions>
	// 	</...>
	aos_assert_r(def, 0);
	AosXmlTagPtr query = def->getFirstChild(AOSTAG_QUERY);    
	aos_assert_r(query, 0); 
	OmnString cid = rdata->getCid();
	AosQueryReqObjPtr trans = AosQueryClient::getSelf()->createQuery(query, rdata);
	if (trans && trans->isGood()) return trans;
	return 0;
}


bool 
AosQuery::doQuery(
		const AosQueryReqObjPtr &query,
		const AosXmlTagPtr &query_xml,
		const AosRundataPtr &rdata)
{
	OmnString cid = rdata->getCid();
	if (cid == "")
	{
		rdata->setError() << "Missing Cloud id";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	bool rslt = query->procPublic(query_xml, rdata->getResults(), rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosQuery::runQuery(
		const i64 &startidx, 						// Chen Ding, 05/16/2012
		const i64 &psize,							// Chen Ding, 05/16/2012
		const OmnString &ctnr_objid, 
		const OmnString &aname, 
		const OmnString &value, 
		const AosOpr opr, 
		const bool reverse, 
		OmnString *values,
		u64 *docids, 
		const bool with_values,
		const AosRundataPtr &rdata)
{
	if (with_values) aos_assert_rr(values, rdata, false);
	aos_assert_rr(docids, rdata, false);

	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setReverse(reverse);
	query_rslt->setWithValues(with_values);
	query_context->setBlockSize(0);

	AosQueryTermObjPtr term = OmnNew AosTermIIL(ctnr_objid, 
				aname, value, opr, reverse, false, rdata);
	term->getDocidsFromIIL(query_rslt, 0, query_context, rdata);
	query_rslt->reset();

	if (with_values)
	{
		OmnString vv;
		u64 did;
		i64 idx = 0;
		while (idx < psize)
		{
			query_rslt->nextDocidValue(did, vv, rdata);
			if (did == 0)
			{
				// psize = idx;			// Chen Ding, 05/16/2012
				return true;
			}
			docids[idx] = did;
			values[idx] = vv;
			idx++;
		}
	}
	else
	{
		u64 did;
		i64 idx = 0;
		bool finished;
		while (idx < psize)
		{
			query_rslt->nextDocid(did, finished, rdata);
			if (did == 0)
			{
				// psize = idx;			// Chen Ding, 05/16/2012
				return true;
			}
			docids[idx] = did;
			idx++;
		}
	}

	return true;
}


// Chen Ding, 11/29/2011
u64 
AosQuery::getMember(
		const OmnString &container_objid, 
		const OmnString &id_aname, 
		const OmnString &id_value, 
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	// This function retrieves a member identified by 'id' in a container.
	// The container is 'container_objid'. The id is a value of the id 
	// attribute named 'id_aname'. This function assumes that the container
	// has the member listing based on 'id_aname'. If not, it is an error.
	is_unique = false;
	aos_assert_rr(container_objid != "", rdata, 0);
	aos_assert_rr(id_aname != "", rdata, 0);

	i64 startidx = 0;
	i64 psize = 2;
	u64 docids[2];
	memset(docids, 0, 2*sizeof(u64));
	bool rslt = runQuery(startidx, psize, container_objid, 
						 id_aname, 
						 id_value, 
						 eAosOpr_eq, 
						 false, 
						 0, 
						 docids, 
						 false, 
						 rdata);
	aos_assert_rr(rslt, rdata, 0);
	if (psize == 1) is_unique = true;
	if (psize < 1) return 0;
	return docids[0];
}


i64 
AosQuery::retrieveContainerMembers(
		const OmnString &ctnr_objid,
		vector<OmnString> &objids, 
		const i64 &psize,
		const i64 &startidx,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	// This function retrieves the objids of the first 'psize' number of 
	// docs, starting from 'start_idx', either in the normal order ('reverse==false')
	// or reversed order ('reverse == true').
	objids.clear();

	// AosBitmapObjPtr bitmap = AosBitmapObj::getBitmapStatic();
	AosBitmapObjPtr bitmap = AosGetBitmap();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(reverse);	
	AosTermIIL term(ctnr_objid, "", "", eAosOpr_an, reverse, true, rdata);
	term.getDocidsFromIIL(0, 0, query_context, rdata);
	if (bitmap->isEmpty())
	{
		return 0;
	}

	bitmap->reset();
	u64 docid;
	OmnString objid;
	while (bitmap->nextDocid(docid))
	{
		objid = AosDocClientObj::getDocClient()->getObjidByDocid(docid, rdata);
		if (objid == "")
		{
			OmnAlarm << "Missing objid: " << docid << enderr;
		}
		else
		{
			objids.push_back(objid);
		}
	}
	return objids.size();
}


i64 
AosQuery::retrieveContainerDocidsMembers(
		const OmnString &ctnr_objid,
		vector<u64> &docids, 
		const i64 &psize,
		const i64 &startidx,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	// This function retrieves the docids of the first 'psize' number of 
	// docs, starting from 'start_idx', either in the normal order ('reverse==false')
	// or reversed order ('reverse == true').
	docids.clear();
	// AosBitmapObjPtr bitmap = AosBitmapObj::getBitmapStatic();
	AosBitmapObjPtr bitmap = AosGetBitmap();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(reverse);	
	AosTermIIL term(ctnr_objid, "", "", eAosOpr_an, reverse, true, rdata);
	term.getDocidsFromIIL(0, bitmap, query_context, rdata);
	if (bitmap->isEmpty())
	{
		return 0;
	}

	bitmap->reset();
	u64 docid;
	while (bitmap->nextDocid(docid))
	{
		if (docid == 0)
		{
			OmnAlarm << "Missing docid" << enderr;
		}
		else
		{
			docids.push_back(docid);
		}
	}
	return docids.size();
}


i64 
AosQuery::retrieveContainerMembers(
		const i64 &startidx,
		const i64 &psize,
		const OmnString &ctnr_objid,
		const OmnString &fnames, 
		OmnString &results,
		const AosRundataPtr &rdata)
{
	// 'fnames' defines the field names to be retrieved. It is in the following format:
	// 			xpath:cname,xpath:cname,...
	// If 'fnames' is empty, AOSTAG_OBJID is assumed. If 'cname' is in the form:
	// 			tagname/_#text
	// it means the results are put in a subtag whose name is 'tagname'.
	//
	// The results are in the following format:
	// 	<Contents>
	// 		<record cname="xxx" cname="xxx" .../>
	// 		<record cname="xxx" cname="xxx" .../>
	// 		...
	// 	</Contents>
	vector<OmnString> attrnames;
	vector<OmnString> cnames;
	vector<int> types;
	results = "";
	if (fnames == "")
	{
		attrnames.push_back(AOSTAG_OBJID);
		cnames.push_back(AOSTAG_OBJID);
		types.push_back(eFieldTypeAttr);
	}
	else
	{
		bool finished;
		AosStrSplit split;
		// Chen Ding, 2013/12/31
		// int numAttrs = AosStrSplit::splitStrByChar(
		// 		fnames.data(), ", ", attrnames, sgMaxAttrs, finished);
		int numAttrs = AosStrSplit::splitStrBySubstr(
				fnames.data(), ", ", attrnames, sgMaxAttrs, finished);
		aos_assert_rr(numAttrs > 0, rdata, 0);
		OmnString pair[2];
		for (u32 i=0; i<attrnames.size(); i++)
		{
			// 'attrnames' temporarily holds the results, which is in the form:
			// 		xpath[:cname]
			// If it contains no ':cname', it defaults to xpath.
			int nn = split.splitStr(attrnames[i].data(), ":", pair, 2, finished);
			if (nn == 1)
			{
				cnames.push_back(attrnames[i]);
			}
			else
			{
				attrnames[i] = pair[0];
				cnames.push_back(pair[1]);
			}
			types.push_back(getFieldType(attrnames[i]));
		}
	}
	// AosBitmapObjPtr bitmap = AosBitmapObj::getBitmapStatic();
	AosBitmapObjPtr bitmap = AosGetBitmap();
	AosTermIIL term(ctnr_objid, "", "", eAosOpr_an, false, true, rdata);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(false);
	
	term.getDocidsFromIIL(0, bitmap, query_context, rdata);
	if (bitmap->isEmpty())
	{
		return 0;
	}

	bitmap->reset();
	u64 docid;
	results = "<Contents>";
	int nn = 0;
	OmnString subtags;
	while (bitmap->nextDocid(docid))
	{
		AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (doc)
		{
			nn++;
			results << "<record";
			subtags = "";
			for (u32 i=0; i<attrnames.size(); i++)
			{
				OmnString value = doc->getAttrStr(attrnames[i]);
				if (value != "")
				{
					switch (types[i])
					{
					case eFieldTypeAttr:
						 results << " " << cnames[i] << "=\"" << value << "\"";
						 break;

					case eFieldTypeSubtag:
						 subtags << "<" << cnames[i] << "><![CDATA[" << value 
							 << "]]><" << cnames[i];
						 break;

					default:
						 OmnAlarm << "Invalid field type: " << types[i] << enderr;
						 break;
					}
				}
			}

			results << ">";
			if (subtags != "")
			{
				results << subtags;
			}

			results << "</record>";
		}
	}	
	return nn;
}


bool 
AosQuery::runQuery(
		const i64 &startidx,					// Chen Ding, 05/16/2012
		const i64 &psize,					// Chen Ding, 05/16/2012
		const u64 *iilid,
		const AosValueRslt *value,
		const AosOpr *opr, 
		const bool *reverse,
		const bool *order,
		const i64 &num_conds,
		AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	AosQueryTermObjPtr cond = OmnNew AosTermAnd();
	cond->setWithValues(query_rslt->isWithValues());
	OmnString vv;
	for(i64 i=0; i<num_conds; i++)
	{
		bool rslt;
		vv = value[i].getStr();
		if (!rslt)
		{
			OmnAlarm << "Failed retrieve string" << enderr;
		}
		else
		{
			AosQueryTermObjPtr term = OmnNew AosTermIIL(iilid[i],
					vv, opr[i], reverse[i], order[i], rdata);
			cond->addTerm(term, rdata);
		}
	}

	bool rslt = cond->loadData(rdata);
	aos_assert_r(rslt, false);

	query_rslt = cond->getQueryData();
	return true;
}


bool 
AosQuery::getAllContainers(
		const i64 &startidx, 			// Chen Ding, 05/16/2012
		const i64 &psize,				// Chen Ding, 05/16/2012
		const bool reverse,
		AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	// All containers otype is AOSOTYPE_CONTAINER. This function 
	// uses this condition to retrieve all containers.
	//
	// TSK001NOTE
	// Terms need to handle pagesize and start index
	return runQuery("", AOSTAG_OTYPE, 
			AOSOTYPE_CONTAINER, eAosOpr_eq, reverse, query_rslt, 0, rdata);
}


bool 
AosQuery::getUserDomains1(
		const i64 &startidx,
		const i64 &psize,
		const bool reverse,
		AosQueryRsltObjPtr &values, 
		const AosRundataPtr &rdata)
{
	// All user domains are XML docs with AOSTAG_OTYPE equals to AOSOTYPE_USERDOMAIN
	return runQuery("", AOSTAG_OTYPE, AOSOTYPE_USERDOMAIN, 
			eAosOpr_eq, reverse, values, 0, rdata);
}


bool 
AosQuery::runQuery(
		const i64 &startidx, 						// Chen Ding, 05/16/2012
		const i64 &psize,							// Chen Ding, 05/16/2012
		const OmnString &iilname, 
		const u64 &value, 
		const AosOpr opr, 
		const u64 *docids, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2013/02/09
bool 
AosQuery::runQuery(
		const i64 &startidx,
		const i64 &psize,
		const OmnString &iilname, 
		const AosValueRslt &value1,
		const AosValueRslt &value2,
		const AosOpr opr, 
		AosQueryRsltObjPtr &query_rslt,
		const bool reverse,
		const bool with_docid,
		const AosRundataPtr &rdata)
{
	// This function queries one IIL with the condition:
	// 		[value, opr]
	// The results are in 'query_rslt'.
	AosQueryTermObjPtr cond = OmnNew AosTermAnd();
	cond->setWithValues(with_docid);
	AosQueryTermObjPtr term = OmnNew AosTermIIL(iilname, value1, value2, opr, reverse, true, false, rdata);
	cond->addTerm(term, rdata);

	bool rslt = cond->loadData(rdata);
	aos_assert_r(rslt, false);

	query_rslt = cond->getQueryData();
	return true;
}


// Chen Ding, 2013/05/08
bool 
AosQuery::runQuery(
		const AosRundataPtr &rdata,
		const i64 &startidx, 
		const i64 &psize,
		const AosXmlTagPtr &query,
		AosQueryRsltObjPtr &query_rslt)
{
	// 'query' is in the following format:
	// 	<query>
	// 		<term .../>
	// 		<term .../>
	// 		...
	// 	</query>
	query_rslt = 0;
	aos_assert_rr(query, rdata, false);

	vector<AosQueryCondInfo> conds;
	AosXmlTagPtr tag = query->getFirstChild();
	while (tag)
	{
		AosQueryCondInfo info(tag);
		conds.push_back(info);
		tag = query->getNextChild();
	}

	if (conds.size() == 0) return true;

	return runQuery(startidx, psize, conds, query_rslt, rdata);
}

