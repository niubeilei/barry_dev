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
#include "Query/TermFilter.h"

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
#include "SEInterfaces/SmartDocObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermFilter::AosTermFilter(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_FILTER, AosQueryType::eFilter, regflag),
mDataLoaded(false),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0),
mDocsNum(0)
{
}


AosTermFilter::AosTermFilter(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_FILTER, AosQueryType::eFilter, false),
mDataLoaded(false),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0),
mDocsNum(0)
{
	mIsGood = parse(def, rdata);
}


AosTermFilter::~AosTermFilter()
{
}


bool 	
AosTermFilter::nextDocid(
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

	if (mCrtIdx >= (i64)mDocs.size())
	{
		finished = true;
		return true;
	}
	docid = ++mCrtIdx;
	return true;
}


bool
AosTermFilter::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermFilter::getDocidsFromIIL(
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
AosTermFilter::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	aos_assert_r(docid > 0, 0);
	u64 tempdocid; 
	if (!mReverseOrder)
	{
		tempdocid = docid - 1;
	}
	else
	{
		tempdocid = mDocs.size() - docid;
	}
	aos_assert_r(tempdocid >= 0 && tempdocid < mDocs.size(), 0);
	return mDocs[tempdocid];
}


bool 	
AosTermFilter::moveTo(const i64 &pos, const AosRundataPtr &rdata)
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
AosTermFilter::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	return mDocsNum;
}


void	
AosTermFilter::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
}

void
AosTermFilter::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermFilter::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	std::map<OmnString, vector<AosXmlTagPtr> > ::iterator it;

	u64 docsSize = 0;
	i64 tmpsize = 0;
	bool finished = false;
	bool isFilter = false;
	bool rslt = true;
	u64 docid; 
	AosXmlTagPtr xml;
	AosXmlTagPtr xmltemp;
	OmnString key;
	OmnString name, vv, type, value, str;
	u64 u64value, u64vv;
	u64value = u64vv = 0;
	double fvalue, fvv;
	fvalue = fvv = 0.0;
	while (rslt)
	{
		if (mOrTerm->withValue())
		{
			rslt = mOrTerm->nextDocid(0, docid, str, finished, rdata);
		}
		else
		{
			rslt = mOrTerm->nextDocid(0, docid, finished, rdata);
		}
		if (!rslt) break;
		if(docid == AOS_INVDID && finished) break;

		if(mOrTerm->withValue())
		{
			xml = mOrTerm->getDoc(docid, str, rdata);
		}
		else
		{
			xml = mOrTerm->getDoc(docid, rdata);
		}
		if (!xml)
		{
			OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			continue;
		}
		
		xml = xml->clone(AosMemoryCheckerArgsBegin);
		if (mSdoc)
		{
			rdata->setWorkingDoc(xml, false);
			AosSmartDocObjPtr sobj = AosSmartDocObj::getSmartDocObj();
			aos_assert_r(sobj, false);
			sobj->runSmartdoc(mSdoc, rdata);
		}

		tmpsize++;

		for (u32 k = 0; k < mFilterPtrs.size(); k++)
		{
			AosOpr opr = mFilterPtrs[k].opr; 
			name = mFilterPtrs[k].name;
			vv = mFilterPtrs[k].value;
			type = mFilterPtrs[k].type;

			if (type == "u64")
			{
				u64value = xml->getAttrU64(name, 0);
				rslt = vv.parseU64(u64vv);
				aos_assert_r(rslt, false);
				isFilter = doFilter(rdata, u64value, u64vv, opr);
			}
			else if (type == "double")
			{
				fvalue = xml->getAttrDouble(name, 0);
				u32 indx = 0;
				u32 len = vv.length();
				rslt = vv.parseDouble(indx, len, fvv);
				aos_assert_r(rslt, false);                  
				isFilter = doFilter(rdata, fvalue, fvv, opr);
			}
			else if (type == "str")
			{
				value = xml->getAttrStr(name, 0);
				isFilter = doFilter(rdata, value, vv, opr);
			}

			if (!isFilter) break;
		}

		if (isFilter)
		{
			mDocs.push_back(xml);
			if ((i64)mDocs.size() > (mStartIdx+1) * mPsize) break;

			if (mDocs.size() >= eDocsMax) break;

			docsSize += xml->getDocsize();
			if (docsSize > eMapContentMax)
			{
				OmnAlarm << "TermFilter's map is to big! map size : " << docsSize<< enderr;
				break;
			}
		}
		isFilter = false;
	}
	
	i64 total = mOrTerm->getTotal(rdata);

	if (total > 0 && tmpsize >= (i64)mDocs.size() && mDocs.size() > 0)
	{
		mDocsNum = i64( total / (tmpsize / (float)mDocs.size()) + 0.5 );
	}
	else
	{
		mDocsNum = mDocs.size();
	}

	if(rslt) mDataLoaded = true;

	return true;
}

bool
AosTermFilter::doFilter(
		const AosRundataPtr &rdata,
		const u64 &value,
		const u64 &vv,
		const AosOpr opr)
{
	switch(opr)
	{
	case eAosOpr_gt: return value > vv;
	case eAosOpr_ge: return value >= vv;
	case eAosOpr_eq: return value == vv;	
	case eAosOpr_lt: return value < vv;	
	case eAosOpr_le: return value <= vv;	
	case eAosOpr_ne: return value != vv;	
	default:
		return false;
	}
	return false;
}


bool
AosTermFilter::doFilter(
		const AosRundataPtr &rdata,
		const OmnString &value,
		const OmnString &vv,
		const AosOpr opr)
{
	switch(opr)
	{
	case eAosOpr_eq: return value == vv;	
	case eAosOpr_ne: return value != vv;	
	case eAosOpr_like: return vv.indexOf(value, 0);	
	case eAosOpr_prefix: return vv.hasPrefix(value.getBuffer());	
	default:
		return false;
	}
	return false;
}


bool
AosTermFilter::doFilter(
		const AosRundataPtr &rdata,
		const double &value,
		const double &vv,
		const AosOpr opr)
{
	switch(opr)
	{
	case eAosOpr_gt: return value > vv;
	case eAosOpr_ge: return value >= vv;
	case eAosOpr_eq: return value == vv;	
	case eAosOpr_lt: return value < vv;	
	case eAosOpr_le: return value <= vv;	
	case eAosOpr_ne: return value != vv;	
	default:
		return false;
	}
	return false;
}


bool
AosTermFilter::parse(const AosXmlTagPtr &conds, const AosRundataPtr &rdata)
{
	// 'conds' should be in the form:
	// <stat>
	//  <cond type="filet" >
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
	//      <stat>
	//      	...........
	//      </stat>
	//		<filter_conds>
	//			<filter_conds  zky_type="str|u64|douible" zky_opr="gt|ge|eq|lt|le|ne|pf|lk" zky_key="xxxx">xxxxx</filter_conds>
	//			...
	//		</filter_conds>
	//  </cond>
	//  <stat>
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

	OmnString objid = conds->getAttrStr("zky_sdoc_objid", "");
	if (objid != "")  mSdoc = AosGetDocByObjid(objid, rdata);

	AosXmlTagPtr filter_conds= conds->getFirstChild("filter_conds");
	aos_assert_r(filter_conds, false);
	
	AosXmlTagPtr filter_cond = filter_conds->getFirstChild("filter_cond");
	AosFilter filter_ptr;
	OmnString vv;
	while(filter_cond)
	{
		vv = filter_cond->getAttrStr("zky_key", "");
		aos_assert_r(vv != "", false);
		filter_ptr.name = vv;

		vv = filter_cond->getAttrStr("zky_opr", "eq");
		filter_ptr.opr = AosOpr_toEnum(vv);

		vv = filter_cond->getAttrStr("zky_type", "str");
		filter_ptr.type = vv;

		vv = filter_cond->getNodeText();
		aos_assert_r(vv != "", false);
		filter_ptr.value = vv;

		mFilterPtrs.push_back(filter_ptr);
		filter_cond = filter_conds->getNextChild();
	}

	AosQueryTermObjPtr cond;

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
		andterm = conds->getNextChild();
		if (andterm->getTagname() == filter_conds->getTagname())
			andterm = 0;

	}

	// Check containers. It is possible to 
	if (cond)
	{
		if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
		mOrTerm->addTerm(cond, rdata);
	}

	if (mOrderFname != "" && mOrTerm) 
	{
		mOrTerm->setOrder(mOrderContainer, mOrderFname, mReverseOrder, rdata);
	}

	// Backward Compatibility, Chen Ding, 09/20/2011
	aos_assert_r(mOrTerm, false);
	mOrTerm->setPagesize(mPsize);
	//mOrTerm->setExpectSize(mStartIdx + mPsize);
	
	mOrTerm->loadData(rdata);
	mTotal = mOrTerm->getTotal(rdata);
	mOrTerm->reset(rdata);

	OmnString str;
	if (mOrTerm) mOrTerm->toString(str);
	OmnScreen << "Query: " << str << endl;
	rdata->setOk();

	if (!mDataLoaded) loadData(rdata);

	mIsGood = true;
	return true;
}


bool
AosTermFilter::processStatistics(
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
		if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
		bool rslt = mOrTerm->addStatTerm(statTerm, rdata);
		aos_assert_rr(rslt, rdata, false);
		rdata->setOk();
		return true;
	}

	AosQueryTermObjPtr term = cond->addTerm(statTerm, rdata);
	aos_assert_r(term, false);

	if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
	cond->setNeedOrder(false);           
	mOrTerm = OmnNew AosTermOr();
	mOrTerm->addTerm(cond, rdata);
	mOrTerm->setDocRetriever(term);
	mOrTerm->loadData(rdata);
	return true;
}


AosQueryTermObjPtr
AosTermFilter::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermFilter(def, rdata);
}

