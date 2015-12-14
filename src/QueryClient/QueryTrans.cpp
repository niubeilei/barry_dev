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
// 01/02/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "QueryClient/QueryTrans.h"

#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"



AosQueryTrans::AosQueryTrans(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnString siteid = rdata->getSiteid();
	OmnString cid = rdata->getCid();
	try
	{
		mQueryReq = OmnNew AosQueryReq(0);
	}

	catch (exception& e)
	{
		rdata->setError() << "Failed to create query transaction (run out of memory)!";
		OmnAlarm << rdata->getErrmsg() << ". " << def->toString() << enderr;
		return;
	}

	u64 queryid = def->getAttrU64(AOSTAG_QUERYID, 0);
	mQueryReq->setQueryId(queryid);
	mQueryReq->setStartIdx(def->getAttrStr("start_idx"));
	mQueryReq->startQuery();

	mQueryReq->setQueryType(def->getAttrStr("qtype"));
	mQueryReq->setGetTotal(def->getAttrStr("get_total") == "true");
	mQueryReq->setPsize(def->getAttrStr("psize"));

	// Chen Ding, 08/20/2011
	// We no longer support this.
	// mQueryReq->setOrderName(def->getAttrStr("order2"));
	mQueryReq->setSortFlag(def->getAttrBool("sortflag"));
	mQueryReq->setIncrease(def->getAttrBool("increase"));
	mQueryReq->setRemoveMeta(def->getAttrStr("rmmetadata"));
	mQueryReq->setQueryType("sql");
	mQueryReq->setSubopr(def->getAttrStr("subopr"));

	// Chen Ding, 08/20/2011
	// mQueryReq->setOrder(def->getAttrStr("order"), def->getAttrStr("reverse"), rdata);
	mQueryReq->setOrder(def->getAttrStr("orderctnr"), 
			def->getAttrStr("order"), def->getAttrStr("reverse"), rdata);
	//bool rslt = mQueryReq->setJoins(def->getAttrStr("joins"), errmsg);
	//if (!rslt) 
	//{
	//	mQueryReq->setIsGood(false);
	//	return;
	//}

	mQueryReq->setIdx(-1);
	if (!mQueryReq->parseSqlPublic(rdata, def)) return;
	rdata->setOk();
	return;
}


AosQueryTrans::~AosQueryTrans()
{
}


bool	
AosQueryTrans::isNew() const
{
	return mQueryReq->isNew();
}


bool	
AosQueryTrans::isGood() const
{
	return mQueryReq->isGood();
}


void
AosQueryTrans::setQueryType(const OmnString &type)
{
	return mQueryReq->setQueryType(type);
}


void
AosQueryTrans::setStmt(const OmnString &query)
{
	mQueryReq->setStmt(query);
}


void
AosQueryTrans::setStartIdx(const OmnString &d)
{
	mQueryReq->setStartIdx(d);
}


void	
AosQueryTrans::setPsize(const OmnString &s)
{
	mQueryReq->setPsize(s);
}


bool 	
AosQueryTrans::setOrder(
		const OmnString &container,
		const OmnString &order_fname, 
		const OmnString &reverse, 
		const AosRundataPtr &rdata)
{
	return mQueryReq->setOrder(container, order_fname, reverse, rdata);
}


bool	
AosQueryTrans::setSubopr(const OmnString &)
{
	return mQueryReq->isGood();
}


void	
AosQueryTrans::setRemoveMeta(const OmnString &ss)
{
	mQueryReq->setRemoveMeta(ss);
}


void	
AosQueryTrans::setIdx(const int i)
{
	mQueryReq->setIdx(i);
}


bool	
AosQueryTrans::setJoins(const OmnString &join, OmnString &errmsg)
{
	return mQueryReq->setJoins(join, errmsg);
}


bool	
AosQueryTrans::startQuery()
{
	return mQueryReq->startQuery();
}


bool	
AosQueryTrans::procPublic(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &cmd,
			OmnString &contents) 
{
	return mQueryReq->procPublic(rdata, cmd, contents);
}


bool	
AosQueryTrans::parseSqlPublic(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cmd)
{
	return mQueryReq->parseSqlPublic(rdata, cmd);
}
#endif
