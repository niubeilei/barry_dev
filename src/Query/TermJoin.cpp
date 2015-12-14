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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 2013/08/12  Andy Zhang
////////////////////////////////////////////////////////////////////////////
#include "Query/TermJoin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "SEUtil/IILName.h"
#include "Query/TermIILType.h"
#include "Query/TermOr.h"
#include "Query/TermAnd.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/Ptrs.h" 
#include "XmlUtil/SeXmlParser.h"


AosTermJoin::AosTermJoin(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_JOIN, AosQueryType::eJoin, regflag),
mDataLoaded(false),
mDocMapSize(0),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0)
{
}


AosTermJoin::AosTermJoin(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_JOIN, AosQueryType::eJoin, false),
mDataLoaded(false),
mDocMapSize(0),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0)
{
	mIsGood = parse(def, rdata);
}


AosTermJoin::~AosTermJoin()
{
}


bool 	
AosTermJoin::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid that is selected by this term.
	// What it does is to retrieve the data based on the condition, and then 
	// returns the next docid. Normally you do not need to override this function. 
	// Refer to AosQueryTerm::nextDocid(...) to see whether it is the one you
	// need. If yes, do not override it. Remove this function. 
	docid = 0;
	finished = false;
	if (!mDataLoaded) loadData(rdata);

	if (mCrtIdx >= (i64)mKeyVec.size())
	{
		finished = true;
		return true;
	}
	docid = ++mCrtIdx;
	return true;
}


bool
AosTermJoin::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermJoin::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	if (query_rslt) 
	{
		query_rslt->appendDocid(0);
		query_rslt->reset();
	}
	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermJoin::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	aos_assert_r(docid > 0, 0);
	OmnString key;
	if (!mReverseOrder)
	{
		aos_assert_r(docid <= mKeyVec.size(), 0);
		key = mKeyVec[(docid-1)];
	}
	else
	{
		i64 idx = mKeyVec.size() - docid;
		aos_assert_r(idx < (i64)mKeyVec.size(), 0);
		key = mKeyVec[idx];
		
	}
	return mDocsMap[key];
}


AosXmlTagPtr
AosTermJoin::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	return 0;
}


AosXmlTagPtr
AosTermJoin::processRecordByOneDime(i64 &index, const AosRundataPtr &rdata)
{
	return 0;
}


AosXmlTagPtr
AosTermJoin::processRecordByTwoDime(i64 &index, const AosRundataPtr &rdata)
{
	return 0;
}

bool 	
AosTermJoin::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	
	mCrtIdx = pos;

	return true;
}


i64		
AosTermJoin::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	if (!mDataLoaded) loadData(rdata);
	return mKeyVec.size();
}


void	
AosTermJoin::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
}

void
AosTermJoin::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermJoin::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	
	i64 numSize = 0;
	for (u32 i = 0; i < mOrTerms.size(); i++)
	{
OmnScreen << "TermJoin " << i << " totals is " << mOrTerms[i]->getTotal(rdata) << endl;
		u64 docsSize = 0 ;
		bool finished = false;
		u64 docid; 
		AosXmlTagPtr xml;
		while (mOrTerms[i]->nextDocid(0, docid, finished, rdata))
		{
			if(docid == AOS_INVDID && finished) break;

			xml = mOrTerms[i]->getDoc(docid, rdata);
			if (!xml)
			{
				break;
				OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
				continue;
			}

			docsSize += xml->getDocsize();

			if (docsSize > eMapContentMax)
			{
				OmnAlarm << "TermJoin's map is too big! map size : " << docsSize<< enderr;
				break;
			}

			OmnString key = xml->getAttrStr(mJoinKey, "");

			if (key == "") continue;

			if (i == 0) 
			{
				numSize++;
				mKeyVec.push_back(key);
				mIsJoined.insert(make_pair(key, false));
				mDocsMap.insert(make_pair(key, xml));	
				mDocsNum++;
			}
			else
			{
				if (mIsJoined[key]) continue;
				AosXmlTagPtr record = mDocsMap[key];
				if (!record) continue;
				for (u32 i = 0; i < mJoinValues.size(); i++)
				{
					OmnString vv = xml->getAttrStr(mJoinValues[i], "");
					record->setAttr(mJoinValues[i], vv);
				}
				mIsJoined[key] = true;
			}
		}
	}

	mDataLoaded = true;

	OmnScreen << "TermJoin docs number: " << numSize << endl;

	return true;
}


bool
AosTermJoin::processDataByOneDime(const AosRundataPtr &rdata)
{
	return true;
}



bool
AosTermJoin::loadTimedData(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosTermJoin::findColumnKeys(
		map<OmnString, i64> &column, 
		i64	&num_row_key,
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosTermJoin::processDataByTwoDime(const AosRundataPtr &rdata)
{
	return true;
}


void
AosTermJoin::retrieveBuffValue(
			OmnString &cname, 
			i64 &value, 
			const bool &timeorder,
			const AosRundataPtr &rdata)
{
}


bool
AosTermJoin::parse(const AosXmlTagPtr &conds, const AosRundataPtr &rdata)
{
	// 'conds' should be in the form:
	//  <conds zky_join_key="xxxx" zky_sep="," zky_join_value="zky_xxx,zky_xxx">
	//      <cond type="AND">
	//          <term type="xxx" .../>
	//          <term type="xxx" .../>
	//          ...
	//          <term type="xxx" .../>
	//      </cond>
	//      <cond type="AND">
	//          <term type="xxx" .../>
	//          <term type="xxx" .../>
	//          ...
	//          <term type="xxx" .../>
	//      </cond>
	//  </conds>
	//
	// If 'mSiteid' is not empty, it is limited to the site. Need
	// to add the condition:
	//  <term type="tag">mSiteid</term>

	// Containers are converted into
	// searching for tags. Each container is treated as a tag.
	// If a query contains a container, it means that the searched
	// should be in that container. If a doc is in a container, 
	// its docid should be added to the IIL:
	// Containers are formatted as:
	//      [true:]<container>, [true:]<container>
	// where '[true:]' is optional. If not present, it is the normal
	// order. Otherwise, it is the reversed order.

	mIsGood = false;
	aos_assert_r(rdata, false);
	aos_assert_rr(conds, rdata, false);

	
	mJoinKey = conds->getAttrStr("zky_join_key", ""); 
	aos_assert_r(mJoinKey != "", false);

	OmnString vv = conds->getAttrStr("zky_join_value", "");
	aos_assert_r(vv != "", false);

	mSep = conds->getAttrStr("zky_sep", ",");

	AosStrSplit split(vv, mSep.data());
	mJoinValues = split.entriesV();

	AosQueryTermObjPtr cond;

	i64 guard = 20;
	AosXmlTagPtr p = conds;
	while ((p = p->getParentTag()) && guard-- > 0)
	{
		if (p->getAttrStr("start_idx") != "")
		{
			mStartIdx = p->getAttrInt64("start_idx", 0);
			mPsize = p->getAttrInt64("psize", eDftPsize);
			mReverseOrder = p->getAttrBool("reverse", false);
			mOrderContainer = p->getAttrStr("orderctnr");
			mOrderFname = p->getAttrStr("order");
			break;
		}
	}

	// 1. Handle all containers. Containers are handled by 'AosTermTag'
	// with the IIL name: ??? + ":" + container name
	OmnString word;

	if (!conds) return false;

	// All other conditions are defined by 'term'. 
	AosXmlTagPtr termtmp;
	guard = 20;
	AosXmlTagPtr andterm = conds->getFirstChild();
	while (guard-- && andterm) 
	{
		OmnString type = andterm->getAttrStr("type");
		if (andterm->getTagname() == AOSTAG_QUERY_STAT)
        {
            // This is a statistics tag
			if (!cond) cond = OmnNew AosTermAnd();
            processStatistics(cond, andterm, rdata);
            andterm = conds->getNextChild();
            continue;
        }

		if (type != "AND")
		{
			rdata->setError() << "Expecting the AND term but failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (!cond) cond = OmnNew AosTermAnd();
		AosXmlTagPtr term = andterm->getFirstChild();
		termtmp = term;

		while (term)
		{
			//jackie 04/19/2013 start
			OmnString t_type = term->getAttrStr("type");
			if(t_type == "batchquery")
			{
				term->setAttr("startidx", mStartIdx);
				term->setAttr("size", mPsize);
			}
			//jackie 04/19/2013 end
			
			if (rdata->getCid()!= "")
			{
				AosXmlTagPtr objcond = term;
				bool exist;
				OmnString objidlhs = objcond->xpathQuery("lhs/_$text", exist, "");
				if (objidlhs == AOSTAG_OBJID)
				{
					OmnString objidrhs = objcond->xpathQuery("rhs/_$text", exist, "");
					OmnString modobjid  = objidrhs;
					OmnString errmsg;
					bool rslt = AosObjid::procObjid(modobjid, rdata->getCid(), errmsg);
					if (!rslt)
					{
						AosSetError(rdata, errmsg);
					}
					else if (modobjid != objidrhs)
					{
						objcond->setNodeText("rhs", modobjid, true);
					}
				}
			}
			cond->addTerm(term, rdata);
			term = andterm->getNextChild();
		}

		// Check containers. It is possible to 
		if (cond)
		{
			mOrTerm = OmnNew AosTermOr();
			mOrTerm->addTerm(cond, rdata);
		}

		if (mOrderFname != "" && mOrTerm) 
		{
			mOrTerm->setOrder(mOrderContainer, mOrderFname, mReverseOrder, rdata);
		}

		// Backward Compatibility, Chen Ding, 09/20/2011
		aos_assert_r(mOrTerm, false);
		mOrTerm->setPagesize(mPsize);
		mOrTerm->setStartIdx(mStartIdx);
		
		mOrTerm->loadData(rdata);
		//mTotal = mOrTerm->getTotal(rdata);
		mOrTerm->reset(rdata);

		OmnString str;
		if (mOrTerm) mOrTerm->toString(str);
		OmnScreen << "Query: " << str << endl;

		mOrTerms.push_back(mOrTerm);

		andterm = conds->getNextChild();
	}


	if (!mDataLoaded) loadData(rdata);

	rdata->setOk();

	mIsGood = true;
	return true;
}


bool
AosTermJoin::retrieveCounterTimeConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	return true;
}

bool
AosTermJoin::processStatistics(
		const AosQueryTermObjPtr &cond,
		const AosXmlTagPtr &stat_tag, 
		const AosRundataPtr &rdata)
{
	// This function processes the <stat> tag:
	// 	<conds>
	// 		<cond ...>
	// 			<term .../>
	// 			<term .../>
	//			...
	// 			<term .../>
	// 		</cond>
	// 		<cond .../>
	// 		<stat> 
	// 		 <statterm type="couterSingle||counterMulti||counterSub">
	// 			<zky_ctnr>xxxx</zky_ctnr>
	// 	        <zky_ctype>xxxx</zky_ctype>
	// 	        <zky_starttime>xxxx</zky_starttime>
	// 	        <zky_endtime>xxxx</zky_endtime>
	// 	        <zky_cname>
	// 	           <termname zky_value_type="attr"  zky_xpath="city">
	// 	                <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 	           </termname>
	// 	  	       <termname zky_value_type="const">xxx</termname>
	// 	             ....
	// 	        </zky_cname>
	// 	      </statterm>
	// 		</stat>
	// 	</conds>
	
	aos_assert_rr(stat_tag, rdata, false);
	AosXmlTagPtr statTerm = stat_tag->getFirstChild();
	if(!statTerm)return true;

	AosQueryType::E type = AosQueryType::toEnum(statTerm->getAttrStr("type", ""));
	if (!AosQueryType::isValidCounterType(type))
	{
		rdata->setError() << "Unrecognized statistics type: " << statTerm->getAttrStr("type", "");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (type == AosQueryType::eCounterMulti)
	{
		mOrTerm = OmnNew AosTermOr();
		mOrTerms.push_back(mOrTerm);
		bool rslt = mOrTerm->addStatTerm(statTerm, rdata);
		aos_assert_rr(rslt, rdata, false);
		rdata->setOk();
		return true;
	}

	AosQueryTermObjPtr term = cond->addTerm(statTerm, rdata);
	aos_assert_r(term, false);

	cond->setNeedOrder(false);
	mOrTerm = OmnNew AosTermOr();
	mOrTerm->addTerm(cond, rdata);
	mOrTerm->setDocRetriever(term);
	mOrTerm->loadData(rdata);
	mOrTerms.push_back(mOrTerm);
	return true;
}


bool
AosTermJoin::retrieveCounterIds(const AosXmlTagPtr &term)
{
	return true;
}


bool
AosTermJoin::retrieveAggregateConf(
		const AosXmlTagPtr &term, 
		i64 &fieldindex)
{
	return true;
}


bool
AosTermJoin::retrieveTwoDimeConf(
		const AosXmlTagPtr &term,
		const i64 &fieldindex)
{
	return true;
}


bool
AosTermJoin::getCounterNames(
		const u16 &entry_type,
		const OmnString &str_header, 
		const AosRundataPtr &rdata)
{
	return true;
}


AosQueryTermObjPtr
AosTermJoin::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermJoin(def, rdata);
}

